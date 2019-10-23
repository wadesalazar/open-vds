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
#include "VolumeDataLayoutImpl.h"
#include "VolumeDataRequestProcessor.h"
#include "DataBlock.h"
#include "DimensionGroup.h"

#include <inttypes.h>

#include <fmt/format.h>

#define VDS_MAX_REQUEST_VOLUME_SUBSET_BYTESIZE           2147483648 // 2 GB

const int MIN_MEMCPY = 256;
template <unsigned int IVAL> struct FIND_SHIFT    { enum { RET = 1 + FIND_SHIFT<IVAL/2>::RET }; };
template <>                  struct FIND_SHIFT<1> { enum { RET = 0 }; };
template <>                  struct FIND_SHIFT<0> { enum { RET = 0 }; };
#define CALC_BIT_SHIFT(IVAL)       (FIND_SHIFT<IVAL>::RET)

#ifdef _DEBUG
#   define force_inline inline
#else
#   if defined _MSC_VER
#      define force_inline __forceinline
#   elif defined __GNUC__
#      define force_inline __attribute__((always_inline)) inline
#   else
#      define force_inline inline
#   endif
#endif

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

static int32_t combineAndReduceDimensions (int32_t (&sourceSize  )[DataStoreDimensionality_Max],
                                           int32_t (&sourceOffset)[DataStoreDimensionality_Max],
                                           int32_t (&targetSize  )[DataStoreDimensionality_Max],
                                           int32_t (&targetOffset)[DataStoreDimensionality_Max],
                                           int32_t (&overlapSize )[DataStoreDimensionality_Max],
                                           const int32_t (&origSourceSize  )[Dimensionality_Max],
                                           const int32_t (&origSourceOffset)[Dimensionality_Max],
                                           const int32_t (&origTargetSize  )[Dimensionality_Max],
                                           const int32_t (&origTargetOffset)[Dimensionality_Max],
                                           const int32_t (&origOverlapSize )[Dimensionality_Max])
{
    int32_t tmpSourceSize  [Dimensionality_Max] = {origSourceSize  [0],origSourceSize  [1],origSourceSize  [2],origSourceSize  [3],origSourceSize  [4],origSourceSize  [5]};
    int32_t tmpSourceOffset[Dimensionality_Max] = {origSourceOffset[0],origSourceOffset[1],origSourceOffset[2],origSourceOffset[3],origSourceOffset[4],origSourceOffset[5]};
    int32_t tmpTargetSize  [Dimensionality_Max] = {origTargetSize  [0],origTargetSize  [1],origTargetSize  [2],origTargetSize  [3],origTargetSize  [4],origTargetSize  [5]};
    int32_t tmpTargetOffset[Dimensionality_Max] = {origTargetOffset[0],origTargetOffset[1],origTargetOffset[2],origTargetOffset[3],origTargetOffset[4],origTargetOffset[5]};
    int32_t tmpOverlapSize [Dimensionality_Max] = {origOverlapSize [0],origOverlapSize [1],origOverlapSize [2],origOverlapSize [3],origOverlapSize [4],origOverlapSize [5]};

  // Combine dimensions where the overlap size is 1
  for (int32_t iCopyDimension = Dimensionality_Max - 1; iCopyDimension > 0; iCopyDimension--)
  {
    if (tmpOverlapSize[iCopyDimension] == 1)
    {
      tmpSourceOffset[iCopyDimension-1] += tmpSourceOffset[iCopyDimension] * tmpSourceSize[iCopyDimension-1];
      tmpTargetOffset[iCopyDimension-1] += tmpTargetOffset[iCopyDimension] * tmpTargetSize[iCopyDimension-1];
      tmpSourceOffset[iCopyDimension  ]  = 0;
      tmpTargetOffset[iCopyDimension  ]  = 0;

      tmpSourceSize[iCopyDimension-1] *= tmpSourceSize[iCopyDimension];
      tmpTargetSize[iCopyDimension-1] *= tmpTargetSize[iCopyDimension];
      tmpSourceSize[iCopyDimension  ]  = 1;
      tmpTargetSize[iCopyDimension  ]  = 1;
    }
  }

  assert(
    (tmpOverlapSize[0] + tmpSourceOffset[0] <= tmpSourceSize[0]) &&
    (tmpOverlapSize[1] + tmpSourceOffset[1] <= tmpSourceSize[1]) &&
    (tmpOverlapSize[2] + tmpSourceOffset[2] <= tmpSourceSize[2]) &&
    (tmpOverlapSize[3] + tmpSourceOffset[3] <= tmpSourceSize[3]) &&
    (tmpOverlapSize[4] + tmpSourceOffset[4] <= tmpSourceSize[4]) &&
    (tmpOverlapSize[5] + tmpSourceOffset[5] <= tmpSourceSize[5]) &&
    (tmpOverlapSize[0] + tmpTargetOffset[0] <= tmpTargetSize[0]) &&
    (tmpOverlapSize[1] + tmpTargetOffset[1] <= tmpTargetSize[1]) &&
    (tmpOverlapSize[2] + tmpTargetOffset[2] <= tmpTargetSize[2]) &&
    (tmpOverlapSize[3] + tmpTargetOffset[3] <= tmpTargetSize[3]) &&
    (tmpOverlapSize[4] + tmpTargetOffset[4] <= tmpTargetSize[4]) &&
    (tmpOverlapSize[5] + tmpTargetOffset[5] <= tmpTargetSize[5]) &&
    "Invalid Copy Parameters #1"
  );

  int32_t nCopyDimensions = 0;

  // Reduce dimensions where the source and target size is 1 (and the offset is 0)
  for (int32_t iDimension = 0; iDimension < DataStoreDimensionality_Max; iDimension++)
  {
    if(tmpSourceSize[iDimension] == 1 && tmpTargetSize[iDimension] == 1)
    {
      assert(tmpSourceOffset[iDimension] == 0);
      assert(tmpTargetOffset[iDimension] == 0);
      assert(tmpOverlapSize [iDimension] == 1);
      continue;
    }

    sourceOffset[nCopyDimensions] = tmpSourceOffset[iDimension];
    targetOffset[nCopyDimensions] = tmpTargetOffset[iDimension];

    sourceSize[nCopyDimensions] = tmpSourceSize[iDimension];
    targetSize[nCopyDimensions] = tmpTargetSize[iDimension];

    overlapSize[nCopyDimensions] = tmpOverlapSize[iDimension];

    nCopyDimensions++;
  }

  // Resulting number of copy dimensions must not exceed HueDataBlock_c::DIMENSIONALITY_MAX
  assert(nCopyDimensions <= DataStoreDimensionality_Max && "Invalid Copy Parameters #4");

  // Further combine inner dimensions if possible, to minimize number of copy invocations
  int32_t nCombineDimensions = nCopyDimensions - 1;

  for (int iCombineDimension = 0; iCombineDimension < nCombineDimensions; iCombineDimension++)
  {
    if (overlapSize[0] != sourceSize[0] || sourceSize[0] != targetSize[0])
    {
      break;
    }

    assert(sourceOffset[0] == 0 && "Invalid Copy Parameters #2");
    assert(targetOffset[0] == 0 && "Invalid Copy Parameters #3");

    sourceOffset[0] = sourceOffset[1] * sourceSize[0];
    targetOffset[0] = targetOffset[1] * targetSize[0];

    sourceSize [0] *= sourceSize [1];
    targetSize [0] *= targetSize [1];
    overlapSize[0] *= overlapSize[1];

    for (int iDim = 1; iDim < nCopyDimensions - 1; iDim++)
    {
      sourceOffset[iDim] = sourceOffset[iDim+1];
      targetOffset[iDim] = targetOffset[iDim+1];

      sourceSize [iDim] = sourceSize [iDim+1];
      targetSize [iDim] = targetSize [iDim+1];
      overlapSize[iDim] = overlapSize[iDim+1];
    }

    nCopyDimensions -= 1;
  }

  // Reset remaining dimensions
  for(int32_t iDimension = nCopyDimensions; iDimension < DataStoreDimensionality_Max; iDimension++)
  {
    sourceOffset[iDimension] = 0;
    targetOffset[iDimension] = 0;

    sourceSize [iDimension] = 1;
    targetSize [iDimension] = 1;
    overlapSize[iDimension] = 1;
  }

  return nCopyDimensions;
}

