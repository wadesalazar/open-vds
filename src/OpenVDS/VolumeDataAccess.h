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
#include <OpenVDS/Vector.h>

namespace OpenVDS {

enum class VDSProduceStatus
{
  Normal,
  Remapped,
  Unavailable
};

class VolumeDataPageAccessor;
class VolumeDataLayout;
struct VDS;

template <typename INDEX, typename T> class VolumeDataReadAccessor;
template <typename INDEX, typename T> class VolumeDataReadWriteAccessor;

template <typename T>
struct IndexRegion
{
    T Min;
    T Max;

    IndexRegion() {}
    IndexRegion(T Min, T Max) : Min(Min), Max(Max) {}
};

class VolumeDataAccessManager;
class VolumeDataAccessorBase;
class VolumeDataAccessor
{
protected:
                VolumeDataAccessor() {}
  virtual      ~VolumeDataAccessor() {}
public:
  virtual VolumeDataAccessManager &GetManager() = 0;
  virtual VolumeDataLayout const *GetLayout() = 0;
  virtual VolumeDataAccessorBase *GetBase() = 0;
  struct IndexOutOfRangeException {};
};

class VolumeDataAccessManager
{
protected:
                VolumeDataAccessManager() {}
  virtual      ~VolumeDataAccessManager() {}
public:
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
  virtual VolumeDataLayout const *GetVolumeDataLayout() const = 0;

  /// <summary>
  /// Get the produce status for the specific DimensionsND/LOD/Channel combination.
  /// </summary>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the VDS that we're getting the produce status for.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensions group we're getting the produce status for.
  /// </param>
  /// <param name="lod">
  /// The LOD level we're getting the produce status for.
  /// </param>
  /// <param name="channel">
  /// The channel index we're getting the produce status for.
  /// </param>
  /// <returns>
  /// The produce status for the specific DimensionsND/LOD/Channel combination.
  /// </returns>
  virtual VDSProduceStatus GetVDSProduceStatus(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel) const = 0;

  /// <summary>
  /// Create a volume data page accessor object for the VDS associated with the given VolumeDataLayout object.
  /// </summary>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the VDS that the volume data page accessor will access.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensions group that the volume data page accessor will access.
  /// </param>
  /// <param name="lod">
  /// The LOD level that the volume data page accessor will access.
  /// </param>
  /// <param name="channel">
  /// The channel index that the volume data page accessor will access.
  /// </param>
  /// <param name="nMaxPages">
  /// The maximum number of pages that the volume data page accessor will cache.
  /// </param>
  /// <param name="accessMode">
  /// This specifies the access mode (ReadOnly/ReadWrite/Create) of the volume data page accessor.
  /// </param>
  /// <returns>
  /// A VolumeDataPageAccessor object for the VDS associated with the given VolumeDataLayout object.
  /// </returns>
  virtual VolumeDataPageAccessor *CreateVolumeDataPageAccessor(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int maxPages, AccessMode accessMode) = 0;

  /// <summary>
  /// Destroy a volume data page accessor object.
  /// </summary>
  /// <param name="pVolumeDataPageAccessor">
  /// The VolumeDataPageAccessor object to destroy.
  /// </param>
  virtual void  DestroyVolumeDataPageAccessor(VolumeDataPageAccessor *volumeDataPageAccessor) = 0;

  virtual void  DestroyVolumeDataAccessor(VolumeDataAccessor *accessor) = 0;

  virtual VolumeDataAccessor * CloneVolumeDataAccessor(VolumeDataAccessor const &accessor) = 0;

  template<typename INDEX, typename T>
  VolumeDataReadWriteAccessor<INDEX, T> *CreateVolumeDataAccessor(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue);

  template<typename INDEX, typename T>
  VolumeDataReadAccessor<INDEX, T > *CreateInterpolatingVolumeDataAccessor(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod);

