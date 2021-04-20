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

#include <OpenVDS/VolumeDataAccessManager.h>

#include "IntrusiveList.h"
#include "VolumeDataPageAccessorImpl.h"
#include "VolumeDataLayoutImpl.h"
#include "VolumeDataStore.h"

#include "VolumeDataChunk.h"
#include "VolumeDataLayer.h"
#include "VolumeDataRequestProcessor.h"

#include <map>
#include "Base64.h"

namespace OpenVDS
{
struct UploadError
{
  UploadError(const Error &error, const std::string &urlObject)
    : error(error)
    , urlObject(urlObject)
  {}
  Error error;
  std::string urlObject;
};

class VolumeDataAccessManagerImpl : public IVolumeDataAccessManager, public IVolumeDataAccessor::Manager
{
  VolumeDataLayoutImpl const *          PrivateGetLayout();
  VolumeDataLayer const *               PrivateGetLayer(DimensionsND dimensionsND, int channel, int LOD);
  void                                  ValidateRequest(int64_t requestID);
  VolumeDataLayer const *               ValidateProduceStatus(VolumeDataLayer const *volumeDataLayer, bool allowLODProduction = false);
  VolumeDataLayoutImpl const *          ValidateVoxelCoordinates(VolumeDataLayoutImpl const *volumeDataLayout, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max]);
  VolumeDataLayer const *               ValidateVolumeSubset(VolumeDataLayer const *volumeDataLayer, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max]);
  VolumeDataLayoutImpl const *          ValidateChannelIndex(VolumeDataLayoutImpl const *volumeDataLayout, int channel);
  VolumeDataLayer const *               ValidateChunkIndex(VolumeDataLayer const *volumeDataLayer, int64_t chunkIndex);
  VolumeDataLayer const *               ValidateVolumeDataStore(VolumeDataLayer const *volumeDataLayer);
  VolumeDataLayoutImpl const *          ValidateTraceDimension(VolumeDataLayoutImpl const *volumeDataLayout, int traceDimension);
  VolumeDataLayer const *               ValidateTraceDimension(VolumeDataLayer const *volumeDataLayer, int traceDimension);

  VolumeDataAccessManagerImpl(VDS &vds);
  ~VolumeDataAccessManagerImpl() override;

public:
  static VolumeDataAccessManagerImpl *  Create(VDS &vds); // Increments RefCount
  void                                  Invalidate();
  bool                                  IsValid();

  // Overrides
  virtual void                          AddRef() override;
  virtual void                          Release() override;
  virtual int                           RefCount() const override;

  VolumeDataLayoutImpl const *GetVolumeDataLayout() override;
  VDSProduceStatus GetVDSProduceStatus(DimensionsND dimensionsND, int LOD, int channel) override;
  VolumeDataPageAccessor *CreateVolumeDataPageAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, VolumeDataAccessManager::AccessMode accessMode, int chunkMetadataPageSize = 1024) override;

  void  DestroyVolumeDataPageAccessor(VolumeDataPageAccessor *volumeDataPageAccessor) override;
  void  DestroyVolumeDataAccessor(IVolumeDataAccessor *accessor) override;
  IVolumeDataAccessor * CloneVolumeDataAccessor(IVolumeDataAccessor const &accessor) override;

  bool  IsCompleted(int64_t requestID) override;
  bool  IsCanceled(int64_t requestID) override; 
  bool  WaitForCompletion(int64_t requestID, int millisecondsBeforeTimeout = 0) override;
  void  Cancel(int64_t requestID) override;
  void  CancelAndWaitForCompletion(int64_t requestID) override;
  float GetCompletionFactor(int64_t requestID) override;

  IVolumeDataReadWriteAccessor<IntVector2, bool>     *Create2DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector2, uint8_t>  *Create2DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector2, uint16_t> *Create2DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector2, uint32_t> *Create2DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector2, uint64_t> *Create2DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector2, float>    *Create2DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector2, double>   *Create2DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;

  IVolumeDataReadWriteAccessor<IntVector3, bool>     *Create3DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector3, uint8_t>  *Create3DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector3, uint16_t> *Create3DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector3, uint32_t> *Create3DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector3, uint64_t> *Create3DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector3, float>    *Create3DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector3, double>   *Create3DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;

  IVolumeDataReadWriteAccessor<IntVector4, bool>     *Create4DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector4, uint8_t>  *Create4DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector4, uint16_t> *Create4DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector4, uint32_t> *Create4DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector4, uint64_t> *Create4DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector4, float>    *Create4DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  IVolumeDataReadWriteAccessor<IntVector4, double>   *Create4DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;

  IVolumeDataReadAccessor<FloatVector2, float > *Create2DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  IVolumeDataReadAccessor<FloatVector2, double> *Create2DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  IVolumeDataReadAccessor<FloatVector3, float > *Create3DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  IVolumeDataReadAccessor<FloatVector3, double> *Create3DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  IVolumeDataReadAccessor<FloatVector4, float > *Create4DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  IVolumeDataReadAccessor<FloatVector4, double> *Create4DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;

  int64_t GetVolumeSubsetBufferSize(const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, int LOD, int channel) override;
  int64_t RequestVolumeSubset(void *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], VolumeDataChannelDescriptor::Format format, optional<float> replacementNoValue) override;
  int64_t GetProjectedVolumeSubsetBufferSize(const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, int LOD, int channel) override;
  int64_t RequestProjectedVolumeSubset(void *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, optional<float> replacementNoValue) override;
  int64_t GetVolumeSamplesBufferSize(int sampleCount, int channel) override;
  int64_t RequestVolumeSamples(float *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const float (*samplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, optional<float> replacementNoValue) override;
  int64_t GetVolumeTracesBufferSize(int traceCount, int traceDimension, int LOD, int channel) override;
  int64_t RequestVolumeTraces(float *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const float(*tracePositions)[Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int traceDimension, optional<float> replacementNoValue) override;
  int64_t PrefetchVolumeChunk(DimensionsND dimensionsND, int LOD, int channel, int64_t chunkIndex) override;

  VolumeDataStore *GetVolumeDataStore();
  void AddUploadError(Error const &error, const std::string &url);

  void FlushUploadQueue(bool writeUpdatedLayerStatus = true) override;
  void ClearUploadErrors() override;
  void ForceClearAllUploadErrors() override;
  int32_t UploadErrorCount() override;
  void GetCurrentUploadError(const char **objectId, int32_t *errorCode, const char **errorString) override;

  void GetCurrentDownloadError(int *code, const char** errorString) override;
 
  void SetCurrentDownloadError(const Error& error)
  {
    m_currentDownloadError = error;
  }

  int CountActivePages() { return m_requestProcessor->CountActivePages(); }
private:
  std::atomic<int> m_refCount;
  bool m_invalidated;
  VDS &m_vds;
  std::unique_ptr<VolumeDataRequestProcessor> m_requestProcessor;
  IntrusiveList<VolumeDataPageAccessorImpl, &VolumeDataPageAccessorImpl::m_volumeDataPageAccessorListNode> m_volumeDataPageAccessorList;
  std::mutex m_mutex;
  std::vector<std::unique_ptr<UploadError>> m_uploadErrors;
  uint32_t m_currentErrorIndex;
  Error m_currentDownloadError;
};

}
#endif //VOLUMEDATAACCESSMANAGERIMPL_H