static force_inline void copyBits(void* target, int64_t targetBit, const void* source, int64_t sourceBit, int32_t bits)
{
  while(bits--)
  {
    dataBlock_WriteElement(reinterpret_cast<bool *>(target), targetBit++, dataBlock_ReadElement(reinterpret_cast<const bool *>(source), sourceBit++));
  }
}

template <typename T>
static force_inline void copyBytesT(T* __restrict target, const T* __restrict source, int32_t size)
{
  if (size >= MIN_MEMCPY)
  {
    memcpy (target, source, size);
  }
  else
  {
    int32_t nBigElements = size >> CALC_BIT_SHIFT(sizeof(T));
    for (int32_t iBigElement = 0; iBigElement < nBigElements; iBigElement++)
    {
      target [iBigElement] = source [iBigElement];
    }
    int32_t nTail = size & ((int32_t) sizeof (T) - 1);
    if (nTail)
    {
      assert(nTail <= 7 && "Invalid Sample Size Remainder\n");
      uint8_t *sourceTail = (uint8_t *) (source + nBigElements);
      uint8_t *targetTail = (uint8_t *) (target + nBigElements);

      int32_t iTail = 0;
      switch (nTail)
      {
      case 7: targetTail [iTail] = sourceTail [iTail]; iTail++;
      case 6: targetTail [iTail] = sourceTail [iTail]; iTail++;
      case 5: targetTail [iTail] = sourceTail [iTail]; iTail++;
      case 4: targetTail [iTail] = sourceTail [iTail]; iTail++;
      case 3: targetTail [iTail] = sourceTail [iTail]; iTail++;
      case 2: targetTail [iTail] = sourceTail [iTail]; iTail++;
      case 1: targetTail [iTail] = sourceTail [iTail];
      }
    }
  }
}

