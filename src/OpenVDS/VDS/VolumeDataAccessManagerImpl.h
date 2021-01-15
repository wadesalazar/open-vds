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

class VolumeDataAccessManagerImpl : public VolumeDataAccessManager
{
public:
  VolumeDataAccessManagerImpl(VDS &vds);
  ~VolumeDataAccessManagerImpl() override;
  VolumeDataLayoutImpl const *GetVolumeDataLayout() const override;
  VDSProduceStatus GetVDSProduceStatus(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel) const override;
  VolumeDataPageAccessor *CreateVolumeDataPageAccessor(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int maxPages, AccessMode accessMode, int chunkMetadataPageSize = 1024) override;

  void  DestroyVolumeDataPageAccessor(VolumeDataPageAccessor *volumeDataPageAccessor) override;
  void  DestroyVolumeDataAccessor(VolumeDataAccessor *accessor) override;
  VolumeDataAccessor * CloneVolumeDataAccessor(VolumeDataAccessor const &accessor) override;

  bool  IsCompleted(int64_t requestID) override;
  bool  IsCanceled(int64_t requestID) override; 
  bool  WaitForCompletion(int64_t requestID, int millisecondsBeforeTimeout = 0) override;
  void  Cancel(int64_t requestID) override;
  float GetCompletionFactor(int64_t requestID) override;

  VolumeDataReadWriteAccessor<IntVector2, bool>     *Create2DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector2, uint8_t>  *Create2DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector2, uint16_t> *Create2DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector2, uint32_t> *Create2DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector2, uint64_t> *Create2DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector2, float>    *Create2DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector2, double>   *Create2DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;

  VolumeDataReadWriteAccessor<IntVector3, bool>     *Create3DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector3, uint8_t>  *Create3DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector3, uint16_t> *Create3DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector3, uint32_t> *Create3DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector3, uint64_t> *Create3DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector3, float>    *Create3DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector3, double>   *Create3DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;

  VolumeDataReadWriteAccessor<IntVector4, bool>     *Create4DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector4, uint8_t>  *Create4DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector4, uint16_t> *Create4DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector4, uint32_t> *Create4DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector4, uint64_t> *Create4DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector4, float>    *Create4DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;
  VolumeDataReadWriteAccessor<IntVector4, double>   *Create4DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) override;

  VolumeDataReadAccessor<FloatVector2, float > *Create2DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  VolumeDataReadAccessor<FloatVector2, double> *Create2DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  VolumeDataReadAccessor<FloatVector3, float > *Create3DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  VolumeDataReadAccessor<FloatVector3, double> *Create3DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  VolumeDataReadAccessor<FloatVector4, float > *Create4DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;
  VolumeDataReadAccessor<FloatVector4, double> *Create4DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) override;

  int64_t GetVolumeSubsetBufferSize(VolumeDataLayout const *volumeDataLayout, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, int lod, int channel) override;
  int64_t RequestVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format) override;
  int64_t RequestVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lOD, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, float replacementNoValue) override;
  int64_t GetProjectedVolumeSubsetBufferSize(VolumeDataLayout const *volumeDataLayout, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, int lod, int channel) override;
  int64_t RequestProjectedVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod) override;
  int64_t RequestProjectedVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, float replacementNoValue) override;
  int64_t GetVolumeSamplesBufferSize(VolumeDataLayout const *volumeDataLayout, int sampleCount, int channel) override;
  int64_t RequestVolumeSamples(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float (*samplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod) override;
  int64_t RequestVolumeSamples(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float (*SamplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, float replacementNoValue) override;
  int64_t GetVolumeTracesBufferSize(VolumeDataLayout const *volumeDataLayout, int traceCount, int iTraceDimension, int lod, int channel) override;
  int64_t RequestVolumeTraces(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int iTraceDimension) override;
  int64_t RequestVolumeTraces(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int nTraceCount, InterpolationMethod eInterpolationMethod, int iTraceDimension, float rReplacementNoValue) override;
  int64_t PrefetchVolumeChunk(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int64_t chunk) override;

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
