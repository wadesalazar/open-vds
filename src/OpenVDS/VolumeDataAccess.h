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

#ifndef VOLUMEDATAACCESS_H
#define VOLUMEDATAACCESS_H

#include <OpenVDS/VolumeData.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <Math/Vector.h>

namespace OpenVDS {

class VolumeDataPageAccessor;
class VolumeDataLayout;
struct VDSHandle;

template <typename INDEX, typename T> class VolumeDataReadAccessor;
template <typename INDEX, typename T> class VolumeDataReadWriteAccessor;

template <typename T>
struct IndexRegion
{
    T min;
    T max;

    IndexRegion() {}
    IndexRegion(T min, T max) : min(min), max(max) {}
};

class VolumeDataAccessor
{
protected:
                VolumeDataAccessor() {}
  virtual      ~VolumeDataAccessor() {}
public:
  class Manager
  {
  protected:
                  Manager() {}
    virtual      ~Manager() {}
  public:
    virtual void  destroyVolumeDataAccessor(VolumeDataAccessor *accessor) = 0;

    virtual VolumeDataAccessor *cloneVolumeDataAccessor(VolumeDataAccessor const &accessor) = 0;
  };

  virtual Manager &getManager() = 0;

  virtual VolumeDataLayout const *getLayout() = 0;

  struct IndexOutOfRangeException {};
};

class VolumeDataAccess : public VolumeDataAccessor::Manager
{
protected:
                VolumeDataAccess() {}
  virtual      ~VolumeDataAccess() {}
public:
  /// <summary>
  /// Get the VolumeDataLayout object for a VDS.
  /// </summary>
  /// <param name="cVDS">
  /// The VDS object to get the layout for.
  /// </param>
  /// <returns>
  /// The VolumeDataLayout object associated with the VDS or NULL if there is no valid VolumeDataLayout.
  /// </returns>
  virtual VolumeDataLayout const *getVolumeDataLayout(VDSHandle *VDS) const = 0;

  /// <summary>
  /// Create a volume data page accessor object for the VDS associated with the given VolumeDataLayout object.
  /// </summary>
  /// <param name="pVolumeDataLayout">
  /// The VolumeDataLayout object associated with the VDS that the volume data page accessor will access.
  /// </param>
  /// <param name="eDimensionsND">
  /// The dimensions group that the volume data page accessor will access.
  /// </param>
  /// <param name="lod">
  /// The lod level that the volume data page accessor will access.
  /// </param>
  /// <param name="iChannel">
  /// The channel index that the volume data page accessor will access.
  /// </param>
  /// <param name="nMaxPages">
  /// The maximum number of pages that the volume data page accessor will cache.
  /// </param>
  /// <param name="isReadWrite">
  /// Set to true to make a read/write accessor. Read/write accessors can only be made for VDSDirect.
  /// </param>
  /// <returns>
  /// A VolumeDataPageAccessor object for the VDS associated with the given VolumeDataLayout object.
  /// </returns>
  virtual VolumeDataPageAccessor *createVolumeDataPageAccessor(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int maxPages, bool isReadWrite) = 0;

  /// <summary>
  /// Destroy a volume data page accessor object.
  /// </summary>
  /// <param name="pVolumeDataPageAccessor">
  /// The VolumeDataPageAccessor object to destroy.
  /// </param>
  virtual void  destroyVolumeDataPageAccessor(VolumeDataPageAccessor *volumeDataPageAccessor) = 0;

  virtual void  destroyVolumeDataAccessor(VolumeDataAccessor *accessor) = 0;

  virtual VolumeDataAccessor * cloneVolumeDataAccessor(VolumeDataAccessor const &accessor) = 0;

  template<typename INDEX, typename T>
  VolumeDataReadWriteAccessor<INDEX, T> *createVolumeDataAccessor(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue);

  template<typename INDEX, typename T>
  VolumeDataReadAccessor<INDEX, T > *createInterpolatingVolumeDataAccessor(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod);

  virtual VolumeDataReadWriteAccessor<IntVector2, bool>     *create2DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector2, uint8_t>  *create2DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector2, uint16_t> *create2DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector2, uint32_t> *create2DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector2, uint64_t> *create2DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector2, float>    *create2DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector2, double>   *create2DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;

  virtual VolumeDataReadWriteAccessor<IntVector3, bool>     *create3DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector3, uint8_t>  *create3DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector3, uint16_t> *create3DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector3, uint32_t> *create3DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector3, uint64_t> *create3DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector3, float>    *create3DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector3, double>   *create3DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;

