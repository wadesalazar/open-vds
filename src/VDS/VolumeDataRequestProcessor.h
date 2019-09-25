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

#include "VolumeDataPageAccessorImpl.h"
#include "VolumeDataChunk.h"

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

struct Job
{
  Job(int64_t job_id)
    : job_id(job_id)
  {}
  int64_t job_id;
  std::vector<VolumeDataPage *> chunks;
  std::function<void(VolumeDataPage *page)> processor;
};

class VolumeDataRequestProcessor
{
public:
  VolumeDataRequestProcessor(VolumeDataAccessManagerImpl &manager);

  int64_t addJob(std::vector<VolumeDataChunk> &chunks, std::function<void(VolumeDataPage *page)> processor);
private:
  VolumeDataAccessManagerImpl &m_manager;
  std::map<PageAccessorKey, VolumeDataPageAccessorImpl *> m_page_accessors;
  std::vector<Job> m_jobs;
  std::mutex m_map_lock;
};

}

#endif
