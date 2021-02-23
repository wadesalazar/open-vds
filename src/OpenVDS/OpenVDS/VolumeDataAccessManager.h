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

#ifndef VOLUMEDATAACCESSMANAGER_H
#define VOLUMEDATAACCESSMANAGER_H

#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/Vector.h>
#include <OpenVDS/Optional.h>
#include <OpenVDS/Exceptions.h>

#include <memory>

namespace OpenVDS {

class IVolumeDataAccessManager
{
protected:
          IVolumeDataAccessManager() {}
  virtual ~IVolumeDataAccessManager() {}
public:
  virtual void AddRef() = 0;
  virtual void Release() = 0;
  virtual int  RefCount() const = 0;

  static constexpr int Dimensionality_Max = VolumeDataLayout::Dimensionality_Max;  ///< the maximum number of dimensions a VDS can have

  enum AccessMode
  {
    AccessMode_ReadOnly,
    AccessMode_ReadWrite,
    AccessMode_Create
  };

  /// <summary>
  /// Get the VolumeDataLayout object for a VDS.
  /// </summary>
  /// <returns>
  /// The VolumeDataLayout object associated with the VDS or NULL if there is no valid VolumeDataLayout.
  /// </returns>
  virtual VolumeDataLayout const *GetVolumeDataLayout() = 0;

  /// <summary>
  /// Get the produce status for the specific DimensionsND/LOD/Channel combination.
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensions group we're getting the produce status for.
  /// </param>
  /// <param name="LOD">
  /// The LOD level we're getting the produce status for.
  /// </param>
  /// <param name="channel">
  /// The channel index we're getting the produce status for.
  /// </param>
  /// <returns>
  /// The produce status for the specific DimensionsND/LOD/Channel combination.
  /// </returns>
  virtual VDSProduceStatus GetVDSProduceStatus(DimensionsND dimensionsND, int LOD, int channel) = 0;

  /// <summary>
  /// Create a VolumeDataPageAccessor object for the VDS.
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensions group that the volume data page accessor will access.
  /// </param>
  /// <param name="LOD">
  /// The LOD level that the volume data page accessor will access.
  /// </param>
  /// <param name="channel">
  /// The channel index that the volume data page accessor will access.
  /// </param>
  /// <param name="maxPages">
  /// The maximum number of pages that the volume data page accessor will cache.
  /// </param>
  /// <param name="accessMode">
  /// This specifies the access mode (ReadOnly/ReadWrite/Create) of the volume data page accessor.
  /// </param>
  /// <param name="chunkMetadataPageSize">
  /// The chunk metadata page size of the layer. This controls how many chunk metadata entries are written per page, and is only used when the access mode is Create.
  /// If this number is too low it will degrade performance, but in certain situations it can be advantageous to make this number a multiple
  /// of the number of chunks in some of the dimensions. Do not change this from the default (1024) unless you know exactly what you are doing.
  /// </param>
  /// <returns>
  /// A VolumeDataPageAccessor object for the VDS.
  /// </returns>
  virtual VolumeDataPageAccessor *CreateVolumeDataPageAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, AccessMode accessMode, int chunkMetadataPageSize = 1024) = 0;

  /// <summary>
  /// Destroy a volume data page accessor object.
  /// </summary>
  /// <param name="volumeDataPageAccessor">
  /// The VolumeDataPageAccessor object to destroy.
  /// </param>
  virtual void  DestroyVolumeDataPageAccessor(VolumeDataPageAccessor *volumeDataPageAccessor) = 0;

  virtual void  DestroyVolumeDataAccessor(IVolumeDataAccessor *accessor) = 0;
                  
  virtual IVolumeDataAccessor *
                CloneVolumeDataAccessor(IVolumeDataAccessor const &accessor) = 0;

  virtual IVolumeDataReadWriteAccessor<IntVector2, bool>     *Create2DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector2, uint8_t>  *Create2DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector2, uint16_t> *Create2DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector2, uint32_t> *Create2DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector2, uint64_t> *Create2DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector2, float>    *Create2DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector2, double>   *Create2DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;

  virtual IVolumeDataReadWriteAccessor<IntVector3, bool>     *Create3DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector3, uint8_t>  *Create3DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector3, uint16_t> *Create3DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector3, uint32_t> *Create3DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector3, uint64_t> *Create3DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector3, float>    *Create3DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector3, double>   *Create3DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;

  virtual IVolumeDataReadWriteAccessor<IntVector4, bool>     *Create4DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector4, uint8_t>  *Create4DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector4, uint16_t> *Create4DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector4, uint32_t> *Create4DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector4, uint64_t> *Create4DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector4, float>    *Create4DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual IVolumeDataReadWriteAccessor<IntVector4, double>   *Create4DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;

  virtual IVolumeDataReadAccessor<FloatVector2, float > *Create2DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual IVolumeDataReadAccessor<FloatVector2, double> *Create2DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual IVolumeDataReadAccessor<FloatVector3, float > *Create3DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual IVolumeDataReadAccessor<FloatVector3, double> *Create3DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual IVolumeDataReadAccessor<FloatVector4, float > *Create4DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual IVolumeDataReadAccessor<FloatVector4, double> *Create4DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;

