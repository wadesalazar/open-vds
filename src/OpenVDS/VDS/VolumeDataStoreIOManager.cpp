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

#define _USE_MATH_DEFINES

#include <BulkDataStore/HueBulkDataStoreFileTypes.h>

#include "VolumeDataStoreIOManager.h"

#include "VDS.h"
#include "ParseVDSJson.h"
#include "ParsedMetadata.h"

#include "WaveletTypes.h"

#include <IO/IOManager.h>

#include <fmt/format.h>

#include <stdlib.h>
#include <assert.h>
#include <cmath>

namespace OpenVDS
{

class MetadataPage;

namespace Internal
{

class SyncTransferHandler : public TransferDownloadHandler
{
public:
  void HandleObjectSize(int64_t size) override
  {
  }
  void HandleObjectLastWriteTime(const std::string &lastWriteTimeISO8601) override
  {
  }
  void HandleMetadata(const std::string &key, const std::string &header) override
  {
  }
  void HandleData(std::vector<uint8_t> &&data) override
  {
    *(this->data) = std::move(data);
  }
  void Completed(const Request &request, const Error &error) override
  {
    *(this->error) = error;
  }

  std::vector<uint8_t> *data;
  Error *error;
};

}

class ReadChunkTransfer : public TransferDownloadHandler
{
public:
  ReadChunkTransfer(CompressionInfo compressionInfo, std::vector<uint8_t> const &metadataFromPage)
    : m_compressionInfo(compressionInfo)
    , m_metadataFromPage(metadataFromPage)
  {}

  ~ReadChunkTransfer() override
  {
  }

  void HandleObjectSize(int64_t size) override
  {
  }

  void HandleObjectLastWriteTime(const std::string &lastWriteTimeISO8601) override
  {
  }

  void HandleMetadata(const std::string& key, const std::string& header) override
  {
    static const char vdschunkmetadata[] = "vdschunkmetadata=";
    static const char vdschunkmetadata_old[] = "VDS-Chunk-Metadata=";
    constexpr int vdschunkmetadataKeySize = sizeof(vdschunkmetadata) - 2;
    constexpr int vdschunkmetadataKeySize_old = sizeof(vdschunkmetadata_old) - 2;
    constexpr int vdschunkmetadataValueSize = sizeof(vdschunkmetadata) - 1;
    if (key.size() >= vdschunkmetadataKeySize)
    {
      if (memcmp(key.data() + key.size() - vdschunkmetadataKeySize, vdschunkmetadata, vdschunkmetadataKeySize) == 0)
      {
        if (!Base64Decode(header.data(), (int)header.size(), m_metadataFromHeader))
        {
          m_error.code = -1;
          m_error.string = "Failed to decode chunk metadata";
        }
        return;
      }
    }
    if (key.size() >= vdschunkmetadataKeySize_old)
    {
      if (memcmp(key.data() + key.size() - vdschunkmetadataKeySize_old, vdschunkmetadata_old, vdschunkmetadataKeySize_old) == 0)
      {
        if (!Base64Decode(header.data(), (int)header.size(), m_metadataFromHeader))
        {
          m_error.code = -1;
          m_error.string = "Failed to decode chunk metadata";
        }
        return;
      }
    }
    if (header.size() > vdschunkmetadataValueSize) // This is to support Azure x-ms-properties header for dfs type blobs
    {
      if (memcmp(header.data(), vdschunkmetadata, vdschunkmetadataValueSize) == 0)
      {
        std::vector<uint8_t> tmp;
        tmp.reserve(12);
        if (!Base64Decode(header.data() + vdschunkmetadataValueSize, (int)header.size() - vdschunkmetadataValueSize, tmp))
        {
          m_error.code = -1;
          m_error.string = "Failed to decode chunk metadata";
          return;
        }
        if (!Base64Decode((const char *)tmp.data(), (int)tmp.size(), m_metadataFromHeader))
        {
          m_error.code = -1;
          m_error.string = "Failed to decode chunk metadata";
        }
        return;
      }
    }
  }