  virtual VolumeDataReadWriteAccessor<IntVector4, bool>     *create4DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector4, uint8_t>  *create4DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector4, uint16_t> *create4DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector4, uint32_t> *create4DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector4, uint64_t> *create4DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector4, float>    *create4DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector4, double>   *create4DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;

  virtual VolumeDataReadAccessor<FloatVector2, float > *create2DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual VolumeDataReadAccessor<FloatVector2, double> *create2DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual VolumeDataReadAccessor<FloatVector3, float > *create3DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual VolumeDataReadAccessor<FloatVector3, double> *create3DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual VolumeDataReadAccessor<FloatVector4, float > *create4DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual VolumeDataReadAccessor<FloatVector4, double> *create4DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;

  /// <summary>
  /// Request a subset of the input VDS.
  /// </summary>
  /// <param name="pBuffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by MinVoxelCoordinates and MaxVoxelCoordinates.
  /// </param>
  /// <param name="pVolumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="eDimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
  /// The lod level the requested data is read from.
  /// </param>
  /// <param name="iChannel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="MinVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="MaxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="eFormat">
  /// Voxel format the final buffer should be in.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t requestVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format) = 0;

  /// <summary>
  /// Request a subset of the input VDS.
  /// </summary>
  /// <param name="pBuffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by MinVoxelCoordinates and MaxVoxelCoordinates.
  /// </param>
  /// <param name="pVolumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="eDimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
  /// The lod level the requested data is read from.
  /// </param>
  /// <param name="iChannel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="MinVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="MaxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="eFormat">
  /// Voxel format the final buffer should be in.
  /// </param>
  /// <param name="rReplacementNoValue">
  /// Value used to replace region of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t requestVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lOD, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, float replacementNoValue) = 0;

  /// <summary>
  /// Request a subset projected from an arbitrary 3D plane through the subset onto one of the sides of the subset.
  /// </summary>
  /// <param name="pVolumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="pBuffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by MinVoxelCoordinates and MaxVoxelCoordinates for the projected dimensions.
  /// </param>
  /// <param name="eDimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
  /// The lod level the requested data is read from.
  /// </param>
  /// <param name="iChannel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="MinVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="MaxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="cVoxelPlane">
  /// The plane equation for the projection from the dimension source to the projected dimensions (which must be a 2D subset of the source dimensions).
  /// </param>
  /// <param name="eProjectedDimensions">
  /// The 2D dimension group that the plane in the source dimensiongroup is projected into. It must be a 2D subset of the source dimensions.
  /// </param>
  /// <param name="eInterpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="eFormat">
  /// Voxel format the final buffer should be in.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t requestProjectedVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod) = 0;

  /// <summary>
  /// Request a subset projected from an arbitrary 3D plane through the subset onto one of the sides of the subset.
  /// </summary>
  /// <param name="pVolumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="pBuffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by MinVoxelCoordinates and MaxVoxelCoordinates for the projected dimensions.
  /// </param>
  /// <param name="eDimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
  /// The lod level the requested data is read from.
  /// </param>
  /// <param name="iChannel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="MinVoxelCoordinates">
  /// The minimum voxel coordinates to request in each dimension (inclusive).
  /// </param>
  /// <param name="MaxVoxelCoordinates">
  /// The maximum voxel coordinates to request in each dimension (exclusive).
  /// </param>
  /// <param name="cVoxelPlane">
  /// The plane equation for the projection from the dimension source to the projected dimensions (which must be a 2D subset of the source dimensions).
  /// </param>
  /// <param name="eProjectedDimensions">
  /// The 2D dimension group that the plane in the source dimensiongroup is projected into. It must be a 2D subset of the source dimensions.
  /// </param>
  /// <param name="eInterpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="eFormat">
  /// Voxel format the final buffer should be in.
  /// </param>
  /// <param name="rReplacementNoValue">
  /// Value used to replace region of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t requestProjectedVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, float replacementNoValue) = 0;

  /// <summary>
  /// Request sampling of the input VDS at the specified coordinates.
  /// </summary>
  /// <param name="pVolumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="pBuffer">
  /// Pointer to a preallocated buffer holding at least nSampleCount elements.
  /// </param>
  /// <param name="eDimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
  /// The lod level the requested data is read from.
  /// </param>
  /// <param name="iChannel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="SamplePositions">
  /// Pointer to array of Dimensionality_Max-elements indicating the positions to sample. May be deleted once requestVolumeSamples return, as OpenVDS makes a deep copy of the data.
  /// </param>
  /// <param name="nSampleCount">
  /// Number of samples to request.
  /// </param>
  /// <param name="eInterpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t requestVolumeSamples(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float (*samplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod) = 0;

