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

#include "VolumeDataStoreIOManager.h"

#include "VDS.h"
#include "ParseVDSJson.h"
#include "ParsedMetadata.h"

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
  ReadChunkTransfer(CompressionMethod compressionMethod, std::vector<uint8_t> const &metadataFromPage, int adaptiveLevel)
    : m_compressionMethod(compressionMethod)
    , m_adaptiveLevel(adaptiveLevel)
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
  
  CompressionMethod m_compressionMethod;
  int m_adaptiveLevel;

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

static int GetEffectiveAdaptiveLoadLevel(float effectiveCompressionTolerance, float compressionTolerance)
{
//  assert(effectiveCompressionTolerance >= adaptiveToleranceMin);
//  assert(compressionTolerance >= adaptiveToleranceMin);

  int adaptiveLoadLevel = (int)(log(effectiveCompressionTolerance / compressionTolerance) / M_LN2);

  return std::max(0, adaptiveLoadLevel);
}

static int GetEffectiveAdaptiveLevel(AdaptiveMode adaptiveMode, float desiredTolerance, float desiredRatio, float remoteTolerance, int64_t const adaptiveLevelSizes[WAVELET_ADAPTIVE_LEVELS], int64_t uncompressedSize)
{
  if (adaptiveMode == AdaptiveMode_BestQuality)
  {
    return -1;
  }
  else if (adaptiveMode == AdaptiveMode_Ratio && desiredRatio <= 1.0f)
  {
    return 0;
  }

  int level = 0;

  if (adaptiveMode == AdaptiveMode_Tolerance)
  {
    level = GetEffectiveAdaptiveLoadLevel(desiredTolerance, remoteTolerance);
  }
  else if (adaptiveMode == AdaptiveMode_Ratio)
  {
    // Matches HueVolumeDataStoreVersion4_c::GetEffectiveAdaptiveLevel
    while (level + 1 < WAVELET_ADAPTIVE_LEVELS)
    {
      if (adaptiveLevelSizes[level + 1] == 0 || ((float)uncompressedSize / (float)adaptiveLevelSizes[level + 1]) > desiredRatio)
      {
        break;
      }

      level++;
    }
  }
  else
  {
    assert(0 && "Unknown compression mode");
  }

  return level;
}

static int WaveletAdaptiveLevelsMetadataDecode(uint64_t totalSize, int targetLevel, uint8_t const *levels)
{
  assert(targetLevel >= -1 && targetLevel < WAVELET_ADAPTIVE_LEVELS);

  int remainingSize = (int)totalSize;

  for (int level = 0; level <= targetLevel; level++)
  {
    if (levels[level] == 0)
    {
      break;
    }
    remainingSize = (int)((uint64_t)remainingSize * levels[level] / 255);
  }

  return remainingSize;
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
  request->WaitForFinish();
  if (!request->IsSuccess(error))
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
  request->WaitForFinish();
  if (!request->IsSuccess(error))
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

  request->WaitForFinish();

  if (!request->IsSuccess(error))
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

  request->WaitForFinish();

  if (!request->IsSuccess(error))
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
  metadataStatus.m_chunkIndexCount = (int)volumeDataLayer->GetTotalChunkCount();
  metadataStatus.m_chunkMetadataPageSize = chunkMetadataPageSize;
  metadataStatus.m_chunkMetadataByteSize = sizeof(int64_t);
  metadataStatus.m_compressionMethod = volumeDataLayer->GetEffectiveCompressionMethod();
  metadataStatus.m_compressionTolerance = volumeDataLayer->GetEffectiveCompressionTolerance();

  int pageLimit = volumeDataLayer->GetLayout()->GetDimensionality() <= 3 ? 64 : 1024;

  SetMetadataStatus(GetLayerName(*volumeDataLayer), metadataStatus, pageLimit);
  return true;
}

