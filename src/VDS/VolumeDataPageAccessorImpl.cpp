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

#include "VolumeDataPageAccessorImpl.h"
#include "VolumeDataLayer.h"
#include "VolumeDataPageImpl.h"

#include <IO/S3_Downloader.h>

namespace OpenVDS
{

static bool readChunkData(VDSHandle &handle, std::vector<uint8_t> &blob, int32_t (&pitch)[Dimensionality_Max])
{
  blob.clear();

  // This can probably be improved by looking up the data directly in the cache and not requesting it if it's valid,
  // similar to the VolumeSamples code

  for(auto &p : pitch)
    p = 0;

  return S3::downloadChunk(handle, blob, pitch);
}


VolumeDataPageAccessorImpl::VolumeDataPageAccessorImpl(VolumeDataLayer* layer, int maxPages, bool isReadWrite)
  : m_layer(layer)
  , m_maxPages(maxPages)
  , m_isReadWrite(isReadWrite)
{
}
  
VolumeDataLayout const* VolumeDataPageAccessorImpl::getLayout() const
{
  return m_layer->getLayout();
}

int VolumeDataPageAccessorImpl::getLod() const
{
  return m_layer->getLod();
}

int VolumeDataPageAccessorImpl::getChannelIndex() const
{
  return m_layer->getChannelIndex();
}

VolumeDataChannelDescriptor const& VolumeDataPageAccessorImpl::getChannelDescriptor() const
{
  return m_layer->getVolumeDataChannelDescriptor();
}

void  VolumeDataPageAccessorImpl::getNumSamples(int(&numSamples)[Dimensionality_Max]) const
{
  for (int i = 0; i < Dimensionality_Max; i++)
  {
    numSamples[i] = m_layer->getDimensionNumSamples(i);
  }
}
  
int64_t VolumeDataPageAccessorImpl::getChunkCount() const
{
  return m_layer->getTotalChunkCount();
}

void  VolumeDataPageAccessorImpl::getChunkMinMax(int64_t chunk, int(&min)[Dimensionality_Max], int(&max)[Dimensionality_Max]) const
{
  m_layer->getChunkMinMax(chunk, min, max, true);
}

void  VolumeDataPageAccessorImpl::getChunkMinMaxExcludingMargin(int64_t chunk, int(&min)[Dimensionality_Max], int(&max)[Dimensionality_Max]) const
{
  m_layer->getChunkMinMax(chunk, min, max, false);
}

int64_t VolumeDataPageAccessorImpl::getChunkIndex(const int(&position)[Dimensionality_Max]) const
{
  int32_t index_array[Dimensionality_Max];
  for (int i = 0; i < Dimensionality_Max; i++)
  {
    index_array[i] = m_layer->voxelToIndex(position[i], i);
  }
  return m_layer->indexArrayToChunkIndex(index_array); 
}

int VolumeDataPageAccessorImpl::addReference()
{
  return ++m_references;
}

int VolumeDataPageAccessorImpl::removeReference()
{
  return --m_references;
}

VolumeDataPage* VolumeDataPageAccessorImpl::readPageAtPosition(const int(&position)[Dimensionality_Max])
{
//  std::lock_guard<std::mutex> pageMutexLocker(m_pagesMutex);
//
//  if(!m_layer)
//  {
//    return NULL;
//  }
//
//  if (m_layer->getProduceStatus() == VolumeDataLayer::ProduceStatusUnavailable)
//  {
//    fprintf(stderr, "The accessed dimension group or channel is unavailable (check produce status on VDS before accessing data)");
//    return nullptr;
//  }
//
//  int32_t indexArray[Dimensionality_Max];
//
//  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
//  {
//    if(position[iDimension] < 0 || position[iDimension] >= m_layer->getDimensionNumSamples(iDimension))
//    {
//      return nullptr;
//    }
//    indexArray[iDimension] = m_layer->voxelToIndex(anPosition[iDimension], iDimension);
//  }
//
//  int64_t chunk = m_layer->indexArrayToChunkIndex(aiIndexArray);
//
//  for(auto page_it = m_pages.begin(); page_it != m_pages.end(); ++page_it)
//  {
//    if(page_it->getChunkIndex() == chunk)
//    {
//      if (page_it !- m_pages.begin())
//      {
//        list.splice(m_pages.begin(), m_pages, page_it, std::next(page_it));
//      }
//      page_it->pin();
//
//      while(page->isEmpty())
//      {
//        m_pageReadCondition.timedWait(1000);
//
//        if(!m_volumeDataLayer)
//        {
//          page_it->unPin();
//          return nullptr;
//        }
//      }
//
//      m_pagesFound++;
//      return *page_it;
//    }
//  }
//
//  // Wait for commit to finish before inserting a new page
//  while(m_isCommitInProgress)
//  {
//    m_commitFinishedCondition.TimedWait(1000);
//  }
//
//  if(!m_volumeDataLayer)
//  {
//    return nullptr;
//  }
//
//  // Not found, we need to create a new page
//  VolumeDataPageImpl *page = new VolumeDataPageImpl(this, chunk);
//
//  m_pages.push_front(page);
//  m_currentPages++;
//
//  assert(page->isPinned());
//
//  std::vector<uint8_t> blob;
//
//  int32_t pitch[Dimensionality_Max];
//
//  Error error;
//  if (!readChunkData(_pcHueVolumeDataLayer->GetChunkFromIndex(iChunk), &cHueBLOB, anPitch, error))
//  {
//    fprintf(stderr, "Failed to download chunk: %s\n", error.string.c_str());
//    return nullptr;
//  }
//
//  pageMutexLocker = std::lock_guard<std::mutex>();
//
//  HueJobStatus_e
//    eHueJobStatus;
//
//  // Run until job is complete (or cancelled)
//  do
//  {
//    eHueJobStatus = HueJobIDList_c::StaticInstance().WaitForJob(iHueJobID, 10);
//
//    if (eHueJobStatus == HUEJOBSTATUS_UNKNOWN && HueRootObj_c::StaticGetInstanceExists())
//    {
//      HueRootObj_c::StaticRun();
//    }
//  } while (eHueJobStatus == HUEJOBSTATUS_UNKNOWN);
//
//  cPageListMutexLock.Lock(__FILE__, __LINE__);
//
//  if (eHueJobStatus == HUEJOBSTATUS_COMPLETE)
//  {
//    HUEDEBUG_ASSERT(!cHueBLOB.IsEmpty());
//    pcPage->SetBufferData(cHueBLOB, anPitch);
//
//    _nPagesRead++;
//  }
//
//  _cPageReadCondition.Broadcast();
//
//  if(!_pcHueVolumeDataLayer)
//  {
//    pcPage->UnPin();
//    pcPage = NULL;
//  }
//
//  LimitPageListSize(_nMaxPages, cPageListMutexLock);
//
//  return pcPage;
return nullptr;
}

void VolumeDataPageAccessorImpl::commit()
{
}
}