  /// <summary>
  /// Request sampling of the input VDS at the specified coordinates.
  /// </summary>
  /// <param name="pVolumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="pBuffer">
  /// Pointer to a preallocated buffer holding at least nSampleCount elements.
  /// </param>
  /// <param name="eDimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
  /// The lod level the requested data is read from.
  /// </param>
  /// <param name="iChannel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="SamplePositions">
  /// Pointer to array of Dimensionality_Max-elements indicating the positions to sample. May be deleted once requestVolumeSamples return, as OpenVDS makes a deep copy of the data.
  /// </param>
  /// <param name="nSampleCount">
  /// Number of samples to request.
  /// </param>
  /// <param name="eInterpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="rReplacementNoValue">
  /// Value used to replace region of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t requestVolumeSamples(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float (*SamplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, float replacementNoValue) = 0;

  /// <summary>
  /// Request traces from the input VDS.
  /// </summary>
  /// <param name="pVolumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="pBuffer">
  /// Pointer to a preallocated buffer holding at least nTraceCount * number of samples in the iTraceDimension.
  /// </param>
  /// <param name="eDimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
  /// The lod level the requested data is read from.
  /// </param>
  /// <param name="iChannel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="TracePositions">
  /// Pointer to array of nTraceCount VolumeDataLayout::Dimensionality_Max-elements indicating the trace positions.
  /// </param>
  /// <param name="nTraceCount">
  /// Number of traces to request.
  /// </param>
  /// <param name="eInterpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="iTraceDimension">
  /// The dimension to trace
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t requestVolumeTraces(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int iTraceDimension) = 0;

  /// <summary>
  /// Request traces from the input VDS.
  /// </summary>
  /// <param name="pVolumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="pBuffer">
  /// Pointer to a preallocated buffer holding at least nTraceCount * number of samples in the iTraceDimension.
  /// </param>
  /// <param name="eDimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
  /// The lod level the requested data is read from.
  /// </param>
  /// <param name="iChannel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="TracePositions">
  /// Pointer to array of nTraceCount VolumeDataLayout::Dimensionality_Max-elements indicating the trace positions.
  /// </param>
  /// <param name="nTraceCount">
  /// Number of traces to request.
  /// </param>
  /// <param name="eInterpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="iTraceDimension">
  /// The dimension to trace
  /// </param>
  /// <param name="rReplacementNoValue">
  /// Value used to replace region of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t requestVolumeTraces(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int nTraceCount, InterpolationMethod eInterpolationMethod, int iTraceDimension, float rReplacementNoValue) = 0;

  /// <summary>
  /// Force production of a specific volume data chunk
  /// </summary>
  /// <param name="pVolumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="eDimensionsND">
  /// The dimensiongroup the requested chunk belongs to.
  /// </param>
  /// <param name="lod">
  /// The lod level the requested chunk belongs to.
  /// </param>
  /// <param name="iChannel">
  /// The channel index the requested chunk belongs to.
  /// </param>
  /// <param name="iChunk">
  /// The index of the chunk to prefetch.
  /// </param>
  /// <returns>
  /// The RequestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t prefetchVolumeChunk(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int64_t chunk) = 0;

  /// <summary>
  /// Check if a request completed successfully. If the request completed, the buffer now contains valid data.
  /// </summary>
  /// <param name="iRequestID">
  /// The RequestID to check for completion.
  /// </param>
  /// <returns>
  /// Either IsCompleted, IsCanceled or WaitForCompletion will return true a single time, after that the request is taken out of the system.
  /// </returns>
  virtual bool  isCompleted(int64_t requestID) = 0;

  /// <summary>
  /// Check if a request was canceled (e.g. the VDS was invalidated before the request was processed). If the request was canceled, the buffer does not contain valid data.
  /// </summary>
  /// <param name="iRequestID">
  /// The RequestID to check for cancellation.
  /// </param>
  /// <returns>
  /// Either IsCompleted, IsCanceled or WaitForCompletion will return true a single time, after that the request is taken out of the system.
  /// </returns>
  virtual bool  isCanceled(int64_t requestID) = 0;

