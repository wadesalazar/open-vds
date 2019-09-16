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

#include <OpenVDSHandle.h>
#include "VolumeDataPageAccessorImpl.h"

#include <cmath>
#include <algorithm>
#include <inttypes.h>

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


static ParsedMetadata parseMetadata(int metadataByteSize, unsigned char const *metadata)
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
    fprintf(stderr, "%s%s\n", std::string(" Unsupported chunkMetadataByteSize: ").c_str(), std::to_string(metadataByteSize).c_str());
  }

  return parsedMetadata;
}

static bool isConstantChunkHash(uint64_t chunkHash)
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

static int getEffectiveAdaptiveLoadLevel(float effectiveCompressionTolerance, float compressionTolerance)
{
//  assert(effectiveCompressionTolerance >= adaptiveToleranceMin);
//  assert(compressionTolerance >= adaptiveToleranceMin);

  int adaptiveLoadLevel = (int)(log(effectiveCompressionTolerance / compressionTolerance) / M_LN2);

  return std::max(0, adaptiveLoadLevel);
}

static int getEffectiveAdaptiveLevel(AdaptiveMode adaptiveMode, float desiredTolerance, float desiredRatio, float remoteTolerance, int64_t const adaptiveLevelSizes[WAVELET_ADAPTIVE_LEVELS], int64_t uncompressedSize)
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
    level = getEffectiveAdaptiveLoadLevel(desiredTolerance, remoteTolerance);
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

static int waveletAdaptiveLevelsMetadataDecode(uint64_t totalSize, int targetLevel, uint8_t const *levels)
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

static IORange calculateRangeHeaderImpl(const ParsedMetadata& parsedMetadata, const MetadataStatus &metadataStatus, int * const adaptiveLevel)
{
  if (isConstantChunkHash(parsedMetadata.m_chunkHash))
  {
    *adaptiveLevel = -1;
  }
  else if (parsedMetadata.m_adaptiveLevels.empty())
  {
    *adaptiveLevel = -1;
  }
  else
  {
    *adaptiveLevel = getEffectiveAdaptiveLevel(AdaptiveMode_BestQuality, 0.01f, 1.0f , metadataStatus.m_compressionTolerance, metadataStatus.m_adaptiveLevelSizes, metadataStatus.m_uncompressedSize);

    int range = waveletAdaptiveLevelsMetadataDecode(parsedMetadata.m_chunkSize, *adaptiveLevel, parsedMetadata.m_adaptiveLevels.data());
    if (range && range != parsedMetadata.m_chunkSize)
    {
    return { size_t(0) , size_t(range - 1 ) };
    }
  }

  return { 0 , 0 };
}

static VolumeDataLayer *getVolumeDataLayer(VolumeDataLayout const *layout, DimensionsND dimension, int channel, int lod, bool isAllowFailure)
{
  if(!layout)
  {
    fprintf(stderr, "Volume data layout is NULL, this is usually because the VDS setup is invalid");
    return nullptr;
  }

  if(channel > layout->getChannelCount())
  {
    fprintf(stderr, "Specified channel doesn't exist");
    return nullptr;
  }

  VolumeDataLayer *layer = layout->getBaseLayer(DimensionGroupUtil::getDimensionGroupFromDimensionsND(dimension), channel);

  if(!layer && !isAllowFailure)
  {
    fprintf(stderr, "Specified dimension group doesn't exist");
    return nullptr;
  }

  while(layer && layer->getLOD() < lod)
  {
    layer = layer->getParentLayer();
  }

  if((!layer || layer->getLayerType() == VolumeDataLayer::Virtual) && !isAllowFailure)
  {
    fprintf(stderr, "Specified LOD doesn't exist");
  }

  assert(layer || isAllowFailure);
  return (layer && layer->getLayerType() != VolumeDataLayer::Virtual) ? layer : nullptr;
}