  virtual VolumeDataReadWriteAccessor<IntVector2, bool>     *Create2DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector2, uint8_t>  *Create2DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector2, uint16_t> *Create2DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector2, uint32_t> *Create2DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector2, uint64_t> *Create2DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector2, float>    *Create2DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector2, double>   *Create2DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;

  virtual VolumeDataReadWriteAccessor<IntVector3, bool>     *Create3DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector3, uint8_t>  *Create3DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector3, uint16_t> *Create3DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector3, uint32_t> *Create3DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector3, uint64_t> *Create3DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector3, float>    *Create3DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector3, double>   *Create3DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;

  virtual VolumeDataReadWriteAccessor<IntVector4, bool>     *Create4DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector4, uint8_t>  *Create4DVolumeDataAccessorU8  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector4, uint16_t> *Create4DVolumeDataAccessorU16 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector4, uint32_t> *Create4DVolumeDataAccessorU32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector4, uint64_t> *Create4DVolumeDataAccessorU64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector4, float>    *Create4DVolumeDataAccessorR32 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;
  virtual VolumeDataReadWriteAccessor<IntVector4, double>   *Create4DVolumeDataAccessorR64 (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) = 0;

  virtual VolumeDataReadAccessor<FloatVector2, float > *Create2DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual VolumeDataReadAccessor<FloatVector2, double> *Create2DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual VolumeDataReadAccessor<FloatVector3, float > *Create3DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual VolumeDataReadAccessor<FloatVector3, double> *Create3DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual VolumeDataReadAccessor<FloatVector4, float > *Create4DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;
  virtual VolumeDataReadAccessor<FloatVector4, double> *Create4DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) = 0;

  /// <summary>
  /// Compute the buffer size for a volume subset request.
  /// </summary>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
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
  /// <param name="lod">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <returns>
  /// The buffer size needed
  /// </returns>
  virtual int64_t GetVolumeSubsetBufferSize(VolumeDataLayout const *volumeDataLayout, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, int lod = 0) = 0;

  /// <summary>
  /// Request a subset of the input VDS.
  /// </summary>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by minVoxelCoordinates and maxVoxelCoordinates.
  /// </param>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
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
  /// <returns>
  /// The requestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t RequestVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format) = 0;

  /// <summary>
  /// Request a subset of the input VDS.
  /// </summary>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by minVoxelCoordinates and maxVoxelCoordinates.
  /// </param>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
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
  /// Value used to replace region of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// The requestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t RequestVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lOD, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, float replacementNoValue) = 0;

  /// <summary>
  /// Compute the buffer size for a projected volume subset request.
  /// </summary>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
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
  /// <param name="lod">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <returns>
  /// The buffer size needed
  /// </returns>
  virtual int64_t GetProjectedVolumeSubsetBufferSize(VolumeDataLayout const *volumeDataLayout, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, int lod = 0) = 0;

  /// <summary>
  /// Request a subset projected from an arbitrary 3D plane through the subset onto one of the sides of the subset.
  /// </summary>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by minVoxelCoordinates and maxVoxelCoordinates for the projected dimensions.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
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
  /// <returns>
  /// The requestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t RequestProjectedVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod) = 0;

  /// <summary>
  /// Request a subset projected from an arbitrary 3D plane through the subset onto one of the sides of the subset.
  /// </summary>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least as many elements of format as indicated by minVoxelCoordinates and maxVoxelCoordinates for the projected dimensions.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
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
  /// Value used to replace region of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// The requestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t RequestProjectedVolumeSubset(void *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, float replacementNoValue) = 0;

  /// <summary>
  /// Request sampling of the input VDS at the specified coordinates.
  /// </summary>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least sampleCount elements.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="samplePositions">
  /// Pointer to array of Dimensionality_Max-elements indicating the positions to sample. May be deleted once requestVolumeSamples return, as OpenVDS makes a deep copy of the data.
  /// </param>
  /// <param name="sampleCount">
  /// Number of samples to request.
  /// </param>
  /// <param name="interpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <returns>
  /// The requestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t RequestVolumeSamples(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float (*samplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod) = 0;

