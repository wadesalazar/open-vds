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
#include "Env.h"

#include <cstdint>
#include <algorithm>
#include <atomic>
#include <thread>
#include <chrono>

#include <fmt/format.h>

namespace OpenVDS
{

OPENVDS_EXPORT int _cleanupthread_timeoutseconds = 30;

static void CleanupThread(std::atomic_bool &exit, std::condition_variable &wakeup,  std::map<PageAccessorKey, VolumeDataPageAccessorImpl *> &pageAccessors, std::mutex &pageAccessorsMutex)
{
  auto long_block = std::chrono::hours(24 * 32 * 12);
  auto in_progress_block = std::chrono::seconds(_cleanupthread_timeoutseconds);
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
      if (jobPage.page)
        jobPage.page->UnPin();
    }
    if (++job->pagesProcessed == job->pagesCount)
    {
      std::unique_lock<std::mutex> lock(job->completed_mutex);
      job->pageAccessor.SetLastUsed(std::chrono::steady_clock::now());
      job->pageAccessor.RemoveReference();
      job->done = true;
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

  if (!jobPage.page)
    return Error();

  if (jobPage.page && jobPage.page->GetError().code)
  {
    job->cancelled = true;
    return jobPage.page->GetError();
  }
  if (job->cancelled)
  {
    auto page = jobPage.page;
    jobPage.page = nullptr;
    pageAccessor->CancelPreparedReadPage(page);
    error.code = -4;
    error.string = fmt::format("Request with id {} has been cancelled.", job->jobId);
    return error;
  }
  if (!pageAccessor->ReadPreparedPaged(jobPage.page))
  {
    job->cancelled = true;
    error = jobPage.page->GetError();
    return error;
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
    pa->RemoveReference();
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

  m_jobs.emplace_back(new Job(GenJobId(), m_jobNotification, *pageAccessor, chunks.size(), m_mutex));
  auto &job = m_jobs.back();

  job->pages.reserve(chunks.size());
  job->future.reserve(chunks.size());
  for (const auto &c : chunks)
  {
    job->pages.emplace_back(static_cast<VolumeDataPageImpl *>(pageAccessor->PrepareReadPage(c.index)), c);
    if (!job->pages.back().page)
    {
      job->cancelled;
      break;
    }
  }
  job->pagesCount = job->pages.size();

  if (job->cancelled)
  {
    for (auto &jobPage : job->pages)
    {
      if (jobPage.page)
      {
        pageAccessor->CancelPreparedReadPage(jobPage.page);
        jobPage.page = nullptr;
      }
    }
    job->pagesProcessed = job->pagesCount;
    return job->jobId;
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
          if (job_ptr->pages[i].page)
          {
            pageAccessor->CancelPreparedReadPage(job_ptr->pages[i].page);
            job_ptr->pages[i].page = nullptr; 
          }
        }
      }
      return error;
    }));
  } 
  else
  {
    auto job_ptr = job.get();
    for (int i = 0; i < job->pages.size(); i++)
    {
      job->future.push_back(m_threadPool.Enqueue([job_ptr, i, pageAccessor, processor]
        {
          return ProcessPageInJob(job_ptr, i, pageAccessor, processor);
        }));
    }
  }
  return job->jobId;
}

bool  VolumeDataRequestProcessor::IsCompleted(int64_t jobID)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  auto job_it = std::find_if(m_jobs.begin(), m_jobs.end(), [jobID](const std::unique_ptr<Job> &job) { return job->jobId == jobID; });
  if (job_it == m_jobs.end())
    return false;

  auto job = job_it->get();
  if (job->done && !job->cancelled)
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

  auto job = job_it->get();
  if (job->done && job->cancelled)
  {
    static bool should_print = getBooleanEnvironmentVariable("OPENVDS_DEBUG_IS_CANCELLED");
    if (should_print)
    {
      std::string out = "Printing cancelled request results\n";
      for (int i = 0; i < job->pagesCount; i++)
      {
        auto& future = job->future[i];
        Error error = future.get();
        if (!error.code)
          error.string = "OK";
        out += fmt::format("Request channel {} chunk {} result: {}\n", job->pages[i].chunk.layer->GetChannelIndex(), job->pages[i].chunk.index, error.string);
      }
      fmt::print(stderr, "{}", out);
    }
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
  if (!job->done)
  {
    if (millisecondsBeforeTimeout > 0)
    {
      std::chrono::milliseconds toWait(millisecondsBeforeTimeout);
      job->doneNotify.wait_for(lock, toWait, [job]
        {
          return job->done.load();
        });
    }
    else
    {
      job->doneNotify.wait(lock, [job]
        {
          return job->done.load();
        });
    }
  }
  if (job->done && !job->cancelled)
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
  m_jobNotification.notify_all();
}

float VolumeDataRequestProcessor::GetCompletionFactor(int64_t jobID)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  auto job_it = std::find_if(m_jobs.begin(), m_jobs.end(), [jobID](std::unique_ptr<Job> &job) { return job->jobId == jobID; });
  if (job_it == m_jobs.end())
    return 0.f;
  return float(job_it->get()->pagesProcessed) / float(job_it->get()->pagesCount);
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