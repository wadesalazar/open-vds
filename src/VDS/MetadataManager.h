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

#ifndef METADATAMANAGER_H
#define METADATAMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <list>


#include <OpenVDS/VolumeData.h>

#include "VolumeDataChunk.h"


namespace OpenVDS
{
  struct MetaDataStatus
  {
    enum
    {
      WAVELET_ADAPTIVE_LEVELS = 16
    };

    int                 m_chunkIndexCount;
    int                 m_chunkMetaDataPageSize;
    int                 m_chunkMetaDataByteSize;
    float               m_compressionTolerance;
    CompressionMethod   m_compressionMethod;
    int64_t             m_uncompressedSize;
    int64_t             m_adaptiveLevelSizes[WAVELET_ADAPTIVE_LEVELS];
  };

  class ObjectRequester;
  class MetaDataManager;
  class MetaDataPage
  {
    friend MetaDataManager;

    MetaDataManager *m_manager;

    int m_pageIndex;
    bool m_valid;
    int m_lockCount;

    std::shared_ptr<ObjectRequester> m_activeTransfer;

    std::vector<uint8_t> m_data;
  public:
    MetaDataManager *GetManager() { return m_manager; }
    int PageIndex() const { return m_pageIndex; }
    bool IsValid()   const { return m_valid; }

    MetaDataPage(MetaDataManager *manager, int pageIndex)
      : m_manager(manager)
      , m_pageIndex(pageIndex)
      , m_valid(false)
      , m_lockCount(0)
      , m_activeTransfer(nullptr)
    {}
  };
  typedef std::list<MetaDataPage> MetaDataPageList;

  class IOManager;
  class MetaDataManager
  {
    IOManager *m_iomanager;
    std::string m_layerUrl;

    MetaDataStatus m_metaDataStatus;

    std::mutex m_mutex;

    int m_pageLimit;

    typedef std::unordered_map<int, MetaDataPageList::iterator> MetaDataPageMap;

    MetaDataPageMap m_pageMap;
    MetaDataPageList m_pageList;

    void limitPages();
  public:
    MetaDataManager(IOManager *iomanager, std::string const &layerURL, MetaDataStatus const &metadataStatus, int pageLimit);
    ~MetaDataManager();

    const char *layerUrl() const { return m_layerUrl.c_str(); }
    const std::string &layerUrlStr() const { return m_layerUrl; }

    MetaDataPage *lockPage(int pageIndex, bool *initiateTransfer);

    void pageTransferError(MetaDataPage *page, const char *msg);

    void pageTransferCompleted(MetaDataPage *page);

    void initiateTransfer(MetaDataPage *page, std::string const &url, bool verbose, const std::vector<std::string>& headers);

    uint8_t const *GetPageEntry(MetaDataPage *page, int entry) const;

    void unlockPage(MetaDataPage *page);

    MetaDataStatus const &metadataStatus() const { return m_metaDataStatus; }
  };
}

#endif //METADATAMANAGER_H