  /// <summary>
  /// Request sampling of the input VDS at the specified coordinates.
  /// </summary>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least sampleCount elements.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested data is read from.
  /// </param>
  /// <param name="samplePositions">
  /// Pointer to array of Dimensionality_Max-elements indicating the positions to sample. May be deleted once requestVolumeSamples return, as OpenVDS makes a deep copy of the data.
  /// </param>
  /// <param name="sampleCount">
  /// Number of samples to request.
  /// </param>
  /// <param name="interpolationMethod">
  /// Interpolation method to use when sampling the buffer.
  /// </param>
  /// <param name="replacementNoValue">
  /// Value used to replace region of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// The requestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t RequestVolumeSamples(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float (*samplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, float replacementNoValue) = 0;

  /// <summary>
  /// Compute the buffer size for a volume traces request.
  /// </summary>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="traceCount">
  /// Number of traces to request.
  /// </param>
  /// <param name="traceDimension">
  /// The dimension to trace
  /// </param>
  /// <param name="lod">
  /// The LOD level the requested data is read from.
  /// </param>
  /// <returns>
  /// The buffer size needed
  /// </returns>
  virtual int64_t GetVolumeTracesBufferSize(VolumeDataLayout const *volumeDataLayout, int traceCount, int traceDimension, int lod = 0) = 0;

  /// <summary>
  /// Request traces from the input VDS.
  /// </summary>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least traceCount * number of samples in the traceDimension.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
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
  /// <returns>
  /// The requestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t RequestVolumeTraces(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int traceDimension) = 0;

  /// <summary>
  /// Request traces from the input VDS.
  /// </summary>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="buffer">
  /// Pointer to a preallocated buffer holding at least traceCount * number of samples in the traceDimension.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested data is read from.
  /// </param>
  /// <param name="lod">
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
  /// Value used to replace region of the input VDS that has no data.
  /// </param>
  /// <returns>
  /// The requestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t RequestVolumeTraces(float *buffer, VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int traceDimension, float replacementNoValue) = 0;

  /// <summary>
  /// Force production of a specific volume data chunk
  /// </summary>
  /// <param name="volumeDataLayout">
  /// The VolumeDataLayout object associated with the input VDS.
  /// </param>
  /// <param name="dimensionsND">
  /// The dimensiongroup the requested chunk belongs to.
  /// </param>
  /// <param name="lod">
  /// The LOD level the requested chunk belongs to.
  /// </param>
  /// <param name="channel">
  /// The channel index the requested chunk belongs to.
  /// </param>
  /// <param name="chunk">
  /// The index of the chunk to prefetch.
  /// </param>
  /// <returns>
  /// The requestID which can be used to query the status of the request, cancel the request or wait for the request to complete
  /// </returns>
  virtual int64_t PrefetchVolumeChunk(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int64_t chunk) = 0;

  /// <summary>
  /// Check if a request completed successfully. If the request completed, the buffer now contains valid data.
  /// </summary>
  /// <param name="requestID">
  /// The requestID to check for completion.
  /// </param>
  /// <returns>
  /// Either IsCompleted, IsCanceled or WaitForCompletion will return true a single time, after that the request is taken out of the system.
  /// </returns>
  virtual bool  IsCompleted(int64_t requestID) = 0;

  /// <summary>
  /// Check if a request was canceled (e.g. the VDS was invalidated before the request was processed). If the request was canceled, the buffer does not contain valid data.
  /// </summary>
  /// <param name="requestID">
  /// The requestID to check for cancellation.
  /// </param>
  /// <returns>
  /// Either IsCompleted, IsCanceled or WaitForCompletion will return true a single time, after that the request is taken out of the system.
  /// </returns>
  virtual bool  IsCanceled(int64_t requestID) = 0;

