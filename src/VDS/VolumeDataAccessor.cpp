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

#include "VolumeDataAccessor.h"

#include "VolumeDataAccessManagerImpl.h"
#include "VolumeDataLayout.h"
#include "VolumeDataRequestProcessor.h"
#include "DataBlock.h"

#include <inttypes.h>

#define VDS_MAX_REQUEST_VOLUME_SUBSET_BYTESIZE           2147483648 // 2 GB

namespace OpenVDS
{

void VolumeDataAccessorBase::updateWrittenRegion()
{
  if(m_writtenRegion.max[0] != 0)
  {
    assert(m_currentPage);

    int writtenMin[Dimensionality_Max] = { m_writtenRegion.min[3], m_writtenRegion.min[2], m_writtenRegion.min[1], m_writtenRegion.min[0], 0, 0 };
    int writtenMax[Dimensionality_Max] = { m_writtenRegion.max[3], m_writtenRegion.max[2], m_writtenRegion.max[1], m_writtenRegion.max[0], 1, 1 };
    m_currentPage->updateWrittenRegion(writtenMin, writtenMax);

    m_writtenRegion = AccessorRegion({0, 0, 0, 0}, {0, 0, 0, 0});
  }
}

//-----------------------------------------------------------------------------

void VolumeDataAccessorBase::makeCurrentPageWritable()
{
  int pitch[Dimensionality_Max];

  m_buffer = m_currentPage->getWritableBuffer(pitch);
  m_pitch = {pitch[3], pitch[2], pitch[1], pitch[0]};
  m_writable = true;
}

//-----------------------------------------------------------------------------

VolumeDataLayout const *VolumeDataAccessorBase::getLayout()
{
  return m_volumeDataPageAccessor->getLayout();
}

//-----------------------------------------------------------------------------

void VolumeDataAccessorBase::commit()
{
  updateWrittenRegion();
  m_volumeDataPageAccessor->commit();
}

//-----------------------------------------------------------------------------

void VolumeDataAccessorBase::cancel()
{
  m_canceled = true;
}

//-----------------------------------------------------------------------------

VolumeDataAccessorBase::VolumeDataAccessorBase(VolumeDataPageAccessor &volumeDataPageAccessor)
  : m_volumeDataPageAccessor(static_cast<VolumeDataPageAccessorImpl *>(&volumeDataPageAccessor))
  , m_canceled(false)
  , m_currentPage(nullptr)
  , m_min{0,0,0,0}
  , m_max{0,0,0,0}
  , m_validRegion(AccessorRegion({0, 0, 0, 0}, {0, 0, 0, 0}))
  , m_writtenRegion(AccessorRegion({0, 0, 0, 0}, {0, 0, 0, 0}))
  , m_writable(false)
  , m_buffer(nullptr)
  , m_pitch{0, 0, 0, 0}
{
  int numSamples[Dimensionality_Max];

  m_volumeDataPageAccessor->getNumSamples(numSamples);
  m_numSamples = {numSamples[3], numSamples[2], numSamples[1], numSamples[0]};
}

//-----------------------------------------------------------------------------

VolumeDataAccessorBase::~VolumeDataAccessorBase()
{
  if(m_currentPage)
  {
    updateWrittenRegion();
    m_currentPage->release();
    m_currentPage = nullptr;
  }
  if(m_volumeDataPageAccessor)
  {
    if(m_volumeDataPageAccessor->removeReference() == 0)
    {
      if(!m_canceled)
      {
        m_volumeDataPageAccessor->commit();
      }

      m_volumeDataPageAccessor->getManager()->destroyVolumeDataPageAccessor(m_volumeDataPageAccessor);
    }
    m_volumeDataPageAccessor = nullptr;
  }
}

void VolumeDataAccessorBase::readPageAtPosition(IntVector4 index, bool enableWriting)
{
  if(m_currentPage)
  {
    updateWrittenRegion();
    m_currentPage->release();
    m_currentPage = nullptr;
  }

  assert(m_writtenRegion.max[0] == 0);

  int position[Dimensionality_Max] = { index[3], index[2], index[1], index[0], 0, 0 };

  VolumeDataPage *page = m_volumeDataPageAccessor->readPageAtPosition(position);

  if(!page)
  {
    m_validRegion = AccessorRegion({0, 0, 0, 0}, {0, 0, 0, 0});

    if(index[0] < 0 || index[0] >= m_numSamples[0] ||
       index[1] < 0 || index[1] >= m_numSamples[1] ||
       index[2] < 0 || index[2] >= m_numSamples[2] ||
       index[3] < 0 || index[3] >= m_numSamples[3])
    {
      throw VolumeDataAccessor::IndexOutOfRangeException();
    }
    else
    {
      m_buffer = nullptr;
      m_min = {0, 0, 0, 0};
      m_max = {0, 0, 0, 0};
      m_pitch = {0, 0, 0, 0};
      m_writable = enableWriting;
      return;
    }
  }

  m_currentPage = page;

  int32_t min[Dimensionality_Max];
  int32_t max[Dimensionality_Max];

  int32_t minExcludingMargin[Dimensionality_Max];
  int32_t maxExcludingMargin[Dimensionality_Max];

  page->getMinMax(min, max);
  m_min = {min[3], min[2], min[1], min[0]};
  m_max = {max[3], max[2], max[1], max[0]};

  page->getMinMaxExcludingMargin(minExcludingMargin, maxExcludingMargin);
  m_validRegion = AccessorRegion({minExcludingMargin[3], minExcludingMargin[2], minExcludingMargin[1], minExcludingMargin[0]}, {maxExcludingMargin[3], maxExcludingMargin[2], maxExcludingMargin[1], maxExcludingMargin[0]});

  int pitch[Dimensionality_Max];

  m_buffer = enableWriting ? page->getWritableBuffer(pitch) : const_cast<void *>(page->getBuffer(pitch));
  m_pitch = {pitch[3], pitch[2], pitch[1], pitch[0]};
  m_writable = enableWriting;
}

template <typename INDEX, typename T>
VolumeDataReadWriteAccessor<INDEX, T> *VolumeDataAccess_CreateVolumeDataAccessor(VolumeDataPageAccessor *v, float replacementNoValue)
{
  assert(v);

  VolumeDataPageAccessorImpl *volumeDataPageAccessor = static_cast<VolumeDataPageAccessorImpl *>(v);
  VolumeDataLayout const *volumeDataLayout = volumeDataPageAccessor->getLayout();
  int32_t channel = volumeDataPageAccessor->getChannelIndex();

  if(volumeDataLayout->isChannelUseNoValue(channel))
  {
    switch(volumeDataLayout->getChannelFormat(channel))
    {
    default:
      assert(0 && "Unknown voxel format");
    case VolumeDataChannelDescriptor::Format_R64:
      return new ConvertingVolumeDataAccessor<INDEX, T, double,   true>(*volumeDataPageAccessor, replacementNoValue);
    case VolumeDataChannelDescriptor::Format_U64:
      return new ConvertingVolumeDataAccessor<INDEX, T, uint64_t, true>(*volumeDataPageAccessor, replacementNoValue);
    case VolumeDataChannelDescriptor::Format_R32:
      return new ConvertingVolumeDataAccessor<INDEX, T, float,    true>(*volumeDataPageAccessor, replacementNoValue);
    case VolumeDataChannelDescriptor::Format_U32:
      return new ConvertingVolumeDataAccessor<INDEX, T, uint32_t, true>(*volumeDataPageAccessor, replacementNoValue);
    case VolumeDataChannelDescriptor::Format_U16:
      return new ConvertingVolumeDataAccessor<INDEX, T, uint16_t, true>(*volumeDataPageAccessor, replacementNoValue);
    case VolumeDataChannelDescriptor::Format_U8:
      return new ConvertingVolumeDataAccessor<INDEX, T, uint8_t,  true>(*volumeDataPageAccessor, replacementNoValue);
    case VolumeDataChannelDescriptor::Format_1Bit:
      return new ConvertingVolumeDataAccessor<INDEX, T, bool,     true>(*volumeDataPageAccessor, replacementNoValue);
    }
  }
  else
  {
    switch(volumeDataLayout->getChannelFormat(channel))
    {
    default:
      assert(0 && "Unknown voxel format");
    case VolumeDataChannelDescriptor::Format_R64:
      return new ConvertingVolumeDataAccessor<INDEX, T, double,   false>(*volumeDataPageAccessor, replacementNoValue);
    case VolumeDataChannelDescriptor::Format_U64:
      return new ConvertingVolumeDataAccessor<INDEX, T, uint64_t, false>(*volumeDataPageAccessor, replacementNoValue);
    case VolumeDataChannelDescriptor::Format_R32:
      return new ConvertingVolumeDataAccessor<INDEX, T, float,    false>(*volumeDataPageAccessor, replacementNoValue);
    case VolumeDataChannelDescriptor::Format_U32:
      return new ConvertingVolumeDataAccessor<INDEX, T, uint32_t, false>(*volumeDataPageAccessor, replacementNoValue);
    case VolumeDataChannelDescriptor::Format_U16:
      return new ConvertingVolumeDataAccessor<INDEX, T, uint16_t, false>(*volumeDataPageAccessor, replacementNoValue);
    case VolumeDataChannelDescriptor::Format_U8:
      return new ConvertingVolumeDataAccessor<INDEX, T, uint8_t,  false>(*volumeDataPageAccessor, replacementNoValue);
    case VolumeDataChannelDescriptor::Format_1Bit:
      return new ConvertingVolumeDataAccessor<INDEX, T, bool,     false>(*volumeDataPageAccessor, replacementNoValue);
    }
  }
}

template<typename INDEX, typename T1, typename T2, bool isUseNoValue>
VolumeDataReadAccessor<INDEX, T1>* CreateInterpolatingVolumeDataAccessorImpl(VolumeDataPageAccessor * volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  switch(interpolationMethod)
  {
  default: 
    assert(0 && "Unknown interpolation format");
  case InterpolationMethod::Nearest:
    return new InterpolatingVolumeDataAccessor<INDEX, T1, T2, isUseNoValue, InterpolationMethod::Nearest>(*volumeDataPageAccessor, replacementNoValue);
  case InterpolationMethod::Linear:
    return new InterpolatingVolumeDataAccessor<INDEX, T1, T2, isUseNoValue, InterpolationMethod::Linear> (*volumeDataPageAccessor, replacementNoValue);
  case InterpolationMethod::Cubic:
    return new InterpolatingVolumeDataAccessor<INDEX, T1, T2, isUseNoValue, InterpolationMethod::Cubic>  (*volumeDataPageAccessor, replacementNoValue);
  case InterpolationMethod::Angular:
    return new InterpolatingVolumeDataAccessor<INDEX, T1, T2, isUseNoValue, InterpolationMethod::Angular>(*volumeDataPageAccessor, replacementNoValue);
  case InterpolationMethod::Triangular:
    return new InterpolatingVolumeDataAccessor<INDEX, T1, T2, isUseNoValue, InterpolationMethod::Triangular>(*volumeDataPageAccessor, replacementNoValue);
  //case InterpolationMethod::TrinTriangular_Excluding_Valuerange_Min_And_Less:
    //return new InterpolatingVolumeDataAccessor<INDEX, T1, T2, isUseNoValue, InterpolationMethod::Triangular_Excluding_Valuerange_Min_And_Less>(*volumeDataPageAccessor, replacementNoValue);
  }

  return nullptr;
}


template <typename INDEX, typename T>
static VolumeDataReadAccessor<INDEX, T>* VolumeDataAccess_CreateInterpolatingVolumeDataAccessor(VolumeDataPageAccessor * volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  int channel = volumeDataPageAccessor->getChannelIndex();

  const VolumeDataLayout *volumeDataLayout = static_cast<VolumeDataPageAccessorImpl *>(volumeDataPageAccessor)->getLayout();

  if(volumeDataLayout->isChannelUseNoValue(channel))
  {
    switch(volumeDataLayout->getChannelFormat(channel))
    {
    default:
      assert(0 && "Unknown voxel format");
    case VolumeDataChannelDescriptor::Format_R64:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, double,   true>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    case VolumeDataChannelDescriptor::Format_U64:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, uint64_t, true>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    case VolumeDataChannelDescriptor::Format_R32:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, float,    true>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    case VolumeDataChannelDescriptor::Format_U32:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, uint32_t, true>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    case VolumeDataChannelDescriptor::Format_U16:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, uint16_t, true>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    case VolumeDataChannelDescriptor::Format_U8:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, uint8_t,  true>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    case VolumeDataChannelDescriptor::Format_1Bit:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, bool,     true>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    }
  }
  else
  {
    switch(volumeDataLayout->getChannelFormat(channel))
    {
    default:
      assert(0 && "Unknown voxel format");
    case VolumeDataChannelDescriptor::Format_R64:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, double,   false>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    case VolumeDataChannelDescriptor::Format_U64:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, uint64_t, false>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    case VolumeDataChannelDescriptor::Format_R32:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, float,    false>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    case VolumeDataChannelDescriptor::Format_U32:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, uint32_t, false>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    case VolumeDataChannelDescriptor::Format_U16:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, uint16_t, false>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    case VolumeDataChannelDescriptor::Format_U8:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, uint8_t,  false>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    case VolumeDataChannelDescriptor::Format_1Bit:
      return CreateInterpolatingVolumeDataAccessorImpl<INDEX, T, bool,     false>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
    }
  }

  return nullptr;
}

VolumeDataReadWriteAccessor<IntVector2, bool>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, bool>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector2, uint8_t>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessorU8(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, uint8_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector2, uint16_t>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessorU16(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, uint16_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector2, uint32_t>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessorU32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, uint32_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector2, uint64_t>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessorU64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, uint64_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector2, float>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, float>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector2, double>* VolumeDataAccessManagerImpl::create2DVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, double>(volumeDataPageAccessor, replacementNoValue);
}

VolumeDataReadWriteAccessor<IntVector3, bool>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, bool>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector3, uint8_t>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessorU8(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, uint8_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector3, uint16_t>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessorU16(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, uint16_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector3, uint32_t>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessorU32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, uint32_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector3, uint64_t>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessorU64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, uint64_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector3, float>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, float>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector3, double>* VolumeDataAccessManagerImpl::create3DVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, double>(volumeDataPageAccessor, replacementNoValue);
}

VolumeDataReadWriteAccessor<IntVector4, bool>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, bool>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector4, uint8_t>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessorU8(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, uint8_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector4, uint16_t>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessorU16(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, uint16_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector4, uint32_t>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessorU32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, uint32_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector4, uint64_t>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessorU64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, uint64_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector4, float>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, float>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector4, double>* VolumeDataAccessManagerImpl::create4DVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, double>(volumeDataPageAccessor, replacementNoValue);
}

VolumeDataReadAccessor<FloatVector2, float >* VolumeDataAccessManagerImpl::create2DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return VolumeDataAccess_CreateInterpolatingVolumeDataAccessor<FloatVector2, float>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
}
VolumeDataReadAccessor<FloatVector2, double>* VolumeDataAccessManagerImpl::create2DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return VolumeDataAccess_CreateInterpolatingVolumeDataAccessor<FloatVector2, double>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
}
VolumeDataReadAccessor<FloatVector3, float >* VolumeDataAccessManagerImpl::create3DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return VolumeDataAccess_CreateInterpolatingVolumeDataAccessor<FloatVector3, float >(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
}
VolumeDataReadAccessor<FloatVector3, double>* VolumeDataAccessManagerImpl::create3DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return VolumeDataAccess_CreateInterpolatingVolumeDataAccessor<FloatVector3, double>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
}
VolumeDataReadAccessor<FloatVector4, float >* VolumeDataAccessManagerImpl::create4DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return VolumeDataAccess_CreateInterpolatingVolumeDataAccessor<FloatVector4, float >(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
}

VolumeDataReadAccessor<FloatVector4, double>* VolumeDataAccessManagerImpl::create4DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
   return VolumeDataAccess_CreateInterpolatingVolumeDataAccessor<FloatVector4, double>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
}