VolumeDataAccessManagerImpl::VolumeDataAccessManagerImpl(VDSHandle* handle)
  : m_layout(handle->volumeDataLayout.get())
  , m_ioManager(handle->ioManager.get())
  , m_layerMetadataContainer(&handle->layerMetadataContainer)
{
}

VolumeDataLayout const* VolumeDataAccessManagerImpl::getVolumeDataLayout() const
{
  return m_layout;
}

VolumeDataPageAccessor* VolumeDataAccessManagerImpl::createVolumeDataPageAccessor(VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int maxPages, bool isReadWrite)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  VolumeDataLayer *layer = getVolumeDataLayer(volumeDataLayout, dimensionsND, channel, lod, true);
  if (!layer)
    return nullptr;

  VolumeDataPageAccessorImpl *accessor = new VolumeDataPageAccessorImpl(this,layer,maxPages, isReadWrite);
  m_volumeDataPageAccessorList.insertLast(accessor);
  return accessor;
}

void  VolumeDataAccessManagerImpl::destroyVolumeDataPageAccessor(VolumeDataPageAccessor* volumeDataPageAccessor)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  VolumeDataPageAccessorImpl *pageAccessorImpl = static_cast<VolumeDataPageAccessorImpl *>(volumeDataPageAccessor);
  m_volumeDataPageAccessorList.remove(pageAccessorImpl);
  delete  pageAccessorImpl;
}

void  VolumeDataAccessManagerImpl::destroyVolumeDataAccessor(VolumeDataAccessor* accessor)
{}

VolumeDataAccessor* VolumeDataAccessManagerImpl::cloneVolumeDataAccessor(VolumeDataAccessor const& accessor)
{
  assert(false);
  return nullptr;
}

bool  VolumeDataAccessManagerImpl::isCompleted(int64_t requestID)
{
  return false;
}
bool  VolumeDataAccessManagerImpl::isCanceled(int64_t requestID)
{
  return false;
}
bool  VolumeDataAccessManagerImpl::waitForCompletion(int64_t requestID, int millisecondsBeforeTimeout)
{
  return false;
}
void  VolumeDataAccessManagerImpl::cancel(int64_t requestID)
{
}
float VolumeDataAccessManagerImpl::getCompletionFactor(int64_t requestID)
{
  return 0.0f;
}

VolumeDataReadWriteAccessor<IntVector2, bool>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector2, uint8_t>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessorU8(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector2, uint16_t>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessorU16(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector2, uint32_t>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessorU32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector2, uint64_t>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessorU64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector2, float>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector2, double>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}

VolumeDataReadWriteAccessor<IntVector3, bool>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector3, uint8_t>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessorU8(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector3, uint16_t>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessorU16(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector3, uint32_t>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessorU32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector3, uint64_t>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessorU64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector3, float>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector3, double>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}

VolumeDataReadWriteAccessor<IntVector4, bool>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector4, uint8_t>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessorU8(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector4, uint16_t>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessorU16(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector4, uint32_t>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessorU32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector4, uint64_t>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessorU64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector4, float>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}
VolumeDataReadWriteAccessor<IntVector4, double>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return nullptr;
}

VolumeDataReadAccessor<FloatVector2, float >* VolumeDataAccessManagerImpl::create2DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return nullptr;
}
VolumeDataReadAccessor<FloatVector2, double>* VolumeDataAccessManagerImpl::create2DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return nullptr;
}
VolumeDataReadAccessor<FloatVector3, float >* VolumeDataAccessManagerImpl::create3DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return nullptr;
}
VolumeDataReadAccessor<FloatVector3, double>* VolumeDataAccessManagerImpl::create3DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return nullptr;
}
VolumeDataReadAccessor<FloatVector4, float >* VolumeDataAccessManagerImpl::create4DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return nullptr;
}
VolumeDataReadAccessor<FloatVector4, double>* VolumeDataAccessManagerImpl::create4DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return nullptr;
}

