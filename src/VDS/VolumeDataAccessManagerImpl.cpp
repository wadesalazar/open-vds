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
#include "VolumeDataAccessManagerImpl.h"

#include "VDS.h"
#include "VolumeDataPageAccessorImpl.h"
#include "ValueConversion.h"
#include "VolumeSampler.h"
#include "ParseVDSJson.h"
#include "VolumeDataStore.h"
#include "VolumeDataHash.h"
#include "VolumeDataLayoutImpl.h"

#include <cmath>
#include <algorithm>
#include <inttypes.h>
#include <assert.h>
#include <atomic>
#include <fmt/format.h>

namespace OpenVDS
{

struct ParsedMetadata
{
  ParsedMetadata()
    : m_chunkHash(0)
    , m_chunkSize(0)
  {}

  uint64_t m_chunkHash;
  
  int32_t m_chunkSize;

  std::vector<uint8_t> m_adaptiveLevels;
};


static ParsedMetadata ParseMetadata(int metadataByteSize, unsigned char const *metadata)
{
  ParsedMetadata parsedMetadata;

  if (metadataByteSize == 4 + 24)
  {
    parsedMetadata.m_chunkSize = *reinterpret_cast<int32_t const *>(metadata);

    parsedMetadata.m_chunkHash = *reinterpret_cast<uint64_t const *>(metadata + 4);

    parsedMetadata.m_adaptiveLevels.resize(WAVELET_ADAPTIVE_LEVELS);

    memcpy(parsedMetadata.m_adaptiveLevels.data(), metadata + 4 + 8, WAVELET_ADAPTIVE_LEVELS);
  }
  else if (metadataByteSize == 8)
  {
    parsedMetadata.m_chunkHash = *reinterpret_cast<uint64_t const *>(metadata);
  }
  else
  {
    throw std::runtime_error(std::string(" Unsupported chunkMetadataByteSize: ") + std::to_string(metadataByteSize));
  }

  return parsedMetadata;
}

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
    return { size_t(0) , size_t(range - 1 ) };
    }
  }

  return { 0 , 0 };
}

static VolumeDataLayer *GetVolumeDataLayer(VolumeDataLayoutImpl const *layout, DimensionsND dimension, int channel, int lod, bool isAllowFailure)
{
  if(!layout)
  {
    throw std::runtime_error("Volume data layout is NULL, this is usually because the VDS setup is invalid");
    return nullptr;
  }

  if(channel > layout->GetChannelCount())
  {
    throw std::runtime_error("Specified channel doesn't exist");
    return nullptr;
  }

  VolumeDataLayer *layer = layout->GetBaseLayer(DimensionGroupUtil::GetDimensionGroupFromDimensionsND(dimension), channel);

  if(!layer && !isAllowFailure)
  {
    throw std::runtime_error("Specified dimension group doesn't exist");
    return nullptr;
  }

  while(layer && layer->GetLOD() < lod)
  {
    layer = layer->GetParentLayer();
  }

  if((!layer || layer->GetLayerType() == VolumeDataLayer::Virtual) && !isAllowFailure)
  {
    throw std::runtime_error("Specified LOD doesn't exist");
  }

  assert(layer || isAllowFailure);
  return (layer && layer->GetLayerType() != VolumeDataLayer::Virtual) ? layer : nullptr;
}

VolumeDataAccessManagerImpl::VolumeDataAccessManagerImpl(VDS &vds)
  : m_vds(vds)
  , m_ioManager(vds.IoManager.get())
  , m_currentErrorIndex(0)
  , m_requestProcessor(*this)
{
}

VolumeDataAccessManagerImpl::~VolumeDataAccessManagerImpl()
{
  if (m_uploadErrors.size())
  {
    fprintf(stderr, "VolumeDataAccessManager destructor: there where upload errors\n");
  }
}

VolumeDataLayout const* VolumeDataAccessManagerImpl::GetVolumeDataLayout() const
{
  return m_vds.VolumeDataLayout.get();
}

