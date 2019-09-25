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
#include "VolumeDataLayout.h"
#include "VolumeDataPageImpl.h"
#include <OpenVDSHandle.h>

#include <algorithm>
#include <atomic>

namespace OpenVDS
{

VolumeDataRequestProcessor::VolumeDataRequestProcessor(VolumeDataAccessManagerImpl& manager)
  : m_manager(manager)
{}

int64_t gen_jobid()
{
  static std::atomic_int64_t id = 0;
  return ++id;
}

int64_t VolumeDataRequestProcessor::addJob(std::vector<VolumeDataChunk>& chunks, std::function<void(VolumeDataPage* page)> processor)
{
  auto layer = chunks.front().layer;
  DimensionsND dimensions = DimensionGroupUtil::getDimensionsNDFromDimensionGroup(layer->getChunkDimensionGroup());
  int channel = layer->getChannelIndex();
  int lod = layer->getLOD();
  auto layout = layer->getLayout();

  PageAccessorKey key = { dimensions, lod, channel };
  auto page_accessor_it = m_page_accessors.find(key);
  if (page_accessor_it == m_page_accessors.end())
  {
    auto pa = static_cast<VolumeDataPageAccessorImpl *>(m_manager.createVolumeDataPageAccessor(layout, dimensions, lod, channel, 100, false));
    auto insert_result = m_page_accessors.insert({key, pa});
    assert(insert_result.second);
    page_accessor_it = insert_result.first;
  }
  m_jobs.emplace_back(gen_jobid());
  auto job = m_jobs.back();
  for (const auto &c : chunks)
  {
    job.chunks.push_back(page_accessor_it->second->readPage(c.chunkIndex));
  }
  return 0;
}


}