  /// <summary>
  /// Wait for a request to complete successfully. If the request completed, the buffer now contains valid data.
  /// </summary>
  /// <param name="requestID">
  /// The requestID to wait for completion of.
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
  /// The requestID to cancel.
  /// </param>
  virtual void  Cancel(int64_t requestID) = 0;

  /// <summary>
  /// Get the completion factor (between 0 and 1) of the request.
  /// </summary>
  /// <param name="requestID">
  /// The requestID to get the completion factor of.
  /// </param>
  /// <returns>
  /// A factor (between 0 and 1) indicating how much of the request has been completed.
  /// </returns>
  virtual float GetCompletionFactor(int64_t requestID) = 0;
  
  virtual void FlushUploadQueue() = 0;
  virtual void ClearUploadErrors() = 0;
  virtual void ForceClearAllUploadErrors() = 0;
  virtual int32_t UploadErrorCount() = 0;
  virtual void GetCurrentUploadError(const char **objectId, int32_t *errorCode, const char **errorString) = 0;
};

template<> inline VolumeDataReadWriteAccessor<IntVector4, double>   *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector4, double>  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create4DVolumeDataAccessorR64 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, double>   *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector3, double>  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create3DVolumeDataAccessorR64 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, double>   *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector2, double>  (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create2DVolumeDataAccessorR64 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, float>    *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector4, float>   (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create4DVolumeDataAccessorR32 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, float>    *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector3, float>   (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create3DVolumeDataAccessorR32 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, float>    *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector2, float>   (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create2DVolumeDataAccessorR32 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint64_t> *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector4, uint64_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create4DVolumeDataAccessorU64 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint64_t> *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector3, uint64_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create3DVolumeDataAccessorU64 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint64_t> *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector2, uint64_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create2DVolumeDataAccessorU64 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint32_t> *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector4, uint32_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create4DVolumeDataAccessorU32 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint32_t> *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector3, uint32_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create3DVolumeDataAccessorU32 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint32_t> *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector2, uint32_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create2DVolumeDataAccessorU32 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint16_t> *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector4, uint16_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create4DVolumeDataAccessorU16 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint16_t> *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector3, uint16_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create3DVolumeDataAccessorU16 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint16_t> *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector2, uint16_t>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create2DVolumeDataAccessorU16 (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, uint8_t>  *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector4, uint8_t> (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create4DVolumeDataAccessorU8  (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, uint8_t>  *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector3, uint8_t> (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create3DVolumeDataAccessorU8  (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, uint8_t>  *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector2, uint8_t> (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create2DVolumeDataAccessorU8  (volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector4, bool>     *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector4, bool>    (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create4DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector3, bool>     *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector3, bool>    (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create3DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue); }
template<> inline VolumeDataReadWriteAccessor<IntVector2, bool>     *VolumeDataAccessManager::CreateVolumeDataAccessor<IntVector2, bool>    (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue) { return Create2DVolumeDataAccessor1Bit(volumeDataPageAccessor, replacementNoValue); }

template<> inline VolumeDataReadAccessor<FloatVector4, double> *VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector4, double>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) { return Create4DInterpolatingVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue, interpolationMethod); }
template<> inline VolumeDataReadAccessor<FloatVector3, double> *VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector3, double>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) { return Create3DInterpolatingVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue, interpolationMethod); }
template<> inline VolumeDataReadAccessor<FloatVector2, double> *VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector2, double>(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) { return Create2DInterpolatingVolumeDataAccessorR64(volumeDataPageAccessor, replacementNoValue, interpolationMethod); }
template<> inline VolumeDataReadAccessor<FloatVector4, float>  *VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector4, float> (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) { return Create4DInterpolatingVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue, interpolationMethod); }
template<> inline VolumeDataReadAccessor<FloatVector3, float>  *VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector3, float> (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) { return Create3DInterpolatingVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue, interpolationMethod); }
template<> inline VolumeDataReadAccessor<FloatVector2, float>  *VolumeDataAccessManager::CreateInterpolatingVolumeDataAccessor<FloatVector2, float> (VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod) { return Create2DInterpolatingVolumeDataAccessorR32(volumeDataPageAccessor, replacementNoValue, interpolationMethod); }

template <typename INDEX>
class VolumeDataRegions
{
protected:
  virtual      ~VolumeDataRegions() {}

public:
  virtual int64_t RegionCount() = 0;

  virtual IndexRegion<INDEX>
                Region(int64_t Region) = 0;

  virtual int64_t RegionFromIndex(INDEX index) = 0;
};

template <typename INDEX>
class VolumeDataAccessorWithRegions : public VolumeDataAccessor, public VolumeDataRegions<INDEX>
{
public:
  virtual IndexRegion<INDEX> CurrentRegion() = 0;
};

template <typename INDEX, typename T>
class VolumeDataReadAccessor : public VolumeDataAccessorWithRegions<INDEX>
{
public:
  virtual T     GetValue(INDEX index) = 0;
};

template <typename INDEX, typename T>
class VolumeDataReadWriteAccessor : public VolumeDataReadAccessor<INDEX, T>
{
public:
  virtual void SetValue(INDEX index, T value) = 0;
  virtual void Commit() = 0;
  virtual void Cancel() = 0;
};

class VolumeDataPage
{
protected:
                VolumeDataPage() {}
  virtual      ~VolumeDataPage() {}
public:
  virtual void  GetMinMax(int (&min)[Dimensionality_Max], int (&max)[Dimensionality_Max]) const = 0;
  virtual void  GetMinMaxExcludingMargin(int (&minExcludingMargin)[Dimensionality_Max], int (&maxExcludingMargin)[Dimensionality_Max]) const = 0;
  virtual const void * GetBuffer(int (&pitch)[Dimensionality_Max]) = 0;
  virtual void *GetWritableBuffer(int (&pitch)[Dimensionality_Max]) = 0;
  virtual void  UpdateWrittenRegion(const int (&writtenMin)[Dimensionality_Max], const int (&writtenMax)[Dimensionality_Max]) = 0;
  virtual void  Release() = 0;
};

class VolumeDataPageAccessor
{
protected:
                VolumeDataPageAccessor() {}
  virtual      ~VolumeDataPageAccessor() {}
public:
  virtual VolumeDataLayout const *GetLayout() const = 0;

  virtual int   GetLOD() const = 0;
  virtual int   GetChannelIndex() const = 0;
  virtual VolumeDataChannelDescriptor const &GetChannelDescriptor() const = 0;
  virtual void  GetNumSamples(int (&numSamples)[Dimensionality_Max]) const = 0;

  virtual int64_t GetChunkCount() const = 0;
  virtual void  GetChunkMinMax(int64_t chunk, int (&min)[Dimensionality_Max], int (&max)[Dimensionality_Max]) const = 0;
  virtual void  GetChunkMinMaxExcludingMargin(int64_t chunk, int (&minExcludingMargin)[Dimensionality_Max], int (&maxExcludingMargin)[Dimensionality_Max]) const = 0;
  virtual int64_t GetChunkIndex(const int (&position)[Dimensionality_Max]) const = 0;

  virtual int   AddReference() = 0;
  virtual int   RemoveReference() = 0;

  virtual int   GetMaxPages() = 0;
  virtual void  SetMaxPages(int maxPages) = 0;

  virtual VolumeDataPage *CreatePage(int64_t chunkIndex) = 0;
  virtual VolumeDataPage *ReadPage(int64_t chunkIndex) = 0;

  VolumeDataPage *ReadPageAtPosition(const int (&position)[Dimensionality_Max]) { return ReadPage(GetChunkIndex(position)); }

  virtual void  Commit() = 0;
};

} /* namespace OpenVDS*/

#endif //VOLUMEDATAACCESS_H
