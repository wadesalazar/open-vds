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

#include "MetaDataManager.h"

#include <IO/IOManager.h>

#include <assert.h>

namespace OpenVDS
{

class MetaDataPageTransfer : public TransferHandler
{
public:
MetaDataPageTransfer(std::vector<uint8_t> &targetData)
    : m_targetData(targetData)
{ }

std::vector<uint8_t> &m_targetData;

void handleData(std::vector<uint8_t> &&data) override
{
  m_targetData = data;
}

void handleError(Error &error) override
{
}

};

MetaDataManager::MetaDataManager(IOManager *iomanager, std::string const &layerUrl, MetaDataStatus const &metaDataStatus, int pageLimit)
    : m_iomanager(iomanager)
    , m_layerUrl(layerUrl)
    , m_metaDataStatus(metaDataStatus)
    , m_pageLimit(pageLimit)
{}

MetaDataManager::~MetaDataManager()
{}

void MetaDataManager::limitPages()
{
  assert(m_pageMap.size() == m_pageList.size());

  while(m_pageList.size() > m_pageLimit && m_pageList.back().m_lockCount == 0)
  {
    m_pageMap.erase(m_pageList.back().m_pageIndex);
    m_pageList.pop_back();
  }

  assert(m_pageMap.size() == m_pageList.size());
}

MetaDataPage *
MetaDataManager::lockPage(int pageIndex, bool *initiateTransfer)
{
  assert(initiateTransfer);

  std::unique_lock<std::mutex>
    lock(m_mutex);

  assert(m_pageMap.size() == m_pageList.size());

  MetaDataPageMap::iterator
    mi = m_pageMap.find(pageIndex);

  if(mi != m_pageMap.end())
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

  MetaDataPage *page = std::addressof(*m_pageList.begin());

  page->m_lockCount++;

  limitPages();

  assert(m_pageMap.size() == m_pageList.size());
  assert(m_pageMap.find(pageIndex) != m_pageMap.end());
  assert(m_pageMap[pageIndex]->m_lockCount > 0);

  return page;
}

void MetaDataManager::initiateTransfer(MetaDataPage *page, std::string const &url, bool verbose, const std::vector<std::string>& headers)
{
  std::unique_lock<std::mutex>
    lock(m_mutex);

  assert(!page->m_valid && !page->m_activeTransfer);

  page->m_activeTransfer = m_iomanager->requestObject(url, std::make_shared<MetaDataPageTransfer>(page->m_data));

}
}