static IORange CalculateRangeHeaderImpl(const ParsedMetadata& parsedMetadata, const MetadataStatus &metadataStatus, int * const adaptiveLevel)
{
  if (IsConstantChunkHash(parsedMetadata.m_chunkHash))
  {
    *adaptiveLevel = -1;
  }
  else if (parsedMetadata.m_adaptiveLevels.empty())
  {
    *adaptiveLevel = -1;
  }
  else
  {
    *adaptiveLevel = GetEffectiveAdaptiveLevel(AdaptiveMode_BestQuality, 0.01f, 1.0f , metadataStatus.m_compressionTolerance, metadataStatus.m_adaptiveLevelSizes, metadataStatus.m_uncompressedSize);

    int range = WaveletAdaptiveLevelsMetadataDecode(parsedMetadata.m_chunkSize, *adaptiveLevel, parsedMetadata.m_adaptiveLevels.data());
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

bool VolumeDataStoreIOManager::PrepareReadChunk(const VolumeDataChunk &chunk, Error &error)
{
  CompressionMethod compressionMethod = CompressionMethod::Wavelet;
  ParsedMetadata parsedMetadata;
  unsigned char const* metadataPageEntry;
  int adaptiveLevel = -1;

  IORange ioRange = IORange();

  std::string layerName = GetLayerName(*chunk.layer);
  auto metadataManager = GetMetadataMangerForLayer(layerName);

  std::unique_lock<std::mutex> lock(m_mutex);

  if (metadataManager)
  {
    int pageIndex  = (int)(chunk.index / metadataManager->GetMetadataStatus().m_chunkMetadataPageSize);
    int entryIndex = (int)(chunk.index % metadataManager->GetMetadataStatus().m_chunkMetadataPageSize);

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
        it = m_pendingDownloadRequests.emplace(chunk, PendingDownloadRequest(metadataPage)).first;
      }
      else
      {
        it->second.m_ref++;
        it->second.m_canMove = false;
      }
      return it->second.m_metadataPageRequestError.code == 0;
    }

    lock.unlock();

    compressionMethod = metadataManager->GetMetadataStatus().m_compressionMethod;

    metadataPageEntry = metadataManager->GetPageEntry(metadataPage, entryIndex);

    parsedMetadata = ParseMetadata(metadataPageEntry, metadataManager->GetMetadataStatus().m_chunkMetadataByteSize, error);
    if (error.code)
    {
      return false;
    }
    
    metadataManager->UnlockPage(metadataPage);

    ioRange = CalculateRangeHeaderImpl(parsedMetadata, metadataManager->GetMetadataStatus(), &adaptiveLevel);

    lock.lock();
  }

  if (m_pendingDownloadRequests.find(chunk) == m_pendingDownloadRequests.end())
  {
    std::string url = CreateUrlForChunk(layerName, chunk.index);
    auto transferHandler = std::make_shared<ReadChunkTransfer>(compressionMethod, (metadataManager != nullptr) ? parsedMetadata.CreateChunkMetadata() : std::vector<uint8_t>(), adaptiveLevel);
    m_pendingDownloadRequests[chunk] = PendingDownloadRequest(m_ioManager->ReadObject(url, transferHandler, ioRange), transferHandler);
  }
  else
  {
    m_pendingDownloadRequests[chunk].m_ref++;
    m_pendingDownloadRequests[chunk].m_canMove = false;
  }

  return true;
}

bool VolumeDataStoreIOManager::ReadChunk(const VolumeDataChunk &chunk, std::vector<uint8_t> &serializedData, std::vector<uint8_t> &metadata, CompressionInfo &compressionInfo, Error &error)
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

  if (!activeTransfer->IsDone())
  {
    lock.unlock();

    activeTransfer->WaitForFinish();

    lock.lock();
    pendingRequestIterator = m_pendingDownloadRequests.find(chunk);
    if (pendingRequestIterator == m_pendingDownloadRequests.end())
    {
      error.code = -1;
      error.string = fmt::format("Request removed while processing: {}\n", chunk.index);
      return false;
    }
  }

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

  compressionInfo = CompressionInfo(transferHandler->m_compressionMethod, transferHandler->m_adaptiveLevel);
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

      int32_t pageIndex = (int)(volumeDataChunk.index / metadataManager->GetMetadataStatus().m_chunkMetadataPageSize);
      int32_t entryIndex = (int)(volumeDataChunk.index % metadataManager->GetMetadataStatus().m_chunkMetadataPageSize);

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
          int adaptiveLevel;

          IORange ioRange = CalculateRangeHeaderImpl(parsedMetadata, metadataManager->GetMetadataStatus(), &adaptiveLevel);

          std::string url = CreateUrlForChunk(metadataManager->LayerUrlStr(), volumeDataChunk.index);
          auto transferHandler = std::make_shared<ReadChunkTransfer>(metadataManager->GetMetadataStatus().m_compressionMethod, parsedMetadata.CreateChunkMetadata(), adaptiveLevel);
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

  req->WaitForFinish();
  bool success = req->IsSuccess(error);

  if(error.code != 0)
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

CompressionInfo VolumeDataStoreIOManager::GetCompressionInfoForChunk(std::vector<uint8_t>& metadata, const VolumeDataChunk &volumeDataChunk, Error &error)
{
  return CompressionInfo();
}

bool VolumeDataStoreIOManager::WriteChunk(const VolumeDataChunk& chunk, const std::vector<uint8_t>& serializedData, const std::vector<uint8_t>& metadata)
{
  Error error;
  std::string layerName = GetLayerName(*chunk.layer);
  std::string url = CreateUrlForChunk(layerName, chunk.index);
  std::string contentDispositionName = layerName + "_" + std::to_string(chunk.index);
  std::shared_ptr<std::vector<uint8_t>> to_write = std::make_shared<std::vector<uint8_t>>(serializedData);

  auto metadataManager = GetMetadataMangerForLayer(layerName);

  int pageIndex  = (int)(chunk.index / metadataManager->GetMetadataStatus().m_chunkMetadataPageSize);
  int entryIndex = (int)(chunk.index % metadataManager->GetMetadataStatus().m_chunkMetadataPageSize);

  bool initiateTransfer;

  MetadataPage* lockedMetadataPage = metadataManager->LockPage(pageIndex, &initiateTransfer);

  // Newly created page
  if(initiateTransfer)
  {
    metadataManager->InitPage(lockedMetadataPage);
  }

  int64_t jobId = CreateUploadJobId();

  auto completedCallback = [this, metadata, metadataManager, lockedMetadataPage, entryIndex, jobId](const Request &request, const Error &error)
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
      metadataManager->SetPageEntry(lockedMetadataPage, entryIndex, metadata.data(), (int)metadata.size());
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
    request->WaitForFinish();
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
