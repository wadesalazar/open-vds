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
#include "VolumeDataRequestProcessor.h"

#include <map>
#include "Base64.h"

namespace OpenVDS
{
class MetadataPage;

class ReadChunkTransfer : public TransferDownloadHandler
{
public:
  ReadChunkTransfer(CompressionMethod compressionMethod, int adaptiveLevel)
    : m_compressionMethod(compressionMethod)
    , m_adaptiveLevel(adaptiveLevel)
  {}

  ~ReadChunkTransfer() override
  {
  }

  void HandleMetadata(const std::string& key, const std::string& header) override
  {
    if (key == "vdschunkmetadata")
    {
      if (!Base64Decode(header.data(), (int)header.size(), m_metadata))
      {
        m_error.code = -1;
        m_error.string = "Failed to decode chunk metadata";
      }
    }
  }

  void HandleData(std::vector<uint8_t>&& data) override
  {
    m_data = data;
  }

  void Completed(const Request &req, const Error & error) override
  {
    m_error = error;
  }
  
  CompressionMethod m_compressionMethod;

  int m_adaptiveLevel;

  Error m_error;

  std::vector<uint8_t> m_data;
  std::vector<uint8_t> m_metadata;
};

struct PendingDownloadRequest
{
  MetadataPage* m_lockedMetadataPage;

  std::shared_ptr<Request> m_activeTransfer;
  std::shared_ptr<ReadChunkTransfer> m_transferHandle;
  int m_ref;
  bool m_canMove;
  PendingDownloadRequest() : m_lockedMetadataPage(nullptr), m_ref(0), m_canMove(true)
  {
  }

  explicit PendingDownloadRequest(MetadataPage* lockedMetadataPage) : m_lockedMetadataPage(lockedMetadataPage), m_activeTransfer(nullptr), m_ref(1), m_canMove(true)
  {
  }
  explicit PendingDownloadRequest(std::shared_ptr<Request> activeTransfer, std::shared_ptr<ReadChunkTransfer> handler) : m_lockedMetadataPage(nullptr), m_activeTransfer(activeTransfer), m_transferHandle(handler), m_ref(1), m_canMove(true)
  {
  }
};

inline bool operator<(const VolumeDataChunk &a, const VolumeDataChunk &b)
{
  if (a.layer->GetChunkDimensionGroup() == b.layer->GetChunkDimensionGroup())
  {
    if (a.layer->GetLOD() == b.layer->GetLOD())
    {
      if (a.layer->GetChannelIndex() == b.layer->GetChannelIndex())
      {
        return a.index < b.index;
      }
      else
      {
        return a.layer->GetChannelIndex() < b.layer->GetChannelIndex();
      }
    }
    else
    {
      return a.layer->GetLOD() < b.layer->GetLOD();
    }
  }
  return DimensionGroupUtil::GetDimensionsNDFromDimensionGroup(a.layer->GetChunkDimensionGroup()) < DimensionGroupUtil::GetDimensionsNDFromDimensionGroup(b.layer->GetChunkDimensionGroup());
}

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
  uint32_t attempts;
  std::shared_ptr<Request> request;

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
  VolumeDataLayout const *GetVolumeDataLayout() const override;
  VolumeDataLayoutImpl const *GetVolumeDataLayoutImpl() const;
  VolumeDataPageAccessor *CreateVolumeDataPageAccessor(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int maxPages, AccessMode accessMode) override;

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

  int64_t GetVolumeSubsetBufferSize(VolumeDataLayout const *volumeDataLayout, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, int lod) override;
  int64_t RequestVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format) override;
  int64_t RequestVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lOD, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, float replacementNoValue) override;
  int64_t GetProjectedVolumeSubsetBufferSize(VolumeDataLayout const *volumeDataLayout, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, int lod) override;
  int64_t RequestProjectedVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod) override;
  int64_t RequestProjectedVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, float replacementNoValue) override;
  int64_t RequestVolumeSamples(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float (*samplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod) override;
  int64_t RequestVolumeSamples(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float (*SamplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, float replacementNoValue) override;
  int64_t GetVolumeTracesBufferSize(VolumeDataLayout const *volumeDataLayout, int traceCount, int iTraceDimension, int lod) override;
  int64_t RequestVolumeTraces(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int iTraceDimension) override;
  int64_t RequestVolumeTraces(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int nTraceCount, InterpolationMethod eInterpolationMethod, int iTraceDimension, float rReplacementNoValue) override;
  int64_t PrefetchVolumeChunk(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int64_t chunk) override;

  bool PrepareReadChunkData(const VolumeDataChunk& chunk, bool verbose, Error& error);
  bool ReadChunk(const VolumeDataChunk& chunk, std::vector<uint8_t>& serializedData, std::vector<uint8_t>& metadata, CompressionInfo& compressionInfo, Error& error);
  void PageTransferCompleted(MetadataPage* metadataPage);
  bool WriteMetadataPage(MetadataPage* metadataPage, const std::vector<uint8_t> &data);

  int64_t RequestWriteChunk(const VolumeDataChunk &chunk, const DataBlock &dataBlock, const std::vector<uint8_t> &data);
  
  IOManager *GetIoManager() const { return m_ioManager; }

  void FlushUploadQueue() override;
  void ClearUploadErrors() override;
  void ForceClearAllUploadErrors() override;
  int32_t UploadErrorCount() override;
  void GetCurrentUploadError(const char **objectId, int32_t *errorCode, const char **errorString) override;

  int CountActivePages() { return m_requestProcessor.CountActivePages(); }
private:
  VDS &m_vds;
  IOManager *m_ioManager;
  VolumeDataRequestProcessor m_requestProcessor;
  IntrusiveList<VolumeDataPageAccessorImpl, &VolumeDataPageAccessorImpl::m_volumeDataPageAccessorListNode> m_volumeDataPageAccessorList;
  std::mutex m_mutex;
  std::condition_variable m_pendingRequestChangedCondition;
  std::map<VolumeDataChunk, PendingDownloadRequest> m_pendingDownloadRequests;
  std::map<int64_t, PendingUploadRequest> m_pendingUploadRequests;
  std::vector<std::unique_ptr<UploadError>> m_uploadErrors;
  uint32_t m_currentErrorIndex;
};

}
#endif //VOLUMEDATAACCESSMANAGERIMPL_H
