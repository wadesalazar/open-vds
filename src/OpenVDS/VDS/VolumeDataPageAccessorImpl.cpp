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
#include "VolumeDataLayoutImpl.h"
#include "VolumeDataAccessManagerImpl.h"
#include "VolumeDataLayer.h"
#include "VolumeDataPageImpl.h"
#include "VolumeDataStore.h"
#include "MetadataManager.h"

#include <IO/IOManager.h>

#include "DimensionGroup.h"

#include <chrono>
#include <algorithm>

#include <fmt/printf.h>

namespace OpenVDS
{

VolumeDataPageAccessorImpl::VolumeDataPageAccessorImpl(VolumeDataAccessManagerImpl* accessManager, VolumeDataLayer* layer, int maxPages, bool isReadWrite)
  : m_accessManager(accessManager)
  , m_layer(layer)
  , m_pagesFound(0)
  , m_pagesRead(0)
  , m_pagesWritten(0)
  , m_maxPages(maxPages)
  , m_references(1)
  , m_isReadWrite(isReadWrite)
  , m_isCommitInProgress(false)
  , m_lastUsed(std::chrono::steady_clock::now())
{
}
  
VolumeDataLayout const* VolumeDataPageAccessorImpl::GetLayout() const
{
  return m_layer->GetLayout();
}

int VolumeDataPageAccessorImpl::GetLOD() const
{
  return m_layer->GetLOD();
}

int VolumeDataPageAccessorImpl::GetChannelIndex() const
{
  return m_layer->GetChannelIndex();
}

VolumeDataChannelDescriptor const& VolumeDataPageAccessorImpl::GetChannelDescriptor() const
{
  return m_layer->GetVolumeDataChannelDescriptor();
}

void  VolumeDataPageAccessorImpl::GetNumSamples(int(&numSamples)[Dimensionality_Max]) const
{
  for (int i = 0; i < Dimensionality_Max; i++)
  {
    numSamples[i] = m_layer->GetDimensionNumSamples(i);
  }
}

int64_t VolumeDataPageAccessorImpl::GetChunkCount() const
{
  return m_layer->GetTotalChunkCount();
}

void  VolumeDataPageAccessorImpl::GetChunkMinMax(int64_t chunk, int(&min)[Dimensionality_Max], int(&max)[Dimensionality_Max]) const
{
  m_layer->GetChunkMinMax(chunk, min, max, true);
}

void  VolumeDataPageAccessorImpl::GetChunkMinMaxExcludingMargin(int64_t chunk, int(&min)[Dimensionality_Max], int(&max)[Dimensionality_Max]) const
{
  m_layer->GetChunkMinMax(chunk, min, max, false);
}

int64_t VolumeDataPageAccessorImpl::GetChunkIndex(const int(&position)[Dimensionality_Max]) const
{
  int32_t index_array[Dimensionality_Max];
  for (int i = 0; i < Dimensionality_Max; i++)
  {
    index_array[i] = m_layer->VoxelToIndex(position[i], i);
  }
  return m_layer->IndexArrayToChunkIndex(index_array);
}

int VolumeDataPageAccessorImpl::AddReference()
{
  return ++m_references;
}

int VolumeDataPageAccessorImpl::RemoveReference()
{
  return --m_references;
}

int VolumeDataPageAccessorImpl::GetMaxPages()
{
  std::unique_lock<std::mutex> pageListMutexLock(m_pagesMutex);
  return m_maxPages;
}

void VolumeDataPageAccessorImpl::SetMaxPages(int maxPages)
{
  std::unique_lock<std::mutex> pageListMutexLock(m_pagesMutex);
  m_maxPages = maxPages;
  LimitPageListSize(m_maxPages, pageListMutexLock);
}

VolumeDataPage* VolumeDataPageAccessorImpl::CreatePage(int64_t chunk)
{
  std::unique_lock<std::mutex> pageListMutexLock(m_pagesMutex);

  if(!m_layer)
  {
    return nullptr;
  }

  if (m_layer->GetProduceStatus() == VolumeDataLayer::ProduceStatus_Unavailable)
  {
    fprintf(stderr, "The accessed dimension group or channel is unavailable (check produce status on VDS before accessing data)");
    return nullptr;
  }

  auto page_it = std::find_if(m_pages.begin(), m_pages.end(), [chunk](VolumeDataPageImpl *page)->bool { return page->GetChunkIndex() == chunk; });
  if(page_it != m_pages.end())
  {
    throw std::runtime_error("Cannot create a page that already exists");
  }

  // Wait for commit to finish before inserting a new page
  while(m_isCommitInProgress)
  {
    m_commitFinishedCondition.wait_for(pageListMutexLock, std::chrono::milliseconds(1000));
    if(!m_layer)
    {
      return nullptr;
    }
  }

  // Create a new page
  VolumeDataPageImpl *page = new VolumeDataPageImpl(this, chunk);

  m_pages.push_front(page);

  assert(page->IsPinned());

  Error error;
  VolumeDataChunk volumeDataChunk = m_layer->GetChunkFromIndex(chunk);

  pageListMutexLock.unlock();

  std::vector<uint8_t> page_data;
  DataBlock dataBlock;
  if (!VolumeDataStore::CreateConstantValueDataBlock(volumeDataChunk, m_layer->GetFormat(), m_layer->GetNoValue(), m_layer->GetComponents(), m_layer->IsUseNoValue() ? VolumeDataHash::NOVALUE : VolumeDataHash(0.0f), dataBlock, page_data, error))
  {
    pageListMutexLock.lock();
    page->UnPin();
    fprintf(stderr, "Failed when creating chunk: %s\n", error.string.c_str());
    return nullptr;
  }

  int pitch[Dimensionality_Max] = {};

  for(int chunkDimension = 0; chunkDimension < m_layer->GetChunkDimensionality(); chunkDimension++)
  {
    int dimension = DimensionGroupUtil::GetDimension(m_layer->GetChunkDimensionGroup(), chunkDimension);

    assert(dimension >= 0 && dimension < Dimensionality_Max);
    pitch[dimension] = dataBlock.Pitch[chunkDimension];
  }

  pageListMutexLock.lock();
  page->SetBufferData(dataBlock, pitch, std::move(page_data));
  page->MakeDirty();

  m_pageReadCondition.notify_all();

  if(!m_layer)
  {
    page->UnPin();
    page = nullptr;
  }

  LimitPageListSize(m_maxPages, pageListMutexLock);

  return page;
}

VolumeDataPage* VolumeDataPageAccessorImpl::PrepareReadPage(int64_t chunk, Error &error)
{
  std::unique_lock<std::mutex> pageListMutexLock(m_pagesMutex);

  if(!m_layer)
  {
    error.code = -1;
    error.string = "PrepareReadPage missing layer";
    return nullptr;
  }

  if (m_layer->GetProduceStatus() == VolumeDataLayer::ProduceStatus_Unavailable)
  {
    error.code = -1;
    error.string = "The accessed dimension group or channel is unavailable (check produce status on VDS before accessing data)";
    return nullptr;
  }

  for(auto page_it = m_pages.begin(); page_it != m_pages.end(); ++page_it)
  {
    if((*page_it)->GetChunkIndex() == chunk)
    {
      if (page_it != m_pages.begin())
      {
        m_pages.splice(m_pages.begin(), m_pages, page_it, std::next(page_it));
      }
      (*page_it)->Pin();

      m_pagesFound++;
      return *page_it;
    }
  }

  // Wait for commit to finish before inserting a new page
  while(m_isCommitInProgress)
  {
    m_commitFinishedCondition.wait_for(pageListMutexLock, std::chrono::milliseconds(1000));
    if(!m_layer)
    {
      error.code = -1;
      error.string = "PrepareReadPage loosing layer while waiting for commit";
      return nullptr;
    }
  }

  // Not found, we need to create a new page
  VolumeDataPageImpl *page = new VolumeDataPageImpl(this, chunk);

  m_pages.push_front(page);

  assert(page->IsPinned());

  VolumeDataChunk volumeDataChunk = m_layer->GetChunkFromIndex(chunk);
  if (!m_accessManager->GetVolumeDataStore()->PrepareReadChunk(volumeDataChunk, error))
  {
    page->SetError(error);
    page->UnPin();
    return nullptr;
  }

  return page;
}

bool VolumeDataPageAccessorImpl::ReadPreparedPaged(VolumeDataPage* page)
{
  std::unique_lock<std::mutex> pageListMutexLock(m_pagesMutex, std::defer_lock);

  VolumeDataPageImpl *pageImpl = static_cast<VolumeDataPageImpl *>(page);

  if (!pageImpl->RequestPrepared())
    return pageImpl->GetError().code == 0;

  if (pageImpl->EnterSettingData())
  {
    if (!pageImpl->RequestPrepared())
    {
      pageImpl->LeaveSettingData();
      return true;
    }
    Error error;
    VolumeDataChunk volumeDataChunk = m_layer->GetChunkFromIndex(pageImpl->GetChunkIndex());
    std::vector<uint8_t> serialized_data;
    std::vector<uint8_t> metadata;
    CompressionInfo compressionInfo;

    if (!m_accessManager->GetVolumeDataStore()->ReadChunk(volumeDataChunk, serialized_data, metadata, compressionInfo, error))
    {
      pageListMutexLock.lock();
      pageImpl->SetError(error);
      pageImpl->SetRequestPrepared(false);
      pageImpl->LeaveSettingData();
      m_pageReadCondition.notify_all();
      //fprintf(stderr, "Failed when waiting for chunk: %s\n", error.string.c_str());
      return false;
    }

    std::vector<uint8_t> page_data;
    DataBlock dataBlock;
    if (!m_accessManager->GetVolumeDataStore()->DeserializeVolumeData(volumeDataChunk, serialized_data, metadata, compressionInfo.GetCompressionMethod(), compressionInfo.GetAdaptiveLevel(), m_layer->GetFormat(), dataBlock, page_data, error))
    {
      pageListMutexLock.lock();
      pageImpl->SetError(error);
      pageImpl->SetRequestPrepared(false);
      pageImpl->LeaveSettingData();
      m_pageReadCondition.notify_all();
      //fprintf(stderr, "Failed when deserializing chunk: %s\n", error.string.c_str());
      return false;
    }

    int pitch[Dimensionality_Max] = {};

    for (int chunkDimension = 0; chunkDimension < m_layer->GetChunkDimensionality(); chunkDimension++)
    {
      int dimension = DimensionGroupUtil::GetDimension(m_layer->GetChunkDimensionGroup(), chunkDimension);

      assert(dimension >= 0 && dimension < Dimensionality_Max);
      pitch[dimension] = dataBlock.Pitch[chunkDimension];
    }

    pageListMutexLock.lock();
    pageImpl->SetBufferData(dataBlock, pitch, std::move(page_data));
    m_pagesRead++;
    pageImpl->SetRequestPrepared(false);
    pageImpl->LeaveSettingData();
    m_pageReadCondition.notify_all();
  }
  else
  {
    pageImpl->LeaveSettingData();
    pageListMutexLock.lock();
    m_pageReadCondition.wait(pageListMutexLock, [pageImpl]{return !pageImpl->SettingData();});
  }

  if (pageImpl->GetError().code)
    return false;

  if(!m_layer)
  {
    page = nullptr;
  }

  LimitPageListSize(m_maxPages, pageListMutexLock);
  return m_layer != nullptr;
}

void VolumeDataPageAccessorImpl::CancelPreparedReadPage(VolumeDataPage* page)
{
  std::unique_lock<std::mutex> pageListMutexLock(m_pagesMutex);

  VolumeDataPageImpl *pageImpl = static_cast<VolumeDataPageImpl *>(page);

  pageImpl->UnPin();
  if (pageImpl->IsPinned())
    return;

  auto page_it = std::find(m_pages.begin(), m_pages.end(), page);
  if (page_it != m_pages.end())
    m_pages.erase(page_it);
  pageListMutexLock.unlock();

  if (pageImpl->RequestPrepared())
  {
    VolumeDataChunk volumeDataChunk = m_layer->GetChunkFromIndex(pageImpl->GetChunkIndex());
    Error error;
    m_accessManager->GetVolumeDataStore()->CancelReadChunk(volumeDataChunk, error);
    if (error.code)
    {
      pageImpl->SetError(error);
    }
  }
  pageImpl->SetRequestPrepared(false);
  pageImpl->LeaveSettingData();
  m_pageReadCondition.notify_all();
}

VolumeDataPage* VolumeDataPageAccessorImpl::ReadPage(int64_t chunk)
{
  Error error;
  VolumeDataPage *page = PrepareReadPage(chunk, error);
  if (!page)
    return nullptr;
  if (error.code)
  {
    page->Release();
    return nullptr;
  }
  if (!ReadPreparedPaged(page))
  {
    page->Release();
    return nullptr;
  }
  return page;
}

void VolumeDataPageAccessorImpl::LimitPageListSize(int maxPages, std::unique_lock<std::mutex>& pageListMutexLock)
{
  while(int(m_pages.size()) > m_maxPages)
  {
    // Wait for commit to finish before deleting a page
    while(m_isCommitInProgress)
    {
      m_commitFinishedCondition.wait_for(pageListMutexLock, std::chrono::milliseconds(1000));
    }

    // Find a page to evict
    auto page_it = std::find_if(m_pages.rbegin(), m_pages.rend(), [](VolumeDataPageImpl *page)->bool { return !page->IsPinned(); });

    if(page_it == m_pages.rend())
    {
      return;
    }

    VolumeDataPageImpl *page = *page_it;

    if(page->IsWritten())
    {
      // Finish reading all pages currently being read
      while(1)
      {
        bool isReadInProgress = false;

        for(VolumeDataPageImpl *targetPage : m_pages)
        {
          if(page->IsCopyMarginNeeded(targetPage))
          {
            if(targetPage->IsEmpty())
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

        page->Pin(); // Make sure this page isn't deleted by LimitPageListSize!

        // Wait for the page getting read
        m_pageReadCondition.wait_for(pageListMutexLock, std::chrono::milliseconds(1000));

        page->UnPin();

        // Check if the page was pinned while we released the mutex, we have to abort evicting this page
        if(page->IsPinned())
        {
          break;
        }
      }

      // Check if the page was pinned while we released the mutex, we have to abort evicting this page
      if(page->IsPinned())
      {
        continue;
      }

      // Copy margins
      if(page->IsWritten())
      {
        for(VolumeDataPageImpl *targetPage : m_pages)
        {
          if(page->IsCopyMarginNeeded(targetPage))
          {
            page->CopyMargin(targetPage);
          }
        }
      }
    }

    m_pages.erase(std::prev(page_it.base()));

    if(page->IsDirty())
    {
      page->WriteBack(m_layer, pageListMutexLock);
      m_pagesWritten++;
    }

    delete page;
  }
}

int64_t VolumeDataPageAccessorImpl::RequestWritePage(int64_t chunk, const DataBlock& dataBlock, const std::vector<uint8_t>& data)
{
  std::vector<uint8_t> serializedData;
  uint64_t hash;

  hash = VolumeDataStore::SerializeVolumeData({ m_layer, chunk }, dataBlock, data, m_layer->GetEffectiveCompressionMethod(), serializedData);

  if (hash == VolumeDataHash::UNKNOWN)
  {
    hash = VolumeDataHash::GetUniqueHash();
  }

  std::vector<uint8_t> metadata(sizeof(hash));
  memcpy(metadata.data(), &hash, sizeof(hash));

  return m_accessManager->GetVolumeDataStore()->WriteChunk({ m_layer, chunk }, serializedData, metadata);
}
/////////////////////////////////////////////////////////////////////////////
// Commit

void VolumeDataPageAccessorImpl::Commit()
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
    while(page->IsEmpty())
    {
      // Wait for the page getting read
      m_pageReadCondition.wait_for(pageListMutexLock, std::chrono::milliseconds(1000));
    }
  }

  // Copy all margins
  for(VolumeDataPageImpl *page : m_pages)
  {
    if(page->IsWritten())
    {
      for(VolumeDataPageImpl *targetPage : m_pages)
      {
        if(page->IsCopyMarginNeeded(targetPage))
        {
          page->CopyMargin(targetPage);
        }
      }
    }
  }

  for(VolumeDataPageImpl *page : m_pages)
  {
    if(page->IsDirty() && m_layer)
    {
      page->WriteBack(m_layer, pageListMutexLock);
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
    m_layer->GetLayout()->CompletePendingWriteChunkRequests(0);
    m_accessManager->FlushUploadQueue();
  }
}
}