  void HandleData(std::vector<uint8_t>&& data) override
  {
    m_data = std::move(data);
  }

  void Completed(const Request &req, const Error & error) override
  {
    m_error = error;
  }
  
  CompressionInfo m_compressionInfo;

  Error m_error;

  std::vector<uint8_t> m_data;
  std::vector<uint8_t> m_metadataFromHeader;
  std::vector<uint8_t> m_metadataFromPage;
};

static bool IsConstantChunkHash(uint64_t chunkHash)
{
  const uint64_t unknownHash = 0;
  const uint64_t noValueHash = ~0ULL;
  const uint64_t constantHash = 0x01010101;

  if (chunkHash == unknownHash || chunkHash == noValueHash || (chunkHash >> 32) == constantHash)
  {
    return true;
  }
  return false;
}

VolumeDataStoreIOManager:: VolumeDataStoreIOManager(VDS &vds, IOManager *ioManager)
  : VolumeDataStore(ioManager->connectionType())
  , m_vds(vds)
  , m_ioManager(ioManager)
  , m_warnedAboutMissingMetadataTag(false)
{
}

VolumeDataStoreIOManager::~VolumeDataStoreIOManager()
{
  assert(m_pendingDownloadRequests.empty());
  assert(m_pendingUploadRequests.empty());
}


bool
VolumeDataStoreIOManager::ReadSerializedVolumeDataLayout(std::vector<uint8_t>& serializedVolumeDataLayout, Error &error)
{
  std::shared_ptr<Internal::SyncTransferHandler> syncTransferHandler = std::make_shared<Internal::SyncTransferHandler>();
  syncTransferHandler->error = &error;
  syncTransferHandler->data = &serializedVolumeDataLayout;
  auto request = m_ioManager->ReadObject("VolumeDataLayout", syncTransferHandler);
  if (!request->WaitForFinish(error))
  {
    error.string = "Error on downloading VolumeDataLayout object: " + error.string;
    return false;
  }
  else if(serializedVolumeDataLayout.empty())
  {
    error.code = -1;
    error.string = "Error on downloading VolumeDataLayout object: Empty response";
    return false;
  }

  std::vector<uint8_t> serializedLayerStatus;
  syncTransferHandler->data = &serializedLayerStatus;
  request = m_ioManager->ReadObject("LayerStatus", syncTransferHandler);
  if (!request->WaitForFinish(error))
  {
    error.string = "Error on downloading LayerStatus object: " + error.string;
    return false;
  }
  else if(serializedLayerStatus.empty())
  {
    error.code = -1;
    error.string = "Error on downloading LayerStatus object: Empty response";
    return false;
  }

  ParseLayerStatus(serializedLayerStatus, m_vds, *this, error);

  return error.code == 0;
}

bool
VolumeDataStoreIOManager::WriteSerializedVolumeDataLayout(const std::vector<uint8_t>& serializedVolumeDataLayout, Error &error)
{
  auto request = m_ioManager->UploadJson("VolumeDataLayout", std::make_shared<std::vector<uint8_t>>(serializedVolumeDataLayout));

  if (!request->WaitForFinish(error))
  {
    error.string = "Error on uploading VolumeDataLayout object: " + error.string;
    return false;
  }

  return true;
}

bool VolumeDataStoreIOManager::SerializeAndUploadLayerStatus(VDS& vds, Error& error)
{
  auto serializedLayerStatus = std::make_shared<std::vector<uint8_t>>(SerializeLayerStatus(vds, *this));

  auto request = m_ioManager->UploadJson("LayerStatus", serializedLayerStatus);

  if (!request->WaitForFinish(error))
  {
    error.string = "Error on uploading LayerStatus object: " + error.string;
    return false;
  }

  return true;
}

bool
VolumeDataStoreIOManager::AddLayer(VolumeDataLayer* volumeDataLayer, int chunkMetadataPageSize)
{
  assert(chunkMetadataPageSize > 0);
  MetadataStatus metadataStatus = {};

  int
    chunkMetadataByteSize = int(sizeof(VDSChunkMetadata));

  if(CompressionMethod_IsWavelet(volumeDataLayer->GetEffectiveCompressionMethod()))
  {
    chunkMetadataByteSize = int(sizeof(uint32_t) + sizeof(VDSWaveletAdaptiveLevelsChunkMetadata));
  }

  metadataStatus.m_chunkIndexCount = (int)volumeDataLayer->GetTotalChunkCount();
  metadataStatus.m_chunkMetadataPageSize = chunkMetadataPageSize;
  metadataStatus.m_chunkMetadataByteSize = chunkMetadataByteSize;
  metadataStatus.m_compressionMethod = volumeDataLayer->GetEffectiveCompressionMethod();
  metadataStatus.m_compressionTolerance = volumeDataLayer->GetEffectiveCompressionTolerance();

  int pageLimit = volumeDataLayer->GetLayout()->GetDimensionality() <= 3 ? 64 : 1024;

  SetMetadataStatus(GetLayerName(*volumeDataLayer), metadataStatus, pageLimit);
  return true;
}

static IORange CalculateRangeHeaderImpl(const ParsedMetadata& parsedMetadata, const MetadataStatus &metadataStatus, int adaptiveLevel)
{
  if (!IsConstantChunkHash(parsedMetadata.m_chunkHash) && !parsedMetadata.m_adaptiveLevels.empty())
  {
    int range = Wavelet_DecodeAdaptiveLevelsMetadata(parsedMetadata.m_chunkSize, adaptiveLevel, parsedMetadata.m_adaptiveLevels.data());
    if (range && range != parsedMetadata.m_chunkSize)
    {
      return { int64_t(0) , int64_t(range - 1 ) };
    }
  }

  return IORange();
}

static inline std::string CreateUrlForChunk(const std::string &layerName, uint64_t chunk)
{
  return fmt::format("{}/{}", layerName, chunk);
}

bool VolumeDataStoreIOManager::PrepareReadChunk(const VolumeDataChunk &chunk, int adaptiveLevel, Error &error)
{
  CompressionInfo compressionInfo;
  ParsedMetadata parsedMetadata;
  unsigned char const* metadataPageEntry;

  IORange ioRange = IORange();

  std::string layerName = GetLayerName(*chunk.layer);
  auto metadataManager = GetMetadataMangerForLayer(layerName);

  std::unique_lock<std::mutex> lock(m_mutex);

  if (metadataManager)
  {
    MetadataStatus const &metadataStatus = metadataManager->GetMetadataStatus();
    compressionInfo = CompressionInfo(metadataStatus.m_compressionMethod, metadataStatus.m_compressionTolerance, adaptiveLevel);

    int pageIndex  = (int)(chunk.index / metadataStatus.m_chunkMetadataPageSize);
    int entryIndex = (int)(chunk.index % metadataStatus.m_chunkMetadataPageSize);

    bool initiateTransfer;

    MetadataPage* metadataPage = metadataManager->LockPage(pageIndex, &initiateTransfer);

    assert(pageIndex == metadataPage->PageIndex());

    if (metadataPage->transferError().code != 0)
    {
      error = metadataPage->transferError();
      metadataManager->UnlockPage(metadataPage);
      return false;
    }

    if (initiateTransfer)
    {
      std::string url = fmt::format("{}/ChunkMetadata/{}", layerName, pageIndex);

      metadataManager->InitiateTransfer(this, metadataPage, url);
    }

    // Check if the page is not valid and we need to add the request later when the metadata page transfer completes
    if (!metadataPage->IsValid())
    {
      auto it = m_pendingDownloadRequests.find(chunk);
      if (it == m_pendingDownloadRequests.end())
      {
        it = m_pendingDownloadRequests.emplace(chunk, PendingDownloadRequest(metadataPage, adaptiveLevel)).first;
      }
      else
      {
        it->second.m_ref++;
        it->second.m_canMove = false;
      }
      return it->second.m_metadataPageRequestError.code == 0;
    }

    lock.unlock();

    metadataPageEntry = metadataManager->GetPageEntry(metadataPage, entryIndex);

    parsedMetadata = ParseMetadata(metadataPageEntry, metadataStatus.m_chunkMetadataByteSize, error);
    if (error.code)
    {
      return false;
    }
    
    metadataManager->UnlockPage(metadataPage);

    ioRange = CalculateRangeHeaderImpl(parsedMetadata, metadataStatus, adaptiveLevel);

    lock.lock();
  }

  if (m_pendingDownloadRequests.find(chunk) == m_pendingDownloadRequests.end())
  {
    std::string url = CreateUrlForChunk(layerName, chunk.index);
    auto transferHandler = std::make_shared<ReadChunkTransfer>(compressionInfo, (metadataManager != nullptr) ? parsedMetadata.CreateChunkMetadata() : std::vector<uint8_t>());
    m_pendingDownloadRequests[chunk] = PendingDownloadRequest(m_ioManager->ReadObject(url, transferHandler, ioRange), transferHandler);
  }
  else
  {
    m_pendingDownloadRequests[chunk].m_ref++;
    m_pendingDownloadRequests[chunk].m_canMove = false;
  }

  return true;
}

bool VolumeDataStoreIOManager::ReadChunk(const VolumeDataChunk &chunk, int adaptiveLevel, std::vector<uint8_t> &serializedData, std::vector<uint8_t> &metadata, CompressionInfo &compressionInfo, Error &error)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  auto pendingRequestIterator = m_pendingDownloadRequests.find(chunk);

