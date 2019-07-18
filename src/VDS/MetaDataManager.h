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

#include <OpenVDS/VolumeData.h>

#include "VolumeDataChunk.h"

namespace OpenVDS
{               
  struct MetadataStatus
  {
    enum
    {
      WAVELET_ADAPTIVE_LEVELS = 16
    };

    int                 m_chunkIndexCount;
    int                 m_chunkMetadataPageSize;
    int                 m_chunkMetadataByteSize;
    float               m_compressionTolerance;
    CompressionMethod   m_compressionMethod;
    int64_t             m_uncompressedSize;
    int64_t             m_adaptiveLevelSizes[WAVELET_ADAPTIVE_LEVELS];
  };

  class MetadataPage
  {
    friend MetadataManager;

    MetadataManager *m_manager;

    int m_pageIndex;
    bool m_valid;
    int m_lockCount;
    
    MetadataPageTransfer *m_activeTransfer;

    std::vector<unsigned char> m_data;
  public:
    MetadataManager *GetManager() { return m_manager; }
    int PageIndex() const { return m_pageIndex; }
    bool IsValid()   const { return m_valid; }

    MetadataPage(MetadataManager *manager, int pageIndex)
      : m_manager(manager)
      , m_pageIndex(pageIndex)
      , m_valid(false)
      , m_lockCount(0)
      , m_activeTransfer(nullptr)
    {}
  };

  class MetadataManager
  {
    std::string m_layerURL;

    std::vector<VDSRemotePlugin *> m_references;

    MetadataStatus m_metadataStatus;

    std::mutex m_mutex;

    int m_pageLimit;

    class PendingReadChunk
    {
    public:
      VolumeDataChunk   m_volumeDataChunk;
      VDSRemotePlugin  *m_pluginInstance;

      PendingReadChunk();
      PendingReadChunk(VolumeDataChunk const &volumeDataChunk, VDSRemotePlugin *pluginInstance);
    };

    typedef std::unordered_map<int, MetadataPageList::iterator> MetadataPageMap;

    MetadataPageMap m_pageMap;
    MetadataPageList m_pageList;

    typedef std::unordered_map<VolumeDataChunk, std::vector<PendingReadChunk>> PendingReadChunkMap;

    PendingReadChunkMap m_pendingReadChunkMap;

    void limitPages();
  public:
    MetadataManager(std::string const &layerURL, VDSRemotePlugin::MetadataStatus const &metadataStatus, int pageLimit);
    ~MetadataManager();

    const char *layerURL() const { return m_layerURL.c_str(); }

    metadataPage *lockPage(int pageIndex, bool *initiateTransfer);

    void pageTransferError(VDSRemotePlugin::MetadataPage *page, const char *msg);

    void pageTransferCompleted(MetadataPage *page);

    void initiateTransfer(MetadataPage *page, std::string const &url, bool verbose, const std::vector<std::string>& headers);

    uint8_t const *GetPageEntry(VDSRemotePlugin::MetadataPage *page, int entry) const;

    void unlockPage(MetadataPage *page);

    MetadataStatus const &metadataStatus() const { return m_metadataStatus; }

  };

}
#endif //METADATAMANAGER_H
