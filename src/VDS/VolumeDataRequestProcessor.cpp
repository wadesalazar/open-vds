/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
****************************************************************************/

#include "VolumeDataRequestProcessor.h"

#include "VolumeDataChunk.h"
#include "VolumeDataChannelMapping.h"
#include "VolumeDataLayoutImpl.h"
#include "VolumeDataPageImpl.h"
#include <OpenVDSHandle.h>

#include <cstdint>
#include <algorithm>
#include <atomic>
#include <thread>
#include <chrono>

#include <fmt/format.h>

namespace OpenVDS
{

VolumeDataRequestProcessor::VolumeDataRequestProcessor(VolumeDataAccessManagerImpl& manager)
  : m_manager(manager)
  , m_threadPool(std::thread::hardware_concurrency())
{}

static int64_t gen_jobid()
{
  static std::atomic< std::int64_t > id(0);
  return ++id;
}

struct MarkJobAsDoneOnExit
{
  MarkJobAsDoneOnExit(Job *job)
    : job(job)
  {}
  ~MarkJobAsDoneOnExit()
  {
    int completed = job->completed.fetch_add(1);
    if (completed == job->pages.size() - 1)
    {
      job->doneNotify.notify_all();
    }
  }
  Job *job;
};

static Error processPageInJob(Job *job, size_t pageIndex, VolumeDataPageAccessorImpl *pageAccessor, std::function<bool(VolumeDataPageImpl *page, const VolumeDataChunk &chunk, Error &error)> processor)
{
  MarkJobAsDoneOnExit jobDone(job);
  Error error;
  JobPage& jobPage = job->pages[pageIndex];
  if (jobPage.needToReadPage)
  {
    if (!pageAccessor->readPreparedPaged(jobPage.page))
    {
      error.code = -1;
      error.string = fmt::format("Failed to read page {}.", jobPage.page->getChunkIndex());
      return error;
    }
  }
  if (job->cancelled)
  {
    error.code = -4;
    error.string = fmt::format("Request: {} has been cancelled.", job->jobId);
    return error;
  }

  processor(jobPage.page, jobPage.chunk, error);
  jobPage.page->unPin();

  return error;
}

int64_t VolumeDataRequestProcessor::addJob(const std::vector<VolumeDataChunk>& chunks, std::function<bool(VolumeDataPageImpl * page, const VolumeDataChunk &volumeDataChunk, Error & error)> processor)
{
  auto layer = chunks.front().layer;
  DimensionsND dimensions = DimensionGroupUtil::getDimensionsNDFromDimensionGroup(layer->getPrimaryChannelLayer().getChunkDimensionGroup());
  int channel = layer->getChannelIndex();
  int lod = layer->getLOD();
  auto layout = layer->getLayout();

  const int maxPages = std::max(8, (int)chunks.size());

  std::unique_lock<std::mutex> lock(m_mutex);
  PageAccessorKey key = { dimensions, lod, channel };
  auto page_accessor_it = m_pageAccessors.find(key);
  if (page_accessor_it == m_pageAccessors.end())
  {
    auto pa = static_cast<VolumeDataPageAccessorImpl *>(m_manager.createVolumeDataPageAccessor(layout, dimensions, lod, channel, maxPages, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly));
    auto insert_result = m_pageAccessors.insert({key, pa});
    assert(insert_result.second);
    page_accessor_it = insert_result.first;
  }

  m_jobs.emplace_back(new Job(gen_jobid(), m_jobNotification));
  auto &job = m_jobs.back();
  job->pages.reserve(chunks.size());

  VolumeDataPageAccessorImpl *pageAccessor = page_accessor_it->second;
  if(pageAccessor->getMaxPages() < maxPages)
  {
    pageAccessor->setMaxPages(maxPages);
  }

  for (const auto &c : chunks)
  {
    job->pages.emplace_back();
    JobPage &jobPage = job->pages.back();
    jobPage.page = static_cast<VolumeDataPageImpl *>(pageAccessor->prepareReadPage(c.chunkIndex, &jobPage.needToReadPage));
    assert(jobPage.page && "Need to add error handling here when the page cannot be read");
    jobPage.chunk = c;
    size_t index = job->pages.size() - 1;
    auto job_ptr = job.get();
    job->future.push_back(m_threadPool.enqueue([job_ptr, index, pageAccessor, processor] { return processPageInJob(job_ptr, index, pageAccessor, processor); }));
  }
  return job->jobId;
}

bool  VolumeDataRequestProcessor::isCompleted(int64_t jobID)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  auto job_it = std::find_if(m_jobs.begin(), m_jobs.end(), [jobID](const std::unique_ptr<Job> &job) { return job->jobId == jobID; });
  if (job_it == m_jobs.end())
    return false;

  if (job_it->get()->completed == job_it->get()->pages.size() && !job_it->get()->cancelled)
  {
    m_jobs.erase(job_it);
    return true;
  }
  return false;
}
  
bool VolumeDataRequestProcessor::isCanceled(int64_t jobID)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  auto job_it = std::find_if(m_jobs.begin(), m_jobs.end(), [jobID](const std::unique_ptr<Job> &job) { return job->jobId == jobID; });
  if (job_it == m_jobs.end())
    return false;
  
  if (job_it->get()->completed == job_it->get()->pages.size() && job_it->get()->cancelled)
  {
    m_jobs.erase(job_it);
    return true;
  }
  return false;
}
  
bool VolumeDataRequestProcessor::waitForCompletion(int64_t jobID, int millisecondsBeforeTimeout)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  auto job_it = std::find_if(m_jobs.begin(), m_jobs.end(), [jobID](const std::unique_ptr<Job> &job) { return job->jobId == jobID; });
  if (job_it == m_jobs.end())
    return false;

  Job *job = job_it->get();
  if (millisecondsBeforeTimeout > 0)
  {
    std::chrono::milliseconds toWait(millisecondsBeforeTimeout);
    job_it->get()->doneNotify.wait_for(lock, toWait, [job]{ return job->completed == job->pages.size();});
  } else
  {
    job_it->get()->doneNotify.wait(lock, [job]{ return job->completed == job->pages.size();});
  }

  if (job->completed == job->pages.size() && !job->cancelled)
  {
    m_jobs.erase(job_it);
    return true;
  }
  return false;
}

void VolumeDataRequestProcessor::cancel(int64_t jobID)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  auto job_it = std::find_if(m_jobs.begin(), m_jobs.end(), [jobID](std::unique_ptr<Job> &job) { return job->jobId == jobID; });
  if (job_it == m_jobs.end())
    return;
  job_it->get()->cancelled = true;
}

}