  if(pendingRequestIterator == m_pendingDownloadRequests.end())
  {
    error.code = -1;
    error.string = "Missing request for chunk: " + std::to_string(chunk.index);
    compressionInfo = CompressionInfo();
    return false;
  }
  PendingDownloadRequest& pendingRequest = pendingRequestIterator->second;

  if (!pendingRequest.m_activeTransfer)
  {
    m_pendingRequestChangedCondition.wait(lock, [&pendingRequest]{ return !pendingRequest.m_lockedMetadataPage || pendingRequest.m_metadataPageRequestError.code != 0; });
  }

  auto activeTransfer = pendingRequest.m_activeTransfer;
  auto transferHandler = pendingRequest.m_transferHandle;

  if (pendingRequest.m_metadataPageRequestError.code != 0)
  {
    error = pendingRequest.m_metadataPageRequestError;
    compressionInfo = CompressionInfo();
    if (--pendingRequest.m_ref == 0)
    {
      m_pendingDownloadRequests.erase(pendingRequestIterator);
    }
    return false;
  }

  if(!activeTransfer)
  {
    error.code = -1;
    error.string = "Failed to read metadata for chunk: " + std::to_string(chunk.index);
    compressionInfo = CompressionInfo();
    if (--pendingRequest.m_ref == 0)
    {
      m_pendingDownloadRequests.erase(pendingRequestIterator);
    }
    return false;
  }

