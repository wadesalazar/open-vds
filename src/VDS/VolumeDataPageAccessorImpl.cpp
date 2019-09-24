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
#include "VolumeDataLayout.h"
#include "VolumeDataLayer.h"
#include "VolumeDataPageImpl.h"
#include "VolumeDataStore.h"
#include "MetadataManager.h"

#include <IO/IOManager.h>
#include "OpenVDSHandle.h"

#include "DimensionGroup.h"

#include <chrono>

namespace OpenVDS
{

VolumeDataPageAccessorImpl::VolumeDataPageAccessorImpl(VolumeDataAccessManagerImpl* accessManager, VolumeDataLayer* layer, int maxPages, bool isReadWrite)
  : m_accessManager(accessManager)
  , m_layer(layer)
  , m_pagesFound(0)
  , m_pagesRead(0)
  , m_pagesWritten(0)
  , m_currentPages(0)
  , m_maxPages(maxPages)
  , m_references(0)
  , m_isReadWrite(isReadWrite)
  , m_isCommitInProgress(false)
{
}
  
VolumeDataLayout const* VolumeDataPageAccessorImpl::getLayout() const
{
  return m_layer->getLayout();
}

int VolumeDataPageAccessorImpl::getLOD() const
{
  return m_layer->getLOD();
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

VolumeDataPage* VolumeDataPageAccessorImpl::createPage(int64_t chunk)
{
  std::unique_lock<std::mutex> pageMutexLocker(m_pagesMutex);

  if(!m_layer)
  {
    return nullptr;
  }

  return nullptr;
}

VolumeDataPage* VolumeDataPageAccessorImpl::readPage(int64_t chunk)
{
  std::unique_lock<std::mutex> pageMutexLocker(m_pagesMutex);

  if(!m_layer)
  {
    return NULL;
  }

  if (m_layer->getProduceStatus() == VolumeDataLayer::ProduceStatus_Unavailable)
  {
    fprintf(stderr, "The accessed dimension group or channel is unavailable (check produce status on VDS before accessing data)");
    return nullptr;
  }

  for(auto page_it = m_pages.begin(); page_it != m_pages.end(); ++page_it)
  {
    if((*page_it)->getChunkIndex() == chunk)
    {
      if (page_it != m_pages.begin())
      {
        m_pages.splice(m_pages.begin(), m_pages, page_it, std::next(page_it));
      }
      (*page_it)->pin();

      while((*page_it)->isEmpty())
      {
        m_pageReadCondition.wait_for(pageMutexLocker, std::chrono::milliseconds(1000));

        if(!m_layer)
        {
          (*page_it)->unPin();
          return nullptr;
        }
      }

      m_pagesFound++;
      return *page_it;
    }
  }

  // Wait for commit to finish before inserting a new page
  while(m_isCommitInProgress)
  {
    m_commitFinishedCondition.wait_for(pageMutexLocker, std::chrono::milliseconds(1000));
  }

  if(!m_layer)
  {
    return nullptr;
  }

  // Not found, we need to create a new page
  VolumeDataPageImpl *page = new VolumeDataPageImpl(this, chunk);

  m_pages.push_front(page);
  m_currentPages++;

  assert(page->isPinned());

  Error error;
  VolumeDataChunk volumeDataChunk = m_layer->getChunkFromIndex(chunk);
  if (!m_accessManager->prepareReadChunkData(volumeDataChunk, true, error))
  {
    page->unPin();
    fprintf(stderr, "Failed to download chunk: %s\n", error.string.c_str());
    return nullptr;
  }

  pageMutexLocker.unlock();

  std::vector<uint8_t> serialized_data;
  std::vector<uint8_t> metadata;
  CompressionInfo compressionInfo;
  if (!m_accessManager->readChunk(volumeDataChunk, serialized_data, metadata, compressionInfo, error))
  {
    pageMutexLocker.lock();
    page->unPin();
    fprintf(stderr, "Failed when waiting for chunk: %s\n", error.string.c_str());
    return nullptr;
  }

  std::vector<uint8_t> page_data;
  DataBlock dataBlock;
  if (!VolumeDataStore::deserializeVolumeData(volumeDataChunk, serialized_data, metadata, compressionInfo.GetCompressionMethod(), compressionInfo.GetAdaptiveLevel(), m_layer->getFormat(), dataBlock, page_data, error))
  {
    pageMutexLocker.lock();
    page->unPin();
    fprintf(stderr, "Failed when deserializing chunk: %s\n", error.string.c_str());
    return nullptr;
  }

  int pitch[Dimensionality_Max];
  for (int i = 0; i < std::size(pitch); i++)
  {
    if (i < std::size(dataBlock.pitch))
      pitch[i] = dataBlock.pitch[i];
    else
      pitch[i] = 1;
  }

  pageMutexLocker.lock();
  page->setBufferData(std::move(page_data), pitch);
  m_pagesRead++;

  m_pageReadCondition.notify_all();

  if(!m_layer)
  {
    page->unPin();
    page = NULL;
  }

  limitPageListSize(m_maxPages, pageMutexLocker);

  return page;
}

void VolumeDataPageAccessorImpl::limitPageListSize(int maxPages, std::unique_lock<std::mutex>& pageListMutexLock)
{
  while(m_currentPages > m_maxPages)
  {
    // Wait for commit to finish before deleting a page
    while(m_isCommitInProgress)
    {
      m_commitFinishedCondition.wait_for(pageListMutexLock, std::chrono::milliseconds(1000));
    }

    // Find a page to evict
    auto page = m_pages.end();

    for (auto r_it = m_pages.rbegin(); r_it != m_pages.rend(); ++r_it)
    {
      if (!(*r_it)->isPinned())
      {
        page = ++r_it.base();
        break;
      }
    }

    if(page == m_pages.end())
    {
      return;
    }

    if((*page)->isWritten())
    {
      // Finish reading all pages currently being read
      while(1)
      {
        bool isReadInProgress = false;

        for(VolumeDataPageImpl *targetPage : m_pages)
        {
          if((*page)->isCopyMarginNeeded(targetPage))
          {
            if(targetPage->isEmpty())
            {
              isReadInProgress = true;
              break;
            }
          }
        }

        if(!isReadInProgress)
        {
          break;
        }

        (*page)->pin(); // Make sure this page isn't deleted by LimitPageListSize!

        // Wait for the page getting read
        m_pageReadCondition.wait_for(pageListMutexLock, std::chrono::milliseconds(1000));

        (*page)->unPin();

        // Check if the page was pinned while we released the mutex, we have to abort evicting this page
        if((*page)->isPinned())
        {
          break;
        }
      }

      // Check if the page was pinned while we released the mutex, we have to abort evicting this page
      if((*page)->isPinned())
      {
        continue;
      }

      // Copy margins
      if((*page)->isWritten())
      {
        for(VolumeDataPageImpl *targetPage : m_pages)
        {
          if((*page)->isCopyMarginNeeded(targetPage))
          {
            (*page)->copyMargin(targetPage);
          }
        }
      }
    }

    auto page_v = *page;
    m_pages.erase(page);
    m_currentPages--;

    if(page_v->isDirty())
    {
      (*page)->writeBack(m_layer, pageListMutexLock);
      m_pagesWritten++;
    }

    delete page_v;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Commit

void VolumeDataPageAccessorImpl::commit()
{
  std::unique_lock<std::mutex> pageListMutexLock(m_pagesMutex);

  if(m_isCommitInProgress)
  {
    return;
  }

  // Make sure we don't start reading any new pages while we're finishing up the current waiting reads
  m_isCommitInProgress = true;

  // Finish reading all pages currently being read
  for(VolumeDataPageImpl *page : m_pages)
  {
    while(page->isEmpty())
    {
      // Wait for the page getting read
      m_pageReadCondition.wait_for(pageListMutexLock, std::chrono::milliseconds(1000));
    }
  }

  // Copy all margins
  for(VolumeDataPageImpl *page : m_pages)
  {
    if(page->isWritten())
    {
      for(VolumeDataPageImpl *targetPage : m_pages)
      {
        if(page->isCopyMarginNeeded(targetPage))
        {
          page->copyMargin(targetPage);
        }
      }
    }
  }

  for(VolumeDataPageImpl *page : m_pages)
  {
    if(page->isDirty() && m_layer)
    {
      page->writeBack(m_layer, pageListMutexLock);
      m_pagesWritten++;
    }
  }

  m_isCommitInProgress = false;
  m_commitFinishedCondition.notify_all();

  // Only access the layout if there has been any write requests.
  // This allows for deleting read and interpolating accessors after their VDS has been deleted,
  // which can happen during project unload in Headwave.
  if (m_isReadWrite && m_pagesWritten > 0 && m_layer)
  {
    // FIXME: Make sure *all* invalidates are received, this is just a stop-gap measure.
    pageListMutexLock.unlock();
    m_layer->getLayout()->completePendingWriteChunkRequests(0);
  }
}
}