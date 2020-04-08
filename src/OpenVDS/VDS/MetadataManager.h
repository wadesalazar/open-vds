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
#include <memory>


#include <OpenVDS/VolumeData.h>

#include "VolumeDataChunk.h"
#include "WaveletTypes.h"

namespace OpenVDS
{
  enum AdaptiveMode
  {
    AdaptiveMode_BestQuality,
    AdaptiveMode_Tolerance,
    AdaptiveMode_Ratio
  };

  struct MetadataStatus
  {
    int                 m_chunkIndexCount;
    int                 m_chunkMetadataPageSize;
    int                 m_chunkMetadataByteSize;
    float               m_compressionTolerance;
    CompressionMethod   m_compressionMethod;
    int64_t             m_uncompressedSize;
    int64_t             m_adaptiveLevelSizes[WAVELET_ADAPTIVE_LEVELS];
  };

  class Request;
  class MetadataManager;
  class MetadataPage
  {
    friend MetadataManager;

    MetadataManager *m_manager;

    int m_pageIndex;
    bool m_valid;
    bool m_dirty;
    int m_lockCount;
    Error m_transferError;

    std::shared_ptr<Request> m_activeTransfer;

    std::vector<uint8_t> m_data;
  public:
    MetadataManager *GetManager() { return m_manager; }
    int PageIndex() const { return m_pageIndex; }
    bool IsValid()   const { return m_valid; }
    bool IsDirty()   const { return m_dirty; }
    const Error &transferError() const { return m_transferError; }

    MetadataPage(MetadataManager *manager, int pageIndex)
      : m_manager(manager)
      , m_pageIndex(pageIndex)
      , m_valid(false)
      , m_dirty(false)
      , m_lockCount(0)
      , m_activeTransfer(nullptr)
    {}
  };
  typedef std::list<MetadataPage> MetadataPageList;

  class IOManager;
  class VolumeDataAccessManagerImpl;

  class MetadataManager
  {
    IOManager *m_iomanager;
    std::string m_layerUrl;

    MetadataStatus m_metadataStatus;

    std::mutex m_mutex;

    int m_pageLimit;

    typedef std::unordered_map<int, MetadataPageList::iterator> MetadataPageMap;

    MetadataPageMap  m_pageMap;
    MetadataPageList m_pageList;
    MetadataPageList m_dirtyPageList;

    void LimitPages();
  public:
    MetadataManager(IOManager *iomanager, std::string const &layerURL, MetadataStatus const &MetadataStatus, int pageLimit);
    ~MetadataManager();

    const char *LayerUrl() const { return m_layerUrl.c_str(); }
    const std::string &LayerUrlStr() const { return m_layerUrl; }

    MetadataPage *LockPage(int pageIndex, bool *InitiateTransfer);

    void InitPage(MetadataPage* page);

    void PageTransferError(VolumeDataAccessManagerImpl* accessManager, MetadataPage* page, const Error &error);

    void PageTransferCompleted(VolumeDataAccessManagerImpl *accessManager, MetadataPage* page, std::vector<uint8_t>&& data);

    void InitiateTransfer(VolumeDataAccessManagerImpl* accessManager, MetadataPage* page, std::string const& url, bool verbose);
    void UploadDirtyPages(VolumeDataAccessManagerImpl* accessManager);

    uint8_t const *GetPageEntry(MetadataPage *page, int entry) const;
    void SetPageEntry(MetadataPage *page, int entryIndex, uint8_t const *metadata, int metadataLength);

    void UnlockPage(MetadataPage *page);

    MetadataStatus const &GetMetadataStatus() const { return m_metadataStatus; }
  };
}

#endif //METADATAMANAGER_H