  lock.unlock();

  if (!activeTransfer->WaitForFinish(error))
    return false;

  lock.lock();

  bool moveData = pendingRequestIterator->second.m_canMove;
  if (--pendingRequestIterator->second.m_ref == 0)
  {
    m_pendingDownloadRequests.erase(pendingRequestIterator);
  }

  lock.unlock();

  if (transferHandler->m_error.code)
  {
    error = transferHandler->m_error;
    return false;
  }

  if (transferHandler->m_data.size())
  {
    m_globalStateVds.addDownload(transferHandler->m_data.size());
    if (moveData)
      serializedData = std::move(transferHandler->m_data);
    else
      serializedData = transferHandler->m_data;
  }

  if(!transferHandler->m_metadataFromHeader.empty())
  {
    if(!transferHandler->m_metadataFromPage.empty() && transferHandler->m_metadataFromPage != transferHandler->m_metadataFromHeader)
    {
      error.string = fmt::format("Inconsistent metadata for chunk {}", CreateUrlForChunk(GetLayerName(*chunk.layer), chunk.index));
      error.code = -1;
      compressionInfo = CompressionInfo();
      return false;
    }

    if (moveData)
      metadata = std::move(transferHandler->m_metadataFromHeader);
    else
      metadata = transferHandler->m_metadataFromHeader;
  }
  else if(!transferHandler->m_metadataFromPage.empty())
  {
    if (!m_warnedAboutMissingMetadataTag) // Log once and move along.
    {
      fmt::print(stderr, "Dataset has missing metadata tags, degraded data verification, reverting to metadata pages");
      m_warnedAboutMissingMetadataTag = true;
    }

    if (moveData)
      metadata = std::move(transferHandler->m_metadataFromPage);
    else
      metadata = transferHandler->m_metadataFromPage;
  }
  else
  {
    error.string = fmt::format("Missing metadata for chunk {}", CreateUrlForChunk(GetLayerName(*chunk.layer), chunk.index));
    error.code = -1;
    compressionInfo = CompressionInfo();
    return false;
  }