  /// <summary>
  /// Wait for a request to complete successfully. If the request completed, the buffer now contains valid data.
  /// </summary>
  /// <param name="iRequestID">
  /// The RequestID to wait for completion of.
  /// </param>
  /// <param name="nMillisecondsBeforeTimeout">
  /// The number of milliseconds to wait before timing out (optional). A value of 0 indicates there is no timeout and we will wait for
  /// however long it takes. Note that the request is not automatically canceled if the wait times out, you can also use this mechanism
  /// to e.g. update a progress bar while waiting. If you want to cancel the request you have to explicitly call CancelRequest() and
  /// then wait for the request to stop writing to the buffer.
  /// </param>
  /// <returns>
  /// Either IsCompleted, IsCanceled or WaitForCompletion will return true a single time, after that the request is taken out of the system.
  /// Whenever WaitForCompletion returns false you need to call IsCanceled() to know if that was because of a timeout or if the request was canceled.
  /// </returns>
  virtual bool  waitForCompletion(int64_t requestID, int millisecondsBeforeTimeout = 0) = 0;

  /// <summary>
  /// Try to cancel the request. You still have to call WaitForCompletion/IsCanceled to make sure the buffer is not being written to and to take the job out of the system.
  /// It is possible that the request has completed concurrently with the call to Cancel in which case WaitForCompletion will return true.
  /// </summary>
  /// <param name="iRequestID">
  /// The RequestID to cancel.
  /// </param>
  virtual void  cancel(int64_t requestID) = 0;

