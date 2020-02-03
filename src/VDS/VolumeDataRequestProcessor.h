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

#ifndef VOLUMEDATAREQUESTPROCESSOR_H
#define VOLUMEDATAREQUESTPROCESSOR_H

#include <OpenVDS/VolumeData.h>
#include <OpenVDS/OpenVDS.h>

#include "VolumeDataPageAccessorImpl.h"
#include "VolumeDataChunk.h"
#include "ThreadPool.h"

#include <stdint.h>
#include <map>
#include <functional>
#include <vector>

namespace OpenVDS
{

class VolumeDataPage;

struct PageAccessorKey
{
  DimensionsND dimensionsND;
  int32_t lod;
  int32_t channel;
  bool operator<(const PageAccessorKey &other) const
  {
    return (int(dimensionsND) == int(other.dimensionsND)) ? lod == other.lod ? channel < other.channel : lod < other.lod : int(dimensionsND) < int(other.dimensionsND);
  }
};

struct JobPage
{
  JobPage(VolumeDataPageImpl *page, const VolumeDataChunk &chunk)
    : page(page)
    , chunk(chunk)
  {}
  VolumeDataPageImpl *page;
  VolumeDataChunk chunk;
};

struct Job
{
  Job(int64_t jobId, std::condition_variable &doneNotify, VolumeDataPageAccessorImpl &pageAccessor, int pagesCount, std::mutex &completed_mutex)
    : jobId(jobId)
    , doneNotify(doneNotify)
    , pageAccessor(pageAccessor)
    , pagesProcessed(0)
    , done(false)
    , cancelled(false)
    , pagesCount(pagesCount)
    , completed_mutex(completed_mutex)
  {}

  int64_t jobId;
  std::condition_variable &doneNotify;
  VolumeDataPageAccessorImpl &pageAccessor;
  std::vector<JobPage> pages;
  std::vector<std::future<Error>> future;
  std::atomic_int pagesProcessed;
  std::atomic_bool done;
  std::atomic_bool cancelled;
  const int pagesCount;
  std::mutex &completed_mutex;
  Error completedError;
};

class VolumeDataRequestProcessor
{
public:
  VolumeDataRequestProcessor(VolumeDataAccessManagerImpl &manager);
  ~VolumeDataRequestProcessor();

  int64_t AddJob(const std::vector<VolumeDataChunk> &chunks, std::function<bool(VolumeDataPageImpl *page, const VolumeDataChunk &volumeDataChunk, Error &error)> processor, bool singleThread = false);
  bool  IsCompleted(int64_t requestID);
  bool  IsCanceled(int64_t requestID);
  bool  WaitForCompletion(int64_t requestID, int millisecondsBeforeTimeout = 0);
  void  Cancel(int64_t requestID);
  float GetCompletionFactor(int64_t requestID);

  int CountActivePages();
private:
  VolumeDataAccessManagerImpl &m_manager;
  std::map<PageAccessorKey, VolumeDataPageAccessorImpl *> m_pageAccessors;
  std::vector<std::unique_ptr<Job>> m_jobs;
  std::mutex m_mutex;
  std::condition_variable m_jobNotification;
  ThreadPool m_threadPool;
  std::atomic_bool m_cleanupExit;
  std::thread m_cleanupThread;
};

}

#endif