VolumeDataLayoutImpl const* VolumeDataAccessManagerImpl::GetVolumeDataLayoutImpl() const
{
  return m_vds.VolumeDataLayout.get();
}

VolumeDataPageAccessor* VolumeDataAccessManagerImpl::CreateVolumeDataPageAccessor(VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int maxPages, AccessMode accessMode)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  VolumeDataLayer *layer = GetVolumeDataLayer(static_cast<VolumeDataLayoutImpl const *>(volumeDataLayout), dimensionsND, channel, lod, true);
  if (!layer)
    return nullptr;

  if(accessMode == VolumeDataAccessManager::AccessMode_Create)
  {
    layer->GetProduceStatus(VolumeDataLayer::ProduceStatus_Normal);
    MetadataStatus metadataStatus = {};
    metadataStatus.m_chunkMetadataPageSize = 1024;
    metadataStatus.m_chunkMetadataByteSize = sizeof(int64_t);
    metadataStatus.m_compressionMethod = layer->GetEffectiveCompressionMethod();
    metadataStatus.m_compressionTolerance = layer->GetEffectiveCompressionTolerance();
    CreateMetadataManager(m_vds, GetLayerName(*layer), metadataStatus);
  }
  else if (layer->GetProduceStatus() == VolumeDataLayer::ProduceStatus_Unavailable)
  {
    throw std::runtime_error("The accessed dimension group or channel is unavailable (check produce status on VDS before accessing data)");
  }

  VolumeDataPageAccessorImpl *accessor = new VolumeDataPageAccessorImpl(this, layer, maxPages, accessMode != VolumeDataAccessManager::AccessMode_ReadOnly);
  m_volumeDataPageAccessorList.InsertLast(accessor);
  return accessor;
}

void  VolumeDataAccessManagerImpl::DestroyVolumeDataPageAccessor(VolumeDataPageAccessor* volumeDataPageAccessor)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  VolumeDataPageAccessorImpl *pageAccessorImpl = static_cast<VolumeDataPageAccessorImpl *>(volumeDataPageAccessor);
  m_volumeDataPageAccessorList.Remove(pageAccessorImpl);
  delete  pageAccessorImpl;
}

void VolumeDataAccessManagerImpl::DestroyVolumeDataAccessor(VolumeDataAccessor* accessor)
{
  //TODO
  assert(false);
  //VolumeDataAccessorBase *imple = static_cast<VolumeDataAccessorBase *>(accessor);
}

VolumeDataAccessor* VolumeDataAccessManagerImpl::CloneVolumeDataAccessor(VolumeDataAccessor const& accessor)
{
  assert(false);
  return nullptr;
}

bool VolumeDataAccessManagerImpl::IsCompleted(int64_t requestID)
{
  return m_requestProcessor.IsCompleted(requestID);
}
bool VolumeDataAccessManagerImpl::IsCanceled(int64_t requestID)
{
  return m_requestProcessor.IsCanceled(requestID);
}
bool VolumeDataAccessManagerImpl::WaitForCompletion(int64_t requestID, int millisecondsBeforeTimeout)
{
  return m_requestProcessor.WaitForCompletion(requestID, millisecondsBeforeTimeout);
}
void VolumeDataAccessManagerImpl::Cancel(int64_t requestID)
{
  m_requestProcessor.Cancel(requestID);
}
float VolumeDataAccessManagerImpl::GetCompletionFactor(int64_t requestID)
{
  return 0.0f;
}

static MetadataManager *GetMetadataMangerForLayer(LayerMetadataContainer const &container, const std::string &layer)
{
  std::unique_lock<std::mutex> lock(container.mutex);

  MetadataManager *metadataManager = nullptr;
  auto metadataManagerIterator = container.managers.find(layer);

  if(metadataManagerIterator != container.managers.end())
  {
    metadataManager = metadataManagerIterator->second.get();
  }

  return metadataManager;
}

static inline std::string CreateUrlForChunk(const std::string &layerName, uint64_t chunk)
{
  return layerName + "/" + std::to_string(chunk);
}

