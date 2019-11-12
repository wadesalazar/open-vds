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
#include "ValueConversion.h"

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

    int64_t nSize = int64_t(iMax) - iMin;

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

struct Box
{
  int32_t min[Dimensionality_Max];
  int32_t max[Dimensionality_Max];
};

static int64_t staticRequestVolumeSubset(VolumeDataRequestProcessor &request_processor, void *buffer, VolumeDataLayer *volumeDataLayer, const int32_t(&minRequested)[Dimensionality_Max], const int32_t (&maxRequested)[Dimensionality_Max], int32_t lod, VolumeDataChannelDescriptor::Format format, bool isReplaceNoValue, float replacementNoValue)
{

  Box boxRequested;
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

  return (volumeDataLayer && volumeDataLayer->getLayerType() != VolumeDataLayer::Virtual) ? volumeDataLayer : NULL;
}

struct ProjectVars
{
  FloatVector4 voxelPlane;

  int requestedMin[Dimensionality_Max];
  int requestedMax[Dimensionality_Max];
  int requestedPitch[Dimensionality_Max];
  int requestedSizeThisLOD[Dimensionality_Max];

  int lod;
  int projectionDimension;
  int projectedDimensions[2];

  int dataIndex(const int32_t (&voxelIndex)[Dimensionality_Max]) const
  {
    int dataIndex = 0;

    for (int i = 0; i < 6; i++)
    {
      int localChunkIndex = voxelIndex[i] - requestedMin[i];
      localChunkIndex >>= lod;
      dataIndex += localChunkIndex * requestedPitch[i];
    }
    
    return dataIndex;
  }

  void voxelIndex(const int32_t (&localChunkIndex)[Dimensionality_Max], int32_t (&voxelIndexR)[Dimensionality_Max]) const
  {
    for (int i = 0; i < 6; i++)
      voxelIndexR[i] = requestedMin[i] + (localChunkIndex[i] << lod);
  }
};

struct IndexValues
{
  float valueRangeMin;
  float valueRangeMax;
  int32_t lod;
  int32_t voxelMin[Dimensionality_Max];
  int32_t voxelMax[Dimensionality_Max];
  int32_t localChunkSamples[Dimensionality_Max];
  int32_t localChunkAllocatedSize[Dimensionality_Max];
  int32_t pitch[Dimensionality_Max];
  int32_t bitPitch[Dimensionality_Max];
  int32_t axisNumSamples[Dimensionality_Max];

  int32_t dataBlockSamples[DataStoreDimensionality_Max];
  int32_t dataBlockAllocatedSize[DataStoreDimensionality_Max];
  int32_t dataBlockPitch[DataStoreDimensionality_Max];
  int32_t dataBlockBitPitch[DataStoreDimensionality_Max];
  int32_t dimensionMap[DataStoreDimensionality_Max];


  float coordinateMin[Dimensionality_Max];
  float coordinateMax[Dimensionality_Max];

  bool isDimensionLODDecimated[Dimensionality_Max];