int64_t VolumeDataAccessManagerImpl::requestVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format)
{
  return int64_t(0);
}
int64_t VolumeDataAccessManagerImpl::requestVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lOD, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, float replacementNoValue)
{
  return int64_t(0);
}
int64_t VolumeDataAccessManagerImpl::requestProjectedVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const& voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod)
{
  return int64_t(0);
}
int64_t VolumeDataAccessManagerImpl::requestProjectedVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const& voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, float replacementNoValue)
{
  return int64_t(0);
}
int64_t VolumeDataAccessManagerImpl::requestVolumeSamples(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*samplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod)
{
  return int64_t(0);
}
int64_t VolumeDataAccessManagerImpl::requestVolumeSamples(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*SamplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, float replacementNoValue)
{
  return int64_t(0);
}
int64_t VolumeDataAccessManagerImpl::requestVolumeTraces(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int iTraceDimension)
{
  return int64_t(0);
}
int64_t VolumeDataAccessManagerImpl::requestVolumeTraces(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int nTraceCount, InterpolationMethod eInterpolationMethod, int iTraceDimension, float rReplacementNoValue)
{
  return int64_t(0);
}
int64_t VolumeDataAccessManagerImpl::prefetchVolumeChunk(VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int64_t chunk)
{
  return int64_t(0);
}

static MetadataManager *getMetadataMangerForLayer(LayerMetadataContainer *container, const std::string &layer)
{
  std::unique_lock<std::mutex> lock(container->mutex);

  MetadataManager *metadataManager = nullptr;
  auto metadataManagerIterator = container->managers.find(layer);

  if(metadataManagerIterator != container->managers.end())
  {
    metadataManager = metadataManagerIterator->second.get();
  }

  return metadataManager;
}

static std::string makeURLForChunk(const std::string &layerUrl, uint64_t chunk)
{
  char url[1000];

  snprintf(url, sizeof(url), "%s/%" PRIu64, layerUrl.c_str(), chunk);

  return std::string(url);
}

bool VolumeDataAccessManagerImpl::prepareReadChunkData(const VolumeDataChunk &chunk, bool verbose, Error &error)
{
  int32_t channel = chunk.layer->getChannelIndex();
  const char *channelName = channel > 0 ? chunk.layer->getLayout()->getChannelName(chunk.layer->getChannelIndex()) : "";
  int32_t lod = chunk.layer->getLOD();
  const char *dimensions_string = DimensionGroupUtil::getDimensionsGroupString(DimensionGroupUtil::getDimensionsNDFromDimensionGroup(chunk.layer->getChunkDimensionGroup()));
  char layerURL[1000];
  snprintf(layerURL, sizeof(layerURL), "%sDimensions_%sLOD%d", channelName, dimensions_string, lod);
  auto metadataManager = getMetadataMangerForLayer(m_layerMetadataContainer, layerURL);
  //do fallback
  if (!metadataManager)
  {
    error.code =- 1;
    error.string = "No metdadataManager";
    return false;
  }
  
  int pageIndex  = (int)(chunk.chunkIndex / metadataManager->metadataStatus().m_chunkMetadataPageSize);
  int entryIndex = (int)(chunk.chunkIndex % metadataManager->metadataStatus().m_chunkMetadataPageSize);

  bool initiateTransfer;

  MetadataPage* metadataPage = metadataManager->lockPage(pageIndex, &initiateTransfer);

  assert(pageIndex == metadataPage->PageIndex());

  std::unique_lock<std::mutex> lock(m_mutex);

  if (initiateTransfer)
  {
    char url[1000];
    snprintf(url, sizeof(url), "%s/ChunkMetadata/%d", layerURL, pageIndex);

    metadataManager->initiateTransfer(this, metadataPage, url, verbose);
  }

  // Check if the page is not valid and we need to add the request later when the metadata page transfer completes
  if (!metadataPage->IsValid())
  {
    m_pendingRequests[chunk] = PendingRequest(metadataPage);
    return true;
  }

  lock.unlock();

  unsigned char const* metadata = metadataManager->getPageEntry(metadataPage, entryIndex);

  ParsedMetadata parsedMetadata = parseMetadata(metadataManager->metadataStatus().m_chunkMetadataByteSize, metadata);
    
  metadataManager->unlockPage(metadataPage);

  int adaptiveLevel;

  IORange ioRange = calculateRangeHeaderImpl(parsedMetadata, metadataManager->metadataStatus(), &adaptiveLevel);

  std::string url = makeURLForChunk(layerURL, chunk.chunkIndex);

  lock.lock();
  auto transferHandler = std::make_shared<ReadChunkTransfer>(metadataManager->metadataStatus().m_compressionMethod, adaptiveLevel);
  m_pendingRequests[chunk] = PendingRequest(m_ioManager->requestObject(url, transferHandler, ioRange), transferHandler);

  return true;
}
  
