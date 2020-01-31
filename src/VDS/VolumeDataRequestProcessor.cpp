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
#include "VDS.h"

#include <cstdint>
#include <algorithm>
#include <atomic>
#include <thread>
#include <chrono>

#include <fmt/format.h>

namespace OpenVDS
{

static void CleanupThread(std::atomic_bool &exit, std::condition_variable &wakeup,  std::map<PageAccessorKey, VolumeDataPageAccessorImpl *> &pageAccessors, std::mutex &pageAccessorsMutex)
{
  auto long_block = std::chrono::hours(24 * 32 * 12);
  auto in_progress_block = std::chrono::seconds(30);
  while(!exit)
  {
    std::unique_lock<std::mutex> lock(pageAccessorsMutex);
    std::chrono::seconds waitFor = long_block;
    for (auto &it: pageAccessors)
    {
      auto &pageAccessor = it.second;
      int ref = pageAccessor->GetReferenceCount();
      if (ref > 0)
      {
        if (waitFor > in_progress_block)
          waitFor = in_progress_block;
      }
      else
      {
        if (pageAccessor->GetMaxPages() > 0)
        {
          auto duration = pageAccessor->GetLastUsed() - (std::chrono::steady_clock::now() - in_progress_block);
          if (duration < std::chrono::seconds(0))
            pageAccessor->SetMaxPages(0);
          else if (duration < waitFor)
          {
            waitFor = std::chrono::duration_cast<std::chrono::seconds>(duration) + std::chrono::seconds(1);
          }
        }
      }
    }
    wakeup.wait_for(lock, waitFor);
  }
}

VolumeDataRequestProcessor::VolumeDataRequestProcessor(VolumeDataAccessManagerImpl& manager)
  : m_manager(manager)
  , m_threadPool(std::thread::hardware_concurrency())
  , m_cleanupExit(false)
  , m_cleanupThread([this]() { CleanupThread(m_cleanupExit, m_jobNotification, m_pageAccessors, m_mutex); } )
{}

VolumeDataRequestProcessor::~VolumeDataRequestProcessor()
{
  m_cleanupExit = true;
  m_jobNotification.notify_all();
  m_cleanupThread.join();
  for (auto &pair : m_pageAccessors)
  {
    m_manager.DestroyVolumeDataPageAccessor(pair.second);
  }
}

static int64_t GenJobId()
{
  static std::atomic< std::int64_t > id(0);
  return ++id;
}

struct MarkJobAsDoneOnExit
{
  MarkJobAsDoneOnExit(Job *job, int index)
    : job(job)
    , index(index)
  {}
  ~MarkJobAsDoneOnExit()
  {

    {
      JobPage& jobPage = job->pages[index];
      jobPage.page->UnPin();
    }
    int completed = job->completed.fetch_add(1);
    if (completed == int(job->pages.size()) - 1)
    {
      std::unique_lock<std::mutex>(job->completed_mutex);
      job->pageAccessor.SetLastUsed(std::chrono::steady_clock::now());
      job->pageAccessor.RemoveReference();
      job->doneNotify.notify_all();
    }
  }
  Job *job;
  int index;
};

static Error ProcessPageInJob(Job *job, size_t pageIndex, VolumeDataPageAccessorImpl *pageAccessor, std::function<bool(VolumeDataPageImpl *page, const VolumeDataChunk &chunk, Error &error)> processor)
{
  MarkJobAsDoneOnExit jobDone(job, pageIndex);
  Error error;
  JobPage& jobPage = job->pages[pageIndex];
  if (!pageAccessor->ReadPreparedPaged(jobPage.page))
  {
    error.code = -1;
    error.string = fmt::format("Failed to read page {}.", jobPage.page->GetChunkIndex());
    return error;
  }
  if (job->cancelled)
  {
    error.code = -4;
    error.string = fmt::format("Request: {} has been cancelled.", job->jobId);
    return error;
  }
  if (jobPage.page->GetError().code)
  {
    job->cancelled = true;
    return jobPage.page->GetError();
  }
  processor(jobPage.page, jobPage.chunk, error);

  return error;
}

int64_t VolumeDataRequestProcessor::AddJob(const std::vector<VolumeDataChunk>& chunks, std::function<bool(VolumeDataPageImpl * page, const VolumeDataChunk &volumeDataChunk, Error & error)> processor, bool singleThread)
{
  auto layer = chunks.front().layer;
  DimensionsND dimensions = DimensionGroupUtil::GetDimensionsNDFromDimensionGroup(layer->GetPrimaryChannelLayer().GetChunkDimensionGroup());
  int channel = layer->GetChannelIndex();
  int lod = layer->GetLOD();
  auto layout = layer->GetLayout();

  const int maxPages = std::max(8, (int)chunks.size());

  std::unique_lock<std::mutex> lock(m_mutex);
  PageAccessorKey key = { dimensions, lod, channel };
  auto page_accessor_it = m_pageAccessors.find(key);
  if (page_accessor_it == m_pageAccessors.end())
  {
    auto pa = static_cast<VolumeDataPageAccessorImpl *>(m_manager.CreateVolumeDataPageAccessor(layout, dimensions, lod, channel, maxPages, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly));
    auto insert_result = m_pageAccessors.emplace(key, pa);
    assert(insert_result.second);
    page_accessor_it = insert_result.first;
  }

  VolumeDataPageAccessorImpl *pageAccessor = page_accessor_it->second;
  assert(pageAccessor);

  if(pageAccessor->GetMaxPages() < maxPages)
  {
    pageAccessor->SetMaxPages(maxPages);
  }

  pageAccessor->AddReference();

  m_jobs.emplace_back(new Job(GenJobId(), m_jobNotification, *pageAccessor, m_mutex));
  auto &job = m_jobs.back();

  job->pages.reserve(chunks.size());
  for (const auto &c : chunks)
  {
    job->pages.emplace_back();
    JobPage &jobPage = job->pages.back();
    jobPage.page = static_cast<VolumeDataPageImpl *>(pageAccessor->PrepareReadPage(c.index));
    assert(jobPage.page && "Need to add error handling here when the page cannot be read");
    jobPage.chunk = c;
    if (!singleThread)
    {
      size_t index = job->pages.size() - 1;
      auto job_ptr = job.get();
      job->future.push_back(m_threadPool.Enqueue([job_ptr, index, pageAccessor, processor] { return ProcessPageInJob(job_ptr, index, pageAccessor, processor); }));
    }
  }
  if (singleThread)
  {
    auto job_ptr = job.get();
    job->future.push_back(m_threadPool.Enqueue([job_ptr, pageAccessor, processor]
    {
      Error error;
      int pages_size = int(job_ptr->pages.size());
      for (int i = 0; i < pages_size; i++)
      {
        if (error.code == 0)
        {
          error = ProcessPageInJob(job_ptr, size_t(i), pageAccessor, processor);
          if (error.code)
          {
            job_ptr->cancelled = true;
          }
        }
        else
        {
          ProcessPageInJob(job_ptr, size_t(i), pageAccessor, processor);
        }
      }
      return error;
    }));
  }
  return job->jobId;
}

bool  VolumeDataRequestProcessor::IsCompleted(int64_t jobID)
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
  
bool VolumeDataRequestProcessor::IsCanceled(int64_t jobID)
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
  
bool VolumeDataRequestProcessor::WaitForCompletion(int64_t jobID, int millisecondsBeforeTimeout)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  auto job_it = std::find_if(m_jobs.begin(), m_jobs.end(), [jobID](const std::unique_ptr<Job> &job) { return job->jobId == jobID; });
  if (job_it == m_jobs.end())
    return false;