  /// <summary>
  /// Compute the buffer size (in bytes) for a volume subset request.
  /// </summary>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="format">
  /// Voxel format of the destination buffer.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <returns>
  /// The buffer size needed.
  /// </returns>
  virtual int64_t GetVolumeSubsetBufferSize(const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], VolumeDataChannelDescriptor::Format format, int LOD = 0, int channel = 0) = 0;

  /// <summary>
  /// Request a subset of the input VDS.
  /// </summary>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by minVoxelCoordinates and maxVoxelCoordinates.
  /// </param>
  /// <param name="bufferByteSize">
  /// The size of the provided buffer, in bytes.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="format">
  /// Voxel format of the destination buffer.
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete.
  /// </returns>
  virtual int64_t RequestVolumeSubset(void *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], VolumeDataChannelDescriptor::Format format, optional<float> replacementNoValue = optional<float>()) = 0;

  /// <summary>
  /// Compute the buffer size (in bytes) for a projected volume subset request.
  /// </summary>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="projectedDimensions">
  /// The 2D dimension group that the plane in the source dimensiongroup is projected into. It must be a 2D subset of the source dimensions.
  /// </param>
  /// <param name="format">
  /// Voxel format of the destination buffer.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <returns>
  /// The buffer size needed.
  /// </returns>
  virtual int64_t GetProjectedVolumeSubsetBufferSize(const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, int LOD = 0, int channel = 0) = 0;

  /// <summary>
  /// Request a subset projected from an arbitrary 3D plane through the subset onto one of the sides of the subset.
  /// </summary>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by minVoxelCoordinates and maxVoxelCoordinates for the projected dimensions.
  /// </param>
  /// <param name="bufferByteSize">
  /// The size of the provided buffer, in bytes.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="voxelPlane">
  /// The plane equation for the projection from the dimension source to the projected dimensions (which must be a 2D subset of the source dimensions).
  /// </param>
  /// <param name="projectedDimensions">
  /// The 2D dimension group that the plane in the source dimensiongroup is projected into. It must be a 2D subset of the source dimensions.
  /// </param>
  /// <param name="interpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="format">
  /// Voxel format of the destination buffer.
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete.
  /// </returns>
  virtual int64_t RequestProjectedVolumeSubset(void *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>()) = 0;

  /// <summary>
  /// Compute the buffer size (in bytes) for a volume samples request.
  /// </summary>
  /// <param name="sampleCount">
  /// Number of samples to request.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <returns>
  /// The buffer size needed.
  /// </returns>
  virtual int64_t GetVolumeSamplesBufferSize(int sampleCount, int channel = 0) = 0;

  /// <summary>
  /// Request sampling of the input VDS at the specified coordinates.
  /// </summary>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least sampleCount elements.
  /// </param>
  /// <param name="bufferByteSize">
  /// The size of the provided buffer, in bytes.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="samplePositions">
  /// Pointer to array of VolumeDataLayout::Dimensionality_Max-elements indicating the positions to sample. May be deleted once RequestVolumeSamples return, as HueSpace makes a deep copy of the data.
  /// </param>
  /// <param name="sampleCount">
  /// Number of samples to request.
  /// </param>
  /// <param name="interpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete.
  /// </returns>
  virtual int64_t RequestVolumeSamples(float *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const float (*samplePositions)[VolumeDataLayout::Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>()) = 0;

  /// <summary>
  /// Compute the buffer size (in bytes) for a volume traces request.
  /// </summary>
  /// <param name="traceCount">
  /// Number of traces to request.
  /// </param>
  /// <param name="traceDimension">
  /// The dimension to trace
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <returns>
  /// The buffer size needed.
  /// </returns>
  virtual int64_t GetVolumeTracesBufferSize(int traceCount, int traceDimension, int LOD = 0, int channel = 0) = 0;

  /// <summary>
  /// Request traces from the input VDS.
  /// </summary>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least traceCount * number of samples in the traceDimension.
  /// </param>
  /// <param name="bufferByteSize">
  /// The size of the provided buffer, in bytes.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="tracePositions">
  /// Pointer to array of traceCount VolumeDataLayout::Dimensionality_Max-elements indicating the trace positions.
  /// </param>
  /// <param name="traceCount">
  /// Number of traces to request.
  /// </param>
  /// <param name="interpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="traceDimension">
  /// The dimension to trace
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete.
  /// </returns>
  virtual int64_t RequestVolumeTraces(float *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const float(*tracePositions)[VolumeDataLayout::Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int traceDimension, optional<float> replacementNoValue = optional<float>()) = 0;

  /// <summary>
  /// Force production of a specific volume data chunk
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested chunk belongs to.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested chunk belongs to.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested chunk belongs to.
  /// </param>
  /// <param name="chunkIndex">
  /// The index of the chunk to prefetch.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete.
  /// </returns>
  virtual int64_t PrefetchVolumeChunk(DimensionsND dimensionsND, int LOD, int channel, int64_t chunkIndex) = 0;

  /// <summary>
  /// Check if a request completed successfully. If the request completed, the buffer now contains valid data.
  /// </summary>
  /// <param name="requestID">
  /// The RequestID to check for completion.
  /// </param>
  /// <returns>
  /// Either IsCompleted, IsCanceled or WaitForCompletion will return true a single time, after that the request is taken out of the system.
  /// </returns>
  virtual bool  IsCompleted(int64_t requestID) = 0;

  /// <summary>
  /// Check if a request was canceled (e.g. the VDS was invalidated before the request was processed). If the request was canceled, the buffer does not contain valid data.
  /// </summary>
  /// <param name="requestID">
  /// The RequestID to check for cancellation.
  /// </param>
  /// <returns>
  /// Either IsCompleted, IsCanceled or WaitForCompletion will return true a single time, after that the request is taken out of the system.
  /// </returns>
  virtual bool  IsCanceled(int64_t requestID) = 0;

  /// <summary>
  /// Wait for a request to complete successfully. If the request completed, the buffer now contains valid data.
  /// </summary>
  /// <param name="requestID">
  /// The RequestID to wait for completion of.
  /// </param>
  /// <param name="millisecondsBeforeTimeout">
  /// The number of milliseconds to wait before timing out (optional). A value of 0 indicates there is no timeout and we will wait for
  /// however long it takes. Note that the request is not automatically canceled if the wait times out, you can also use this mechanism
  /// to e.g. update a progress bar while waiting. If you want to cancel the request you have to explicitly call CancelRequest() and
  /// then wait for the request to stop writing to the buffer.
  /// </param>
  /// <returns>
  /// Either IsCompleted, IsCanceled or WaitForCompletion will return true a single time, after that the request is taken out of the system.
  /// Whenever WaitForCompletion returns false you need to call IsCanceled() to know if that was because of a timeout or if the request was canceled.
  /// </returns>
  virtual bool  WaitForCompletion(int64_t requestID, int millisecondsBeforeTimeout = 0) = 0;

  /// <summary>
  /// Try to cancel the request. You still have to call WaitForCompletion/IsCanceled to make sure the buffer is not being written to and to take the job out of the system.
  /// It is possible that the request has completed concurrently with the call to Cancel in which case WaitForCompletion will return true.
  /// </summary>
  /// <param name="requestID">
  /// The RequestID to cancel.
  /// </param>
  virtual void  Cancel(int64_t requestID) = 0;

  /// <summary>
  /// Cancel the request and wait for it to complete. This call will block until the request has completed so you can be sure the buffer is not being written to and the job is taken out of the system.
  /// </summary>
  /// <param name="requestID">
  /// The RequestID to cancel.
  /// </param>
  virtual void  CancelAndWaitForCompletion(int64_t requestID) = 0;

  /// <summary>
  /// Get the completion factor (between 0 and 1) of the request.
  /// </summary>
  /// <param name="requestID">
  /// The RequestID to get the completion factor of.
  /// </param>
  /// <returns>
  /// A factor (between 0 and 1) indicating how much of the request has been completed.
  /// </returns>
  virtual float GetCompletionFactor(int64_t requestID) = 0;

  /// <summary>
  /// Flush any pending writes and write updated layer status
  /// </summary>
  /// <param name="writeUpdatedLayerStatus">
  /// Write the updated layer status (or only flush pending writes of chunks and chunk-metadata).
  /// </param>
  virtual void FlushUploadQueue(bool writeUpdatedLayerStatus = true) = 0;

  virtual void ClearUploadErrors() = 0;
  virtual void ForceClearAllUploadErrors() = 0;
  virtual int32_t UploadErrorCount() = 0;
  virtual void GetCurrentUploadError(const char **objectId, int32_t *errorCode, const char **errorString) = 0;
  virtual void GetCurrentDownloadError(int *code, const char** errorString) = 0;
};

class IHasVolumeDataAccess
{
protected:
          IHasVolumeDataAccess() {}
  virtual ~IHasVolumeDataAccess() {}
public:
  virtual IVolumeDataAccessManager* GetVolumeDataAccessManager() = 0;
};

class VolumeDataRequest
{
protected:
  friend class VolumeDataAccessManager;
  template<typename T> struct RequestFormat;

  IVolumeDataAccessManager* 
                          m_Manager;
  void*                   m_Buffer;
  int64_t                 m_BufferByteSize;
  VolumeDataChannelDescriptor::Format
                          m_BufferDataType;
  bool                    m_IsCompleted;
  bool                    m_IsCanceled;
  int64_t                 m_JobID;
  std::vector<uint8_t>    m_Data;

  static void
  Deleter(VolumeDataRequest* pcThis)
  {
    delete pcThis;
  }

  void
  SetJobID(int64_t id)
  {
    m_JobID = id;
  }

  virtual bool 
  IsDataOwner() const
  {
    return m_Data.size() > 0;
  }

  void 
  CancelInternal() 
  {
    if (!m_IsCompleted && !m_IsCanceled && m_Manager != nullptr)
    {
      m_Manager->CancelAndWaitForCompletion(m_JobID);
      m_IsCanceled = true;
    }
  }

public:
  VolumeDataRequest() : m_Manager(nullptr), m_Buffer(nullptr), m_BufferByteSize(0), m_BufferDataType(VolumeDataChannelDescriptor::Format_U8), m_IsCompleted(false), m_IsCanceled(false), m_JobID(0)
  {
  }

  VolumeDataRequest(IVolumeDataAccessManager* manager) : m_Manager(manager), m_Buffer(nullptr), m_BufferByteSize(0), m_BufferDataType(VolumeDataChannelDescriptor::Format_Any), m_IsCompleted(false), m_IsCanceled(false), m_JobID(0)
  {
    m_Manager->AddRef();
  }

  VolumeDataRequest(IVolumeDataAccessManager* manager, void* buffer, int64_t bufferByteSize, VolumeDataChannelDescriptor::Format bufferDataType) : m_Manager(manager), m_Buffer(buffer), m_BufferByteSize(bufferByteSize), m_BufferDataType(bufferDataType), m_IsCompleted(false), m_IsCanceled(false), m_JobID(0)
  {
    m_Manager->AddRef();
  }

  VolumeDataRequest(IVolumeDataAccessManager* manager, int64_t bufferByteSize, VolumeDataChannelDescriptor::Format bufferDataType) : m_Manager(manager), m_Buffer(nullptr), m_BufferByteSize(bufferByteSize), m_BufferDataType(bufferDataType), m_IsCompleted(false), m_IsCanceled(false), m_JobID(0)
  {
    m_Manager->AddRef();
    m_Data.resize(bufferByteSize);
    m_Buffer = &m_Data[0];
  }

  virtual ~VolumeDataRequest()
  {
    CancelInternal();
    if (m_Manager)
    {
      m_Manager->Release();
    }
  }

  /// <summary>
  /// Check if the request object is valid. Throws an InvalidOperation exception if the request object is not valid.
  /// </summary>
  void
  ValidateRequest()
  {
    if (m_Manager == nullptr || m_JobID <= 0)
    {
      throw InvalidOperation("Invalid request object");
    }
  }

  /// <summary>
  /// Get the ID of the request.
  /// </summary>
  /// <returns>
  /// The ID of the request.
  /// </returns>
  int64_t
  RequestID() const
  {
    return m_JobID;
  }

  /// <summary>
  /// Check if the request completed successfully. If the request completed, the buffer now contains valid data.
  /// </summary>
  /// <returns>
  /// The request is active until either IsCompleted, IsCanceled or WaitForCompletion returns true.
  /// </returns>
  bool
  IsCompleted()
  {
    ValidateRequest();
    if (!m_IsCompleted && !m_IsCanceled)
    {
      m_IsCompleted = m_Manager->IsCompleted(m_JobID);
    }
    return m_IsCompleted;
  }

  /// <summary>
  /// Check if the request was canceled (e.g. the VDS was invalidated before the request was processed). If the request was canceled, the buffer does not contain valid data.
  /// </summary>
  /// <returns>
  /// The request is active until either IsCompleted, IsCanceled or WaitForCompletion returns true.
  /// </returns>
  bool
  IsCanceled()
  {
    ValidateRequest();
    if (!m_IsCompleted && !m_IsCanceled)
    {
      m_IsCanceled = m_Manager->IsCanceled(m_JobID);
    }
    return m_IsCanceled;
  }

  /// <summary>
  /// Wait for the VolumeDataRequest to complete successfully. If the request completed, the buffer now contains valid data.
  /// </summary>
  /// <param name="millisecondsBeforeTimeout">
  /// The number of milliseconds to wait before timing out (optional). A value of 0 indicates there is no timeout and we will wait for
  /// however long it takes. Note that the request is not automatically canceled if the wait times out, you can also use this mechanism
  /// to e.g. update a progress bar while waiting. If you want to cancel the request you have to explicitly call CancelRequest() and
  /// then wait for the request to stop writing to the buffer.
  /// </param>
  /// <returns>
  /// The request is active until either IsCompleted, IsCanceled or WaitForCompletion returns true.
  /// Whenever WaitForCompletion returns false you need to call IsCanceled() to know if that was because of a timeout or if the request was canceled.
  /// </returns>
  bool
  WaitForCompletion(int millisecondsBeforeTimeout = 0)
  {
    ValidateRequest();
    if (!m_IsCompleted && !m_IsCanceled)
    {
      m_IsCompleted = m_Manager->WaitForCompletion(m_JobID, millisecondsBeforeTimeout);
    }
    return m_IsCompleted;
  }

  /// <summary>
  /// Try to cancel the request. You still have to call WaitForCompletion/IsCanceled to make sure the buffer is not being written to and to take the job out of the system.
  /// It is possible that the request has completed concurrently with the call to Cancel in which case WaitForCompletion will return true.
  /// </summary>
  void
  Cancel()
  {
    ValidateRequest();
    if (!m_IsCompleted && !m_IsCanceled)
    {
      m_Manager->Cancel(m_JobID);
    }
  }

  /// <summary>
  /// Cancel the request and wait for it to complete. This call will block until the request has completed so you can be sure the buffer is not being written to and the job is taken out of the system.
  /// </summary>
  void
  CancelAndWaitForCompletion()
  {
    ValidateRequest();
    if (!m_IsCompleted && !m_IsCanceled)
    {
      m_Manager->CancelAndWaitForCompletion(m_JobID);
      m_IsCanceled = true;
    }
  }

  /// <summary>
  /// Get the completion factor (between 0 and 1) of the request.
  /// </summary>
  /// <returns>
  /// A factor (between 0 and 1) indicating how much of the request has been completed.
  /// </returns>
  float
  GetCompletionFactor()
  {
    ValidateRequest();
    if (!m_IsCompleted && !m_IsCanceled)
    {
      return m_Manager->GetCompletionFactor(m_JobID);
    }
    return m_IsCompleted ? 1.0f : 0.0f;
  }

  /// <summary>
  /// Get the pointer to the buffer the request is writing to.
  /// </summary>
  /// <returns>
  /// The pointer to the buffer the request is writing to.
  /// </returns>
  void*
  Buffer() const
  {
    return m_Buffer;
  }

  /// <summary>
  /// Get the size of the buffer the request is writing to.
  /// </summary>
  /// <returns>
  /// The size of the buffer the request is writing to.
  /// </returns>
  int64_t
  BufferByteSize() const
  {
    return m_BufferByteSize;
  }

  /// <summary>
  /// Get the volume data format of the buffer the request is writing to.
  /// </summary>
  /// <returns>
  /// The volume data format of the buffer the request is writing to.
  /// </returns>
  VolumeDataChannelDescriptor::Format
  BufferDataType() const
  {
    return m_BufferDataType;
  }
};

template<> struct VolumeDataRequest::RequestFormat<uint8_t>  { static constexpr VolumeDataChannelDescriptor::Format format = VolumeDataChannelDescriptor::Format_U8;   };
template<> struct VolumeDataRequest::RequestFormat<uint16_t> { static constexpr VolumeDataChannelDescriptor::Format format = VolumeDataChannelDescriptor::Format_U16;  };
template<> struct VolumeDataRequest::RequestFormat<uint32_t> { static constexpr VolumeDataChannelDescriptor::Format format = VolumeDataChannelDescriptor::Format_U32;  };
template<> struct VolumeDataRequest::RequestFormat<uint64_t> { static constexpr VolumeDataChannelDescriptor::Format format = VolumeDataChannelDescriptor::Format_U64;  };
template<> struct VolumeDataRequest::RequestFormat<float>    { static constexpr VolumeDataChannelDescriptor::Format format = VolumeDataChannelDescriptor::Format_R32;  };
template<> struct VolumeDataRequest::RequestFormat<double>   { static constexpr VolumeDataChannelDescriptor::Format format = VolumeDataChannelDescriptor::Format_R64;  };

template<typename VALUETYPE>
class VolumeDataRequest_t : public VolumeDataRequest
{
  friend class VolumeDataAccessManager;
protected:
  std::vector<VALUETYPE>  m_TypedData;

  virtual bool IsDataOwner() const override
  {
    return m_TypedData.size() > 0;
  }

  VolumeDataRequest_t(IVolumeDataAccessManager* manager, void* buffer, int64_t bufferByteSize) : VolumeDataRequest(manager, buffer, bufferByteSize, RequestFormat<VALUETYPE>::format)
  {
  }

  VolumeDataRequest_t(IVolumeDataAccessManager* manager, int64_t bufferByteSize) : VolumeDataRequest(manager, nullptr, bufferByteSize, RequestFormat<VALUETYPE>::format)
  {
    m_TypedData.resize(bufferByteSize / sizeof(VALUETYPE));
    m_Buffer = (void*)&m_TypedData[0];
  }

  void
  EnsureRequestCompleted()
  {
    if (!WaitForCompletion() || IsCanceled())
    {
      throw InvalidOperation("Volume data request was canceled");
    }
  }

public:
  using element_t = VALUETYPE;

  virtual ~VolumeDataRequest_t() override
  {
    CancelInternal();
  }

  virtual std::vector<VALUETYPE>& Data()
  {
    if (!IsDataOwner())
    {
      throw InvalidOperation("buffer is not owner by request.");
    }
    EnsureRequestCompleted();
    return m_TypedData;
  }
};

using VolumeDataRequest1Bit       = VolumeDataRequest_t<uint8_t>;
using VolumeDataRequestByte       = VolumeDataRequest_t<uint8_t>;
using VolumeDataRequestUShort     = VolumeDataRequest_t<uint16_t>;
using VolumeDataRequestUInt       = VolumeDataRequest_t<uint32_t>;
using VolumeDataRequestULong      = VolumeDataRequest_t<uint64_t>;
using VolumeDataRequestFloat      = VolumeDataRequest_t<float>;
using VolumeDataRequestDouble     = VolumeDataRequest_t<double>;

class VolumeDataAccessManager
{
public:
  typedef IVolumeDataAccessManager::AccessMode AccessMode;
  static constexpr AccessMode AccessMode_ReadOnly  = IVolumeDataAccessManager::AccessMode_ReadOnly;
  static constexpr AccessMode AccessMode_ReadWrite = IVolumeDataAccessManager::AccessMode_ReadWrite;
  static constexpr AccessMode AccessMode_Create    = IVolumeDataAccessManager::AccessMode_Create;

  static constexpr int Dimensionality_Max = VolumeDataLayout::Dimensionality_Max;  ///< the maximum number of dimensions a VDS can have

  static constexpr int maxPagesDefault = 8;  ///< the default maxPages for VolumeDataPageAccessors

private:
  IVolumeDataAccessManager* 
                          m_IVolumeDataAccessManager;

  bool
  IsValid() const
  {
    return m_IVolumeDataAccessManager != nullptr;
  }

  bool
  EnsureValid() const
  {
    if (!IsValid())
    { 
      throw InvalidOperation("Invalid access manager.");
    }
    return true;
  }

  void 
  Dispose()
  {
    if (m_IVolumeDataAccessManager)
    {
      m_IVolumeDataAccessManager->Release();
      m_IVolumeDataAccessManager = nullptr;
    }
  }

  template<class VOLUMEDATAREQUEST>
  std::shared_ptr<VOLUMEDATAREQUEST>
  DoRequestVolumeSubset(VOLUMEDATAREQUEST* request, void *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], VolumeDataChannelDescriptor::Format format, optional<float> const& replacementNoValue)
  {
    request->SetJobID(m_IVolumeDataAccessManager->RequestVolumeSubset(buffer, bufferByteSize, dimensionsND, LOD, channel, minVoxelCoordinates, maxVoxelCoordinates, format, replacementNoValue));
    return std::shared_ptr<VOLUMEDATAREQUEST>(request, &VolumeDataRequest::Deleter);
  }

  template<class VOLUMEDATAREQUEST>
  std::shared_ptr<VOLUMEDATAREQUEST>
  DoRequestProjectedVolumeSubset(VOLUMEDATAREQUEST* request, void *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, optional<float> const& replacementNoValue)
  {
    request->SetJobID(m_IVolumeDataAccessManager->RequestProjectedVolumeSubset(buffer, bufferByteSize, dimensionsND, LOD, channel, minVoxelCoordinates, maxVoxelCoordinates, voxelPlane, projectedDimensions, format, interpolationMethod, replacementNoValue));
    return std::shared_ptr<VOLUMEDATAREQUEST>(request, &VolumeDataRequest::Deleter);
  }

  std::shared_ptr<VolumeDataRequestFloat>
  DoRequestVolumeSamples(VolumeDataRequestFloat* request, float *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const float (*samplePositions)[VolumeDataLayout::Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, optional<float> const& replacementNoValue)
  {
    request->SetJobID(m_IVolumeDataAccessManager->RequestVolumeSamples(buffer, bufferByteSize, dimensionsND, LOD, channel, samplePositions, sampleCount, interpolationMethod, replacementNoValue));
    return std::shared_ptr<VolumeDataRequestFloat>(request, &VolumeDataRequest::Deleter);
  }

  std::shared_ptr<VolumeDataRequestFloat>
  DoRequestVolumeTraces(VolumeDataRequestFloat* request, float *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const float(*tracePositions)[VolumeDataLayout::Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int traceDimension, optional<float> const& replacementNoValue)
  {
    request->SetJobID(m_IVolumeDataAccessManager->RequestVolumeTraces(buffer, bufferByteSize, dimensionsND, LOD, channel, tracePositions, traceCount, interpolationMethod, traceDimension, replacementNoValue));
    return std::shared_ptr<VolumeDataRequestFloat>(request, &VolumeDataRequest::Deleter);
  }

public:
  VolumeDataAccessManager() : m_IVolumeDataAccessManager(nullptr)
  {
  }

  VolumeDataAccessManager(IVolumeDataAccessManager* manager) : m_IVolumeDataAccessManager(manager)
  {
    manager->AddRef();
  }

  VolumeDataAccessManager(VolumeDataAccessManager const& rhs) : m_IVolumeDataAccessManager(nullptr)
  {
    *this = rhs;
  }

  ~VolumeDataAccessManager()
  {
    Dispose();
  }

  VolumeDataAccessManager&
  operator=(const VolumeDataAccessManager& rhs) 
  {
    if (rhs.m_IVolumeDataAccessManager)
    {
      rhs.m_IVolumeDataAccessManager->AddRef();
    }
    Dispose();
    m_IVolumeDataAccessManager = rhs.m_IVolumeDataAccessManager;
    return *this;
  }

  /// <summary>
  /// Get the VolumeDataLayout object for a VDS.
  /// </summary>
  /// <returns>
  /// The VolumeDataLayout object associated with the VDS or NULL if there is no valid VolumeDataLayout.
  /// </returns>
  VolumeDataLayout const *GetVolumeDataLayout()
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->GetVolumeDataLayout();
  }

  /// <summary>
  /// Get the produce status for the specific DimensionsND/LOD/Channel combination.
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensions group we're getting the produce status for.
  /// </param>
  /// <param name="LOD">
  /// The LOD level we're getting the produce status for.
  /// </param>
  /// <param name="channel">
  /// The channel index we're getting the produce status for.
  /// </param>
  /// <returns>
  /// The produce status for the specific DimensionsND/LOD/Channel combination.
  /// </returns>
  VDSProduceStatus GetVDSProduceStatus(DimensionsND dimensionsND, int LOD, int channel) const
  {
    return m_IVolumeDataAccessManager->GetVDSProduceStatus(dimensionsND, LOD, channel);
  }

  /// <summary>
  /// Create a VolumeDataPageAccessor object for the VDS.
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensions group that the volume data page accessor will access.
  /// </param>
  /// <param name="LOD">
  /// The LOD level that the volume data page accessor will access.
  /// </param>
  /// <param name="channel">
  /// The channel index that the volume data page accessor will access.
  /// </param>
  /// <param name="maxPages">
  /// The maximum number of pages that the volume data page accessor will cache.
  /// </param>
  /// <param name="accessMode">
  /// This specifies the access mode (ReadOnly/ReadWrite/Create) of the volume data page accessor.
  /// </param>
  /// <param name="chunkMetadataPageSize">
  /// The chunk metadata page size of the layer. This controls how many chunk metadata entries are written per page, and is only used when the access mode is Create.
  /// If this number is too low it will degrade performance, but in certain situations it can be advantageous to make this number a multiple
  /// of the number of chunks in some of the dimensions. Do not change this from the default (1024) unless you know exactly what you are doing.
  /// </param>
  /// <returns>
  /// A VolumeDataPageAccessor object for the VDS.
  /// </returns>
  VolumeDataPageAccessor *CreateVolumeDataPageAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, AccessMode accessMode, int chunkMetadataPageSize = 1024)
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode(accessMode), chunkMetadataPageSize);
  }

  /// <summary>
  /// Destroy a volume data page accessor object.
  /// </summary>
  /// <param name="volumeDataPageAccessor">
  /// The VolumeDataPageAccessor object to destroy.
  /// </param>
  void DestroyVolumeDataPageAccessor(VolumeDataPageAccessor *volumeDataPageAccessor)
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->DestroyVolumeDataPageAccessor(volumeDataPageAccessor);
  }

  //-----------------------------------------------------------------------------
  // Templated VolumeDataAccessors
  //-----------------------------------------------------------------------------

  template<typename INDEX, typename T>
  VolumeDataReadAccessor<INDEX, T>
                CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>());

  template<typename INDEX, typename T>
  VolumeDataReadAccessor<INDEX, T>
                CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>());

  template<typename INDEX, typename T>
  VolumeDataReadWriteAccessor<INDEX, T>
                CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>());

  template<typename INDEX, typename T>
  VolumeDataReadWriteAccessor<INDEX, T>
                CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>());

  template<typename INDEX, typename T>
  VolumeDataReadAccessor<INDEX, T >
                CreateInterpolatingVolumeDataAccessor(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue = optional<float>());

  template<typename INDEX, typename T>
  VolumeDataReadAccessor<INDEX, T >
                CreateInterpolatingVolumeDataAccessor(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>());

  //-----------------------------------------------------------------------------
  // 2D VolumeDataAccessors
  //-----------------------------------------------------------------------------

  VolumeData2DInterpolatingAccessorR64 CreateVolumeData2DInterpolatingAccessorR64(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DInterpolatingAccessorR64(m_IVolumeDataAccessManager->Create2DInterpolatingVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue()), interpolationMethod));
  }
  VolumeData2DInterpolatingAccessorR64 CreateVolumeData2DInterpolatingAccessorR64(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DInterpolatingAccessorR64(m_IVolumeDataAccessManager->Create2DInterpolatingVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue()), interpolationMethod));
  }
  VolumeData2DInterpolatingAccessorR32 CreateVolumeData2DInterpolatingAccessorR32(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DInterpolatingAccessorR32(m_IVolumeDataAccessManager->Create2DInterpolatingVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue()), interpolationMethod));
  }
  VolumeData2DInterpolatingAccessorR32 CreateVolumeData2DInterpolatingAccessorR32(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DInterpolatingAccessorR32(m_IVolumeDataAccessManager->Create2DInterpolatingVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue()), interpolationMethod));
  }
  VolumeData2DReadAccessor1Bit CreateVolumeData2DReadAccessor1Bit(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessor1Bit(m_IVolumeDataAccessManager->Create2DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessor1Bit CreateVolumeData2DReadAccessor1Bit(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessor1Bit(m_IVolumeDataAccessManager->Create2DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessorU8 CreateVolumeData2DReadAccessorU8(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessorU8(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU8(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessorU8 CreateVolumeData2DReadAccessorU8(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessorU8(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU8(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessorU16 CreateVolumeData2DReadAccessorU16(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessorU16(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU16(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessorU16 CreateVolumeData2DReadAccessorU16(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessorU16(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU16(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessorU32 CreateVolumeData2DReadAccessorU32(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessorU32(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessorU32 CreateVolumeData2DReadAccessorU32(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessorU32(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessorU64 CreateVolumeData2DReadAccessorU64(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessorU64(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessorU64 CreateVolumeData2DReadAccessorU64(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessorU64(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessorR32 CreateVolumeData2DReadAccessorR32(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessorR32(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessorR32 CreateVolumeData2DReadAccessorR32(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessorR32(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessorR64 CreateVolumeData2DReadAccessorR64(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessorR64(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadAccessorR64 CreateVolumeData2DReadAccessorR64(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData2DReadAccessorR64(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessor1Bit CreateVolumeData2DReadWriteAccessor1Bit(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessor1Bit(m_IVolumeDataAccessManager->Create2DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessor1Bit CreateVolumeData2DReadWriteAccessor1Bit(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessor1Bit(m_IVolumeDataAccessManager->Create2DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessorU8 CreateVolumeData2DReadWriteAccessorU8(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessorU8(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU8(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessorU8 CreateVolumeData2DReadWriteAccessorU8(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessorU8(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU8(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessorU16 CreateVolumeData2DReadWriteAccessorU16(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessorU16(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU16(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessorU16 CreateVolumeData2DReadWriteAccessorU16(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessorU16(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU16(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessorU32 CreateVolumeData2DReadWriteAccessorU32(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessorU32(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessorU32 CreateVolumeData2DReadWriteAccessorU32(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessorU32(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessorU64 CreateVolumeData2DReadWriteAccessorU64(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessorU64(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessorU64 CreateVolumeData2DReadWriteAccessorU64(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessorU64(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorU64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessorR32 CreateVolumeData2DReadWriteAccessorR32(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessorR32(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessorR32 CreateVolumeData2DReadWriteAccessorR32(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessorR32(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessorR64 CreateVolumeData2DReadWriteAccessorR64(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessorR64(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData2DReadWriteAccessorR64 CreateVolumeData2DReadWriteAccessorR64(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData2DReadWriteAccessorR64(m_IVolumeDataAccessManager->Create2DVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }

  //-----------------------------------------------------------------------------
  // 3D VolumeDataAccessors
  //-----------------------------------------------------------------------------

  VolumeData3DInterpolatingAccessorR64 CreateVolumeData3DInterpolatingAccessorR64(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DInterpolatingAccessorR64(m_IVolumeDataAccessManager->Create3DInterpolatingVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue()), interpolationMethod));
  }
  VolumeData3DInterpolatingAccessorR64 CreateVolumeData3DInterpolatingAccessorR64(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DInterpolatingAccessorR64(m_IVolumeDataAccessManager->Create3DInterpolatingVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue()), interpolationMethod));
  }
  VolumeData3DInterpolatingAccessorR32 CreateVolumeData3DInterpolatingAccessorR32(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DInterpolatingAccessorR32(m_IVolumeDataAccessManager->Create3DInterpolatingVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue()), interpolationMethod));
  }
  VolumeData3DInterpolatingAccessorR32 CreateVolumeData3DInterpolatingAccessorR32(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DInterpolatingAccessorR32(m_IVolumeDataAccessManager->Create3DInterpolatingVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue()), interpolationMethod));
  }
  VolumeData3DReadAccessor1Bit CreateVolumeData3DReadAccessor1Bit(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessor1Bit(m_IVolumeDataAccessManager->Create3DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessor1Bit CreateVolumeData3DReadAccessor1Bit(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessor1Bit(m_IVolumeDataAccessManager->Create3DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessorU8 CreateVolumeData3DReadAccessorU8(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessorU8(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU8(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessorU8 CreateVolumeData3DReadAccessorU8(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessorU8(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU8(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessorU16 CreateVolumeData3DReadAccessorU16(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessorU16(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU16(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessorU16 CreateVolumeData3DReadAccessorU16(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessorU16(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU16(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessorU32 CreateVolumeData3DReadAccessorU32(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessorU32(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessorU32 CreateVolumeData3DReadAccessorU32(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessorU32(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessorU64 CreateVolumeData3DReadAccessorU64(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessorU64(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessorU64 CreateVolumeData3DReadAccessorU64(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessorU64(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessorR32 CreateVolumeData3DReadAccessorR32(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessorR32(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessorR32 CreateVolumeData3DReadAccessorR32(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessorR32(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessorR64 CreateVolumeData3DReadAccessorR64(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessorR64(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadAccessorR64 CreateVolumeData3DReadAccessorR64(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData3DReadAccessorR64(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessor1Bit CreateVolumeData3DReadWriteAccessor1Bit(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessor1Bit(m_IVolumeDataAccessManager->Create3DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessor1Bit CreateVolumeData3DReadWriteAccessor1Bit(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessor1Bit(m_IVolumeDataAccessManager->Create3DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessorU8 CreateVolumeData3DReadWriteAccessorU8(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessorU8(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU8(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessorU8 CreateVolumeData3DReadWriteAccessorU8(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessorU8(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU8(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessorU16 CreateVolumeData3DReadWriteAccessorU16(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessorU16(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU16(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessorU16 CreateVolumeData3DReadWriteAccessorU16(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessorU16(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU16(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessorU32 CreateVolumeData3DReadWriteAccessorU32(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessorU32(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessorU32 CreateVolumeData3DReadWriteAccessorU32(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessorU32(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessorU64 CreateVolumeData3DReadWriteAccessorU64(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessorU64(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessorU64 CreateVolumeData3DReadWriteAccessorU64(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessorU64(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorU64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessorR32 CreateVolumeData3DReadWriteAccessorR32(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessorR32(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessorR32 CreateVolumeData3DReadWriteAccessorR32(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessorR32(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessorR64 CreateVolumeData3DReadWriteAccessorR64(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessorR64(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData3DReadWriteAccessorR64 CreateVolumeData3DReadWriteAccessorR64(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData3DReadWriteAccessorR64(m_IVolumeDataAccessManager->Create3DVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }

  //-----------------------------------------------------------------------------
  // 4D VolumeDataAccessors
  //-----------------------------------------------------------------------------

  VolumeData4DInterpolatingAccessorR64 CreateVolumeData4DInterpolatingAccessorR64(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DInterpolatingAccessorR64(m_IVolumeDataAccessManager->Create4DInterpolatingVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue()), interpolationMethod));
  }
  VolumeData4DInterpolatingAccessorR64 CreateVolumeData4DInterpolatingAccessorR64(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DInterpolatingAccessorR64(m_IVolumeDataAccessManager->Create4DInterpolatingVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue()), interpolationMethod));
  }
  VolumeData4DInterpolatingAccessorR32 CreateVolumeData4DInterpolatingAccessorR32(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DInterpolatingAccessorR32(m_IVolumeDataAccessManager->Create4DInterpolatingVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue()), interpolationMethod));
  }
  VolumeData4DInterpolatingAccessorR32 CreateVolumeData4DInterpolatingAccessorR32(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DInterpolatingAccessorR32(m_IVolumeDataAccessManager->Create4DInterpolatingVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue()), interpolationMethod));
  }
  VolumeData4DReadAccessor1Bit CreateVolumeData4DReadAccessor1Bit(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessor1Bit(m_IVolumeDataAccessManager->Create4DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessor1Bit CreateVolumeData4DReadAccessor1Bit(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessor1Bit(m_IVolumeDataAccessManager->Create4DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessorU8 CreateVolumeData4DReadAccessorU8(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessorU8(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU8(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessorU8 CreateVolumeData4DReadAccessorU8(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessorU8(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU8(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessorU16 CreateVolumeData4DReadAccessorU16(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessorU16(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU16(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessorU16 CreateVolumeData4DReadAccessorU16(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessorU16(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU16(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessorU32 CreateVolumeData4DReadAccessorU32(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessorU32(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessorU32 CreateVolumeData4DReadAccessorU32(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessorU32(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessorU64 CreateVolumeData4DReadAccessorU64(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessorU64(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessorU64 CreateVolumeData4DReadAccessorU64(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessorU64(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessorR32 CreateVolumeData4DReadAccessorR32(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessorR32(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessorR32 CreateVolumeData4DReadAccessorR32(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessorR32(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessorR64 CreateVolumeData4DReadAccessorR64(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessorR64(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadAccessorR64 CreateVolumeData4DReadAccessorR64(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadOnly);
    return VolumeData4DReadAccessorR64(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessor1Bit CreateVolumeData4DReadWriteAccessor1Bit(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessor1Bit(m_IVolumeDataAccessManager->Create4DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessor1Bit CreateVolumeData4DReadWriteAccessor1Bit(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessor1Bit(m_IVolumeDataAccessManager->Create4DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessorU8 CreateVolumeData4DReadWriteAccessorU8(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessorU8(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU8(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessorU8 CreateVolumeData4DReadWriteAccessorU8(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessorU8(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU8(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessorU16 CreateVolumeData4DReadWriteAccessorU16(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessorU16(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU16(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessorU16 CreateVolumeData4DReadWriteAccessorU16(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessorU16(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU16(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessorU32 CreateVolumeData4DReadWriteAccessorU32(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessorU32(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessorU32 CreateVolumeData4DReadWriteAccessorU32(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessorU32(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessorU64 CreateVolumeData4DReadWriteAccessorU64(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessorU64(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessorU64 CreateVolumeData4DReadWriteAccessorU64(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessorU64(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorU64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessorR32 CreateVolumeData4DReadWriteAccessorR32(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessorR32(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessorR32 CreateVolumeData4DReadWriteAccessorR32(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessorR32(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessorR64 CreateVolumeData4DReadWriteAccessorR64(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPages, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessorR64(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }
  VolumeData4DReadWriteAccessorR64 CreateVolumeData4DReadWriteAccessorR64(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    VolumeDataPageAccessor *volumeDataPageAccessor = m_IVolumeDataAccessManager->CreateVolumeDataPageAccessor(dimensionsND, LOD, channel, maxPagesDefault, IVolumeDataAccessManager::AccessMode_ReadWrite);
    return VolumeData4DReadWriteAccessorR64(m_IVolumeDataAccessManager->Create4DVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue.value_or(volumeDataPageAccessor->GetChannelDescriptor().GetNoValue())));
  }

  /// <summary>
  /// Compute the buffer size (in bytes) for a volume subset request.
  /// </summary>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="format">
  /// Voxel format of the destination buffer.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <returns>
  /// The buffer size needed.
  /// </returns>
  int64_t           
  GetVolumeSubsetBufferSize(const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], VolumeDataChannelDescriptor::Format format, int LOD = 0, int channel = 0)
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->GetVolumeSubsetBufferSize(minVoxelCoordinates, maxVoxelCoordinates, format, LOD, channel);
  }

  template<typename VALUETYPE>
  int64_t
  GetVolumeSubsetBufferSize(const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], int LOD = 0, int channel = 0)
  {
    EnsureValid();
    auto format = VolumeDataRequest::RequestFormat<VALUETYPE>::format;
    return GetVolumeSubsetBufferSize(minVoxelCoordinates, maxVoxelCoordinates, format, LOD, channel);
  }

  /// <summary>
  /// Request a subset of the input VDS.
  /// </summary>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by minVoxelCoordinates and maxVoxelCoordinates.
  /// </param>
  /// <param name="bufferByteSize">
  /// The size of the provided buffer, in bytes.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="format">
  /// Voxel format of the destination buffer.
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  std::shared_ptr<VolumeDataRequest>
  RequestVolumeSubset(void *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], VolumeDataChannelDescriptor::Format format, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    auto request = new VolumeDataRequest(m_IVolumeDataAccessManager, buffer, bufferByteSize, format);
    return DoRequestVolumeSubset(request, buffer, bufferByteSize, dimensionsND, LOD, channel, minVoxelCoordinates, maxVoxelCoordinates, format, replacementNoValue);
  }

  /// <summary>
  /// Request a subset of the input VDS.
  /// </summary>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by minVoxelCoordinates and maxVoxelCoordinates.
  /// </param>
  /// <param name="bufferByteSize">
  /// The size of the provided buffer, in bytes.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  std::shared_ptr<VolumeDataRequest1Bit>
  RequestVolumeSubset1Bit(uint8_t *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max])
  {
    EnsureValid();
    VolumeDataChannelDescriptor::Format format = VolumeDataChannelDescriptor::Format::Format_1Bit;
    auto request = new VolumeDataRequest1Bit(m_IVolumeDataAccessManager, buffer, bufferByteSize);
    return DoRequestVolumeSubset(request, buffer, bufferByteSize, dimensionsND, LOD, channel, minVoxelCoordinates, maxVoxelCoordinates, format, optional<float>());
  }

  /// <summary>
  /// Request a subset of the input VDS.
  /// </summary>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by minVoxelCoordinates and maxVoxelCoordinates.
  /// </param>
  /// <param name="bufferByteSize">
  /// The size of the provided buffer, in bytes.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  template<typename VALUETYPE>
  std::shared_ptr<VolumeDataRequest_t<VALUETYPE>>
  RequestVolumeSubset(VALUETYPE *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    auto format = VolumeDataRequest::RequestFormat<VALUETYPE>::format;
    auto request = new VolumeDataRequest_t<VALUETYPE>(m_IVolumeDataAccessManager, buffer, bufferByteSize);
    return DoRequestVolumeSubset(request, request->Buffer(), bufferByteSize, dimensionsND, LOD, channel, minVoxelCoordinates, maxVoxelCoordinates, format, replacementNoValue);
  }

  /// <summary>
  /// Request a subset of the input VDS, using an automatically allocated buffer.
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="format">
  /// Voxel format of the destination buffer.
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  std::shared_ptr<VolumeDataRequest>
  RequestVolumeSubset(DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], VolumeDataChannelDescriptor::Format format, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    auto bufferByteSize = GetVolumeSubsetBufferSize(minVoxelCoordinates, maxVoxelCoordinates, format, LOD, channel);
    auto request = new VolumeDataRequest(m_IVolumeDataAccessManager, bufferByteSize, format);
    return DoRequestVolumeSubset(request, request->Buffer(), bufferByteSize, dimensionsND, LOD, channel, minVoxelCoordinates, maxVoxelCoordinates, format, replacementNoValue);
  }

  /// <summary>
  /// Request a subset of the input VDS, using an automatically allocated buffer.
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  std::shared_ptr<VolumeDataRequest1Bit>
  RequestVolumeSubset1Bit(DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max])
  {
    EnsureValid();
    VolumeDataChannelDescriptor::Format format = VolumeDataChannelDescriptor::Format::Format_1Bit;
    auto bufferByteSize = GetVolumeSubsetBufferSize(minVoxelCoordinates, maxVoxelCoordinates, format, LOD, channel);
    auto request = new VolumeDataRequest1Bit(m_IVolumeDataAccessManager, bufferByteSize);
    return DoRequestVolumeSubset(request, request->Buffer(), bufferByteSize, dimensionsND, LOD, channel, minVoxelCoordinates, maxVoxelCoordinates, format, optional<float>());
  }

  /// <summary>
  /// Request a subset of the input VDS, using an automatically allocated typed buffer.
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  template<typename VALUETYPE>
  std::shared_ptr<VolumeDataRequest_t<VALUETYPE>>
  RequestVolumeSubset(DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    auto format = VolumeDataRequest::RequestFormat<VALUETYPE>::format;
    auto bufferByteSize = GetVolumeSubsetBufferSize(minVoxelCoordinates, maxVoxelCoordinates, format, LOD, channel);
    auto request = new VolumeDataRequest_t<VALUETYPE>(m_IVolumeDataAccessManager, bufferByteSize);
    return DoRequestVolumeSubset(request, request->Buffer(), bufferByteSize, dimensionsND, LOD, channel, minVoxelCoordinates, maxVoxelCoordinates, format, replacementNoValue);
  }

  /// <summary>
  /// Compute the buffer size (in bytes) for a projected volume subset request.
  /// </summary>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="projectedDimensions">
  /// The 2D dimension group that the plane in the source dimensiongroup is projected into. It must be a 2D subset of the source dimensions.
  /// </param>
  /// <param name="format">
  /// Voxel format of the destination buffer.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <returns>
  /// The buffer size needed.
  /// </returns>
  int64_t           
  GetProjectedVolumeSubsetBufferSize(const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, int LOD = 0, int channel = 0)
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->GetProjectedVolumeSubsetBufferSize(minVoxelCoordinates, maxVoxelCoordinates, projectedDimensions, format, LOD, channel);
  }

  template<typename VALUETYPE>
  int64_t           
  GetProjectedVolumeSubsetBufferSize(const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], DimensionsND projectedDimensions, int LOD = 0, int channel = 0)
  {
    EnsureValid();
    auto format = VolumeDataRequest::RequestFormat<VALUETYPE>::format;
    return GetProjectedVolumeSubsetBufferSize(minVoxelCoordinates, maxVoxelCoordinates, projectedDimensions, format, LOD, channel);
  }

  /// <summary>
  /// Request a subset projected from an arbitrary 3D plane through the subset onto one of the sides of the subset.
  /// </summary>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by minVoxelCoordinates and maxVoxelCoordinates for the projected dimensions.
  /// </param>
  /// <param name="bufferByteSize">
  /// The size of the provided buffer, in bytes.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="voxelPlane">
  /// The plane equation for the projection from the dimension source to the projected dimensions (which must be a 2D subset of the source dimensions).
  /// </param>
  /// <param name="projectedDimensions">
  /// The 2D dimension group that the plane in the source dimensiongroup is projected into. It must be a 2D subset of the source dimensions.
  /// </param>
  /// <param name="interpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="format">
  /// Voxel format of the destination buffer.
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  std::shared_ptr<VolumeDataRequest>
  RequestProjectedVolumeSubset(void *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    auto request = new VolumeDataRequest(m_IVolumeDataAccessManager, buffer, bufferByteSize, format);
    return DoRequestProjectedVolumeSubset(request, buffer, bufferByteSize, dimensionsND, LOD, channel, minVoxelCoordinates, maxVoxelCoordinates, voxelPlane, projectedDimensions, format, interpolationMethod, replacementNoValue);
  }

  /// <summary>
  /// Request a subset projected from an arbitrary 3D plane through the subset onto one of the sides of the subset, using an automatically allocated buffer.
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="voxelPlane">
  /// The plane equation for the projection from the dimension source to the projected dimensions (which must be a 2D subset of the source dimensions).
  /// </param>
  /// <param name="projectedDimensions">
  /// The 2D dimension group that the plane in the source dimensiongroup is projected into. It must be a 2D subset of the source dimensions.
  /// </param>
  /// <param name="format">
  /// Voxel format of the destination buffer.
  /// </param>
  /// <param name="interpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  std::shared_ptr<VolumeDataRequest>
  RequestProjectedVolumeSubset(DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    auto bufferByteSize = GetProjectedVolumeSubsetBufferSize(minVoxelCoordinates, maxVoxelCoordinates, projectedDimensions, format, LOD, channel);
    auto request = new VolumeDataRequest(m_IVolumeDataAccessManager, bufferByteSize, format);
    return DoRequestProjectedVolumeSubset(request, request->Buffer(), bufferByteSize, dimensionsND, LOD, channel, minVoxelCoordinates, maxVoxelCoordinates, voxelPlane, projectedDimensions, format, interpolationMethod, replacementNoValue);
  }

  /// <summary>
  /// Request a subset projected from an arbitrary 3D plane through the subset onto one of the sides of the subset, using an automatically allocated typed buffer.
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="minVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="maxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="voxelPlane">
  /// The plane equation for the projection from the dimension source to the projected dimensions (which must be a 2D subset of the source dimensions).
  /// </param>
  /// <param name="projectedDimensions">
  /// The 2D dimension group that the plane in the source dimensiongroup is projected into. It must be a 2D subset of the source dimensions.
  /// </param>
  /// <param name="interpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  template<typename VALUETYPE>
  std::shared_ptr<VolumeDataRequest_t<VALUETYPE>>
  RequestProjectedVolumeSubset(DimensionsND dimensionsND, int LOD, int channel, const int (&minVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], const int (&maxVoxelCoordinates)[VolumeDataLayout::Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    auto format = VolumeDataRequest::RequestFormat<VALUETYPE>::format;
    auto bufferByteSize = GetProjectedVolumeSubsetBufferSize(minVoxelCoordinates, maxVoxelCoordinates, projectedDimensions, format, LOD, channel);
    auto request = new VolumeDataRequest_t<VALUETYPE>(m_IVolumeDataAccessManager, bufferByteSize);
    return DoRequestProjectedVolumeSubset(request, request->Buffer(), bufferByteSize, dimensionsND, LOD, channel, minVoxelCoordinates, maxVoxelCoordinates, voxelPlane, projectedDimensions, format, interpolationMethod, replacementNoValue);
  }

  /// <summary>
  /// Compute the buffer size (in bytes) for a volume samples request.
  /// </summary>
  /// <param name="sampleCount">
  /// Number of samples to request.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <returns>
  /// The buffer size needed.
  /// </returns>
  int64_t           
  GetVolumeSamplesBufferSize(int sampleCount, int channel = 0)
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->GetVolumeSamplesBufferSize(sampleCount, channel);
  }

  /// <summary>
  /// Request sampling of the input VDS at the specified coordinates.
  /// </summary>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least sampleCount elements.
  /// </param>
  /// <param name="bufferByteSize">
  /// The size of the provided buffer, in bytes.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="samplePositions">
  /// Pointer to array of VolumeDataLayout::Dimensionality_Max-elements indicating the positions to sample. May be deleted once RequestVolumeSamples return, as HueSpace makes a deep copy of the data.
  /// </param>
  /// <param name="sampleCount">
  /// Number of samples to request.
  /// </param>
  /// <param name="interpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  std::shared_ptr<VolumeDataRequestFloat>
  RequestVolumeSamples(float *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const float (*samplePositions)[VolumeDataLayout::Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    auto request = new VolumeDataRequestFloat(m_IVolumeDataAccessManager, buffer, bufferByteSize);
    return DoRequestVolumeSamples(request, buffer, bufferByteSize, dimensionsND, LOD, channel, samplePositions, sampleCount, interpolationMethod, replacementNoValue);
  }

  /// <summary>
  /// Request sampling of the input VDS at the specified coordinates, using an automatically allocated buffer.
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="samplePositions">
  /// Pointer to array of VolumeDataLayout::Dimensionality_Max-elements indicating the positions to sample. May be deleted once RequestVolumeSamples return, as HueSpace makes a deep copy of the data.
  /// </param>
  /// <param name="sampleCount">
  /// Number of samples to request.
  /// </param>
  /// <param name="interpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  std::shared_ptr<VolumeDataRequestFloat>
  RequestVolumeSamples(DimensionsND dimensionsND, int LOD, int channel, const float (*samplePositions)[VolumeDataLayout::Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    auto bufferByteSize = GetVolumeSamplesBufferSize(sampleCount, channel);
    auto request = new VolumeDataRequestFloat(m_IVolumeDataAccessManager, bufferByteSize);
    return DoRequestVolumeSamples(request, (float*)request->Buffer(), bufferByteSize, dimensionsND, LOD, channel, samplePositions, sampleCount, interpolationMethod, replacementNoValue);
  }

  /// <summary>
  /// Compute the buffer size (in bytes) for a volume traces request.
  /// </summary>
  /// <param name="traceCount">
  /// Number of traces to request.
  /// </param>
  /// <param name="traceDimension">
  /// The dimension to trace
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <returns>
  /// The buffer size needed.
  /// </returns>
  int64_t           
  GetVolumeTracesBufferSize(int traceCount, int traceDimension, int LOD = 0, int channel = 0)
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->GetVolumeTracesBufferSize(traceCount, traceDimension, LOD, channel);
  }

  /// <summary>
  /// Request traces from the input VDS, using an automatically allocated buffer.
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="tracePositions">
  /// Pointer to array of traceCount VolumeDataLayout::Dimensionality_Max-elements indicating the trace positions.
  /// </param>
  /// <param name="traceCount">
  /// Number of traces to request.
  /// </param>
  /// <param name="interpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="traceDimension">
  /// The dimension to trace
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  std::shared_ptr<VolumeDataRequestFloat>
  RequestVolumeTraces(DimensionsND dimensionsND, int LOD, int channel, const float(*tracePositions)[VolumeDataLayout::Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int traceDimension, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    auto bufferByteSize = GetVolumeTracesBufferSize(traceCount, traceDimension, LOD, channel);
    auto request = new VolumeDataRequestFloat(m_IVolumeDataAccessManager, bufferByteSize);
    return DoRequestVolumeTraces(request, (float*)request->Buffer(), bufferByteSize, dimensionsND, LOD, channel, tracePositions, traceCount, interpolationMethod, traceDimension, replacementNoValue);
  }

  /// <summary>
  /// Request traces from the input VDS.
  /// </summary>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least traceCount * number of samples in the traceDimension.
  /// </param>
  /// <param name="bufferByteSize">
  /// The size of the provided buffer, in bytes.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="tracePositions">
  /// Pointer to array of traceCount VolumeDataLayout::Dimensionality_Max-elements indicating the trace positions.
  /// </param>
  /// <param name="traceCount">
  /// Number of traces to request.
  /// </param>
  /// <param name="interpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="traceDimension">
  /// The dimension to trace
  /// </param>
  /// <param name="replacementNoValue">
  /// If specified, this value is used to replace regions of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status and buffer.
  /// </returns>
  std::shared_ptr<VolumeDataRequestFloat>
  RequestVolumeTraces(float *buffer, int64_t bufferByteSize, DimensionsND dimensionsND, int LOD, int channel, const float(*tracePositions)[VolumeDataLayout::Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int traceDimension, optional<float> replacementNoValue = optional<float>())
  {
    EnsureValid();
    auto request = new VolumeDataRequestFloat(m_IVolumeDataAccessManager, buffer, bufferByteSize);
    return DoRequestVolumeTraces(request, buffer, bufferByteSize, dimensionsND, LOD, channel, tracePositions, traceCount, interpolationMethod, traceDimension, replacementNoValue);
  }

  /// <summary>
  /// Force production of a specific volume data chunk
  /// </summary>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested chunk belongs to.
  /// </param>
  /// <param name="LOD">
  /// The LOD level the requested chunk belongs to.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested chunk belongs to.
  /// </param>
  /// <param name="chunkIndex">
  /// The index of the chunk to prefetch.
  /// </param>
  /// <returns>
  /// A VolumeDataRequest instance encapsulating the request status.
  /// </returns>
  std::shared_ptr<VolumeDataRequest>
  PrefetchVolumeChunk(DimensionsND dimensionsND, int LOD, int channel, int64_t chunkIndex)
  {
    EnsureValid();
    auto request = new VolumeDataRequest(m_IVolumeDataAccessManager);
    request->SetJobID(m_IVolumeDataAccessManager->PrefetchVolumeChunk(dimensionsND, LOD, channel, chunkIndex));
    return std::shared_ptr<VolumeDataRequest>(request, &VolumeDataRequest::Deleter);
  }

  /// <summary>
  /// Flush any pending writes and write updated layer status
  /// </summary>
  /// <param name="writeUpdatedLayerStatus">
  /// Write the updated layer status (or only flush pending writes of chunks and chunk-metadata).
  /// </param>
  void
  FlushUploadQueue(bool writeUpdatedLayerStatus = true)
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->FlushUploadQueue(writeUpdatedLayerStatus);
  }

  /// <summary>
  /// Clear all upload errors that have been retrieved
  /// </summary>
  void ClearUploadErrors()
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->ClearUploadErrors();
  }

  /// <summary>
  /// Clear all upload errors
  /// </summary>
  void ForceClearAllUploadErrors()
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->ForceClearAllUploadErrors();
  }

  /// <summary>
  /// Get the number of unretrieved upload errors
  /// </summary>
  int32_t
  UploadErrorCount()
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->UploadErrorCount();
  }

  /// <summary>
  /// Get the next unretrieved upload error or an empty error if there are no more errors to retrieve
  /// </summary>
  void
  GetCurrentUploadError(const char **objectId, int32_t *errorCode, const char **errorString)
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->GetCurrentUploadError(objectId, errorCode, errorString);
  }

  /// <summary>
  /// Get the download error from the most recent operation that failed
  /// </summary>
  void
  GetCurrentDownloadError(int *code, const char** errorString)
  {
    EnsureValid();
    return m_IVolumeDataAccessManager->GetCurrentDownloadError(code, errorString);
  }
};

template<> inline VolumeDataReadAccessor<IntVector2, double>   VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessorR64(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector2, uint64_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessorU64(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector2, float>    VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessorR32(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector2, uint32_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessorU32(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector2, uint16_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessorU16(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector2, uint8_t>  VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessorU8(dimensionsND, LOD, channel, maxPages, replacementNoValue);   }
template<> inline VolumeDataReadAccessor<IntVector2, bool>     VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessor1Bit(dimensionsND, LOD, channel, maxPages, replacementNoValue); }
template<> inline VolumeDataReadAccessor<IntVector3, double>   VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessorR64(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector3, uint64_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessorU64(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector3, float>    VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessorR32(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector3, uint32_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessorU32(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector3, uint16_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessorU16(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector3, uint8_t>  VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessorU8(dimensionsND, LOD, channel, maxPages, replacementNoValue);   }
template<> inline VolumeDataReadAccessor<IntVector3, bool>     VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessor1Bit(dimensionsND, LOD, channel, maxPages, replacementNoValue); }
template<> inline VolumeDataReadAccessor<IntVector4, double>   VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessorR64(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector4, uint64_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessorU64(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector4, float>    VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessorR32(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector4, uint32_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessorU32(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector4, uint16_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessorU16(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector4, uint8_t>  VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessorU8(dimensionsND, LOD, channel, maxPages, replacementNoValue);   }
template<> inline VolumeDataReadAccessor<IntVector4, bool>     VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessor1Bit(dimensionsND, LOD, channel, maxPages, replacementNoValue); }

template<> inline VolumeDataReadAccessor<IntVector2, double>   VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessorR64(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector2, uint64_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessorU64(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector2, float>    VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessorR32(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector2, uint32_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessorU32(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector2, uint16_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessorU16(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector2, uint8_t>  VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessorU8(dimensionsND, LOD, channel, replacementNoValue);   }
template<> inline VolumeDataReadAccessor<IntVector2, bool>     VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData2DReadAccessor1Bit(dimensionsND, LOD, channel, replacementNoValue); }
template<> inline VolumeDataReadAccessor<IntVector3, double>   VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessorR64(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector3, uint64_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessorU64(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector3, float>    VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessorR32(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector3, uint32_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessorU32(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector3, uint16_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessorU16(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector3, uint8_t>  VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessorU8(dimensionsND, LOD, channel, replacementNoValue);   }
template<> inline VolumeDataReadAccessor<IntVector3, bool>     VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData3DReadAccessor1Bit(dimensionsND, LOD, channel, replacementNoValue); }
template<> inline VolumeDataReadAccessor<IntVector4, double>   VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessorR64(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector4, uint64_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessorU64(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector4, float>    VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessorR32(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector4, uint32_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessorU32(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector4, uint16_t> VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessorU16(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadAccessor<IntVector4, uint8_t>  VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessorU8(dimensionsND, LOD, channel, replacementNoValue);   }
template<> inline VolumeDataReadAccessor<IntVector4, bool>     VolumeDataAccessManager::CreateVolumeDataReadAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue) { return CreateVolumeData4DReadAccessor1Bit(dimensionsND, LOD, channel, replacementNoValue); }

template<> inline VolumeDataReadWriteAccessor<IntVector2, double>   VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessorR64(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint64_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessorU64(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector2, float>    VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessorR32(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint32_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessorU32(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint16_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessorU16(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint8_t>  VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessorU8(dimensionsND, LOD, channel, maxPages, replacementNoValue);   }
template<> inline VolumeDataReadWriteAccessor<IntVector2, bool>     VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessor1Bit(dimensionsND, LOD, channel, maxPages, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, double>   VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessorR64(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint64_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessorU64(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector3, float>    VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessorR32(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint32_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessorU32(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint16_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessorU16(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint8_t>  VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessorU8(dimensionsND, LOD, channel, maxPages, replacementNoValue);   }
template<> inline VolumeDataReadWriteAccessor<IntVector3, bool>     VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessor1Bit(dimensionsND, LOD, channel, maxPages, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, double>   VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessorR64(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint64_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessorU64(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector4, float>    VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessorR32(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint32_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessorU32(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint16_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessorU16(dimensionsND, LOD, channel, maxPages, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint8_t>  VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessorU8(dimensionsND, LOD, channel, maxPages, replacementNoValue);   }
template<> inline VolumeDataReadWriteAccessor<IntVector4, bool>     VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, int maxPages, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessor1Bit(dimensionsND, LOD, channel, maxPages, replacementNoValue); }

template<> inline VolumeDataReadWriteAccessor<IntVector2, double>   VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessorR64(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint64_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessorU64(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector2, float>    VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessorR32(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint32_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessorU32(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint16_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessorU16(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint8_t>  VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessorU8(dimensionsND, LOD, channel, replacementNoValue);   }
template<> inline VolumeDataReadWriteAccessor<IntVector2, bool>     VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData2DReadWriteAccessor1Bit(dimensionsND, LOD, channel, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, double>   VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessorR64(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint64_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessorU64(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector3, float>    VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessorR32(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint32_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessorU32(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint16_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessorU16(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint8_t>  VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessorU8(dimensionsND, LOD, channel, replacementNoValue);   }
template<> inline VolumeDataReadWriteAccessor<IntVector3, bool>     VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData3DReadWriteAccessor1Bit(dimensionsND, LOD, channel, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, double>   VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessorR64(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint64_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessorU64(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector4, float>    VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessorR32(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint32_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessorU32(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint16_t> VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessorU16(dimensionsND, LOD, channel, replacementNoValue);  }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint8_t>  VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessorU8(dimensionsND, LOD, channel, replacementNoValue);   }
template<> inline VolumeDataReadWriteAccessor<IntVector4, bool>     VolumeDataAccessManager::CreateVolumeDataReadWriteAccessor(DimensionsND dimensionsND, int LOD, int channel, optional<float> replacementNoValue){ return CreateVolumeData4DReadWriteAccessor1Bit(dimensionsND, LOD, channel, replacementNoValue); }

template<> inline VolumeDataReadAccessor<FloatVector2, double> VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector2, double>(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData2DInterpolatingAccessorR64(dimensionsND, LOD, channel, interpolationMethod, maxPages, replacementNoValue); }
template<> inline VolumeDataReadAccessor<FloatVector2, float>  VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector2, float> (DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData2DInterpolatingAccessorR32(dimensionsND, LOD, channel, interpolationMethod, maxPages, replacementNoValue); }
template<> inline VolumeDataReadAccessor<FloatVector3, double> VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector3, double>(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData3DInterpolatingAccessorR64(dimensionsND, LOD, channel, interpolationMethod, maxPages, replacementNoValue); }
template<> inline VolumeDataReadAccessor<FloatVector3, float>  VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector3, float> (DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData3DInterpolatingAccessorR32(dimensionsND, LOD, channel, interpolationMethod, maxPages, replacementNoValue); }
template<> inline VolumeDataReadAccessor<FloatVector4, double> VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector4, double>(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData4DInterpolatingAccessorR64(dimensionsND, LOD, channel, interpolationMethod, maxPages, replacementNoValue); }
template<> inline VolumeDataReadAccessor<FloatVector4, float>  VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector4, float> (DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, int maxPages, optional<float> replacementNoValue) { return CreateVolumeData4DInterpolatingAccessorR32(dimensionsND, LOD, channel, interpolationMethod, maxPages, replacementNoValue); }

template<> inline VolumeDataReadAccessor<FloatVector2, double> VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector2, double>(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue) { return CreateVolumeData2DInterpolatingAccessorR64(dimensionsND, LOD, channel, interpolationMethod, replacementNoValue); }
template<> inline VolumeDataReadAccessor<FloatVector2, float>  VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector2, float> (DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue) { return CreateVolumeData2DInterpolatingAccessorR32(dimensionsND, LOD, channel, interpolationMethod, replacementNoValue); }
template<> inline VolumeDataReadAccessor<FloatVector3, double> VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector3, double>(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue) { return CreateVolumeData3DInterpolatingAccessorR64(dimensionsND, LOD, channel, interpolationMethod, replacementNoValue); }
template<> inline VolumeDataReadAccessor<FloatVector3, float>  VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector3, float> (DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue) { return CreateVolumeData3DInterpolatingAccessorR32(dimensionsND, LOD, channel, interpolationMethod, replacementNoValue); }
template<> inline VolumeDataReadAccessor<FloatVector4, double> VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector4, double>(DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue) { return CreateVolumeData4DInterpolatingAccessorR64(dimensionsND, LOD, channel, interpolationMethod, replacementNoValue); }
template<> inline VolumeDataReadAccessor<FloatVector4, float>  VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector4, float> (DimensionsND dimensionsND, int LOD, int channel, InterpolationMethod interpolationMethod, optional<float> replacementNoValue) { return CreateVolumeData4DInterpolatingAccessorR32(dimensionsND, LOD, channel, interpolationMethod, replacementNoValue); }

} /* namespace OpenVDS*/

#endif // VOLUMEDATAACCESSMANAGER_H