  compressionInfo = transferHandler->m_compressionInfo;
  return true;
}

bool VolumeDataStoreIOManager::CancelReadChunk(const VolumeDataChunk& chunk, Error& error)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  auto pendingRequestIterator = m_pendingDownloadRequests.find(chunk);

  if(pendingRequestIterator == m_pendingDownloadRequests.end())
  {
    error.code = -1;
    error.string = "Missing request for chunk: " + std::to_string(chunk.index);
    return false;
  }

  PendingDownloadRequest& pendingRequest = pendingRequestIterator->second;

  if (--pendingRequest.m_ref == 0)
  {
    if(pendingRequest.m_activeTransfer)
    {
      pendingRequest.m_activeTransfer->Cancel();
    }

    auto lockedMetadataPage = pendingRequest.m_lockedMetadataPage;

    m_pendingDownloadRequests.erase(pendingRequestIterator);

    lock.unlock();

    if (lockedMetadataPage)
    {
      lockedMetadataPage->GetManager()->UnlockPage(lockedMetadataPage);
    }
  }

  return true;
}

void VolumeDataStoreIOManager::PageTransferCompleted(MetadataPage* metadataPage, const Error &error)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  for(auto &pendingRequestKeyValuePair : m_pendingDownloadRequests)
  {
    VolumeDataChunk const volumeDataChunk = pendingRequestKeyValuePair.first;
    PendingDownloadRequest &pendingRequest = pendingRequestKeyValuePair.second;

    if(pendingRequest.m_lockedMetadataPage == metadataPage)
    {
      MetadataManager *metadataManager = metadataPage->GetManager();

      MetadataStatus const &metadataStatus = metadataManager->GetMetadataStatus();
      CompressionInfo compressionInfo = CompressionInfo(metadataStatus.m_compressionMethod, metadataStatus.m_compressionTolerance, pendingRequest.m_adaptiveLevelToRequest);

      int pageIndex  = (int)(volumeDataChunk.index / metadataStatus.m_chunkMetadataPageSize);
      int entryIndex = (int)(volumeDataChunk.index % metadataStatus.m_chunkMetadataPageSize);

      (void)pageIndex; // Silence gcc warning in release builds
      assert(pageIndex == metadataPage->PageIndex());

      if (error.code != 0)
        pendingRequest.m_metadataPageRequestError = error;

      if (metadataPage->IsValid())
      {
        uint8_t const *metadata = metadataManager->GetPageEntry(metadataPage, entryIndex);

        Error metaParseError;
        ParsedMetadata parsedMetadata = ParseMetadata(metadata, metadataManager->GetMetadataStatus().m_chunkMetadataByteSize, metaParseError);
        if (metaParseError.code)
        {
          pendingRequest.m_metadataPageRequestError = metaParseError;
        }
        else
        {
          IORange ioRange = CalculateRangeHeaderImpl(parsedMetadata, metadataManager->GetMetadataStatus(), pendingRequest.m_adaptiveLevelToRequest);

          std::string url = CreateUrlForChunk(metadataManager->LayerUrlStr(), volumeDataChunk.index);
          auto transferHandler = std::make_shared<ReadChunkTransfer>(compressionInfo, parsedMetadata.CreateChunkMetadata());
          pendingRequest.m_activeTransfer = m_ioManager->ReadObject(url, transferHandler, ioRange);
          pendingRequest.m_transferHandle = transferHandler;
        }
      }

      // Unlock the metadata page, even if invalid, to avoid infinite wait for pendingRequest.m_lockedMetadataPage to change in ReadChunk()
      metadataManager->UnlockPage(metadataPage);
      pendingRequest.m_lockedMetadataPage = nullptr;
    }
  }
  m_pendingRequestChangedCondition.notify_all();
}

