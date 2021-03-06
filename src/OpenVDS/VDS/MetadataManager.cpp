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

#include "MetadataManager.h"

#include <IO/IOManager.h>
#include "VolumeDataStoreIOManager.h"
#include "WaveletTypes.h"
#include <assert.h>
#include <algorithm>
#include <fmt/format.h>

namespace OpenVDS
{

class MetadataPageTransfer : public TransferDownloadHandler
{
public:
MetadataPageTransfer(MetadataManager *manager, VolumeDataStoreIOManager *volumeDataStore, MetadataPage *metadataPage)
  : manager(manager)
  , volumeDataStore(volumeDataStore)
  , metadataPage(metadataPage)
{ }

void HandleObjectSize(int64_t size) override {}
void HandleObjectLastWriteTime(const std::string &lastWriteTimeISO8601) override {}
void HandleMetadata(const std::string &key, const std::string &header) override {};
void HandleData(std::vector<uint8_t> &&data) override
{
  metadata = std::move(data);
}

void Completed(const Request &request, const Error &e) override
{
  Error error = e;
  if (error.code == 0 && metadata.empty())
  {
    error.code = -1;
    error.string = "Empty medata page data downloaded";
  }

  if(error.code == 0)
  {
    manager->PageTransferCompleted(volumeDataStore, metadataPage, std::move(metadata));
  }
  else
  {
    manager->PageTransferError(volumeDataStore, metadataPage, error);
  }
}

MetadataManager *manager;
VolumeDataStoreIOManager *volumeDataStore;
MetadataPage *metadataPage;
std::vector<uint8_t> metadata;
};

MetadataManager::MetadataManager(IOManager* iomanager, std::string const& layerUrl, MetadataStatus const& metadataStatus, int pageLimit)
  : m_iomanager(iomanager)
  , m_layerUrl(layerUrl)
  , m_metadataStatus(metadataStatus)
  , m_pageLimit(pageLimit)
{
}

MetadataManager::~MetadataManager()
{
}

void MetadataManager::LimitPages()
{
  assert(m_pageMap.size() == m_pageList.size() + m_dirtyPageList.size());

  while (int(m_pageList.size()) > m_pageLimit && m_pageList.back().m_lockCount == 0)
  {
    m_pageMap.erase(m_pageList.back().m_pageIndex);
    m_pageList.pop_back();
  }

  assert(m_pageMap.size() == m_pageList.size() + m_dirtyPageList.size());
}

MetadataPage*
MetadataManager::LockPage(int pageIndex, bool* initiateTransfer)
{
  assert(initiateTransfer);

  std::unique_lock<std::mutex> lock(m_mutex);

  assert(m_pageMap.size() == m_pageList.size() + m_dirtyPageList.size());

  MetadataPageMap::iterator mi = m_pageMap.find(pageIndex);

  if (mi != m_pageMap.end())
  {
    // Move page to front of list
    if (!mi->second->IsDirty())
    {
      m_pageList.splice(m_pageList.begin(), m_pageList, mi->second);
    }

    *initiateTransfer = false;
  }
  else
  {
    // Create page in front of list
    m_pageList.emplace_front(this, pageIndex);
    mi = m_pageMap.insert(MetadataPageMap::value_type(pageIndex, m_pageList.begin())).first;

    *initiateTransfer = true;
  }

  MetadataPage &page = *mi->second;

  page.m_lockCount++;

  LimitPages();

  assert(m_pageMap.size() == m_pageList.size() + m_dirtyPageList.size());
  assert(m_pageMap.find(pageIndex) != m_pageMap.end());
  assert(m_pageMap[pageIndex]->m_lockCount > 0);

  return &page;
}

void
MetadataManager::InitPage(MetadataPage* page)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  page->m_data.resize(m_metadataStatus.m_chunkMetadataByteSize * m_metadataStatus.m_chunkMetadataPageSize);
  page->m_valid = true;
  lock.unlock();
}

void MetadataManager::InitiateTransfer(VolumeDataStoreIOManager *volumeDataStore, MetadataPage* page, std::string const& url)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  assert(!page->m_valid && !page->m_activeTransfer);

  page->m_activeTransfer = m_iomanager->ReadObject(url, std::make_shared<MetadataPageTransfer>(this, volumeDataStore, page));
}

