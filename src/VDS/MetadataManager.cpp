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
#include "VolumeDataAccessManagerImpl.h"
#include <assert.h>

namespace OpenVDS
{

class MetadataPageTransfer : public TransferHandler
{
public:
MetadataPageTransfer(MetadataManager *manager, VolumeDataAccessManagerImpl *accessManager, MetadataPage *metadataPage)
  : manager(manager)
  , accessManager(accessManager)
  , metadataPage(metadataPage)
{ }

void handleData(std::vector<uint8_t> &&data) override
{
  manager->pageTransferCompleted(accessManager, metadataPage, std::move(data));
}

void handleError(Error &error) override
{
}

MetadataManager *manager;
VolumeDataAccessManagerImpl *accessManager;
MetadataPage *metadataPage;
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

void MetadataManager::limitPages()
{
  assert(m_pageMap.size() == m_pageList.size());

  while (m_pageList.size() > m_pageLimit && m_pageList.back().m_lockCount == 0)
  {
    m_pageMap.erase(m_pageList.back().m_pageIndex);
    m_pageList.pop_back();
  }

  assert(m_pageMap.size() == m_pageList.size());
}

MetadataPage*
MetadataManager::lockPage(int pageIndex, bool* initiateTransfer)
{
  assert(initiateTransfer);

  std::unique_lock<std::mutex> lock(m_mutex);

  assert(m_pageMap.size() == m_pageList.size());

  MetadataPageMap::iterator mi = m_pageMap.find(pageIndex);

  if (mi != m_pageMap.end())
  {
    // Move page to front of list
    m_pageList.splice(m_pageList.begin(), m_pageList, mi->second);

    *initiateTransfer = false;
  }
  else
  {
    // Create page in front of list
    m_pageList.emplace_front(this, pageIndex);
    m_pageMap[pageIndex] = m_pageList.begin();

    *initiateTransfer = true;
  }

  MetadataPage* page = std::addressof(*m_pageList.begin());

  page->m_lockCount++;

  limitPages();

  assert(m_pageMap.size() == m_pageList.size());
  assert(m_pageMap.find(pageIndex) != m_pageMap.end());
  assert(m_pageMap[pageIndex]->m_lockCount > 0);

  return page;
}

void MetadataManager::initiateTransfer(VolumeDataAccessManagerImpl *accessManager, MetadataPage* page, std::string const& url, bool verbose)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  assert(!page->m_valid && !page->m_activeTransfer);

  page->m_activeTransfer = m_iomanager->requestObject(url, std::make_shared<MetadataPageTransfer>(this, accessManager, page));
}

void MetadataManager::pageTransferError(MetadataPage* page, const char* msg)
{
  std::string
    errorString = std::string("Failed to transfer metadata page: ") + std::string(msg);

  fprintf(stderr, "OpenVDS: %s\n", errorString.c_str());

  // what to do
  //  std::unique_lock<std::mutex> metadataManagersMutexLock(VDSRemotePlugin::s_metadataManagersMutex);
  //
  //  for(auto pluginInstance : m_references)
  //  {
  //    pluginInstance->PageTransferCompleted(page);
//  }

  //TODO pendingrequestchangedcondition.notify_all
}

void MetadataManager::pageTransferCompleted(VolumeDataAccessManagerImpl* accessManager, MetadataPage* page, std::vector<uint8_t> &&data)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  page->m_data = std::move(data);
  page->m_valid = true;
  lock.unlock();

  accessManager->pageTransferCompleted(page);
}


uint8_t const *MetadataManager::getPageEntry(MetadataPage *page, int entryIndex) const
{
  assert(page->IsValid());

  return &page->m_data[entryIndex * m_metadataStatus.m_chunkMetadataByteSize];
}

void MetadataManager::unlockPage(MetadataPage *page)
{
  assert(page);
  assert(page->m_lockCount > 0);

  std::unique_lock<std::mutex> lock(m_mutex);

  assert(m_pageMap.size() == m_pageList.size());
  assert(m_pageMap.find(page->m_pageIndex) != m_pageMap.end());

  page->m_lockCount--;

  if(page->m_lockCount == 0 && !page->m_valid)
  {
    if(page->m_activeTransfer)
    {
      page->m_activeTransfer->cancel();
      page->m_activeTransfer = nullptr;
    }

    MetadataPageMap::iterator pageMapIterator = m_pageMap.find(page->m_pageIndex);

    MetadataPageList::iterator pageListIterator = pageMapIterator->second;

    m_pageMap.erase(pageMapIterator);
    m_pageList.erase(pageListIterator);
  }

  limitPages();
}

} 