bool VolumeDataStoreIOManager::WriteMetadataPage(MetadataPage* metadataPage, const std::vector<uint8_t> &data)
{
  assert(metadataPage->IsValid());

  MetadataManager *metadataManager = metadataPage->GetManager();

  std::string url = fmt::format("{}/ChunkMetadata/{}", metadataManager->LayerUrlStr(), metadataPage->PageIndex());

  std::string contentDispositionName = fmt::format("{}_ChunkMetadata_{}", metadataManager->LayerUrlStr(), metadataPage->PageIndex());

  Error error;

  auto req = m_ioManager->UploadBinary(url, contentDispositionName, std::vector<std::pair<std::string, std::string>>(), std::make_shared<std::vector<uint8_t>>(data));

  bool success = req->WaitForFinish(error);

  if(!success)
  {
    m_vds.accessManager->AddUploadError(error, url);
  }

  return success;
}

static int64_t CreateUploadJobId()
{
  static std::atomic< std::int64_t > id(0);
  return --id;
}

CompressionInfo VolumeDataStoreIOManager::GetEffectiveAdaptiveLevel(VolumeDataLayer * volumeDataLayer, WaveletAdaptiveMode waveletAdaptiveMode, float tolerance, float ratio)
{
  CompressionMethod
    compressionMethod = CompressionMethod::None;

  float
    compressionTolerance = 0.0f;
    compressionTolerance = 0.0f;

  int
    adaptiveLevel = (waveletAdaptiveMode == WaveletAdaptiveMode::BestQuality) ? -1 : 0;

  std::string layerName = GetLayerName(*volumeDataLayer);
  auto metadataManager = GetMetadataMangerForLayer(layerName);

  if(metadataManager)
  {
    MetadataStatus const &metadataStatus = metadataManager->GetMetadataStatus();

    compressionMethod = metadataStatus.m_compressionMethod;
    compressionTolerance = metadataStatus.m_compressionTolerance;

    if(waveletAdaptiveMode == WaveletAdaptiveMode::Tolerance)
    {
      adaptiveLevel = Wavelet_GetEffectiveWaveletAdaptiveLoadLevel(tolerance, metadataStatus.m_compressionTolerance);
    }
    else if(waveletAdaptiveMode == WaveletAdaptiveMode::Ratio)
    {
      adaptiveLevel = Wavelet_GetEffectiveWaveletAdaptiveLoadLevel(ratio, metadataStatus.m_adaptiveLevelSizes, metadataStatus.m_uncompressedSize);
    }
    else
    {
      assert(waveletAdaptiveMode == WaveletAdaptiveMode::BestQuality && "Illegal WaveletAdaptiveMode");
    }
  }

  return CompressionInfo(compressionMethod, compressionTolerance, adaptiveLevel);
}

