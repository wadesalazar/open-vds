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

#ifndef VOLUMEDATASTOREIOMANAGER_H
#define VOLUMEDATASTOREIOMANAGER_H

#include <IO/IOManager.h>

#include "VDS.h"
#include "MetadataManager.h"
#include "VolumeDataStore.h"

#include <vector>
#include <mutex>
#include <unordered_map>

namespace OpenVDS
{
class MetadataPage;
class ReadChunkTransfer;

struct PendingDownloadRequest
{
  MetadataPage* m_lockedMetadataPage;
  int           m_adaptiveLevelToRequest;
  Error m_metadataPageRequestError;

  std::shared_ptr<Request> m_activeTransfer;
  std::shared_ptr<ReadChunkTransfer> m_transferHandle;
  int m_ref;
  bool m_canMove;
  PendingDownloadRequest() : m_lockedMetadataPage(nullptr), m_ref(0), m_canMove(true)
  {
  }

  explicit PendingDownloadRequest(MetadataPage* lockedMetadataPage, int adaptiveLevelToRequest) : m_lockedMetadataPage(lockedMetadataPage), m_adaptiveLevelToRequest(adaptiveLevelToRequest), m_activeTransfer(nullptr), m_ref(1), m_canMove(true)
  {
  }
  explicit PendingDownloadRequest(std::shared_ptr<Request> activeTransfer, std::shared_ptr<ReadChunkTransfer> handler) : m_lockedMetadataPage(nullptr), m_adaptiveLevelToRequest(-1), m_activeTransfer(activeTransfer), m_transferHandle(handler), m_ref(1), m_canMove(true)
  {
  }
};

struct RetryInfo
{
  std::string url;
  std::string contentDispositionName;
  std::vector<std::pair<std::string, std::string>> metaMap;
  std::shared_ptr<std::vector<uint8_t>> data;
  std::function<void(const Request & request, const Error & error)> completedCallback;
};

struct PendingUploadRequest
{
  RetryInfo retryInfo;
  std::shared_ptr<Request> request;
  uint32_t attempts;

  PendingUploadRequest()
    : request()
    , attempts(0)
  {
  }

  void StartNewUpload(IOManager &ioManager,  const std::string &url, const std::string &contentDispositionName,  std::vector<std::pair<std::string, std::string>> &metaMap, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback)
  {
    retryInfo = {url, contentDispositionName, metaMap, data, completedCallback };
    request = ioManager.UploadBinary(url, contentDispositionName, metaMap, data, completedCallback);
    attempts++;
  }

  void Retry(IOManager &ioManager)
  {
    request = ioManager.UploadBinary(retryInfo.url, retryInfo.contentDispositionName, retryInfo.metaMap, retryInfo.data, retryInfo.completedCallback);
    attempts++;
  }
};

class VolumeDataStoreIOManager : public VolumeDataStore, public LayerMetadataContainer
{
  VDS &m_vds;

  mutable std::mutex    m_mutex;

  std::unique_ptr<IOManager>
                        m_ioManager;

  std::condition_variable
                        m_pendingRequestChangedCondition;

  std::unordered_map<VolumeDataChunk, PendingDownloadRequest>
                        m_pendingDownloadRequests;

  std::unordered_map<int64_t, PendingUploadRequest>
                        m_pendingUploadRequests;

  bool                  m_warnedAboutMissingMetadataTag;

  std::unordered_map<std::string, std::unique_ptr<MetadataManager>> m_metadataManagers;

  MetadataManager *GetMetadataMangerForLayer(const std::string &layerName) const;

  bool          SerializeAndUploadLayerStatus(VDS& vds, Error& error);

public:
  CompressionInfo
                GetEffectiveAdaptiveLevel(VolumeDataLayer* volumeDataLayer, WaveletAdaptiveMode waveletAdaptiveMode, float tolerance, float ratio) override;
  bool          PrepareReadChunk(const VolumeDataChunk &volumeDataChunk, int adaptiveLevel, Error &error) override;
  bool          ReadChunk(const VolumeDataChunk& chunk, int adaptiveLevel, std::vector<uint8_t>& serializedData, std::vector<uint8_t>& metadata, CompressionInfo& compressionInfo, Error& error) override;
  bool          CancelReadChunk(const VolumeDataChunk& chunk, Error& error) override;
  bool          WriteChunk(const VolumeDataChunk& chunk, const std::vector<uint8_t>& serializedData, const std::vector<uint8_t>& metadata) override;
  bool          Flush(bool writeUpdatedLayerStatus) override;
  bool          ReadSerializedVolumeDataLayout(std::vector<uint8_t>& serializedVolumeDataLayout, Error &error) override;
  bool          WriteSerializedVolumeDataLayout(const std::vector<uint8_t>& serializedVolumeDataLayout, Error &error) override;
  bool          AddLayer(VolumeDataLayer* volumeDataLayer, int chunkMetadataPageSize) override;
  bool          RemoveLayer(VolumeDataLayer* volumeDataLayer) override { return false; }

  bool          GetMetadataStatus(std::string const &layerName, MetadataStatus &metadataStatus) const override;
  void          SetMetadataStatus(std::string const &layerName, MetadataStatus &metadataStatus, int pageLimit) override;

  VolumeDataStoreIOManager(VDS &vds, IOManager *ioManager);
 ~VolumeDataStoreIOManager();

  void PageTransferCompleted(MetadataPage* metadataPage, const Error &error);
  bool WriteMetadataPage(MetadataPage* metadataPage, const std::vector<uint8_t> &data);

};

}

#endif //VOLUMEDATASTOREIOMANAGER_H