  Job *job = job_it->get();
  if (job->completed < job->pages.size())
  {
    if (millisecondsBeforeTimeout > 0)
    {
      std::chrono::milliseconds toWait(millisecondsBeforeTimeout);
      job_it->get()->doneNotify.wait_for(lock, toWait, [job]{ return job->completed == job->pages.size();});
    } else
    {
      job_it->get()->doneNotify.wait(lock, [job]{ return job->completed == job->pages.size();});
    }
  }

  if (job->completed == job->pages.size() && !job->cancelled)
  {
    m_jobs.erase(job_it);
    return true;
  }
  return false;
}

void VolumeDataRequestProcessor::Cancel(int64_t jobID)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  auto job_it = std::find_if(m_jobs.begin(), m_jobs.end(), [jobID](std::unique_ptr<Job> &job) { return job->jobId == jobID; });
  if (job_it == m_jobs.end())
    return;
  job_it->get()->cancelled = true;
}

float VolumeDataRequestProcessor::GetCompletionFactor(int64_t jobID)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  auto job_it = std::find_if(m_jobs.begin(), m_jobs.end(), [jobID](std::unique_ptr<Job> &job) { return job->jobId == jobID; });
  if (job_it == m_jobs.end())
    return 0.f;
  return float(job_it->get()->completed) / float(job_it->get()->pages.size());
}

int VolumeDataRequestProcessor::CountActivePages()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  int ret = 0;
  for (auto &pa : m_pageAccessors)
    ret += pa.second->GetMaxPages();
  return ret;
}

}