bool VolumeDataStoreIOManager::WriteChunk(const VolumeDataChunk& chunk, const std::vector<uint8_t>& serializedData, const std::vector<uint8_t>& metadata)
{
  Error error;
  std::string layerName = GetLayerName(*chunk.layer);
  std::string url = CreateUrlForChunk(layerName, chunk.index);
  std::string contentDispositionName = layerName + "_" + std::to_string(chunk.index);
  std::shared_ptr<std::vector<uint8_t>> to_write = std::make_shared<std::vector<uint8_t>>(serializedData);

  auto metadataManager = GetMetadataMangerForLayer(layerName);
  MetadataStatus metadataStatus = metadataManager->GetMetadataStatus();

  int pageIndex  = (int)(chunk.index / metadataStatus.m_chunkMetadataPageSize);
  int entryIndex = (int)(chunk.index % metadataStatus.m_chunkMetadataPageSize);

  std::vector<uint8_t> indexEntry(metadataStatus.m_chunkMetadataByteSize);

  // If adaptive wavelet, we store the size of the serialized chunk in front of the chunk metadata
  if(metadataStatus.m_chunkMetadataByteSize == sizeof(uint32_t) + sizeof(VDSWaveletAdaptiveLevelsChunkMetadata))
  {
    uint32_t
      serializedSize = (uint32_t)serializedData.size();
    indexEntry[0] = (serializedSize >>  0) & 0xff;
    indexEntry[1] = (serializedSize >>  8) & 0xff;
    indexEntry[2] = (serializedSize >> 16) & 0xff;
    indexEntry[3] = (serializedSize >> 24) & 0xff;
    std::copy(metadata.begin(), metadata.end(), indexEntry.begin() + 4);
  }
  else
  {
    indexEntry = metadata;
  }

  bool initiateTransfer;

  MetadataPage* lockedMetadataPage = metadataManager->LockPage(pageIndex, &initiateTransfer);

  // Newly created page
  if(initiateTransfer)
  {
    metadataManager->InitPage(lockedMetadataPage);
  }

  int64_t jobId = CreateUploadJobId();

  auto completedCallback = [this, chunk, indexEntry, metadataManager, lockedMetadataPage, entryIndex, jobId](const Request &request, const Error &error)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    if(error.code != 0)
    {
      auto &uploadRequest = m_pendingUploadRequests[jobId];
      if (uploadRequest.attempts < 2)
      {
        uploadRequest.Retry(*m_ioManager);
        return;
      }
      else
      {
        m_vds.accessManager->AddUploadError(error, request.GetObjectName());
      }
    }
    else
    {
      //Update MetadataStatus
      MetadataStatus metadataStatus = metadataManager->GetMetadataStatus();

      std::vector<uint8_t> oldIndexEntry(metadataStatus.m_chunkMetadataByteSize);
      metadataManager->SetPageEntry(lockedMetadataPage, entryIndex, indexEntry.data(), (int)indexEntry.size(), oldIndexEntry.data());

      if(metadataStatus.m_chunkMetadataByteSize == sizeof(uint32_t) + sizeof(VDSWaveletAdaptiveLevelsChunkMetadata))
      {
        int size[DataBlock::Dimensionality_Max];
        chunk.layer->GetChunkVoxelSize(chunk.index, size);
        int64_t uncompressedSize = GetByteSize(size, chunk.layer->GetFormat(), chunk.layer->GetComponents());

        // If adaptive wavelet, we store the size of the serialized chunk in front of the chunk metadata
        uint32_t const &newSize = *reinterpret_cast<const uint32_t *>(indexEntry.data());
        VDSWaveletAdaptiveLevelsChunkMetadata const &newMetadata = *reinterpret_cast<const VDSWaveletAdaptiveLevelsChunkMetadata *>(indexEntry.data() + sizeof(uint32_t));
        uint32_t const &oldSize = *reinterpret_cast<const uint32_t *>(oldIndexEntry.data());
        VDSWaveletAdaptiveLevelsChunkMetadata const &oldMetadata = *reinterpret_cast<const VDSWaveletAdaptiveLevelsChunkMetadata *>(oldIndexEntry.data() + sizeof(uint32_t));

        if (newMetadata.m_hash != VolumeDataHash::UNKNOWN && !VolumeDataHash(newMetadata.m_hash).IsConstant())
        {
          metadataManager->UpdateMetadataStatus(uncompressedSize, newSize, false, newMetadata.m_levels);
        }

        if (oldMetadata.m_hash != VolumeDataHash::UNKNOWN && !VolumeDataHash(oldMetadata.m_hash).IsConstant())
        {
          metadataManager->UpdateMetadataStatus(uncompressedSize, oldSize, true, oldMetadata.m_levels);
        }
      }

    }
    m_vds.volumeDataLayout->ChangePendingWriteRequestCount(-1);

    m_pendingUploadRequests.erase(jobId);

    lockedMetadataPage->GetManager()->UnlockPage(lockedMetadataPage);
  };

  std::vector<char> base64Hash;
  Base64Encode(metadata.data(), (int)metadata.size(), base64Hash);
  std::vector<std::pair<std::string, std::string>> meta_map;
  meta_map.emplace_back("vdschunkmetadata", std::string(base64Hash.begin(), base64Hash.end()));
  // add new pending upload request
  m_vds.volumeDataLayout->ChangePendingWriteRequestCount(1);
  std::unique_lock<std::mutex> lock(m_mutex);
  m_pendingUploadRequests[jobId].StartNewUpload(*m_ioManager, url, contentDispositionName, meta_map, to_write, completedCallback);
  return true;
}