  /// <summary>
  /// Get the completion factor (between 0 and 1) of the request.
  /// </summary>
  /// <param name="iRequestID">
  /// The RequestID to get the completion factor of.
  /// </param>
  /// <returns>
  /// A factor (between 0 and 1) indicating how much of the request has been completed.
  /// </returns>
  virtual float getCompletionFactor(int64_t requestID) = 0;
};

template<> inline VolumeDataReadWriteAccessor<IntVector4, double>   *VolumeDataAccess::createVolumeDataAccessor<IntVector4, double>  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create4DVolumeDataAccessorR64 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, double>   *VolumeDataAccess::createVolumeDataAccessor<IntVector3, double>  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create3DVolumeDataAccessorR64 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, double>   *VolumeDataAccess::createVolumeDataAccessor<IntVector2, double>  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create2DVolumeDataAccessorR64 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, float>    *VolumeDataAccess::createVolumeDataAccessor<IntVector4, float>   (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create4DVolumeDataAccessorR32 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, float>    *VolumeDataAccess::createVolumeDataAccessor<IntVector3, float>   (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create3DVolumeDataAccessorR32 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, float>    *VolumeDataAccess::createVolumeDataAccessor<IntVector2, float>   (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create2DVolumeDataAccessorR32 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint64_t> *VolumeDataAccess::createVolumeDataAccessor<IntVector4, uint64_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create4DVolumeDataAccessorU64 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint64_t> *VolumeDataAccess::createVolumeDataAccessor<IntVector3, uint64_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create3DVolumeDataAccessorU64 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint64_t> *VolumeDataAccess::createVolumeDataAccessor<IntVector2, uint64_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create2DVolumeDataAccessorU64 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint32_t> *VolumeDataAccess::createVolumeDataAccessor<IntVector4, uint32_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create4DVolumeDataAccessorU32 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint32_t> *VolumeDataAccess::createVolumeDataAccessor<IntVector3, uint32_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create3DVolumeDataAccessorU32 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint32_t> *VolumeDataAccess::createVolumeDataAccessor<IntVector2, uint32_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create2DVolumeDataAccessorU32 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint16_t> *VolumeDataAccess::createVolumeDataAccessor<IntVector4, uint16_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create4DVolumeDataAccessorU16 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint16_t> *VolumeDataAccess::createVolumeDataAccessor<IntVector3, uint16_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create3DVolumeDataAccessorU16 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint16_t> *VolumeDataAccess::createVolumeDataAccessor<IntVector2, uint16_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create2DVolumeDataAccessorU16 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint8_t>  *VolumeDataAccess::createVolumeDataAccessor<IntVector4, uint8_t> (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create4DVolumeDataAccessorU8  (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint8_t>  *VolumeDataAccess::createVolumeDataAccessor<IntVector3, uint8_t> (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create3DVolumeDataAccessorU8  (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint8_t>  *VolumeDataAccess::createVolumeDataAccessor<IntVector2, uint8_t> (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create2DVolumeDataAccessorU8  (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, bool>     *VolumeDataAccess::createVolumeDataAccessor<IntVector4, bool>    (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create4DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, bool>     *VolumeDataAccess::createVolumeDataAccessor<IntVector3, bool>    (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create3DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, bool>     *VolumeDataAccess::createVolumeDataAccessor<IntVector2, bool>    (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return create2DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue); }

template<> inline VolumeDataReadAccessor<FloatVector4, double> *VolumeDataAccess::createInterpolatingVolumeDataAccessor<FloatVector4, double>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) { return create4DInterpolatingVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue, interpolationMethod); }
template<> inline VolumeDataReadAccessor<FloatVector3, double> *VolumeDataAccess::createInterpolatingVolumeDataAccessor<FloatVector3, double>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) { return create3DInterpolatingVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue, interpolationMethod); }
template<> inline VolumeDataReadAccessor<FloatVector2, double> *VolumeDataAccess::createInterpolatingVolumeDataAccessor<FloatVector2, double>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) { return create2DInterpolatingVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue, interpolationMethod); }
template<> inline VolumeDataReadAccessor<FloatVector4, float>  *VolumeDataAccess::createInterpolatingVolumeDataAccessor<FloatVector4, float> (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) { return create4DInterpolatingVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue, interpolationMethod); }
template<> inline VolumeDataReadAccessor<FloatVector3, float>  *VolumeDataAccess::createInterpolatingVolumeDataAccessor<FloatVector3, float> (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) { return create3DInterpolatingVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue, interpolationMethod); }
template<> inline VolumeDataReadAccessor<FloatVector2, float>  *VolumeDataAccess::createInterpolatingVolumeDataAccessor<FloatVector2, float> (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) { return create2DInterpolatingVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue, interpolationMethod); }

template <typename INDEX>
class VolumeDataRegions
{
protected:
  virtual      ~VolumeDataRegions() {}

public:
  virtual int64_t regionCount() = 0;

  virtual IndexRegion<INDEX>
                region(int64_t region) = 0;

  virtual int64_t regionFromIndex(INDEX index) = 0;
};

template <typename INDEX>
class VolumeDataAccessorWithRegions : public VolumeDataAccessor, public VolumeDataRegions<INDEX>
{
public:
  virtual IndexRegion<INDEX> currentRegion() = 0;
};

template <typename INDEX, typename T>
class VolumeDataReadAccessor : public VolumeDataAccessorWithRegions<INDEX>
{
public:
  virtual T     getValue(INDEX index) = 0;
};

template <typename INDEX, typename T>
class VolumeDataReadWriteAccessor : public VolumeDataReadAccessor<INDEX, T>
{
public:
  virtual void setValue(INDEX index, T value) = 0;
  virtual void commit() = 0;
  virtual void cancel() = 0;
};

class VolumeDataPage
{
protected:
                VolumeDataPage() {}
  virtual      ~VolumeDataPage() {}
public:
  virtual void  getMinMax(int (&min)[Dimensionality_Max], int (&max)[Dimensionality_Max]) const = 0;
  virtual void  getMinMaxExcludingMargin(int (&anMinExcludingMargin)[Dimensionality_Max], int (&anMaxExcludingMargin)[Dimensionality_Max]) const = 0;
  virtual const void * getBuffer(int (&anPitch)[Dimensionality_Max]) = 0;
  virtual void *getWritableBuffer(int (&anPitch)[Dimensionality_Max]) = 0;
  virtual void  updateWrittenRegion(const int (&anWrittenMin)[Dimensionality_Max], const int (&anWrittenMax)[Dimensionality_Max]) = 0;
  virtual void  release() = 0;
};

class VolumeDataPageAccessor
{
protected:
                VolumeDataPageAccessor() {}
  virtual      ~VolumeDataPageAccessor() {}
public:
  virtual VolumeDataLayout const *getLayout() const = 0;

  virtual int   getLod() const = 0;
  virtual int   getChannelIndex() const = 0;
  virtual VolumeDataChannelDescriptor const &getChannelDescriptor() const = 0;
  virtual void  getNumSamples(int (&numSamples)[Dimensionality_Max]) const = 0;

  virtual int64_t getChunkCount() const = 0;
  virtual void  getChunkMinMax(int64_t chunk, int (&min)[Dimensionality_Max], int (&max)[Dimensionality_Max]) const = 0;
  virtual void  getChunkMinMaxExcludingMargin(int64_t iChunk, int (&minExcludingMargin)[Dimensionality_Max], int (&maxExcludingMargin)[Dimensionality_Max]) const = 0;
  virtual int64_t getChunkIndex(const int (&position)[Dimensionality_Max]) const = 0;

  virtual int   addReference() = 0;
  virtual int   removeReference() = 0;

  virtual VolumeDataPage *readPageAtPosition(const int (&anPosition)[Dimensionality_Max]) = 0;

  virtual void  commit() = 0;
};

} /* namespace OpenVDS*/

#endif //VOLUMEDATAACCESS_H