bool VolumeDataAccessManagerImpl::PrepareReadChunkData(const VolumeDataChunk &chunk, bool verbose, Error &error)
{
  std::string layerName = GetLayerName(*chunk.Layer);
  auto metadataManager = GetMetadataMangerForLayer(m_vds.LayerMetadataContainer, layerName);
  //do fallback
  if (!metadataManager)
  {
    error.Code = -1;
    error.String = "No metdadataManager";
    return false;
  }
  
  int pageIndex  = (int)(chunk.Index / metadataManager->GetMetadataStatus().m_chunkMetadataPageSize);
  int entryIndex = (int)(chunk.Index % metadataManager->GetMetadataStatus().m_chunkMetadataPageSize);

  bool initiateTransfer;

  MetadataPage* metadataPage = metadataManager->LockPage(pageIndex, &initiateTransfer);

  assert(pageIndex == metadataPage->PageIndex());

  std::unique_lock<std::mutex> lock(m_mutex);

  if (initiateTransfer)
  {
    std::string url = fmt::format("{}/ChunkMetadata/{}", layerName, pageIndex);

    metadataManager->InitiateTransfer(this, metadataPage, url, verbose);
  }

  // Check if the page is not valid and we need to add the request later when the metadata page transfer completes
  if (!metadataPage->IsValid())
  {
    m_pendingDownloadRequests[chunk] = PendingDownloadRequest(metadataPage);
    return true;
  }

  lock.unlock();

  unsigned char const* metadata = metadataManager->GetPageEntry(metadataPage, entryIndex);

  ParsedMetadata parsedMetadata = ParseMetadata(metadataManager->GetMetadataStatus().m_chunkMetadataByteSize, metadata);
    
  metadataManager->UnlockPage(metadataPage);

  int adaptiveLevel;

  IORange ioRange = CalculateRangeHeaderImpl(parsedMetadata, metadataManager->GetMetadataStatus(), &adaptiveLevel);

  std::string url = CreateUrlForChunk(layerName, chunk.Index);

  lock.lock();
  auto transferHandler = std::make_shared<ReadChunkTransfer>(metadataManager->GetMetadataStatus().m_compressionMethod, adaptiveLevel);
  m_pendingDownloadRequests[chunk] = PendingDownloadRequest(m_ioManager->Download(url, transferHandler, ioRange), transferHandler);

  return true;
}

bool VolumeDataAccessManagerImpl::ReadChunk(const VolumeDataChunk &chunk, std::vector<uint8_t> &serializedData, std::vector<uint8_t> &metadata, CompressionInfo &compressionInfo, Error &error)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  auto pendingRequestIterator = m_pendingDownloadRequests.find(chunk);

  if(pendingRequestIterator == m_pendingDownloadRequests.end())
  {
    error.Code = -1;
    error.String = "Missing request for chunk: " + std::to_string(chunk.Index);
    return false;
  }
  PendingDownloadRequest& pendingRequest = pendingRequestIterator->second;

  if (!pendingRequest.m_activeTransfer)
  {
    m_pendingRequestChangedCondition.wait(lock, [&pendingRequest]{ return !pendingRequest.m_lockedMetadataPage; });
  }

  auto activeTransfer = pendingRequest.m_activeTransfer;
  auto transferHandler = pendingRequest.m_transferHandle;

  m_pendingDownloadRequests.erase(pendingRequestIterator);

  lock.unlock();

  activeTransfer->WaitForFinish();
  if (transferHandler->m_error.Code)
  {
    error = transferHandler->m_error;
    return false;
  }

  if (transferHandler->m_data.size())
  {
    serializedData = std::move(transferHandler->m_data);
  }

  if (transferHandler->m_metadata.size())
  {
    metadata = std::move(transferHandler->m_metadata);
  }

  compressionInfo = CompressionInfo(transferHandler->m_compressionMethod, transferHandler->m_adaptiveLevel);
  return true;
}