static force_inline void copyBytes(void* target, const void* source, int32_t size)
{
  if (size >= sizeof (int64_t) && !((intptr_t) source & (sizeof (int64_t)-1)) && !((intptr_t) target & (sizeof (int64_t)-1)))
    copyBytesT ((int64_t*) target, (int64_t*) source, size);
  else if (size >= sizeof (int32_t) && !((intptr_t) source & (sizeof (int32_t)-1)) && !((intptr_t) target & (sizeof (int32_t)-1)))
    copyBytesT ((int32_t*) target, (int32_t*) source, size);
  else if (size >= sizeof (int16_t) && !((intptr_t) source & (sizeof (int16_t)-1)) && !((intptr_t) target & (sizeof (int16_t)-1)))
    copyBytesT ((int16_t*) target, (int16_t*) source, size);
  else
    copyBytesT ((int8_t*) target, (int8_t*) source, size);
}

static void blockCopy(void       *target, const int32_t (&targetOffset)[DataStoreDimensionality_Max], const int32_t (&targetSize)[DataStoreDimensionality_Max],
                      void const *source, const int32_t (&sourceOffset)[DataStoreDimensionality_Max], const int32_t (&sourceSize)[DataStoreDimensionality_Max],
                      const int32_t (&overlapSize) [DataStoreDimensionality_Max], int32_t elementSize, int32_t copyDimensions, bool is1Bit)
{

  int64_t sourceLocalBaseSize = ((((int64_t)sourceOffset[3] * sourceSize[2] + sourceOffset[2]) * sourceSize[1] + sourceOffset[1]) * sourceSize[0] + sourceOffset[0]) * elementSize;
  int64_t targetLocalBaseSize = ((((int64_t)targetOffset[3] * targetSize[2] + targetOffset[2]) * targetSize[1] + targetOffset[1]) * targetSize[0] + targetOffset[0]) * elementSize;

  if (is1Bit)
  {
    for (int iDim3 = 0; iDim3 < overlapSize[3]; iDim3++)
    {
      for (int iDim2 = 0; iDim2 < overlapSize[2]; iDim2++)
      {
        for (int iDim1 = 0; iDim1 < overlapSize[1]; iDim1++)
        {
          int64_t sourceLocal = (((int64_t)iDim3 * sourceSize[2] + iDim2) * sourceSize[1] + iDim1) * (int64_t)sourceSize[0] * elementSize;
          int64_t targetLocal = (((int64_t)iDim3 * targetSize[2] + iDim2) * targetSize[1] + iDim1) * (int64_t)targetSize[0] * elementSize;

          copyBits(target, targetLocalBaseSize + targetLocal, source, sourceLocalBaseSize + sourceLocal, overlapSize[0]);
        }
      }
    }
  }
  else
  {
    const uint8_t *sourceLocalBase = reinterpret_cast<const uint8_t *>(source) + sourceLocalBaseSize;

    uint8_t *targetLocalBase = reinterpret_cast<uint8_t *>(target) + targetLocalBaseSize;

    for (int iDim3 = 0; iDim3 < overlapSize[3]; iDim3++)
    {
      for (int iDim2 = 0; iDim2 < overlapSize[2]; iDim2++)
      {
        for (int iDim1 = 0; iDim1 < overlapSize[1]; iDim1++)
        {
          int64_t iSourceLocal = (((int64_t)iDim3 * sourceSize[2] + iDim2) * sourceSize[1] + iDim1) * (int64_t)sourceSize[0] * elementSize;
          int64_t iTargetLocal = (((int64_t)iDim3 * targetSize[2] + iDim2) * targetSize[1] + iDim1) * (int64_t)targetSize[0] * elementSize;

          copyBytes(targetLocalBase + iTargetLocal, sourceLocalBase + iSourceLocal, overlapSize[0] * elementSize);
        }
      }
    }
  }
}

