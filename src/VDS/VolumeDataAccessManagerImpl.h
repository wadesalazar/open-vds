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

#ifndef VOLUMEDATAACCESSMANAGERIMPL_H
#define VOLUMEDATAACCESSMANAGERIMPL_H

#include <OpenVDS/VolumeDataAccess.h>

#include "IntrusiveList.h"
#include "VolumeDataPageAccessorImpl.h"
#include <IO/IOManager.h>

#include "VolumeDataChunk.h"
#include "VolumeDataLayer.h"

#include <map>
#include "Base64.h"

namespace OpenVDS
{
class LayerMetadataContainer;
class MetadataPage;

class ReadChunkTransfer : public TransferHandler
{
public:
  ReadChunkTransfer(CompressionMethod compressionMethod, int adaptiveLevel)
    : m_compressionMethod(compressionMethod)
    , m_adaptiveLevel(adaptiveLevel)
  {}

  ~ReadChunkTransfer()
  {
  }

  void handleMetadata(const std::string& key, const std::string& header) override
  {
    if (key == "vds-chunk-metadata")
    {
      if (!Base64Decode(header.data(), (int)header.size(), m_metadata))
      {
        m_error.code = -1;
        m_error.string = "Failed to decode chunk metadata";
      }
    }
  }

  void handleData(std::vector<uint8_t>&& data) override
  {
    m_data = data;
  }
  void handleError(Error& error) override
  {
    m_error = error;
  }
  
  CompressionMethod m_compressionMethod;

  int m_adaptiveLevel;

  Error m_error;

  std::vector<uint8_t> m_data;
  std::vector<uint8_t> m_metadata;
};
struct PendingRequest
{
  MetadataPage* m_lockedMetadataPage;

  std::shared_ptr<ObjectRequester> m_activeTransfer;
  std::shared_ptr<ReadChunkTransfer> m_transferHandle;

  PendingRequest() : m_lockedMetadataPage(nullptr)
  {
  }

  explicit PendingRequest(MetadataPage* lockedMetadataPage) : m_lockedMetadataPage(lockedMetadataPage), m_activeTransfer(nullptr)
  {
  }
  explicit PendingRequest(std::shared_ptr<ObjectRequester> activeTransfer, std::shared_ptr<ReadChunkTransfer> handler) : m_lockedMetadataPage(nullptr), m_activeTransfer(activeTransfer), m_transferHandle(handler)
  {
  }
};

static bool operator<(const VolumeDataChunk &a, const VolumeDataChunk &b)
{
  if (a.layer->getChunkDimensionGroup() == b.layer->getChunkDimensionGroup())
  {
    if (a.layer->getLOD() == b.layer->getLOD())
    {
      if (a.layer->getChannelIndex() == b.layer->getChannelIndex())
      {
        return a.chunkIndex < b.chunkIndex;
      }
      else
      {
        return a.layer->getChannelIndex() < b.layer->getChannelIndex();
      }
    }
    else
    {
      return a.layer->getLOD() < b.layer->getLOD();
    }
  }
  return DimensionGroupUtil::getDimensionsNDFromDimensionGroup(a.layer->getChunkDimensionGroup()) < DimensionGroupUtil::getDimensionsNDFromDimensionGroup(b.layer->getChunkDimensionGroup());
}

class VolumeDataAccessManagerImpl : public VolumeDataAccessManager
{
public:
  VolumeDataAccessManagerImpl(VDSHandle *handle);
  VolumeDataLayout const *getVolumeDataLayout() const override;
  VolumeDataPageAccessor *createVolumeDataPageAccessor(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int maxPages, AccessMode accessMode) override;

  void  destroyVolumeDataPageAccessor(VolumeDataPageAccessor *volumeDataPageAccessor) override;
  void  destroyVolumeDataAccessor(VolumeDataAccessor *accessor) override;
  VolumeDataAccessor * cloneVolumeDataAccessor(VolumeDataAccessor const &accessor) override;

  bool  isCompleted(int64_t requestID) override;
  bool  isCanceled(int64_t requestID) override; 
  bool  waitForCompletion(int64_t requestID, int millisecondsBeforeTimeout = 0) override;
  void  cancel(int64_t requestID) override;
  float getCompletionFactor(int64_t requestID) override;

  VolumeDataReadWriteAccessor<IntVector2, bool>     *create2DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector2, uint8_t>  *create2DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector2, uint16_t> *create2DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector2, uint32_t> *create2DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector2, uint64_t> *create2DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector2, float>    *create2DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector2, double>   *create2DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;

  VolumeDataReadWriteAccessor<IntVector3, bool>     *create3DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector3, uint8_t>  *create3DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector3, uint16_t> *create3DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector3, uint32_t> *create3DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector3, uint64_t> *create3DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector3, float>    *create3DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector3, double>   *create3DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;

  VolumeDataReadWriteAccessor<IntVector4, bool>     *create4DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector4, uint8_t>  *create4DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector4, uint16_t> *create4DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector4, uint32_t> *create4DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector4, uint64_t> *create4DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector4, float>    *create4DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector4, double>   *create4DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;

  VolumeDataReadAccessor<FloatVector2, float > *create2DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  VolumeDataReadAccessor<FloatVector2, double> *create2DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  VolumeDataReadAccessor<FloatVector3, float > *create3DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  VolumeDataReadAccessor<FloatVector3, double> *create3DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  VolumeDataReadAccessor<FloatVector4, float > *create4DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  VolumeDataReadAccessor<FloatVector4, double> *create4DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;

  int64_t requestVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format) override;
  int64_t requestVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lOD, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, float replacementNoValue) override;
  int64_t requestProjectedVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod) override;
  int64_t requestProjectedVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, float replacementNoValue) override;
  int64_t requestVolumeSamples(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float (*samplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod) override;
  int64_t requestVolumeSamples(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float (*SamplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, float replacementNoValue) override;
  int64_t requestVolumeTraces(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int iTraceDimension) override;
  int64_t requestVolumeTraces(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int nTraceCount, InterpolationMethod eInterpolationMethod, int iTraceDimension, float rReplacementNoValue) override;
  int64_t prefetchVolumeChunk(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int64_t chunk) override;

  bool prepareReadChunkData(const VolumeDataChunk& chunk, bool verbose, Error& error);
  bool readChunk(const VolumeDataChunk& chunk, std::vector<uint8_t>& serializedData, std::vector<uint8_t>& metadata, CompressionInfo& compressionInfo, Error& error);
  void pageTransferCompleted(MetadataPage* page);
  
private:
  VolumeDataLayout *m_layout;
  IOManager *m_ioManager;
  LayerMetadataContainer *m_layerMetadataContainer;
  IntrusiveList<VolumeDataPageAccessorImpl, &VolumeDataPageAccessorImpl::m_volumeDataPageAccessorListNode> m_volumeDataPageAccessorList;
  std::mutex m_mutex;
  std::condition_variable m_pendingRequestChangedCondition;
  std::map<VolumeDataChunk, PendingRequest> m_pendingRequests;
};
}
#endif //VOLUMEDATAACCESSMANAGERIMPL_H