void VolumeDataAccessManagerImpl::PageTransferCompleted(MetadataPage* metadataPage)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  for(auto &pendingRequestKeyValuePair : m_pendingDownloadRequests)
  {
    VolumeDataChunk const volumeDataChunk = pendingRequestKeyValuePair.first;
    PendingDownloadRequest &pendingRequest = pendingRequestKeyValuePair.second;

    if(pendingRequest.m_lockedMetadataPage == metadataPage)
    {
      MetadataManager *metadataManager = metadataPage->GetManager();

      int32_t pageIndex = (int)(volumeDataChunk.Index / metadataManager->GetMetadataStatus().m_chunkMetadataPageSize);
      int32_t entryIndex = (int)(volumeDataChunk.Index % metadataManager->GetMetadataStatus().m_chunkMetadataPageSize);

      assert(pageIndex == metadataPage->PageIndex());

      if (metadataPage->IsValid())
      {
        uint8_t const *metadata = metadataManager->GetPageEntry(metadataPage, entryIndex);

        ParsedMetadata parsedMetadata = ParseMetadata(metadataManager->GetMetadataStatus().m_chunkMetadataByteSize, metadata);
      
        int adaptiveLevel;

        IORange ioRange = CalculateRangeHeaderImpl(parsedMetadata, metadataManager->GetMetadataStatus(), &adaptiveLevel);

        std::string url = CreateUrlForChunk(metadataManager->LayerUrlStr(), volumeDataChunk.Index);

        auto transferHandler = std::make_shared<ReadChunkTransfer>(metadataManager->GetMetadataStatus().m_compressionMethod, adaptiveLevel);
        pendingRequest.m_activeTransfer = m_ioManager->Download(url, transferHandler, ioRange);
        pendingRequest.m_transferHandle = transferHandler;
      }

      metadataManager->UnlockPage(metadataPage);
      pendingRequest.m_lockedMetadataPage = nullptr;
    }
  }
  m_pendingRequestChangedCondition.notify_all();
}

bool VolumeDataAccessManagerImpl::WriteMetadataPage(MetadataPage* metadataPage, const std::vector<uint8_t> &data)
{
  assert(metadataPage->IsValid());

  MetadataManager *metadataManager = metadataPage->GetManager();

  std::string url = fmt::format("{}/ChunkMetadata/{}", metadataManager->LayerUrlStr(), metadataPage->PageIndex());

  std::string contentDispositionName = fmt::format("{}_ChunkMetadata_{}", metadataManager->LayerUrlStr(), metadataPage->PageIndex());

  Error error;

  auto req = m_ioManager->UploadBinary(url, contentDispositionName, std::vector<std::pair<std::string, std::string>>(), std::make_shared<std::vector<uint8_t>>(data));

  req->WaitForFinish();
  bool success = req->IsSuccess(error);

  if(error.Code != 0)
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_uploadErrors.emplace_back(new UploadError(error, "LayerStatus"));
  }

  return success;
}

static int64_t CreateUploadJobId()
{
  static std::atomic< std::int64_t > id(0);
  return --id;
}