static bool requestSubsetProcessPage(VolumeDataPageImpl* page, const VolumeDataChunk &chunk, const int32_t (&destMin)[Dimensionality_Max], const int32_t (&destMax)[Dimensionality_Max], void *destBuffer, Error &error)
{
  int32_t sourceMin[Dimensionality_Max];
  int32_t sourceMax[Dimensionality_Max];
  int32_t sourceMinExcludingMargin[Dimensionality_Max];
  int32_t sourceMaxExcludingMargin[Dimensionality_Max];

  page->getMinMax(sourceMin, sourceMax);
  page->getMinMaxExcludingMargin(sourceMinExcludingMargin, sourceMaxExcludingMargin);

  int32_t iLOD = chunk.layer->getLOD();

  VolumeDataLayoutImpl *volumeDataLayout = chunk.layer->getLayout();

  int32_t overlapMin[Dimensionality_Max];
  int32_t overlapMax[Dimensionality_Max];

  int32_t sizeThisLod[Dimensionality_Max];

  for (int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    overlapMin[iDimension] = std::max(sourceMinExcludingMargin[iDimension], destMin[iDimension]);
    overlapMax[iDimension] = std::min(sourceMaxExcludingMargin[iDimension], destMax[iDimension]);
    if (chunk.layer->getLayout()->getFullResolutionDimension())
    {
      sizeThisLod[iDimension] = destMax[iDimension] - destMin[iDimension];
    }
    else
    {
      sizeThisLod[iDimension] = (destMax[iDimension] - destMin[iDimension] + (1 << iLOD) - 1) >> iLOD;
    }
  }

  DimensionGroup sourceDimensionGroup = chunk.layer->getChunkDimensionGroup();

  VolumeDataChannelDescriptor::Format format = chunk.layer->getFormat();
  bool is1Bit = ( format == VolumeDataChannelDescriptor::Format_1Bit);

  int32_t globalSourceSize[Dimensionality_Max];
  int32_t globalSourceOffset[Dimensionality_Max];
  int32_t globalTargetSize[Dimensionality_Max];
  int32_t globalTargetOffset[Dimensionality_Max];
  int32_t globalOverlapSize[Dimensionality_Max];

  for (int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    globalSourceSize[iDimension] = 1;
    for (int iCopyDimension = 0; iCopyDimension < DataStoreDimensionality_Max; iCopyDimension++)
    {
      if (iDimension == DimensionGroupUtil::getDimension(sourceDimensionGroup, iCopyDimension))
      {
        
        globalSourceSize[iDimension] = page->getDataBlock().allocatedSize[iCopyDimension];
        if (is1Bit && iCopyDimension == 0)
        {
          globalSourceSize[iDimension] *= 8;
        }
        break;
      }
    }
    globalTargetSize[iDimension] = sizeThisLod[iDimension];

    if (volumeDataLayout->isDimensionLODDecimated(iDimension))
    {
      globalSourceOffset[iDimension] = (overlapMin[iDimension] - sourceMin[iDimension]) >> iLOD;
      globalTargetOffset[iDimension] = (overlapMin[iDimension] - destMin[iDimension]) >> iLOD;
      globalOverlapSize[iDimension] = (overlapMax[iDimension] - overlapMin[iDimension] + (1 << iLOD) - 1) >> iLOD;
    }
    else
    {
      globalSourceOffset[iDimension] = (overlapMin[iDimension] - sourceMin[iDimension]);
      globalTargetOffset[iDimension] = (overlapMin[iDimension] - destMin[iDimension]);
      globalOverlapSize[iDimension] = (overlapMax[iDimension] - overlapMin[iDimension]);
    }
  }

  int32_t sourceSize[DataStoreDimensionality_Max];
  int32_t sourceOffset[DataStoreDimensionality_Max];
  int32_t targetSize[DataStoreDimensionality_Max];
  int32_t targetOffset[DataStoreDimensionality_Max];
  int32_t overlapSize[DataStoreDimensionality_Max];

  int32_t nCopyDimensions = combineAndReduceDimensions(sourceSize, sourceOffset, targetSize, targetOffset, overlapSize, globalSourceSize, globalSourceOffset, globalTargetSize, globalTargetOffset, globalOverlapSize);

  int32_t nBytesPerVoxel = is1Bit ? 1 : getVoxelFormatByteSize(format);

  void *source = page->getRawBufferInternal();

  blockCopy(destBuffer, targetOffset, targetSize,
    source, sourceOffset, sourceSize,
    overlapSize, nBytesPerVoxel, nCopyDimensions, is1Bit);

  return true;
}