  void initialize(const VolumeDataChunk &dataChunk, const DataBlock &dataBlock)
  {
    const VolumeDataLayout *dataLayout = dataChunk.layer->getLayout();

    valueRangeMin = dataLayout->getChannelDescriptor(dataChunk.layer->getChannelIndex()).getValueRangeMin();
    valueRangeMax = dataLayout->getChannelDescriptor(dataChunk.layer->getChannelIndex()).getValueRangeMax();

    lod = dataChunk.layer->getLOD();
    dataChunk.layer->getChunkMinMax(dataChunk.chunkIndex, voxelMin, voxelMax, true);

    for (int iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
    {
      pitch[iDimension] = 0;
      bitPitch[iDimension] = 0;

      axisNumSamples[iDimension] = dataLayout->getDimensionNumSamples(iDimension);
      coordinateMin[iDimension] = (iDimension < dataLayout->getDimensionality()) ? dataLayout->getDimensionMin(iDimension) : 0;
      coordinateMax[iDimension] = (iDimension < dataLayout->getDimensionality()) ? dataLayout->getDimensionMax(iDimension) : 0;

      localChunkSamples[iDimension] = 1;
      isDimensionLODDecimated[iDimension] = false;

      localChunkAllocatedSize[iDimension] = 1;
    }

    for (int iDimension = 0; iDimension < DataStoreDimensionality_Max; iDimension++)
    {
      dataBlockPitch[iDimension] = dataBlock.pitch[iDimension];
      dataBlockAllocatedSize[iDimension] = dataBlock.allocatedSize[iDimension];
      dataBlockSamples[iDimension] = dataBlock.size[iDimension];

      for (int iDataBlockDim = 0; iDataBlockDim < DataStoreDimensionality_Max; iDataBlockDim++)
      {
        dataBlockBitPitch[iDataBlockDim] = dataBlockPitch[iDataBlockDim] * (iDataBlockDim == 0 ? 1 : 8);

        int iDimension = DimensionGroupUtil::getDimension(dataChunk.layer->getChunkDimensionGroup(), iDataBlockDim);
        dimensionMap[iDataBlockDim] = iDimension;
        if (iDimension >= 0 && iDimension < Dimensionality_Max)
        {
          pitch[iDimension] = dataBlockPitch[iDataBlockDim];
          bitPitch[iDimension] = dataBlockBitPitch[iDataBlockDim];
          localChunkAllocatedSize[iDimension] = dataBlockAllocatedSize[iDataBlockDim];

          isDimensionLODDecimated[iDimension] = (dataBlockSamples[iDataBlockDim] < voxelMax[iDimension] - voxelMin[iDimension]);
          localChunkSamples[iDimension] = dataBlockSamples[iDataBlockDim];
        }
      }
    }
  }
};

static bool voxelIndexInProcessArea(const IndexValues &indexValues, const int32_t (&iVoxelIndex)[Dimensionality_Max])
{
  bool ret = true;

  for (int i = 0; i < Dimensionality_Max; i++)
  {
    ret = ret && (iVoxelIndex[i] < indexValues.voxelMax[i]) && (iVoxelIndex[i] >= indexValues.voxelMin[i]);
  }

  return ret;
}
  
static void voxelIndexToLocalIndexFloat(const IndexValues &indexValues, const float (&iVoxelIndex)[Dimensionality_Max], float (&localIndex)[Dimensionality_Max] )
  {
    for (int i = 0; i < Dimensionality_Max; i++)
      localIndex[i] = 0.0f;

    for (int i = 0; i < DataStoreDimensionality_Max; i++)
    {
      if (indexValues.dimensionMap[i] >= 0)
      {
        localIndex[i] = iVoxelIndex[indexValues.dimensionMap[i]] - indexValues.voxelMin[indexValues.dimensionMap[i]];
        localIndex[i] /= (1 << (indexValues.isDimensionLODDecimated[indexValues.dimensionMap[i]] ? indexValues.lod : 0));
      }
    }
}

template <typename T, InterpolationMethod INTERPMETHOD, bool isUseNoValue>
void projectValuesKernel(T *output, const T *input, const ProjectVars &projectVars, const IndexValues &inputIndexer, const int32_t (&voxelOutIndex)[Dimensionality_Max], VolumeSampler<T, INTERPMETHOD, isUseNoValue> &sampler, QuantizingValueConverterWithNoValue<T, typename InterpolatedRealType<T>::type, isUseNoValue> &converter, float voxelCenterOffset)
{
  float zValue = (projectVars.voxelPlane.X * (voxelOutIndex[projectVars.projectedDimensions[0]] + voxelCenterOffset) + projectVars.voxelPlane.Y * (voxelOutIndex[projectVars.projectedDimensions[1]] + voxelCenterOffset) + projectVars.voxelPlane.T) / (-projectVars.voxelPlane.Z);

  //clamp so it's inside the volume
  if (zValue < 0) zValue = 0;
  else if (zValue >= inputIndexer.axisNumSamples[projectVars.projectionDimension]) zValue = (float)(inputIndexer.axisNumSamples[projectVars.projectionDimension] - 1);

  float voxelCenterInIndex[Dimensionality_Max];
  voxelCenterInIndex[0] = (float)voxelOutIndex[0] + voxelCenterOffset;
  voxelCenterInIndex[1] = (float)voxelOutIndex[1] + voxelCenterOffset;
  voxelCenterInIndex[2] = (float)voxelOutIndex[2] + voxelCenterOffset;
  voxelCenterInIndex[3] = (float)voxelOutIndex[3] + voxelCenterOffset;
  voxelCenterInIndex[4] = (float)voxelOutIndex[4] + voxelCenterOffset;
  voxelCenterInIndex[5] = (float)voxelOutIndex[5] + voxelCenterOffset;

  voxelCenterInIndex[projectVars.projectionDimension] = zValue;

  int32_t voxelInIndexInt[Dimensionality_Max]; 
  voxelInIndexInt[0] = voxelOutIndex[0];
  voxelInIndexInt[1] = voxelOutIndex[1];
  voxelInIndexInt[2] = voxelOutIndex[2];
  voxelInIndexInt[3] = voxelOutIndex[3];
  voxelInIndexInt[4] = voxelOutIndex[4];
  voxelInIndexInt[5] = voxelOutIndex[5];

  voxelInIndexInt[projectVars.projectionDimension] = (int)zValue;

  if (voxelIndexInProcessArea(inputIndexer, voxelInIndexInt))
  {
    float localInIndex[Dimensionality_Max];
    voxelIndexToLocalIndexFloat(inputIndexer, voxelCenterInIndex, localInIndex);
    FloatVector3 localInIndex3D(localInIndex[0], localInIndex[1], localInIndex[2]);

    typedef typename InterpolatedRealType<T>::type TREAL;
    TREAL value = sampler.sample3D(input, localInIndex3D);

    //TODO - 1Bit
    output[projectVars.dataIndex(voxelOutIndex)] = converter.convertValue(value);
  }
}

template <typename T, InterpolationMethod INTERPMETHOD, bool isUseNoValue>
void projectValuesKernelCPU(T *pxOutput, const T *pxInput, const ProjectVars &projectVars, const IndexValues &indexValues, float scale, float offset, float noValue)
{
  VolumeSampler<T, INTERPMETHOD, isUseNoValue> sampler(indexValues.dataBlockSamples, indexValues.dataBlockPitch, indexValues.valueRangeMin, indexValues.valueRangeMax, scale, offset, noValue, noValue);
  QuantizingValueConverterWithNoValue<T, typename InterpolatedRealType<T>::type, isUseNoValue> converter(indexValues.valueRangeMin, indexValues.valueRangeMax, scale, offset, noValue, noValue, false);

  int32_t numSamples[2];
  int32_t offsetPair[2];

  for (int i = 0; i < 2; i++)
  {
    int nMin = std::max(projectVars.requestedMin[projectVars.projectedDimensions[i]], indexValues.voxelMin[projectVars.projectedDimensions[i]]);
    int nMax = std::min(projectVars.requestedMax[projectVars.projectedDimensions[i]], indexValues.voxelMax[projectVars.projectedDimensions[i]]);

    numSamples[i] = (nMax - nMin + (1 << projectVars.lod) - 1) >> projectVars.lod;
    offsetPair[i] = (nMin - projectVars.requestedMin[projectVars.projectedDimensions[i]] + (1 << projectVars.lod) - 1) >> projectVars.lod;
  }

  float voxelCenterOffset = (1 << projectVars.lod) / 2.0f;

  // we can keep this to two dimensions because we know the input chunk is 3D
//#pragma omp parallel for
  for (int iDim1 = 0; iDim1 < numSamples[1]; iDim1++)
  for (int iDim0 = 0; iDim0 < numSamples[0]; iDim0++)
  {
    // this looks really strange, but since we know that the chunk dimension group for the input is always the projected and projection dimensions, this works
    int32_t localChunkIndex[Dimensionality_Max];
    for (int i = 0; i < 6; i++)
      localChunkIndex[i] = (indexValues.voxelMin[i] - projectVars.requestedMin[i]) >> projectVars.lod;
    
    localChunkIndex[projectVars.projectedDimensions[0]] = iDim0 + offsetPair[0];
    localChunkIndex[projectVars.projectedDimensions[1]] = iDim1 + offsetPair[1];
    localChunkIndex[projectVars.projectionDimension] = 0;

    int32_t voxelIndex[Dimensionality_Max];
    projectVars.voxelIndex(localChunkIndex, voxelIndex);
    projectValuesKernel<T, INTERPMETHOD, isUseNoValue>(pxOutput, pxInput, projectVars, indexValues, voxelIndex, sampler, converter, voxelCenterOffset);
  }
}

template <typename T, bool isUseNoValue>
static void projectValuesInitCPU(T *output, const T *input, const ProjectVars &projectVars, const IndexValues &indexValues, float scale, float offset, float noValue, InterpolationMethod interpolationMethod)
{
  switch(interpolationMethod)
  {
  case InterpolationMethod::Nearest: projectValuesKernelCPU<T, InterpolationMethod::Nearest, isUseNoValue>(output, input, projectVars, indexValues, scale, offset, noValue); break;
  case InterpolationMethod::Linear:  projectValuesKernelCPU<T, InterpolationMethod::Linear,  isUseNoValue>(output, input, projectVars, indexValues, scale, offset, noValue); break;
  case InterpolationMethod::Cubic:   projectValuesKernelCPU<T, InterpolationMethod::Cubic,   isUseNoValue>(output, input, projectVars, indexValues, scale, offset, noValue); break;
  case InterpolationMethod::Angular: projectValuesKernelCPU<T, InterpolationMethod::Angular, isUseNoValue>(output, input, projectVars, indexValues, scale, offset, noValue); break;
  case InterpolationMethod::Triangular: projectValuesKernelCPU<T, InterpolationMethod::Triangular, isUseNoValue>(output, input, projectVars, indexValues, scale, offset, noValue); break;
  //case InterpolationMethod::TriangularExcludingValuerangeMinAndLess: ProjectValuesKernelCPU<T, InterpolationMethod::TriangularExcludingValuerangeMinAndLess, isUseNoValue>(output, input, projectVars, scale, offset, noValue); break;
  }
}

static void projectValuesCPU(void *output, const void *input, const ProjectVars &projectVars, const IndexValues &indexValues, VolumeDataChannelDescriptor::Format format, InterpolationMethod eInterpolationMethod, float scale, float offset, bool isUseNoValue, float noValue)
{
  if (isUseNoValue)
  {
    switch(format)
    {
    case VolumeDataChannelDescriptor::Format_U8:  projectValuesInitCPU<unsigned char, true>((unsigned char*)output, (const unsigned char*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_U16: projectValuesInitCPU<unsigned short, true>((unsigned short*)output, (const unsigned short*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_R32: projectValuesInitCPU<float, true>((float*)output, (const float*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_U32: projectValuesInitCPU<unsigned int, true>((unsigned int*)output, (const unsigned int*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_R64: projectValuesInitCPU<double, true>((double*)output, (const double*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_U64: projectValuesInitCPU<uint64_t, true>((uint64_t *)output, (const uint64_t *)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    }
  }
  else
  {
    switch(format)
    {
    case VolumeDataChannelDescriptor::Format_U8:  projectValuesInitCPU<unsigned char, false>((unsigned char*)output, (const unsigned char*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_U16: projectValuesInitCPU<unsigned short, false>((unsigned short*)output, (const unsigned short*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_R32: projectValuesInitCPU<float, false>((float*)output, (const float*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_U32: projectValuesInitCPU<unsigned int, false>((unsigned int*)output, (const unsigned int*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_R64: projectValuesInitCPU<double, false>((double*)output, (const double*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_U64: projectValuesInitCPU<uint64_t, false>((uint64_t *)output, (const uint64_t *)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    }
  }
}

static bool requestProjectedVolumeSubsetProcessPage(VolumeDataPageImpl* page, const VolumeDataChunk &chunk, const int32_t (&destMin)[Dimensionality_Max], const int32_t (&destMax)[Dimensionality_Max], DimensionGroup projectedDimensionsEnum, FloatVector4 voxelPlane, InterpolationMethod interpolationMethod, bool useNoValue, float noValue, void *destBuffer, Error &error)
{
  VolumeDataChannelDescriptor::Format voxelFormat = chunk.layer->getFormat();

  VolumeDataLayer const *volumeDataLayer = chunk.layer;

  DataBlock const &dataBlock = page->getDataBlock();

  if (dataBlock.components != VolumeDataChannelDescriptor::Components_1)
  {
    error.string = "Cannot request volume subset from multi component VDSs";
    error.code = -1;
    return false;
  }

  int32_t iLOD = volumeDataLayer->getLOD();

  int32_t projectionDimension = -1;
  int32_t projectedDimensions[2] = { -1, -1 };

  if (DimensionGroupUtil::getDimensionality(volumeDataLayer->getChunkDimensionGroup()) < 3)
  {
    error.string = "The requested dimension group must contain at least 3 dimensions.";
    error.code = -1;
    return false;
  }

  if (DimensionGroupUtil::getDimensionality(projectedDimensionsEnum) != 2)
  {
    error.string = "The projected dimension group must contain 2 dimensions.";
    error.code = -1;
    return false;
  }

  for (int iDimIndex = 0; iDimIndex < DimensionGroupUtil::getDimensionality(volumeDataLayer->getChunkDimensionGroup()); iDimIndex++)
  {
    int32_t iDim = DimensionGroupUtil::getDimension(volumeDataLayer->getChunkDimensionGroup(), iDimIndex);

    if (!DimensionGroupUtil::isDimensionInGroup(projectedDimensionsEnum, iDim))
    {
      projectionDimension = iDim;
    }
  }

  assert(projectionDimension != -1);
  
  for (int32_t iDimIndex = 0, projectionDimensionality = DimensionGroupUtil::getDimensionality(projectedDimensionsEnum); iDimIndex < projectionDimensionality; iDimIndex++)
  {
    int32_t iDim = DimensionGroupUtil::getDimension(projectedDimensionsEnum, iDimIndex);
    projectedDimensions[iDimIndex] = iDim;

    if (!DimensionGroupUtil::isDimensionInGroup(volumeDataLayer->getChunkDimensionGroup(), iDim))
    {
      error.string = "The requested dimension group must contain the dimensions of the projected dimension group.";
      error.code = -1;
      return false;
    }
  }

  int32_t sizeThisLod[Dimensionality_Max];
  for (int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    if (chunk.layer->getLayout()->getFullResolutionDimension())
    {
      sizeThisLod[iDimension] = destMax[iDimension] - destMin[iDimension];
    }
    else
    {
      sizeThisLod[iDimension] = (destMax[iDimension] - destMin[iDimension] + (1 << iLOD) - 1) >> iLOD;
    }
  }

  ProjectVars projectVars;
  for (int iDim = 0; iDim < Dimensionality_Max; iDim++)
  {
    projectVars.requestedMin[iDim] = destMin[iDim];
    projectVars.requestedMax[iDim] = destMax[iDim];
    projectVars.requestedSizeThisLOD[iDim] = sizeThisLod[iDim];
    projectVars.requestedPitch[iDim] = iDim == 0 ? 1 : projectVars.requestedPitch[iDim - 1] * projectVars.requestedSizeThisLOD[iDim - 1];
  }

  projectVars.lod = iLOD;
  projectVars.voxelPlane = FloatVector4(voxelPlane.X, voxelPlane.Y, voxelPlane.Z, voxelPlane.T);
  projectVars.projectionDimension = projectionDimension;
  projectVars.projectedDimensions[0] = projectedDimensions[0];
  projectVars.projectedDimensions[1] = projectedDimensions[1];

  const void* sourceBuffer = page->getRawBufferInternal();

  IndexValues indexValues;
  indexValues.initialize(chunk, page->getDataBlock());

  projectValuesCPU(destBuffer, sourceBuffer, projectVars, indexValues, voxelFormat, interpolationMethod, volumeDataLayer->getIntegerScale(), volumeDataLayer->getIntegerOffset(), useNoValue, noValue);
  return true;
}

int64_t staticRequestProjectedVolumeSubset(VolumeDataRequestProcessor &request_processor, void *buffer, VolumeDataLayer *volumeDataLayer, const int32_t (&minRequested)[Dimensionality_Max], const int32_t (&maxRequested)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionGroup projectedDimensions, int32_t iLOD, VolumeDataChannelDescriptor::Format eFormat, InterpolationMethod interpolationMethod, bool isReplaceNoValue, float replacementNoValue)
{
  Box boxRequested;
  memcpy(boxRequested.min, minRequested, sizeof(boxRequested.min));
  memcpy(boxRequested.max, maxRequested, sizeof(boxRequested.max));

  // Initialized unused dimensions
  for (int32_t iDimension = volumeDataLayer->getLayout()->getDimensionality(); iDimension < Dimensionality_Max; iDimension++)
  {
    boxRequested.min[iDimension] = 0;
    boxRequested.min[iDimension] = 1;
  }

  std::vector<VolumeDataChunk> chunksInRegion;

  int32_t projectionDimension = -1;
  int32_t projectionDimensionPosition;
  int32_t projectedDimensionsPair[2] = { -1, -1 };

  int32_t layerDimensionGroup = DimensionGroupUtil::getDimensionality(volumeDataLayer->getChunkDimensionGroup());
  if (layerDimensionGroup < 3)
  {
    fmt::print(stderr, "The requested dimension group must contain at least 3 dimensions.");
    abort();
  }

  if (DimensionGroupUtil::getDimensionality(projectedDimensions) != 2)
  {
    fmt::print(stderr, "The projected dimension group must contain 2 dimensions.");
    abort();
  }

  for (int iDimIndex = 0; iDimIndex < layerDimensionGroup; iDimIndex++)
  {
    int32_t iDim = DimensionGroupUtil::getDimension(volumeDataLayer->getChunkDimensionGroup(), iDimIndex);

    if (!DimensionGroupUtil::isDimensionInGroup(projectedDimensions, iDim))
    {
      projectionDimension = iDim;
      projectionDimensionPosition = iDimIndex;

      // min/max in the projection dimension is not used
      boxRequested.min[iDim] = 0;
      boxRequested.max[iDim] = 1;
    }
  }

  assert(projectionDimension != -1);

  for (int iDimIndex = 0; iDimIndex < DimensionGroupUtil::getDimensionality(projectedDimensions); iDimIndex++)
  {
    int32_t iDim = DimensionGroupUtil::getDimension(projectedDimensions, iDimIndex);
    projectedDimensionsPair[iDimIndex] = iDim;

    if (!DimensionGroupUtil::isDimensionInGroup(volumeDataLayer->getChunkDimensionGroup(), iDim))
    {
      fmt::print(stderr,"The requested dimension group must contain the dimensions of the projected dimension group.");
      abort();
    }
  }

  //Swap components of VoxelPlane based on projection dimension
  FloatVector4 voxelPlaneSwapped(voxelPlane);

  if (projectionDimensionPosition < 2)
  {
    //need to swap
    if (projectionDimensionPosition == 1)
    {
      voxelPlaneSwapped.Y = voxelPlane.Z;
      voxelPlaneSwapped.Z = voxelPlane.Y;
    }
    else
    {
      voxelPlaneSwapped.X = voxelPlane.Y;
      voxelPlaneSwapped.Y = voxelPlane.Z;
      voxelPlaneSwapped.Z = voxelPlane.X;
    }
  }

  if (voxelPlaneSwapped.Z == 0)
  {
    fmt::print(stderr, "The Voxel plane cannot be perpindicular to the projected dimensions.");
    abort();
  }

  std::vector<VolumeDataChunk> chunksInProjectedRegion;
  std::vector<VolumeDataChunk> chunksIntersectingPlane;

  volumeDataLayer->getChunksInRegion(boxRequested.min,
                                     boxRequested.max,
                                     &chunksInProjectedRegion);

  for (int iChunk = 0; iChunk < chunksInProjectedRegion.size(); iChunk++)
  {
    int32_t min[Dimensionality_Max];
    int32_t max[Dimensionality_Max];

    chunksInProjectedRegion[iChunk].layer->getChunkMinMax(chunksInProjectedRegion[iChunk].chunkIndex, min, max, true);

    for (int iDimIndex = 0; iDimIndex < 2; iDimIndex++)
    {
      int32_t iDim = projectedDimensionsPair[iDimIndex];

      if (min[iDim] < minRequested[iDim]) min[iDim] = minRequested[iDim];
      if (max[iDim] > maxRequested[iDim]) max[iDim] = maxRequested[iDim];
    }

    int32_t corners[4];

    corners[0] = (int32_t)(((voxelPlaneSwapped.X * min[projectedDimensionsPair[0]] + voxelPlaneSwapped.Y * min[projectedDimensionsPair[1]] + voxelPlaneSwapped.T) / (-voxelPlaneSwapped.Z)) + 0.5f);
    corners[1] = (int32_t)(((voxelPlaneSwapped.X * min[projectedDimensionsPair[0]] + voxelPlaneSwapped.Y * max[projectedDimensionsPair[1]] + voxelPlaneSwapped.T) / (-voxelPlaneSwapped.Z)) + 0.5f);
    corners[2] = (int32_t)(((voxelPlaneSwapped.X * max[projectedDimensionsPair[0]] + voxelPlaneSwapped.Y * min[projectedDimensionsPair[1]] + voxelPlaneSwapped.T) / (-voxelPlaneSwapped.Z)) + 0.5f);
    corners[3] = (int32_t)(((voxelPlaneSwapped.X * max[projectedDimensionsPair[0]] + voxelPlaneSwapped.Y * max[projectedDimensionsPair[1]] + voxelPlaneSwapped.T) / (-voxelPlaneSwapped.Z)) + 0.5f);

    int32_t nMin = corners[0];
    int32_t nMax = corners[0] + 1;

    for (int i = 1; i < 4; i++)
    {
      if (corners[i] < nMin) nMin = corners[i];
      if (corners[i] + 1 > nMax) nMax = corners[i] + 1;
    }

    Box boxProjected = boxRequested;

    boxProjected.min[projectionDimension] = nMin;
    boxProjected.max[projectionDimension] = nMax;

    boxProjected.min[projectedDimensionsPair[0]] = min[projectedDimensionsPair[0]];
    boxProjected.max[projectedDimensionsPair[0]] = max[projectedDimensionsPair[0]];

    boxProjected.min[projectedDimensionsPair[1]] = min[projectedDimensionsPair[1]];
    boxProjected.max[projectedDimensionsPair[1]] = max[projectedDimensionsPair[1]];

    volumeDataLayer->getChunksInRegion(boxProjected.min,
                                       boxProjected.max,
                                            &chunksIntersectingPlane);

    for (int iChunkInPlane = 0; iChunkInPlane < chunksIntersectingPlane.size(); iChunkInPlane++)
    {
      VolumeDataChunk &chunkInIntersectingPlane = chunksIntersectingPlane[iChunkInPlane];
      auto chunk_it = std::find_if(chunksInRegion.begin(), chunksInRegion.end(), [&chunkInIntersectingPlane] (const VolumeDataChunk &a) { return a.chunkIndex == chunkInIntersectingPlane.chunkIndex && a.layer == chunkInIntersectingPlane.layer; });
      if (chunk_it == chunksInRegion.end())
      {
        chunksInRegion.push_back(chunkInIntersectingPlane);
      }
    }

    chunksIntersectingPlane.clear();
  }

  if(chunksInRegion.size() == 0)
  {
    fmt::print(stderr, "Requested volume subset does not contain any data");
    abort();
  }

  return request_processor.addJob(chunksInRegion, [boxRequested, buffer, projectedDimensions, voxelPlaneSwapped, interpolationMethod, isReplaceNoValue, replacementNoValue](VolumeDataPageImpl* page, VolumeDataChunk dataChunk, Error &error) { return requestProjectedVolumeSubsetProcessPage(page, dataChunk, boxRequested.min, boxRequested.max, projectedDimensions, voxelPlaneSwapped, interpolationMethod, isReplaceNoValue, replacementNoValue, buffer, error);});
}

struct VolumeDataSamplePos
{
  NDPos pos;
  int32_t originalSample;
  int64_t chunkIndex;

  bool operator < (VolumeDataSamplePos const &rhs) const
  {
    return chunkIndex < rhs.chunkIndex;
  }
};

template <typename T, InterpolationMethod INTERPMETHOD, bool isUseNoValue>
static void SampleVolume(VolumeDataPageImpl *page, const VolumeDataLayer *volumeDataLayer, const std::vector<VolumeDataSamplePos> &volumeSamplePositions, int32_t iStartSamplePos, int32_t nSamplePos, float noValue, void *destBuffer)
{
  const DataBlock &dataBlock = page->getDataBlock();
  int64_t chunkIndex = page->getChunkIndex();

  int32_t chunkDimension0 = volumeDataLayer->getChunkDimension(0);
  int32_t chunkDimension1 = volumeDataLayer->getChunkDimension(1);
  int32_t chunkDimension2 = volumeDataLayer->getChunkDimension(2);

  assert(chunkDimension0 >= 0 && chunkDimension1 >= 0);

  VolumeDataChunk volumeDataChunk = { volumeDataLayer, chunkIndex };

  int32_t min[Dimensionality_Max];
  int32_t max[Dimensionality_Max];

  volumeDataLayer->getChunkMinMax(chunkIndex, min, max, true);

  int32_t lod = volumeDataLayer->getLOD();

  float lodScale = 1.0f / (1 << lod);

  int32_t iFullResolutionDimension = volumeDataLayer->getLayout()->getFullResolutionDimension();

  VolumeSampler<T, INTERPMETHOD, isUseNoValue> volumeSampler(dataBlock.size, dataBlock.pitch, volumeDataLayer->getValueRange().min, volumeDataLayer->getValueRange().max,
    volumeDataLayer->getIntegerScale(), volumeDataLayer->getIntegerOffset(), noValue, noValue);

  const T*buffer = (const T*)(page->getRawBufferInternal());

  for (int32_t iSamplePos = iStartSamplePos; iSamplePos < nSamplePos; iSamplePos++)
  {
    const VolumeDataSamplePos &volumeDataSamplePos = volumeSamplePositions[iSamplePos];

    if (volumeDataSamplePos.chunkIndex != chunkIndex) break;

    FloatVector3 pos((volumeDataSamplePos.pos.data[chunkDimension0] - min[chunkDimension0]) * (chunkDimension0 == iFullResolutionDimension ? 1 : lodScale),
                     (volumeDataSamplePos.pos.data[chunkDimension1] - min[chunkDimension1]) * (chunkDimension1 == iFullResolutionDimension ? 1 : lodScale),
                      0);

    if (chunkDimension2 >= 0)
    {
      pos[2] = (volumeDataSamplePos.pos.data[chunkDimension2] - min[chunkDimension2]) * (chunkDimension2 == iFullResolutionDimension ? 1 : lodScale);
    }


    typename InterpolatedRealType<T>::type value = volumeSampler.sample3D(buffer, pos);

    static_cast<float *>(destBuffer)[volumeDataSamplePos.originalSample] = (float)value;
  }
}

template <typename T>
static void SampleVolumeInit(VolumeDataPageImpl *page, const VolumeDataLayer *volumeDataLayer, const std::vector<VolumeDataSamplePos> &volumeDataSamplePositions, InterpolationMethod interpolationMethod, int32_t iStartSamplePos, int32_t nSamplePos, float noValue, void *destBuffer)
{
  if (volumeDataLayer->isUseNoValue())
  {
    switch (interpolationMethod)
    {
    case InterpolationMethod::Nearest:
      SampleVolume<T, InterpolationMethod::Nearest, true>(page, volumeDataLayer, volumeDataSamplePositions, iStartSamplePos, nSamplePos, noValue, destBuffer);
      break;
    case InterpolationMethod::Linear:
      SampleVolume<T, InterpolationMethod::Linear, true>(page, volumeDataLayer, volumeDataSamplePositions, iStartSamplePos, nSamplePos, noValue, destBuffer);
      break;
    case InterpolationMethod::Cubic:
      SampleVolume<T, InterpolationMethod::Cubic, true>(page, volumeDataLayer, volumeDataSamplePositions, iStartSamplePos, nSamplePos, noValue, destBuffer);
      break;
    case InterpolationMethod::Angular:
      SampleVolume<T, InterpolationMethod::Angular, true>(page, volumeDataLayer, volumeDataSamplePositions, iStartSamplePos, nSamplePos, noValue, destBuffer);
      break;
    case InterpolationMethod::Triangular:
      SampleVolume<T, InterpolationMethod::Triangular, true>(page, volumeDataLayer, volumeDataSamplePositions, iStartSamplePos, nSamplePos, noValue, destBuffer);
      break;
//    case InterpolationMethod::TriangularExcludingValuerangeMinAndLess:
//      SampleVolume<T, InterpolationMethod::TriangularExcludingValuerangeMinAndLess, true>(page, volumeDataLayer, iStartSamplePos, nSamplePos, noValue, destBuffer);
//      break;
    default:
      fmt::print(stderr, "Unknown interpolation method");
      abort();
      break;
    }
  }
  else
  {
    switch (interpolationMethod)
    {
    case InterpolationMethod::Nearest:
      SampleVolume<T, InterpolationMethod::Nearest, false>(page, volumeDataLayer, volumeDataSamplePositions, iStartSamplePos, nSamplePos, noValue, destBuffer);
      break;
    case InterpolationMethod::Linear:
      SampleVolume<T, InterpolationMethod::Linear, false>(page, volumeDataLayer, volumeDataSamplePositions, iStartSamplePos, nSamplePos, noValue, destBuffer);
      break;
    case InterpolationMethod::Cubic:
      SampleVolume<T, InterpolationMethod::Cubic, false>(page, volumeDataLayer, volumeDataSamplePositions, iStartSamplePos, nSamplePos, noValue, destBuffer);
      break;
    case InterpolationMethod::Angular:
      SampleVolume<T, InterpolationMethod::Angular, false>(page, volumeDataLayer, volumeDataSamplePositions, iStartSamplePos, nSamplePos, noValue, destBuffer);
      break;
    case InterpolationMethod::Triangular:
      SampleVolume<T, InterpolationMethod::Triangular, false>(page, volumeDataLayer, volumeDataSamplePositions, iStartSamplePos, nSamplePos, noValue, destBuffer);
      break;
//    case InterpolationMethod::TriangularExcludingValuerangeMinAndLess
//      SampleVolume<T, InterpolationMethod::TriangularExcludingValuerangeMinAndLess, false>(page, volumeDataLayer, iStartSamplePos, nSamplePos, noValue, destBuffer);
//      break;
    default:
      fmt::print(stderr, "Unknown interpolation method");
      abort();
      break;
    }
  }
}

static bool requestVolumeSamplesProcessPage(VolumeDataPageImpl *page, VolumeDataChunk &dataChunk, const std::vector<VolumeDataSamplePos> &volumeDataSamplePositions, InterpolationMethod interpolationMethod, bool isUseNoValue, bool isReplaceNoValue, float replacementNoValue, void *buffer, Error &error)
{
  int32_t  samplePosCount = int32_t(volumeDataSamplePositions.size());

  int32_t iStartSamplePos = 0;
  int32_t iEndSamplePos = samplePosCount - 1;

  // Binary search to find samples within chunk
  while (iStartSamplePos < iEndSamplePos)
  {
    int32_t iSamplePos = (iStartSamplePos + iEndSamplePos) / 2;

    int64_t iSampleChunkIndex = volumeDataSamplePositions[iSamplePos].chunkIndex;

    if (iSampleChunkIndex >= dataChunk.chunkIndex)
    {
      iEndSamplePos = iSamplePos;
    }
    else
    {
      iStartSamplePos = iSamplePos + 1;
    }
  }

  assert(volumeDataSamplePositions[iStartSamplePos].chunkIndex == dataChunk.chunkIndex &&
    (iStartSamplePos == 0 || volumeDataSamplePositions[size_t(iStartSamplePos) - 1].chunkIndex < dataChunk.chunkIndex));

  VolumeDataChannelDescriptor::Format format = page->getDataBlock().format;

  switch (format)
  {
  case VolumeDataChannelDescriptor::Format_1Bit:
    SampleVolumeInit<bool>(page, dataChunk.layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_U8:
    SampleVolumeInit<uint8_t>(page, dataChunk.layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_U16:
    SampleVolumeInit<uint16_t>(page, dataChunk.layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_U32:
    SampleVolumeInit<uint32_t>(page, dataChunk.layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_R32:
    SampleVolumeInit<float>(page, dataChunk.layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_U64:
    SampleVolumeInit<uint64_t>(page, dataChunk.layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_R64:
    SampleVolumeInit<double>(page, dataChunk.layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  }

  return true;
}

int64_t StaticRequestVolumeSamples(VolumeDataRequestProcessor &request_processor, void *buffer, VolumeDataLayer *volumeDataLayer, const float(*samplePositions)[Dimensionality_Max], int32_t samplePosCount, InterpolationMethod interpolationMethod, bool isReplaceNoValue, float replacementNoValue)
{
  std::shared_ptr<std::vector<VolumeDataSamplePos>> volumeDataSamplePositions = std::make_shared<std::vector<VolumeDataSamplePos>>();

  volumeDataSamplePositions->resize(samplePosCount);

  for(int32_t samplePos = 0 ; samplePos < samplePosCount; samplePos++)
  {
    VolumeDataSamplePos &volumeDataSamplePos = volumeDataSamplePositions->at(samplePos);

    std::copy(&samplePositions[samplePos][0], &samplePositions[samplePos][Dimensionality_Max], volumeDataSamplePos.pos.data);
    volumeDataSamplePos.chunkIndex = volumeDataLayer->getChunkIndexFromNDPos(volumeDataSamplePos.pos);
    volumeDataSamplePos.originalSample = samplePos;
  }

  std::sort(volumeDataSamplePositions->begin(), volumeDataSamplePositions->end());

  // Force NEAREST interpolation for discrete volume data
  if (volumeDataLayer->isDiscrete())
  {
    interpolationMethod = InterpolationMethod::Nearest;
  }

  std::vector<VolumeDataChunk> volumeDataChunks;
  int64_t currentChunkIndex = -1;

  for (int32_t samplePos = 0; samplePos < volumeDataSamplePositions->size(); samplePos++)
  {
    VolumeDataSamplePos &volumeDataSamplePos = volumeDataSamplePositions->at(samplePos);
    if (volumeDataSamplePos.chunkIndex != currentChunkIndex)
    {
      currentChunkIndex = volumeDataSamplePos.chunkIndex;
      volumeDataChunks.push_back(volumeDataLayer->getChunkFromIndex(currentChunkIndex));
    }
  }

  return request_processor.addJob(volumeDataChunks, [buffer, volumeDataSamplePositions, interpolationMethod, isReplaceNoValue, replacementNoValue](VolumeDataPageImpl* page, VolumeDataChunk dataChunk, Error& error)
    {
      return requestVolumeSamplesProcessPage(page, dataChunk,  *volumeDataSamplePositions, interpolationMethod, dataChunk.layer->isUseNoValue(), isReplaceNoValue, isReplaceNoValue ? replacementNoValue : dataChunk.layer->getNoValue(), buffer, error);
    });
}

template <typename T, InterpolationMethod INTERPMETHOD, bool isUseNoValue>
void TraceVolume(VolumeDataPageImpl *page, const VolumeDataChunk &chunk, const std::vector<VolumeDataSamplePos> &volumeDataSamplePositions, int32_t traceDimension, float noValue, void *targetBuffer)
{
  int32_t traceSize = chunk.layer->getDimensionNumSamples(traceDimension);

  float *traceBuffer = reinterpret_cast<float *>(targetBuffer);

  const DataBlock & dataBlock = page->getDataBlock();

  const VolumeDataLayer *volumeDataLayer = chunk.layer;

  int32_t chunkDimension0 = volumeDataLayer->getChunkDimension(0);
  int32_t chunkDimension1 = volumeDataLayer->getChunkDimension(1);
  int32_t chunkDimension2 = volumeDataLayer->getChunkDimension(2);

  assert(chunkDimension0 >= 0 && chunkDimension1 >= 0);

  int32_t  traceDimensionInChunk = -1;

  if (chunkDimension0 == traceDimension)
    traceDimensionInChunk = 0;
  else if (chunkDimension1 == traceDimension)
    traceDimensionInChunk = 1;
  else if (chunkDimension2 == traceDimension)
    traceDimensionInChunk = 2;

  int32_t min[Dimensionality_Max];
  int32_t max[Dimensionality_Max];
  int32_t minExcludingMargin[Dimensionality_Max];
  int32_t maxExcludingMargin[Dimensionality_Max];

  volumeDataLayer->getChunkMinMax(chunk.chunkIndex, min, max, true);
  volumeDataLayer->getChunkMinMax(chunk.chunkIndex, minExcludingMargin, maxExcludingMargin, false);

  int32_t lod = volumeDataLayer->getLOD();

  float lodScale = 1.0f / (1 << lod);

  int32_t fullResolutionDimension = volumeDataLayer->getLayout()->getFullResolutionDimension();

  VolumeSampler<T, INTERPMETHOD, isUseNoValue> volumeSampler(dataBlock.size, dataBlock.pitch, volumeDataLayer->getValueRange().min, volumeDataLayer->getValueRange().max, volumeDataLayer->getIntegerScale(), volumeDataLayer->getIntegerOffset(), noValue, noValue);

  const T* pBuffer = (const T*) page->getRawBufferInternal();

  int32_t overlapCount = (maxExcludingMargin[traceDimension] - minExcludingMargin[traceDimension] + (1 << lod) - 1) >> lod;

  int32_t offsetSource = (minExcludingMargin[traceDimension] - min[traceDimension]) >> lod;
  int32_t offsetTarget = (minExcludingMargin[traceDimension]) >> lod;

  int32_t traceCount = int32_t(volumeDataSamplePositions.size());

  for (int32_t trace = 0; trace < traceCount; trace++)
  {
    const VolumeDataSamplePos &volumeDataSamplePos = volumeDataSamplePositions[trace];

    bool isInside = true;

    for (int dim = 0; dim < Dimensionality_Max; dim++)
    {
      if (dim != traceDimension &&
        ((int32_t)volumeDataSamplePos.pos.data[dim] < minExcludingMargin[dim] ||
         (int32_t)volumeDataSamplePos.pos.data[dim] >= maxExcludingMargin[dim]))
      {
        isInside = false;
        break;
      }
    }

    if (!isInside) continue;

    FloatVector3 pos((volumeDataSamplePos.pos.data[chunkDimension0] - min[chunkDimension0]) * (chunkDimension0 == fullResolutionDimension ? 1 : lodScale),
      (volumeDataSamplePos.pos.data[chunkDimension1] - min[chunkDimension1]) * (chunkDimension1 == fullResolutionDimension ? 1 : lodScale),
      0.5f);

    if (chunkDimension2 >= 0)
    {
      pos[2] = (volumeDataSamplePos.pos.data[chunkDimension2] - min[chunkDimension2]) * (chunkDimension2 == fullResolutionDimension ? 1 : lodScale);
    }

    for (int overlap = 0; overlap < overlapCount; overlap++)
    {
      if (traceDimensionInChunk != -1)
      {
        pos[traceDimensionInChunk] = overlap + offsetSource + 0.5f; // so that we sample the center of the voxel
      }

      typename InterpolatedRealType<T>::type value = volumeSampler.sample3D(pBuffer, pos);

      traceBuffer[traceSize * volumeDataSamplePos.originalSample + overlap + offsetTarget] = (float)value;
    }
  }
}

template <typename T>
static void TraceVolumeInit(VolumeDataPageImpl *page, const VolumeDataChunk &chunk, const std::vector<VolumeDataSamplePos> &volumeDataSamplePositions, InterpolationMethod interpolationMethod, int32_t traceDimension, float noValue, void *targetBuffer)
{
  if (chunk.layer->isUseNoValue())
  {
    switch (interpolationMethod)
    {
    case InterpolationMethod::Nearest:
      TraceVolume<T, InterpolationMethod::Nearest, true>(page, chunk, volumeDataSamplePositions, traceDimension, noValue, targetBuffer);
      break;
    case InterpolationMethod::Linear:
      TraceVolume<T, InterpolationMethod::Linear, true>(page, chunk, volumeDataSamplePositions, traceDimension, noValue, targetBuffer);
      break;
    case InterpolationMethod::Cubic:
      TraceVolume<T, InterpolationMethod::Cubic, true>(page, chunk, volumeDataSamplePositions, traceDimension, noValue, targetBuffer);
      break;
    case InterpolationMethod::Angular:
      TraceVolume<T, InterpolationMethod::Angular, true>(page, chunk, volumeDataSamplePositions, traceDimension, noValue, targetBuffer);
      break;
    case InterpolationMethod::Triangular:
      TraceVolume<T, InterpolationMethod::Triangular, true>(page, chunk, volumeDataSamplePositions, traceDimension, noValue, targetBuffer);
      break;
//    case InterpolationMethod::TriangularExcludingValuerangeMinAndLess:
//      TraceVolume<T, InterpolationMethod::TriangularExcludingValuerangeMinAndLess, true>(page, chunk, volumeDataSamplePositions, traceDimension, noValue, targetBuffer);
//      break;
    default:
      fmt::print(stderr, "Unknown interpolation method");
      abort();
      break;
    }
  }
  else
  {
    switch (interpolationMethod)
    {
    case InterpolationMethod::Nearest:
      TraceVolume<T, InterpolationMethod::Nearest, false>(page, chunk, volumeDataSamplePositions, traceDimension, noValue, targetBuffer);
      break;
    case InterpolationMethod::Linear:
      TraceVolume<T, InterpolationMethod::Linear, false>(page, chunk, volumeDataSamplePositions, traceDimension, noValue, targetBuffer);
      break;
    case InterpolationMethod::Cubic:
      TraceVolume<T, InterpolationMethod::Cubic, false>(page, chunk, volumeDataSamplePositions, traceDimension, noValue, targetBuffer);
      break;
    case InterpolationMethod::Angular:
      TraceVolume<T, InterpolationMethod::Angular, false>(page, chunk, volumeDataSamplePositions, traceDimension, noValue, targetBuffer);
      break;
    case InterpolationMethod::Triangular:
      TraceVolume<T, InterpolationMethod::Triangular, false>(page, chunk, volumeDataSamplePositions, traceDimension, noValue, targetBuffer);
      break;
//    case InterpolationMethod::TriangularExcludingValuerangeMinAndLess:
//      TraceVolume<T, InterpolationMethod::TriangularExcludingValuerangeMinAndLess, false>(page, chunk, volumeDataSamplePositions, traceDimension, noValue, targetBuffer);
//      break;
    default:
      fmt::print(stderr, "Unknown interpolation method");
      abort();
      break;
    }
  }
}

static bool requestVolumeTracesProcessPage (VolumeDataPageImpl *page, VolumeDataChunk &dataChunk, const std::vector<VolumeDataSamplePos> &volumeDataSamplePositions, InterpolationMethod interpolationMethod, int32_t traceDimension, float noValue, void *buffer, Error &error)
{
  VolumeDataChannelDescriptor::Format format = dataChunk.layer->getFormat();
  switch (format)
  {
  case VolumeDataChannelDescriptor::Format_1Bit:
    TraceVolumeInit<bool>(page, dataChunk, volumeDataSamplePositions, interpolationMethod, traceDimension, noValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_U8:
    TraceVolumeInit<uint8_t>(page, dataChunk, volumeDataSamplePositions, interpolationMethod, traceDimension, noValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_U16:
    TraceVolumeInit<uint16_t>(page, dataChunk, volumeDataSamplePositions, interpolationMethod, traceDimension, noValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_U32:
    TraceVolumeInit<uint32_t>(page, dataChunk, volumeDataSamplePositions, interpolationMethod, traceDimension, noValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_R32:
    TraceVolumeInit<float>(page, dataChunk, volumeDataSamplePositions, interpolationMethod, traceDimension, noValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_U64:
    TraceVolumeInit<uint64_t>(page, dataChunk, volumeDataSamplePositions, interpolationMethod, traceDimension, noValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_R64:
    TraceVolumeInit<double>(page, dataChunk, volumeDataSamplePositions, interpolationMethod, traceDimension, noValue, buffer);
    break;
  }
  return true;
}

static int64_t StaticRequestVolumeTraces(VolumeDataRequestProcessor &request_processor, void *buffer, VolumeDataLayer *volumeDataLayer, const float(*tracePositions)[Dimensionality_Max], int32_t tracePositionsCount, int32_t lod, InterpolationMethod interpolationMethod, int32_t traceDimension, bool isReplaceNoValue, float replacementNoValue)
{
  if (traceDimension < 0 || traceDimension >= Dimensionality_Max)
  {
    fmt::print("The trace dimension must be a valid dimension.");
    abort();
  }

  std::shared_ptr<std::vector<VolumeDataSamplePos>> volumeDataSamplePositions = std::make_shared<std::vector<VolumeDataSamplePos>>();
  volumeDataSamplePositions->resize(tracePositionsCount);

  for (int32_t tracePos = 0; tracePos < tracePositionsCount; tracePos++)
  {
    VolumeDataSamplePos &volumeDataSamplePos = volumeDataSamplePositions->at(tracePos);

    std::copy(&tracePositions[tracePos][0], &tracePositions[tracePos][Dimensionality_Max], volumeDataSamplePos.pos.data);
    volumeDataSamplePos.chunkIndex = volumeDataLayer->getChunkIndexFromNDPos(volumeDataSamplePos.pos);
    volumeDataSamplePos.originalSample = tracePos;
  }

  std::sort(volumeDataSamplePositions->begin(), volumeDataSamplePositions->end());

  // Force NEAREST interpolation for discrete volume data
  if (volumeDataLayer->isDiscrete())
  {
    interpolationMethod = InterpolationMethod::Nearest;
  }

  int64_t currentChunkIndex = -1;
  int32_t totalChunks = 0;
  
  std::vector<VolumeDataChunk> volumeDataChunks;
  int32_t traceMin[Dimensionality_Max];
  memset(traceMin, 0, sizeof(traceMin));
  int32_t traceMax[Dimensionality_Max];
  memset(traceMax, 0, sizeof(traceMax));

  for (int32_t tracePos = 0; tracePos < tracePositionsCount; tracePos++)
  {
    VolumeDataSamplePos &volumeDataSamplePos = volumeDataSamplePositions->at(tracePos);
    if (volumeDataSamplePos.chunkIndex != currentChunkIndex)
    {
      currentChunkIndex = volumeDataSamplePos.chunkIndex;

      VolumeDataChunk volumeDataChunk(volumeDataLayer->getChunkFromIndex(currentChunkIndex));
      volumeDataChunks.push_back(volumeDataChunk); 
      
      for (int dim = 0; dim < Dimensionality_Max; dim++)
      {
        traceMin[dim] = (int32_t)volumeDataSamplePos.pos.data[dim];
        traceMax[dim] = (int32_t)volumeDataSamplePos.pos.data[dim] + 1;
      }

      traceMin[traceDimension] = 0;
      traceMax[traceDimension] = volumeDataLayer->getDimensionNumSamples(traceDimension);

      int32_t currentChunksCount = int32_t(volumeDataChunks.size());

      volumeDataLayer->getChunksInRegion(traceMin, traceMax, &volumeDataChunks, true);

      totalChunks += int32_t(volumeDataChunks.size()) - currentChunksCount;
    }
  }

  return request_processor.addJob(volumeDataChunks, [buffer, volumeDataSamplePositions, interpolationMethod, traceDimension, replacementNoValue](VolumeDataPageImpl* page, VolumeDataChunk dataChunk, Error& error)
    {
      return requestVolumeTracesProcessPage(page, dataChunk,  *volumeDataSamplePositions, interpolationMethod, traceDimension, replacementNoValue, buffer, error);
    });
}

int64_t VolumeDataAccessManagerImpl::requestVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format)
{
  return staticRequestVolumeSubset(m_requestProcessor, buffer, getLayer(volumeDataLayout, dimensionsND, lod, channel), minVoxelCoordinates, maxVoxelCoordinates, lod, format, false, 0.0f);
}

int64_t VolumeDataAccessManagerImpl::requestVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, float replacementNoValue)
{
  return staticRequestVolumeSubset(m_requestProcessor, buffer, getLayer(volumeDataLayout, dimensionsND, lod, channel), minVoxelCoordinates, maxVoxelCoordinates, lod, format, true, replacementNoValue);
}
int64_t VolumeDataAccessManagerImpl::requestProjectedVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const& voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod)
{
  return staticRequestProjectedVolumeSubset(m_requestProcessor, buffer, getLayer(volumeDataLayout, dimensionsND, lod, channel), minVoxelCoordinates, maxVoxelCoordinates, voxelPlane, DimensionGroupUtil::getDimensionGroupFromDimensionsND(projectedDimensions), lod, format, interpolationMethod, false, 0.0f);
}

int64_t VolumeDataAccessManagerImpl::requestProjectedVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const& voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, float replacementNoValue)
{
  return staticRequestProjectedVolumeSubset(m_requestProcessor, buffer, getLayer(volumeDataLayout, dimensionsND, lod, channel), minVoxelCoordinates, maxVoxelCoordinates, voxelPlane, DimensionGroupUtil::getDimensionGroupFromDimensionsND(projectedDimensions), lod, format, interpolationMethod, true, replacementNoValue);
}

int64_t VolumeDataAccessManagerImpl::requestVolumeSamples(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*samplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod)
{
  return StaticRequestVolumeSamples(m_requestProcessor, buffer, getLayer(volumeDataLayout, dimensionsND, lod, channel), samplePositions, sampleCount, interpolationMethod, false, 0.0f);
}
int64_t VolumeDataAccessManagerImpl::requestVolumeSamples(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*samplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, float replacementNoValue)
{
  return StaticRequestVolumeSamples(m_requestProcessor, buffer, getLayer(volumeDataLayout, dimensionsND, lod, channel), samplePositions, sampleCount, interpolationMethod, true, replacementNoValue);
}
int64_t VolumeDataAccessManagerImpl::requestVolumeTraces(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int traceDimension)
{
  return StaticRequestVolumeTraces(m_requestProcessor, buffer, getLayer(volumeDataLayout, dimensionsND, lod, channel), tracePositions, traceCount, lod, interpolationMethod, traceDimension, false, 0.0f);
}
int64_t VolumeDataAccessManagerImpl::requestVolumeTraces(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int traceDimension, float replacementNoValue)
{
  return StaticRequestVolumeTraces(m_requestProcessor, buffer, getLayer(volumeDataLayout, dimensionsND, lod, channel), tracePositions, traceCount, lod, interpolationMethod, traceDimension, true, replacementNoValue);
}
int64_t VolumeDataAccessManagerImpl::prefetchVolumeChunk(VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int64_t chunk)
{
  return int64_t(0);
}
}