int64_t VolumeDataAccessManagerImpl::RequestWriteChunk(const VolumeDataChunk &chunk, const DataBlock &dataBlock, const std::vector<uint8_t> &data)
{
  Error error;
  std::string layerName = GetLayerName(*chunk.Layer);
  std::string url = CreateUrlForChunk(layerName, chunk.Index);
  std::string contentDispositionName = layerName + "_" + std::to_string(chunk.Index);
  std::shared_ptr<std::vector<uint8_t>> to_write = std::make_shared<std::vector<uint8_t>>();
  uint64_t hash;

  if (!VolumeDataStore::SerializeVolumeData(chunk, dataBlock, data, GetVolumeDataLayoutImpl()->GetCompressionMethod(), *to_write, hash, error))
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_uploadErrors.emplace_back(new UploadError(error, url));
    return 0;
  }

  if (hash == VolumeDataHash::UNKNOWN)
  {
    hash = VolumeDataHash::GetUniqueHash();
  }

  auto metadataManager = GetMetadataMangerForLayer(m_vds.LayerMetadataContainer, layerName);

  int pageIndex  = (int)(chunk.Index / metadataManager->GetMetadataStatus().m_chunkMetadataPageSize);
  int entryIndex = (int)(chunk.Index % metadataManager->GetMetadataStatus().m_chunkMetadataPageSize);

  bool initiateTransfer;

  MetadataPage* lockedMetadataPage = metadataManager->LockPage(pageIndex, &initiateTransfer);

  // Newly created page
  if(initiateTransfer)
  {
    metadataManager->InitPage(lockedMetadataPage);
  }

  int64_t jobId = CreateUploadJobId();

  auto completedCallback = [this, hash, metadataManager, lockedMetadataPage, entryIndex, jobId](const Request &request, const Error &error)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    if(error.Code != 0)
    {
      auto &uploadRequest = m_pendingUploadRequests[jobId];
      if (uploadRequest.attempts < 2)
      {
        uploadRequest.StartNewUpload(*m_ioManager);
        return;
      }
      else
      {
        m_uploadErrors.emplace_back(new UploadError(error, request.GetObjectName()));
      }
    }
    else
    {
      metadataManager->SetPageEntry(lockedMetadataPage, entryIndex, reinterpret_cast<const uint8_t *>(&hash), sizeof(hash));
    }

    m_pendingUploadRequests.erase(jobId);

    lockedMetadataPage->GetManager()->UnlockPage(lockedMetadataPage);
  };

  std::vector<char> base64Hash;
  Base64Encode((const unsigned char *)&hash, sizeof(hash), base64Hash);
  std::vector<std::pair<std::string, std::string>> meta_map;
  meta_map.emplace_back("vdschunkmetadata", std::string(base64Hash.begin(), base64Hash.end()));
  // add new pending upload request
  std::unique_lock<std::mutex> lock(m_mutex);
  m_pendingUploadRequests[jobId] = PendingUploadRequest(*m_ioManager, url, contentDispositionName, meta_map, to_write, completedCallback);
  return jobId;
}

void VolumeDataAccessManagerImpl::FlushUploadQueue()
{
  while(true)
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    if(m_pendingUploadRequests.empty()) break;
    Request &request = *m_pendingUploadRequests.begin()->second.request;
    lock.unlock();
    request.WaitForFinish();
  }

  for(auto it = m_vds.LayerMetadataContainer.managers.begin(); it != m_vds.LayerMetadataContainer.managers.end(); ++it)
  {
    auto metadataManager = it->second.get();

    metadataManager->UploadDirtyPages(this);
  }

  Error error;
  SerializeAndUploadLayerStatus(m_vds, error);

  if(error.Code != 0)
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_uploadErrors.emplace_back(new UploadError(error, "LayerStatus"));
  }
}

void VolumeDataAccessManagerImpl::ClearUploadErrors()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_uploadErrors.erase(m_uploadErrors.begin(), m_uploadErrors.begin() + m_currentErrorIndex);
  m_currentErrorIndex = 0;
}

void VolumeDataAccessManagerImpl::ForceClearAllUploadErrors()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_uploadErrors.clear();
  m_currentErrorIndex = 0;
}


int32_t VolumeDataAccessManagerImpl::UploadErrorCount()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  return int32_t(m_uploadErrors.size() - m_currentErrorIndex);
}

void VolumeDataAccessManagerImpl::GetCurrentUploadError(const char** objectId, int32_t* errorCode, const char** errorString)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_currentErrorIndex >= m_uploadErrors.size())
  {
    if (objectId)
      *objectId = nullptr;
    if (errorCode)
      *errorCode = 0;
    if (errorString)
      *errorString = nullptr;
  }

  const auto &error = m_uploadErrors[m_currentErrorIndex];
  m_currentErrorIndex++;
  if (objectId)
    *objectId = error->urlObject.c_str();
  if (errorCode)
    *errorCode = error->error.Code;
  if (errorString)
    *errorString = error->error.String.c_str();
  lock.unlock();
}
}