static int64_t getVoxelCount(const int32_t(&min)[Dimensionality_Max], const int32_t (&max)[Dimensionality_Max], int32_t lod, int32_t dimensionality)
{
  int64_t  voxel = 1;

  for (int32_t iCount = 0; iCount < dimensionality; iCount++)
  {
    int32_t iMin = min[iCount];
    int32_t iMax = max[iCount];

    int64_t nSize = iMax - iMin;

    assert(nSize == 1 || (nSize % (int64_t(1) << int64_t(lod)) == 0));

    nSize >>= lod;

    if (nSize <= 0)
    {          
      if (min[iCount] >= max[iCount])
      {
        return 0;
      }
    }
    else
    {
      voxel *= nSize;
    }
  }

  return voxel;
}

static int64_t StaticRequestVolumeSubset(VolumeDataRequestProcessor &request_processor, void *buffer, VolumeDataLayer *volumeDataLayer, const int32_t(&minRequested)[Dimensionality_Max], const int32_t (&maxRequested)[Dimensionality_Max], int32_t lod, VolumeDataChannelDescriptor::Format format, bool isReplaceNoValue, float replacementNoValue)
{

  int32_t boxMinRequested[Dimensionality_Max];
  int32_t boxMaxRequested[Dimensionality_Max];

  memcpy(boxMinRequested, minRequested, sizeof(boxMinRequested));
  memcpy(boxMaxRequested, maxRequested, sizeof(boxMaxRequested));

  // Initialized unused dimensions
  for (int32_t iDimension = volumeDataLayer->getLayout()->getDimensionality(); iDimension < Dimensionality_Max; iDimension++)
  {
    boxMinRequested[iDimension] = 0;
    boxMaxRequested[iDimension] = 1;
  }

  int64_t voxelCount = getVoxelCount(boxMinRequested, boxMaxRequested, lod, volumeDataLayer->getLayout()->getDimensionality());
  int64_t requestByteSize = voxelCount * getElementSize(format, VolumeDataChannelDescriptor::Components_1);

  if (requestByteSize > VDS_MAX_REQUEST_VOLUME_SUBSET_BYTESIZE)
  {
    fprintf(stderr, "Requested volume subset is larger than 2GB %" PRId64 "\n", requestByteSize);
    abort();
  }

  VolumeDataLayer* childLayer = volumeDataLayer;

  while (childLayer && childLayer->getLOD() > 0)
  {
    childLayer = childLayer->getChildLayer();
  }

  if (childLayer->getProduceStatus() == VolumeDataLayer::ProduceStatus_Unavailable)
  {
    fprintf(stderr, "The requested dimension group or channel is unavailable (check produce status on VDS before requesting data)\n");
    abort();
  }

  std::vector<VolumeDataChunk> chunksInRegion;

  volumeDataLayer->getChunksInRegion(boxMinRequested, boxMaxRequested, &chunksInRegion);

  if (chunksInRegion.size() == 0)
  {
    fprintf(stderr, "Requested volume subset does not contain any data");
    abort();
  }

  return request_processor.addJob(chunksInRegion, [](VolumeDataPage* page) {});
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
int64_t VolumeDataAccessManagerImpl::requestVolumeTraces(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int nTraceCount, InterpolationMethod interpolationMethod, int iTraceDimension, float rReplacementNoValue)
{
  return int64_t(0);
}
int64_t VolumeDataAccessManagerImpl::prefetchVolumeChunk(VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int64_t chunk)
{
  return int64_t(0);
}
}
