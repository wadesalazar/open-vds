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

#include "VolumeDataAccessManagerImpl.h"

#include <OpenVDSHandle.h>
#include "VolumeDataPageAccessorImpl.h"

namespace OpenVDS
{

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

  while(layer && layer->getLod() < lod)
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
}