void MetadataManager::UploadDirtyPages(VolumeDataStoreIOManager *volumeDataStore)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  for(MetadataPageList::iterator it = m_dirtyPageList.begin(), next; it != m_dirtyPageList.end(); it = next)
  {
    auto &page = *it;
    assert(page.IsDirty());

    // We need to keep a separate 'next' iterator since we're moving the current element to another list if the write is successful
    next = std::next(it);

    bool success = volumeDataStore->WriteMetadataPage(&page, page.m_data);
    if(success)
    {
      page.m_dirty = false;
      m_pageList.splice(m_pageList.begin(), m_dirtyPageList, it);
    }
  }
}

void MetadataManager::PageTransferError(VolumeDataStoreIOManager* volumeDataStore, MetadataPage* page, const Error &error)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  page->m_valid = false;
  page->m_transferError = error;
  lock.unlock();
  volumeDataStore->PageTransferCompleted(page, error);
}

void MetadataManager::PageTransferCompleted(VolumeDataStoreIOManager* volumeDataStore, MetadataPage* page, std::vector<uint8_t> &&data)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  page->m_data = std::move(data);
  page->m_valid = true;
  lock.unlock();

  Error error;
  volumeDataStore->PageTransferCompleted(page, error);
}

uint8_t const *MetadataManager::GetPageEntry(MetadataPage *page, int entryIndex) const
{
  assert(page->IsValid());

  return &page->m_data[entryIndex * m_metadataStatus.m_chunkMetadataByteSize];
}

void MetadataManager::SetPageEntry(MetadataPage *page, int entryIndex, uint8_t const *metadata, int metadataLength, uint8_t *oldMetadata)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  assert(page->IsValid());
  assert(metadataLength == m_metadataStatus.m_chunkMetadataByteSize);

  if(!page->m_dirty)
  {
    page->m_dirty = true;

    MetadataPageMap::iterator pageMapIterator = m_pageMap.find(page->m_pageIndex);

    m_dirtyPageList.splice(m_dirtyPageList.end(), m_pageList, pageMapIterator->second);
  }
  if(oldMetadata)
  {
    memcpy(oldMetadata, &page->m_data[entryIndex * m_metadataStatus.m_chunkMetadataByteSize], metadataLength);
  }
  //std::copy(metadata, metadata + metadataLength, &page->m_data[entryIndex * m_metadataStatus.m_chunkMetadataByteSize]);
  memcpy(&page->m_data[entryIndex * m_metadataStatus.m_chunkMetadataByteSize], metadata, metadataLength);
}

void MetadataManager::UnlockPage(MetadataPage *page)
{
  assert(page);
  assert(page->m_lockCount > 0);

  std::unique_lock<std::mutex> lock(m_mutex);

  assert(m_pageMap.size() == m_pageList.size() + m_dirtyPageList.size());
  assert(m_pageMap.find(page->m_pageIndex) != m_pageMap.end());

  page->m_lockCount--;

  if(page->m_lockCount == 0 && !page->m_valid)
  {
    if(page->m_activeTransfer)
    {
      page->m_activeTransfer->Cancel();
      page->m_activeTransfer = nullptr;
    }

    MetadataPageMap::iterator pageMapIterator = m_pageMap.find(page->m_pageIndex);

    MetadataPageList::iterator pageListIterator = pageMapIterator->second;

    m_pageMap.erase(pageMapIterator);
    m_pageList.erase(pageListIterator);
  }

  LimitPages();
}

void MetadataManager::UpdateMetadataStatus(int64_t uncompressedSize, int serializedSize, bool subtract, const uint8_t (&targetLevels)[WAVELET_ADAPTIVE_LEVELS])
{
  std::unique_lock<std::mutex> lock(m_mutex);

  if(subtract)
  {
    m_metadataStatus.m_uncompressedSize -= uncompressedSize;
  }
  else
  {
    m_metadataStatus.m_uncompressedSize += uncompressedSize;
  }

  Wavelet_AccumulateAdaptiveLevelSizes(serializedSize, m_metadataStatus.m_adaptiveLevelSizes, subtract, targetLevels);
}

}