struct BoxRequested
{
  int32_t min[Dimensionality_Max];
  int32_t max[Dimensionality_Max];
};

static int64_t StaticRequestVolumeSubset(VolumeDataRequestProcessor &request_processor, void *buffer, VolumeDataLayer *volumeDataLayer, const int32_t(&minRequested)[Dimensionality_Max], const int32_t (&maxRequested)[Dimensionality_Max], int32_t lod, VolumeDataChannelDescriptor::Format format, bool isReplaceNoValue, float replacementNoValue)
{

  BoxRequested boxRequested;
  memcpy(boxRequested.min, minRequested, sizeof(boxRequested.min));
  memcpy(boxRequested.max, maxRequested, sizeof(boxRequested.max));

  // Initialized unused dimensions
  for (int32_t iDimension = volumeDataLayer->getLayout()->getDimensionality(); iDimension < Dimensionality_Max; iDimension++)
  {
    boxRequested.min[iDimension] = 0;
    boxRequested.max[iDimension] = 1;
  }

  int64_t voxelCount = getVoxelCount(boxRequested.min, boxRequested.max, lod, volumeDataLayer->getLayout()->getDimensionality());
  int64_t requestByteSize = voxelCount * getElementSize(format, VolumeDataChannelDescriptor::Components_1);

  if (requestByteSize > VDS_MAX_REQUEST_VOLUME_SUBSET_BYTESIZE)
  {
    fprintf(stderr, "Requested volume subset is larger than 2GB %" PRId64 "\n", requestByteSize);
    abort();
  }

  std::vector<VolumeDataChunk> chunksInRegion;

  volumeDataLayer->getChunksInRegion(boxRequested.min, boxRequested.max, &chunksInRegion);

  if (chunksInRegion.size() == 0)
  {
    fprintf(stderr, "Requested volume subset does not contain any data");
    abort();
  }

  return request_processor.addJob(chunksInRegion, [boxRequested, buffer](VolumeDataPageImpl* page, VolumeDataChunk dataChunk, Error &error) {return requestSubsetProcessPage(page, dataChunk, boxRequested.min, boxRequested.max, buffer, error);});
}