bool VolumeDataStoreIOManager::Flush(bool writeUpdatedLayerStatus)
{
  while(true)
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    if(m_pendingUploadRequests.empty()) break;
    std::shared_ptr<Request> request = m_pendingUploadRequests.begin()->second.request;
    lock.unlock();
    Error error;
    if (!request->WaitForFinish(error))
    {
      m_vds.accessManager->AddUploadError(error, request->GetObjectName());
    }
  }

  for(auto it = m_metadataManagers.begin(); it != m_metadataManagers.end(); ++it)
  {
    auto metadataManager = it->second.get();

    metadataManager->UploadDirtyPages(this);
  }

  if(writeUpdatedLayerStatus)
  {
    Error error;
    SerializeAndUploadLayerStatus(m_vds, error);

    if(error.code != 0)
    {
      m_vds.accessManager->AddUploadError(error, "LayerStatus");
      return false;
    }
  }

  return true;
}

MetadataManager *
VolumeDataStoreIOManager::GetMetadataMangerForLayer(const std::string &layerName) const
{
  std::unique_lock<std::mutex> lock(m_mutex);

  auto it = m_metadataManagers.find(layerName);
  return it != m_metadataManagers.end() ? it->second.get() : nullptr;
}

bool
VolumeDataStoreIOManager::GetMetadataStatus(std::string const &layerName, MetadataStatus &metadataStatus) const
{
  std::unique_lock<std::mutex> lock(m_mutex);

  auto it = m_metadataManagers.find(layerName);

  if (it != m_metadataManagers.end())
  {
    metadataStatus = it->second->GetMetadataStatus();
    return true;
  }
  else
  {
    metadataStatus = MetadataStatus();
    return false;
  }
}

void
VolumeDataStoreIOManager::SetMetadataStatus(std::string const &layerName, MetadataStatus &metadataStatus, int pageLimit)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  if (m_metadataManagers.find(layerName) == m_metadataManagers.end())
  {
    m_metadataManagers.insert(std::make_pair(layerName, std::unique_ptr<MetadataManager>(new MetadataManager(m_ioManager.get(), layerName, metadataStatus, pageLimit))));
  }
}

}