bool VolumeDataAccessManagerImpl::readChunk(const VolumeDataChunk &chunk, std::vector<uint8_t> &serializedData, std::vector<uint8_t> &metadata, CompressionInfo &compressionInfo, Error &error)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  auto pendingRequestIterator = m_pendingRequests.find(chunk);

  if(pendingRequestIterator == m_pendingRequests.end())
  {
    error.code = -1;
    error.string = "Missing request for chunk: " + std::to_string(chunk.chunkIndex);
    return false;
  }
  PendingRequest& pendingRequest = pendingRequestIterator->second;

  if (!pendingRequest.m_activeTransfer)
  {
    while (pendingRequest.m_lockedMetadataPage)
    {
      m_pendingRequestChangedCondition.wait(lock);
    }
  }

  auto activeTransfer = pendingRequest.m_activeTransfer;
  auto transferHandler = pendingRequest.m_transferHandle;

  m_pendingRequests.erase(pendingRequestIterator);

  lock.unlock();

  activeTransfer->waitForFinish();
  if (transferHandler->m_error.code)
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

void VolumeDataAccessManagerImpl::pageTransferCompleted(MetadataPage* metadataPage)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  for(auto &pendingRequestKeyValuePair : m_pendingRequests)
  {
    VolumeDataChunk const volumeDataChunk = pendingRequestKeyValuePair.first;
    PendingRequest &pendingRequest = pendingRequestKeyValuePair.second;

    if(pendingRequest.m_lockedMetadataPage == metadataPage)
    {
      MetadataManager *metadataManager = metadataPage->GetManager();

      int32_t pageIndex = (int)(volumeDataChunk.chunkIndex / metadataManager->metadataStatus().m_chunkMetadataPageSize);
      int32_t entryIndex = (int)(volumeDataChunk.chunkIndex % metadataManager->metadataStatus().m_chunkMetadataPageSize);

      assert(pageIndex == metadataPage->PageIndex());

      if (metadataPage->IsValid())
      {

        uint8_t const *metadata = metadataManager->getPageEntry(metadataPage, entryIndex);

        ParsedMetadata parsedMetadata = parseMetadata(metadataManager->metadataStatus().m_chunkMetadataByteSize, metadata);
      
        metadataManager->unlockPage(metadataPage);
        pendingRequest.m_lockedMetadataPage = nullptr;

        int adaptiveLevel;

        IORange ioRange = calculateRangeHeaderImpl(parsedMetadata, metadataManager->metadataStatus(), &adaptiveLevel);

        std::string url = makeURLForChunk(metadataManager->layerUrlStr(), volumeDataChunk.chunkIndex);

        auto transferHandler = std::make_shared<ReadChunkTransfer>(metadataManager->metadataStatus().m_compressionMethod, adaptiveLevel);
        pendingRequest.m_activeTransfer = m_ioManager->requestObject(url, transferHandler, ioRange);
        pendingRequest.m_transferHandle = transferHandler;
      }
    }
  }
  m_pendingRequestChangedCondition.notify_all();
}

}