static VolumeDataLayer *getLayer(VolumeDataLayout const *layout, DimensionsND dimensionsND, int lod, int channel)
{
  if(!layout)
  {
    return nullptr;
  }

  VolumeDataLayoutImpl const *volumeDataLayout = static_cast<VolumeDataLayoutImpl const *>(layout);

  if(channel > volumeDataLayout->getChannelCount())
  {
    return nullptr;
  }

  VolumeDataLayer *volumeDataLayer = volumeDataLayout->getBaseLayer(DimensionGroupUtil::getDimensionGroupFromDimensionsND(dimensionsND), channel);

  while(volumeDataLayer && volumeDataLayer->getLOD() < lod)
  {
    volumeDataLayer = volumeDataLayer->getParentLayer();
  }

  return (volumeDataLayer->getLayerType() != VolumeDataLayer::Virtual) ? volumeDataLayer : NULL;
}

int64_t VolumeDataAccessManagerImpl::requestVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format)
{
  // Initialized unused dimensions
  int minVoxelCoordinatesFixed[Dimensionality_Max];
  int maxVoxelCoordinatesFixed[Dimensionality_Max];

  int layoutDimensionality = volumeDataLayout->getDimensionality();
  for (int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    if (iDimension < layoutDimensionality)
    {
      minVoxelCoordinatesFixed[iDimension] = minVoxelCoordinates[iDimension];
      maxVoxelCoordinatesFixed[iDimension] = maxVoxelCoordinates[iDimension];
    }
    else
    {
      minVoxelCoordinatesFixed[iDimension] = 0;
      maxVoxelCoordinatesFixed[iDimension] = 1;
    }
  }
  
  VolumeDataLayer *volumeDataLayer = getLayer(volumeDataLayout, dimensionsND, lod, channel);

  std::vector<VolumeDataChunk> chunksInRegion;

  volumeDataLayer->getChunksInRegion(minVoxelCoordinates, maxVoxelCoordinates, &chunksInRegion);

  if (chunksInRegion.empty())
  {
    fmt::print("Requested volume subset does not contain any data");
    abort();
  }

  return StaticRequestVolumeSubset(m_requestProcessor, buffer, volumeDataLayer, minVoxelCoordinatesFixed, maxVoxelCoordinatesFixed, lod, format, volumeDataLayer->isUseNoValue(), volumeDataLayer->getNoValue());
}

int64_t VolumeDataAccessManagerImpl::requestVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, float replacementNoValue)
{
  // Initialized unused dimensions
  int minVoxelCoordinatesFixed[Dimensionality_Max];
  int maxVoxelCoordinatesFixed[Dimensionality_Max];

  int layoutDimensionality = volumeDataLayout->getDimensionality();
  for (int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    if (iDimension < layoutDimensionality)
    {
      minVoxelCoordinatesFixed[iDimension] = minVoxelCoordinates[iDimension];
      maxVoxelCoordinatesFixed[iDimension] = maxVoxelCoordinates[iDimension];
    }
    else
    {
      minVoxelCoordinatesFixed[iDimension] = 0;
      maxVoxelCoordinatesFixed[iDimension] = 1;
    }
  }

  VolumeDataLayer *volumeDataLayer = getLayer(volumeDataLayout, dimensionsND, lod, channel);

  std::vector<VolumeDataChunk> chunksInRegion;

  volumeDataLayer->getChunksInRegion(minVoxelCoordinates, maxVoxelCoordinates, &chunksInRegion);

  if (chunksInRegion.empty())
  {
    fmt::print("Requested volume subset does not contain any data");
    abort();
  }

  return StaticRequestVolumeSubset(m_requestProcessor, buffer, volumeDataLayer, minVoxelCoordinatesFixed, maxVoxelCoordinatesFixed, lod, format, true, replacementNoValue);
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
