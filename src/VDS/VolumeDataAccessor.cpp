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
#include <OpenVDS/ValueConversion.h>

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

void VolumeDataAccessorBase::UpdateWrittenRegion()
{
  if(m_writtenRegion.Max[0] != 0)
  {
    assert(m_currentPage);

    int writtenMin[Dimensionality_Max] = { m_writtenRegion.Min[3], m_writtenRegion.Min[2], m_writtenRegion.Min[1], m_writtenRegion.Min[0], 0, 0 };
    int writtenMax[Dimensionality_Max] = { m_writtenRegion.Max[3], m_writtenRegion.Max[2], m_writtenRegion.Max[1], m_writtenRegion.Max[0], 1, 1 };
    m_currentPage->UpdateWrittenRegion(writtenMin, writtenMax);

    m_writtenRegion = AccessorRegion({0, 0, 0, 0}, {0, 0, 0, 0});
  }
}

//-----------------------------------------------------------------------------

void VolumeDataAccessorBase::MakeCurrentPageWritable()
{
  int pitch[Dimensionality_Max];

  m_buffer = m_currentPage->GetWritableBuffer(pitch);
  m_pitch = {pitch[3], pitch[2], pitch[1], pitch[0]};
  m_writable = true;
}

//-----------------------------------------------------------------------------

VolumeDataLayout const *VolumeDataAccessorBase::GetLayout()
{
  return m_volumeDataPageAccessor->GetLayout();
}

//-----------------------------------------------------------------------------

void VolumeDataAccessorBase::Commit()
{
  UpdateWrittenRegion();
  m_volumeDataPageAccessor->Commit();
}

//-----------------------------------------------------------------------------

void VolumeDataAccessorBase::Cancel()
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

  m_volumeDataPageAccessor->GetNumSamples(numSamples);
  m_numSamples = {numSamples[3], numSamples[2], numSamples[1], numSamples[0]};
}

//-----------------------------------------------------------------------------

VolumeDataAccessorBase::~VolumeDataAccessorBase()
{
  if(m_currentPage)
  {
    UpdateWrittenRegion();
    m_currentPage->Release();
    m_currentPage = nullptr;
  }
  if(m_volumeDataPageAccessor)
  {
    if(m_volumeDataPageAccessor->RemoveReference() == 0)
    {
      if(!m_canceled)
      {
        m_volumeDataPageAccessor->Commit();
      }

      m_volumeDataPageAccessor->GetManager()->DestroyVolumeDataPageAccessor(m_volumeDataPageAccessor);
    }
    m_volumeDataPageAccessor = nullptr;
  }
}

void VolumeDataAccessorBase::ReadPageAtPosition(IntVector4 index, bool enableWriting)
{
  if(m_currentPage)
  {
    UpdateWrittenRegion();
    m_currentPage->Release();
    m_currentPage = nullptr;
  }

  assert(m_writtenRegion.Max[0] == 0);

  int position[Dimensionality_Max] = { index[3], index[2], index[1], index[0], 0, 0 };

  VolumeDataPage *page = m_volumeDataPageAccessor->ReadPageAtPosition(position);

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

  page->GetMinMax(min, max);
  m_min = {min[3], min[2], min[1], min[0]};
  m_max = {max[3], max[2], max[1], max[0]};

  page->GetMinMaxExcludingMargin(minExcludingMargin, maxExcludingMargin);
  m_validRegion = AccessorRegion({minExcludingMargin[3], minExcludingMargin[2], minExcludingMargin[1], minExcludingMargin[0]}, {maxExcludingMargin[3], maxExcludingMargin[2], maxExcludingMargin[1], maxExcludingMargin[0]});

  int pitch[Dimensionality_Max];

  m_buffer = enableWriting ? page->GetWritableBuffer(pitch) : const_cast<void *>(page->GetBuffer(pitch));
  m_pitch = {pitch[3], pitch[2], pitch[1], pitch[0]};
  m_writable = enableWriting;
}

template <typename INDEX, typename T>
VolumeDataReadWriteAccessor<INDEX, T> *VolumeDataAccess_CreateVolumeDataAccessor(VolumeDataPageAccessor *v, float replacementNoValue)
{
  assert(v);

  VolumeDataPageAccessorImpl *volumeDataPageAccessor = static_cast<VolumeDataPageAccessorImpl *>(v);
  VolumeDataLayout const *volumeDataLayout = volumeDataPageAccessor->GetLayout();
  int32_t channel = volumeDataPageAccessor->GetChannelIndex();

  if(volumeDataLayout->IsChannelUseNoValue(channel))
  {
    switch(volumeDataLayout->GetChannelFormat(channel))
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
    switch(volumeDataLayout->GetChannelFormat(channel))
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
  int channel = volumeDataPageAccessor->GetChannelIndex();

  const VolumeDataLayout *volumeDataLayout = static_cast<VolumeDataPageAccessorImpl *>(volumeDataPageAccessor)->GetLayout();

  if(volumeDataLayout->IsChannelUseNoValue(channel))
  {
    switch(volumeDataLayout->GetChannelFormat(channel))
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
    switch(volumeDataLayout->GetChannelFormat(channel))
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

VolumeDataReadWriteAccessor<IntVector2, bool>* VolumeDataAccessManagerImpl::Create2DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, bool>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector2, uint8_t>* VolumeDataAccessManagerImpl::Create2DVolumeDataAccessorU8(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, uint8_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector2, uint16_t>* VolumeDataAccessManagerImpl::Create2DVolumeDataAccessorU16(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, uint16_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector2, uint32_t>* VolumeDataAccessManagerImpl::Create2DVolumeDataAccessorU32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, uint32_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector2, uint64_t>* VolumeDataAccessManagerImpl::Create2DVolumeDataAccessorU64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, uint64_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector2, float>* VolumeDataAccessManagerImpl::Create2DVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, float>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector2, double>* VolumeDataAccessManagerImpl::Create2DVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector2, double>(volumeDataPageAccessor, replacementNoValue);
}

VolumeDataReadWriteAccessor<IntVector3, bool>* VolumeDataAccessManagerImpl::Create3DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, bool>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector3, uint8_t>* VolumeDataAccessManagerImpl::Create3DVolumeDataAccessorU8(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, uint8_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector3, uint16_t>* VolumeDataAccessManagerImpl::Create3DVolumeDataAccessorU16(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, uint16_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector3, uint32_t>* VolumeDataAccessManagerImpl::Create3DVolumeDataAccessorU32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, uint32_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector3, uint64_t>* VolumeDataAccessManagerImpl::Create3DVolumeDataAccessorU64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, uint64_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector3, float>* VolumeDataAccessManagerImpl::Create3DVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, float>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector3, double>* VolumeDataAccessManagerImpl::Create3DVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector3, double>(volumeDataPageAccessor, replacementNoValue);
}

VolumeDataReadWriteAccessor<IntVector4, bool>* VolumeDataAccessManagerImpl::Create4DVolumeDataAccessor1Bit(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, bool>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector4, uint8_t>* VolumeDataAccessManagerImpl::Create4DVolumeDataAccessorU8(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, uint8_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector4, uint16_t>* VolumeDataAccessManagerImpl::Create4DVolumeDataAccessorU16(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, uint16_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector4, uint32_t>* VolumeDataAccessManagerImpl::Create4DVolumeDataAccessorU32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, uint32_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector4, uint64_t>* VolumeDataAccessManagerImpl::Create4DVolumeDataAccessorU64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, uint64_t>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector4, float>* VolumeDataAccessManagerImpl::Create4DVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, float>(volumeDataPageAccessor, replacementNoValue);
}
VolumeDataReadWriteAccessor<IntVector4, double>* VolumeDataAccessManagerImpl::Create4DVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue)
{
  return VolumeDataAccess_CreateVolumeDataAccessor<IntVector4, double>(volumeDataPageAccessor, replacementNoValue);
}

VolumeDataReadAccessor<FloatVector2, float >* VolumeDataAccessManagerImpl::Create2DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return VolumeDataAccess_CreateInterpolatingVolumeDataAccessor<FloatVector2, float>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
}
VolumeDataReadAccessor<FloatVector2, double>* VolumeDataAccessManagerImpl::Create2DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return VolumeDataAccess_CreateInterpolatingVolumeDataAccessor<FloatVector2, double>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
}
VolumeDataReadAccessor<FloatVector3, float >* VolumeDataAccessManagerImpl::Create3DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return VolumeDataAccess_CreateInterpolatingVolumeDataAccessor<FloatVector3, float >(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
}
VolumeDataReadAccessor<FloatVector3, double>* VolumeDataAccessManagerImpl::Create3DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return VolumeDataAccess_CreateInterpolatingVolumeDataAccessor<FloatVector3, double>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
}
VolumeDataReadAccessor<FloatVector4, float >* VolumeDataAccessManagerImpl::Create4DInterpolatingVolumeDataAccessorR32(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
  return VolumeDataAccess_CreateInterpolatingVolumeDataAccessor<FloatVector4, float >(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
}

VolumeDataReadAccessor<FloatVector4, double>* VolumeDataAccessManagerImpl::Create4DInterpolatingVolumeDataAccessorR64(VolumeDataPageAccessor* volumeDataPageAccessor, float replacementNoValue, InterpolationMethod interpolationMethod)
{
   return VolumeDataAccess_CreateInterpolatingVolumeDataAccessor<FloatVector4, double>(volumeDataPageAccessor, replacementNoValue, interpolationMethod);
}

static int32_t CombineAndReduceDimensions (int32_t (&sourceSize  )[DataStoreDimensionality_Max],
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
  for (int32_t dimension = 0; dimension < DataStoreDimensionality_Max; dimension++)
  {
    if(tmpSourceSize[dimension] == 1 && tmpTargetSize[dimension] == 1)
    {
      assert(tmpSourceOffset[dimension] == 0);
      assert(tmpTargetOffset[dimension] == 0);
      assert(tmpOverlapSize [dimension] == 1);
      continue;
    }

    sourceOffset[nCopyDimensions] = tmpSourceOffset[dimension];
    targetOffset[nCopyDimensions] = tmpTargetOffset[dimension];

    sourceSize[nCopyDimensions] = tmpSourceSize[dimension];
    targetSize[nCopyDimensions] = tmpTargetSize[dimension];

    overlapSize[nCopyDimensions] = tmpOverlapSize[dimension];

    nCopyDimensions++;
  }

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

    for (int dimension = 1; dimension < nCopyDimensions - 1; dimension++)
    {
      sourceOffset[dimension] = sourceOffset[dimension+1];
      targetOffset[dimension] = targetOffset[dimension+1];

      sourceSize [dimension] = sourceSize [dimension+1];
      targetSize [dimension] = targetSize [dimension+1];
      overlapSize[dimension] = overlapSize[dimension+1];
    }

    nCopyDimensions -= 1;
  }

  // Reset remaining dimensions
  for(int32_t dimension = nCopyDimensions; dimension < DataStoreDimensionality_Max; dimension++)
  {
    sourceOffset[dimension] = 0;
    targetOffset[dimension] = 0;

    sourceSize [dimension] = 1;
    targetSize [dimension] = 1;
    overlapSize[dimension] = 1;
  }

  return nCopyDimensions;
}

static force_inline void CopyBits(void* target, int64_t targetBit, const void* source, int64_t sourceBit, int32_t bits)
{
  while(bits--)
  {
    DataBlock_WriteElement(reinterpret_cast<bool *>(target), targetBit++, DataBlock_ReadElement(reinterpret_cast<const bool *>(source), sourceBit++));
  }
}

template <typename T>
static force_inline void CopyBytesT(T* __restrict target, const T* __restrict source, int32_t size)
{
  if (size >= MIN_MEMCPY)
  {
    memcpy (target, source, size_t(size));
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
      const uint8_t *sourceTail = (const uint8_t *) (source + nBigElements);
      uint8_t *targetTail = (uint8_t *) (target + nBigElements);

      int32_t iTail = 0;
      switch (nTail)
      {
      case 7: targetTail [iTail] = sourceTail [iTail]; iTail++; [[clang::fallthrough]];
      case 6: targetTail [iTail] = sourceTail [iTail]; iTail++; [[clang::fallthrough]];
      case 5: targetTail [iTail] = sourceTail [iTail]; iTail++; [[clang::fallthrough]];
      case 4: targetTail [iTail] = sourceTail [iTail]; iTail++; [[clang::fallthrough]];
      case 3: targetTail [iTail] = sourceTail [iTail]; iTail++; [[clang::fallthrough]];
      case 2: targetTail [iTail] = sourceTail [iTail]; iTail++; [[clang::fallthrough]];
      case 1: targetTail [iTail] = sourceTail [iTail];
      }
    }
  }
}

static force_inline void CopyBytes(void* target, const void* source, int32_t size)
{
  if (size >= sizeof (int64_t) && !((intptr_t) source & (sizeof (int64_t)-1)) && !((intptr_t) target & (sizeof (int64_t)-1)))
    CopyBytesT ((int64_t*) target, (int64_t*) source, size);
  else if (size >= sizeof (int32_t) && !((intptr_t) source & (sizeof (int32_t)-1)) && !((intptr_t) target & (sizeof (int32_t)-1)))
    CopyBytesT ((int32_t*) target, (int32_t*) source, size);
  else if (size >= sizeof (int16_t) && !((intptr_t) source & (sizeof (int16_t)-1)) && !((intptr_t) target & (sizeof (int16_t)-1)))
    CopyBytesT ((int16_t*) target, (int16_t*) source, size);
  else
    CopyBytesT ((int8_t*) target, (int8_t*) source, size);
}

template<typename T, int targetOneBit, typename S, int sourceOneBit>
struct BlockCopy
{
  static void Do(void       *target, const int32_t (&targetOffset)[DataStoreDimensionality_Max], const int32_t (&targetSize)[DataStoreDimensionality_Max],
                 void const *source, const int32_t (&sourceOffset)[DataStoreDimensionality_Max], const int32_t (&sourceSize)[DataStoreDimensionality_Max],
                 const int32_t (&overlapSize) [DataStoreDimensionality_Max], int32_t elementSize, int32_t copyDimensions)
  {
  }
};

template<typename T, int is1Bit>
struct BlockCopy<T, is1Bit, T, is1Bit>
{
  static void Do(void       *target, const int32_t (&targetOffset)[DataStoreDimensionality_Max], const int32_t (&targetSize)[DataStoreDimensionality_Max],
                 void const *source, const int32_t (&sourceOffset)[DataStoreDimensionality_Max], const int32_t (&sourceSize)[DataStoreDimensionality_Max],
                 const int32_t (&overlapSize) [DataStoreDimensionality_Max], int32_t elementSize, int32_t copyDimensions)
  {
  int64_t sourceLocalBaseSize = ((((int64_t)sourceOffset[3] * sourceSize[2] + sourceOffset[2]) * sourceSize[1] + sourceOffset[1]) * sourceSize[0] + sourceOffset[0]) * elementSize;
  int64_t targetLocalBaseSize = ((((int64_t)targetOffset[3] * targetSize[2] + targetOffset[2]) * targetSize[1] + targetOffset[1]) * targetSize[0] + targetOffset[0]) * elementSize;

  if (is1Bit)
  {
    for (int dimension3 = 0; dimension3 < overlapSize[3]; dimension3++)
    {
      for (int dimension2 = 0; dimension2 < overlapSize[2]; dimension2++)
      {
        for (int dimension1 = 0; dimension1 < overlapSize[1]; dimension1++)
        {
          int64_t sourceLocal = (((int64_t)dimension3 * sourceSize[2] + dimension2) * sourceSize[1] + dimension1) * (int64_t)sourceSize[0] * elementSize;
          int64_t targetLocal = (((int64_t)dimension3 * targetSize[2] + dimension2) * targetSize[1] + dimension1) * (int64_t)targetSize[0] * elementSize;

          CopyBits(target, targetLocalBaseSize + targetLocal, source, sourceLocalBaseSize + sourceLocal, overlapSize[0]);
        }
      }
    }
  }
  else
  {
    const uint8_t *sourceLocalBase = reinterpret_cast<const uint8_t *>(source) + sourceLocalBaseSize;

    uint8_t *targetLocalBase = reinterpret_cast<uint8_t *>(target) + targetLocalBaseSize;

    for (int dimension3 = 0; dimension3 < overlapSize[3]; dimension3++)
    {
      for (int dimension2 = 0; dimension2 < overlapSize[2]; dimension2++)
      {
        for (int dimension1 = 0; dimension1 < overlapSize[1]; dimension1++)
        {
          int64_t iSourceLocal = (((int64_t)dimension3 * sourceSize[2] + dimension2) * sourceSize[1] + dimension1) * (int64_t)sourceSize[0] * elementSize;
          int64_t iTargetLocal = (((int64_t)dimension3 * targetSize[2] + dimension2) * targetSize[1] + dimension1) * (int64_t)targetSize[0] * elementSize;

          CopyBytes(targetLocalBase + iTargetLocal, sourceLocalBase + iSourceLocal, overlapSize[0] * elementSize);
        }
      }
    }
  }
  }
};

template<typename T, int targetOneBit>
static void DispatchBlockCopy2(void *target, const int32_t (&targetOffset)[DataStoreDimensionality_Max], const int32_t (&targetSize)[DataStoreDimensionality_Max],
                              VolumeDataChannelDescriptor::Format sourceFormat,
                              void const *source, const int32_t (&sourceOffset)[DataStoreDimensionality_Max], const int32_t (&sourceSize)[DataStoreDimensionality_Max],
                              const int32_t (&overlapSize) [DataStoreDimensionality_Max], int32_t elementSize, int32_t copyDimensions)
{
  switch(sourceFormat)
  {
  case VolumeDataChannelDescriptor::Format_1Bit:
    return BlockCopy<T, targetOneBit, uint8_t, true>::Do(target, targetOffset, targetSize, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_U8:
    return BlockCopy<T, targetOneBit, uint8_t, false>::Do(target, targetOffset, targetSize, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_U16:
    return BlockCopy<T, targetOneBit, uint16_t, false>::Do(target, targetOffset, targetSize, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_R32:
    return BlockCopy<T, targetOneBit, float, false>::Do(target, targetOffset, targetSize, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_U32:
    return BlockCopy<T, targetOneBit, uint32_t, false>::Do(target, targetOffset, targetSize, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_R64:
    return BlockCopy<T, targetOneBit, double, false>::Do(target, targetOffset, targetSize, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_U64:
    return BlockCopy<T, targetOneBit, uint64_t, false>::Do(target, targetOffset, targetSize, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_Any:
    return BlockCopy<T, targetOneBit, uint8_t, false>::Do(target, targetOffset, targetSize, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  }
}

static void DispatchBlockCopy(VolumeDataChannelDescriptor::Format destinationFormat,
                              void       *target, const int32_t (&targetOffset)[DataStoreDimensionality_Max], const int32_t (&targetSize)[DataStoreDimensionality_Max],
                              VolumeDataChannelDescriptor::Format sourceFormat,
                              void const *source, const int32_t (&sourceOffset)[DataStoreDimensionality_Max], const int32_t (&sourceSize)[DataStoreDimensionality_Max],
                              const int32_t (&overlapSize) [DataStoreDimensionality_Max], int32_t elementSize, int32_t copyDimensions)
{
  switch(destinationFormat)
  {
  case VolumeDataChannelDescriptor::Format_1Bit:
    return DispatchBlockCopy2<uint8_t, true>(target, targetOffset, targetSize, sourceFormat, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_U8:
    return DispatchBlockCopy2<uint8_t, false>(target, targetOffset, targetSize, sourceFormat, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_U16:
    return DispatchBlockCopy2<uint16_t, false>(target, targetOffset, targetSize, sourceFormat, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_R32:
    return DispatchBlockCopy2<float, false>(target, targetOffset, targetSize, sourceFormat, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_U32:
    return DispatchBlockCopy2<uint32_t, false>(target, targetOffset, targetSize, sourceFormat, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_R64:
    return DispatchBlockCopy2<double, false>(target, targetOffset, targetSize, sourceFormat, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_U64:
    return DispatchBlockCopy2<uint64_t, false>(target, targetOffset, targetSize, sourceFormat, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  case VolumeDataChannelDescriptor::Format_Any:
    return DispatchBlockCopy2<uint8_t, false>(target, targetOffset, targetSize, sourceFormat, source, sourceOffset, sourceSize, overlapSize, elementSize, copyDimensions);
  }
}

static bool RequestSubsetProcessPage(VolumeDataPageImpl* page, const VolumeDataChunk &chunk, const int32_t (&destMin)[Dimensionality_Max], const int32_t (&destMax)[Dimensionality_Max], VolumeDataChannelDescriptor::Format destinationFormat, void *destBuffer, Error &error)
{
  int32_t sourceMin[Dimensionality_Max];
  int32_t sourceMax[Dimensionality_Max];
  int32_t sourceMinExcludingMargin[Dimensionality_Max];
  int32_t sourceMaxExcludingMargin[Dimensionality_Max];

  page->GetMinMax(sourceMin, sourceMax);
  page->GetMinMaxExcludingMargin(sourceMinExcludingMargin, sourceMaxExcludingMargin);

  int32_t lod = chunk.Layer->GetLOD();

  VolumeDataLayoutImpl *volumeDataLayout = chunk.Layer->GetLayout();

  int32_t overlapMin[Dimensionality_Max];
  int32_t overlapMax[Dimensionality_Max];

  int32_t sizeThisLod[Dimensionality_Max];

  for (int32_t dimension = 0; dimension < Dimensionality_Max; dimension++)
  {
    overlapMin[dimension] = std::max(sourceMinExcludingMargin[dimension], destMin[dimension]);
    overlapMax[dimension] = std::min(sourceMaxExcludingMargin[dimension], destMax[dimension]);
    if (volumeDataLayout->IsDimensionLODDecimated(dimension))
    {
      sizeThisLod[dimension] = GetLODSize(destMin[dimension], destMax[dimension], lod);
    }
    else
    {
      sizeThisLod[dimension] = destMax[dimension] - destMin[dimension];
    }
  }

  DimensionGroup sourceDimensionGroup = chunk.Layer->GetChunkDimensionGroup();

  VolumeDataChannelDescriptor::Format sourceFormat = chunk.Layer->GetFormat();
  bool sourceIs1Bit = (sourceFormat == VolumeDataChannelDescriptor::Format_1Bit);

  int32_t globalSourceSize[Dimensionality_Max];
  int32_t globalSourceOffset[Dimensionality_Max];
  int32_t globalTargetSize[Dimensionality_Max];
  int32_t globalTargetOffset[Dimensionality_Max];
  int32_t globalOverlapSize[Dimensionality_Max];

  for (int32_t dimension = 0; dimension < Dimensionality_Max; dimension++)
  {
    globalSourceSize[dimension] = 1;
    for (int iCopyDimension = 0; iCopyDimension < DataStoreDimensionality_Max; iCopyDimension++)
    {
      if (dimension == DimensionGroupUtil::GetDimension(sourceDimensionGroup, iCopyDimension))
      {

        globalSourceSize[dimension] = page->GetDataBlock().AllocatedSize[iCopyDimension];
        if (sourceIs1Bit && iCopyDimension == 0)
        {
          globalSourceSize[dimension] *= 8;
        }
        break;
      }
    }
    globalTargetSize[dimension] = sizeThisLod[dimension];

    if (volumeDataLayout->IsDimensionLODDecimated(dimension))
    {
      globalSourceOffset[dimension] = (overlapMin[dimension] - sourceMin[dimension]) >> lod;
      globalTargetOffset[dimension] = (overlapMin[dimension] - destMin[dimension]) >> lod;
      globalOverlapSize[dimension] = GetLODSize(overlapMin[dimension], overlapMax[dimension], lod, overlapMax[dimension] == destMax[dimension]);
    }
    else
    {
      globalSourceOffset[dimension] = (overlapMin[dimension] - sourceMin[dimension]);
      globalTargetOffset[dimension] = (overlapMin[dimension] - destMin[dimension]);
      globalOverlapSize[dimension] = (overlapMax[dimension] - overlapMin[dimension]);
    }
  }

  int32_t sourceSize[DataStoreDimensionality_Max];
  int32_t sourceOffset[DataStoreDimensionality_Max];
  int32_t targetSize[DataStoreDimensionality_Max];
  int32_t targetOffset[DataStoreDimensionality_Max];
  int32_t overlapSize[DataStoreDimensionality_Max];

  int32_t copyDimensions = CombineAndReduceDimensions(sourceSize, sourceOffset, targetSize, targetOffset, overlapSize, globalSourceSize, globalSourceOffset, globalTargetSize, globalTargetOffset, globalOverlapSize);

  int32_t sourceBytesPerVoxel = sourceIs1Bit ? 1 : GetVoxelFormatByteSize(sourceFormat);

  void *source = page->GetRawBufferInternal();

  DispatchBlockCopy(destinationFormat, destBuffer, targetOffset, targetSize,
    sourceFormat, source, sourceOffset, sourceSize,
    overlapSize, sourceBytesPerVoxel, copyDimensions);

  return true;
}

struct Box
{
  int32_t min[Dimensionality_Max];
  int32_t max[Dimensionality_Max];
};

static int64_t StaticRequestVolumeSubset(VolumeDataRequestProcessor &request_processor, void *buffer, VolumeDataLayer *volumeDataLayer, const int32_t(&minRequested)[Dimensionality_Max], const int32_t (&maxRequested)[Dimensionality_Max], int32_t lod, VolumeDataChannelDescriptor::Format format, bool isReplaceNoValue, float replacementNoValue)
{

  Box boxRequested;
  memcpy(boxRequested.min, minRequested, sizeof(boxRequested.min));
  memcpy(boxRequested.max, maxRequested, sizeof(boxRequested.max));

  // Initialized unused dimensions
  for (int32_t dimension = volumeDataLayer->GetLayout()->GetDimensionality(); dimension < Dimensionality_Max; dimension++)
  {
    boxRequested.min[dimension] = 0;
    boxRequested.max[dimension] = 1;
  }

  std::vector<VolumeDataChunk> chunksInRegion;

  volumeDataLayer->GetChunksInRegion(boxRequested.min, boxRequested.max, &chunksInRegion);

  if (chunksInRegion.size() == 0)
  {
    fprintf(stderr, "Requested volume subset does not contain any data");
    abort();
  }

  return request_processor.AddJob(chunksInRegion, [boxRequested, buffer, format](VolumeDataPageImpl* page, VolumeDataChunk dataChunk, Error &error) {return RequestSubsetProcessPage(page, dataChunk, boxRequested.min, boxRequested.max, format, buffer, error);}, format == VolumeDataChannelDescriptor::Format_1Bit);
}

static VolumeDataLayer *GetLayer(VolumeDataLayout const *layout, DimensionsND dimensionsND, int lod, int channel)
{
  if(!layout)
  {
    return nullptr;
  }

  VolumeDataLayoutImpl const *volumeDataLayout = static_cast<VolumeDataLayoutImpl const *>(layout);

  if(channel > volumeDataLayout->GetChannelCount())
  {
    return nullptr;
  }

  VolumeDataLayer *volumeDataLayer = volumeDataLayout->GetBaseLayer(DimensionGroupUtil::GetDimensionGroupFromDimensionsND(dimensionsND), channel);

  while(volumeDataLayer && volumeDataLayer->GetLOD() < lod)
  {
    volumeDataLayer = volumeDataLayer->GetParentLayer();
  }

  return (volumeDataLayer && volumeDataLayer->GetLayerType() != VolumeDataLayer::Virtual) ? volumeDataLayer : NULL;
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

  int DataIndex(const int32_t (&voxelIndex)[Dimensionality_Max]) const
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

  void VoxelIndex(const int32_t (&localChunkIndex)[Dimensionality_Max], int32_t (&voxelIndexR)[Dimensionality_Max]) const
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

  void Initialize(const VolumeDataChunk &dataChunk, const DataBlock &dataBlock)
  {
    const VolumeDataLayout *dataLayout = dataChunk.Layer->GetLayout();

    valueRangeMin = dataLayout->GetChannelDescriptor(dataChunk.Layer->GetChannelIndex()).GetValueRangeMin();
    valueRangeMax = dataLayout->GetChannelDescriptor(dataChunk.Layer->GetChannelIndex()).GetValueRangeMax();

    lod = dataChunk.Layer->GetLOD();
    dataChunk.Layer->GetChunkMinMax(dataChunk.Index, voxelMin, voxelMax, true);

    for (int dimension = 0; dimension < Dimensionality_Max; dimension++)
    {
      pitch[dimension] = 0;
      bitPitch[dimension] = 0;

      axisNumSamples[dimension] = dataLayout->GetDimensionNumSamples(dimension);
      coordinateMin[dimension] = (dimension < dataLayout->GetDimensionality()) ? dataLayout->GetDimensionMin(dimension) : 0;
      coordinateMax[dimension] = (dimension < dataLayout->GetDimensionality()) ? dataLayout->GetDimensionMax(dimension) : 0;

      localChunkSamples[dimension] = 1;
      isDimensionLODDecimated[dimension] = false;

      localChunkAllocatedSize[dimension] = 1;
    }

    for (int dimension = 0; dimension < DataStoreDimensionality_Max; dimension++)
    {
      dataBlockPitch[dimension] = dataBlock.Pitch[dimension];
      dataBlockAllocatedSize[dimension] = dataBlock.AllocatedSize[dimension];
      dataBlockSamples[dimension] = dataBlock.Size[dimension];

      for (int iDataBlockDim = 0; iDataBlockDim < DataStoreDimensionality_Max; iDataBlockDim++)
      {
        dataBlockBitPitch[iDataBlockDim] = dataBlockPitch[iDataBlockDim] * (iDataBlockDim == 0 ? 1 : 8);

        int dimension = DimensionGroupUtil::GetDimension(dataChunk.Layer->GetChunkDimensionGroup(), iDataBlockDim);
        dimensionMap[iDataBlockDim] = dimension;
        if (dimension >= 0 && dimension < Dimensionality_Max)
        {
          pitch[dimension] = dataBlockPitch[iDataBlockDim];
          bitPitch[dimension] = dataBlockBitPitch[iDataBlockDim];
          localChunkAllocatedSize[dimension] = dataBlockAllocatedSize[iDataBlockDim];

          isDimensionLODDecimated[dimension] = (dataBlockSamples[iDataBlockDim] < voxelMax[dimension] - voxelMin[dimension]);
          localChunkSamples[dimension] = dataBlockSamples[iDataBlockDim];
        }
      }
    }
  }
};

static bool VoxelIndexInProcessArea(const IndexValues &indexValues, const int32_t (&iVoxelIndex)[Dimensionality_Max])
{
  bool ret = true;

  for (int i = 0; i < Dimensionality_Max; i++)
  {
    ret = ret && (iVoxelIndex[i] < indexValues.voxelMax[i]) && (iVoxelIndex[i] >= indexValues.voxelMin[i]);
  }

  return ret;
}
  
static void VoxelIndexToLocalIndexFloat(const IndexValues &indexValues, const float (&iVoxelIndex)[Dimensionality_Max], float (&localIndex)[Dimensionality_Max] )
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
void ProjectValuesKernel(T *output, const T *input, const ProjectVars &projectVars, const IndexValues &inputIndexer, const int32_t (&voxelOutIndex)[Dimensionality_Max], VolumeSampler<T, INTERPMETHOD, isUseNoValue> &sampler, QuantizingValueConverterWithNoValue<T, typename InterpolatedRealType<T>::type, isUseNoValue> &converter, float voxelCenterOffset)
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

  if (VoxelIndexInProcessArea(inputIndexer, voxelInIndexInt))
  {
    float localInIndex[Dimensionality_Max];
    VoxelIndexToLocalIndexFloat(inputIndexer, voxelCenterInIndex, localInIndex);
    FloatVector3 localInIndex3D(localInIndex[0], localInIndex[1], localInIndex[2]);

    typedef typename InterpolatedRealType<T>::type TREAL;
    TREAL value = sampler.Sample3D(input, localInIndex3D);

    //TODO - 1Bit
    output[projectVars.DataIndex(voxelOutIndex)] = converter.ConvertValue(value);
  }
}

template <typename T, InterpolationMethod INTERPMETHOD, bool isUseNoValue>
void ProjectValuesKernelCPU(T *pxOutput, const T *pxInput, const ProjectVars &projectVars, const IndexValues &indexValues, float scale, float offset, float noValue)
{
  VolumeSampler<T, INTERPMETHOD, isUseNoValue> sampler(indexValues.dataBlockSamples, indexValues.dataBlockPitch, indexValues.valueRangeMin, indexValues.valueRangeMax, scale, offset, noValue, noValue);
  QuantizingValueConverterWithNoValue<T, typename InterpolatedRealType<T>::type, isUseNoValue> converter(indexValues.valueRangeMin, indexValues.valueRangeMax, scale, offset, noValue, noValue, false);

  int32_t numSamples[2];
  int32_t offsetPair[2];

  for (int i = 0; i < 2; i++)
  {
    int nMin = std::max(projectVars.requestedMin[projectVars.projectedDimensions[i]], indexValues.voxelMin[projectVars.projectedDimensions[i]]);
    int nMax = std::min(projectVars.requestedMax[projectVars.projectedDimensions[i]], indexValues.voxelMax[projectVars.projectedDimensions[i]]);

    numSamples[i] = GetLODSize(nMin, nMax, projectVars.lod, nMax == projectVars.requestedMax[projectVars.projectedDimensions[i]]);
    offsetPair[i] = (nMin - projectVars.requestedMin[projectVars.projectedDimensions[i]]) >> projectVars.lod;
  }

  float voxelCenterOffset = (1 << projectVars.lod) / 2.0f;

  // we can keep this to two dimensions because we know the input chunk is 3D
//#pragma omp parallel for
  for (int dimension1 = 0; dimension1 < numSamples[1]; dimension1++)
  for (int dimension0 = 0; dimension0 < numSamples[0]; dimension0++)
  {
    // this looks really strange, but since we know that the chunk dimension group for the input is always the projected and projection dimensions, this works
    int32_t localChunkIndex[Dimensionality_Max];
    for (int i = 0; i < 6; i++)
      localChunkIndex[i] = (indexValues.voxelMin[i] - projectVars.requestedMin[i]) >> projectVars.lod;
    
    localChunkIndex[projectVars.projectedDimensions[0]] = dimension0 + offsetPair[0];
    localChunkIndex[projectVars.projectedDimensions[1]] = dimension1 + offsetPair[1];
    localChunkIndex[projectVars.projectionDimension] = 0;

    int32_t voxelIndex[Dimensionality_Max];
    projectVars.VoxelIndex(localChunkIndex, voxelIndex);
    ProjectValuesKernel<T, INTERPMETHOD, isUseNoValue>(pxOutput, pxInput, projectVars, indexValues, voxelIndex, sampler, converter, voxelCenterOffset);
  }
}

template <typename T, bool isUseNoValue>
static void ProjectValuesInitCPU(T *output, const T *input, const ProjectVars &projectVars, const IndexValues &indexValues, float scale, float offset, float noValue, InterpolationMethod interpolationMethod)
{
  switch(interpolationMethod)
  {
  case InterpolationMethod::Nearest: ProjectValuesKernelCPU<T, InterpolationMethod::Nearest, isUseNoValue>(output, input, projectVars, indexValues, scale, offset, noValue); break;
  case InterpolationMethod::Linear:  ProjectValuesKernelCPU<T, InterpolationMethod::Linear,  isUseNoValue>(output, input, projectVars, indexValues, scale, offset, noValue); break;
  case InterpolationMethod::Cubic:   ProjectValuesKernelCPU<T, InterpolationMethod::Cubic,   isUseNoValue>(output, input, projectVars, indexValues, scale, offset, noValue); break;
  case InterpolationMethod::Angular: ProjectValuesKernelCPU<T, InterpolationMethod::Angular, isUseNoValue>(output, input, projectVars, indexValues, scale, offset, noValue); break;
  case InterpolationMethod::Triangular: ProjectValuesKernelCPU<T, InterpolationMethod::Triangular, isUseNoValue>(output, input, projectVars, indexValues, scale, offset, noValue); break;
  //case InterpolationMethod::TriangularExcludingValuerangeMinAndLess: ProjectValuesKernelCPU<T, InterpolationMethod::TriangularExcludingValuerangeMinAndLess, isUseNoValue>(output, input, projectVars, scale, offset, noValue); break;
  }
}

static void ProjectValuesCPU(void *output, const void *input, const ProjectVars &projectVars, const IndexValues &indexValues, VolumeDataChannelDescriptor::Format format, InterpolationMethod eInterpolationMethod, float scale, float offset, bool isUseNoValue, float noValue)
{
  if (isUseNoValue)
  {
    switch(format)
    {
    case VolumeDataChannelDescriptor::Format_U8:  ProjectValuesInitCPU<unsigned char, true>((unsigned char*)output, (const unsigned char*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_U16: ProjectValuesInitCPU<unsigned short, true>((unsigned short*)output, (const unsigned short*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_R32: ProjectValuesInitCPU<float, true>((float*)output, (const float*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_U32: ProjectValuesInitCPU<unsigned int, true>((unsigned int*)output, (const unsigned int*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_R64: ProjectValuesInitCPU<double, true>((double*)output, (const double*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_U64: ProjectValuesInitCPU<uint64_t, true>((uint64_t *)output, (const uint64_t *)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    }
  }
  else
  {
    switch(format)
    {
    case VolumeDataChannelDescriptor::Format_U8:  ProjectValuesInitCPU<unsigned char, false>((unsigned char*)output, (const unsigned char*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_U16: ProjectValuesInitCPU<unsigned short, false>((unsigned short*)output, (const unsigned short*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_R32: ProjectValuesInitCPU<float, false>((float*)output, (const float*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_U32: ProjectValuesInitCPU<unsigned int, false>((unsigned int*)output, (const unsigned int*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_R64: ProjectValuesInitCPU<double, false>((double*)output, (const double*)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    case VolumeDataChannelDescriptor::Format_U64: ProjectValuesInitCPU<uint64_t, false>((uint64_t *)output, (const uint64_t *)input, projectVars, indexValues, scale, offset, noValue, eInterpolationMethod); break;
    }
  }
}

static bool RequestProjectedVolumeSubsetProcessPage(VolumeDataPageImpl* page, const VolumeDataChunk &chunk, const int32_t (&destMin)[Dimensionality_Max], const int32_t (&destMax)[Dimensionality_Max], DimensionGroup projectedDimensionsEnum, FloatVector4 voxelPlane, InterpolationMethod interpolationMethod, bool useNoValue, float noValue, void *destBuffer, Error &error)
{
  VolumeDataChannelDescriptor::Format voxelFormat = chunk.Layer->GetFormat();

  VolumeDataLayer const *volumeDataLayer = chunk.Layer;

  DataBlock const &dataBlock = page->GetDataBlock();

  if (dataBlock.Components != VolumeDataChannelDescriptor::Components_1)
  {
    error.string = "Cannot request volume subset from multi component VDSs";
    error.code = -1;
    return false;
  }

  int32_t lod = volumeDataLayer->GetLOD();

  int32_t projectionDimension = -1;
  int32_t projectedDimensions[2] = { -1, -1 };

  if (DimensionGroupUtil::GetDimensionality(volumeDataLayer->GetChunkDimensionGroup()) < 3)
  {
    error.string = "The requested dimension group must contain at least 3 dimensions.";
    error.code = -1;
    return false;
  }

  if (DimensionGroupUtil::GetDimensionality(projectedDimensionsEnum) != 2)
  {
    error.string = "The projected dimension group must contain 2 dimensions.";
    error.code = -1;
    return false;
  }

  for (int dimensionIndex = 0; dimensionIndex < DimensionGroupUtil::GetDimensionality(volumeDataLayer->GetChunkDimensionGroup()); dimensionIndex++)
  {
    int32_t dimension = DimensionGroupUtil::GetDimension(volumeDataLayer->GetChunkDimensionGroup(), dimensionIndex);

    if (!DimensionGroupUtil::IsDimensionInGroup(projectedDimensionsEnum, dimension))
    {
      projectionDimension = dimension;
    }
  }

  assert(projectionDimension != -1);
  
  for (int32_t dimensionIndex = 0, projectionDimensionality = DimensionGroupUtil::GetDimensionality(projectedDimensionsEnum); dimensionIndex < projectionDimensionality; dimensionIndex++)
  {
    int32_t dimension = DimensionGroupUtil::GetDimension(projectedDimensionsEnum, dimensionIndex);
    projectedDimensions[dimensionIndex] = dimension;

    if (!DimensionGroupUtil::IsDimensionInGroup(volumeDataLayer->GetChunkDimensionGroup(), dimension))
    {
      error.string = "The requested dimension group must contain the dimensions of the projected dimension group.";
      error.code = -1;
      return false;
    }
  }

  int32_t sizeThisLod[Dimensionality_Max];
  for (int32_t dimension = 0; dimension < Dimensionality_Max; dimension++)
  {
    if (chunk.Layer->GetLayout()->IsDimensionLODDecimated(dimension))
    {
      sizeThisLod[dimension] = GetLODSize(destMin[dimension], destMax[dimension], lod);
    }
    else
    {
      sizeThisLod[dimension] = destMax[dimension] - destMin[dimension];
    }
  }

  ProjectVars projectVars;
  for (int dimension = 0; dimension < Dimensionality_Max; dimension++)
  {
    projectVars.requestedMin[dimension] = destMin[dimension];
    projectVars.requestedMax[dimension] = destMax[dimension];
    projectVars.requestedSizeThisLOD[dimension] = sizeThisLod[dimension];
    projectVars.requestedPitch[dimension] = dimension == 0 ? 1 : projectVars.requestedPitch[dimension - 1] * projectVars.requestedSizeThisLOD[dimension - 1];
  }

  projectVars.lod = lod;
  projectVars.voxelPlane = FloatVector4(voxelPlane.X, voxelPlane.Y, voxelPlane.Z, voxelPlane.T);
  projectVars.projectionDimension = projectionDimension;
  projectVars.projectedDimensions[0] = projectedDimensions[0];
  projectVars.projectedDimensions[1] = projectedDimensions[1];

  const void* sourceBuffer = page->GetRawBufferInternal();

  IndexValues indexValues;
  indexValues.Initialize(chunk, page->GetDataBlock());

  ProjectValuesCPU(destBuffer, sourceBuffer, projectVars, indexValues, voxelFormat, interpolationMethod, volumeDataLayer->GetIntegerScale(), volumeDataLayer->GetIntegerOffset(), useNoValue, noValue);
  return true;
}

static int64_t StaticRequestProjectedVolumeSubset(VolumeDataRequestProcessor &request_processor, void *buffer, VolumeDataLayer *volumeDataLayer, const int32_t (&minRequested)[Dimensionality_Max], const int32_t (&maxRequested)[Dimensionality_Max], FloatVector4 const &voxelPlane, DimensionGroup projectedDimensions, int32_t lod, VolumeDataChannelDescriptor::Format eFormat, InterpolationMethod interpolationMethod, bool isReplaceNoValue, float replacementNoValue)
{
  Box boxRequested;
  memcpy(boxRequested.min, minRequested, sizeof(boxRequested.min));
  memcpy(boxRequested.max, maxRequested, sizeof(boxRequested.max));

  // Initialized unused dimensions
  for (int32_t dimension = volumeDataLayer->GetLayout()->GetDimensionality(); dimension < Dimensionality_Max; dimension++)
  {
    boxRequested.min[dimension] = 0;
    boxRequested.min[dimension] = 1;
  }

  std::vector<VolumeDataChunk> chunksInRegion;

  int32_t projectionDimension = -1;
  int32_t projectionDimensionPosition;
  int32_t projectedDimensionsPair[2] = { -1, -1 };

  int32_t layerDimensionGroup = DimensionGroupUtil::GetDimensionality(volumeDataLayer->GetChunkDimensionGroup());
  if (layerDimensionGroup < 3)
  {
    fmt::print(stderr, "The requested dimension group must contain at least 3 dimensions.");
    abort();
  }

  if (DimensionGroupUtil::GetDimensionality(projectedDimensions) != 2)
  {
    fmt::print(stderr, "The projected dimension group must contain 2 dimensions.");
    abort();
  }

  for (int dimensionIndex = 0; dimensionIndex < layerDimensionGroup; dimensionIndex++)
  {
    int32_t dimension = DimensionGroupUtil::GetDimension(volumeDataLayer->GetChunkDimensionGroup(), dimensionIndex);

    if (!DimensionGroupUtil::IsDimensionInGroup(projectedDimensions, dimension))
    {
      projectionDimension = dimension;
      projectionDimensionPosition = dimensionIndex;

      // min/max in the projection dimension is not used
      boxRequested.min[dimension] = 0;
      boxRequested.max[dimension] = 1;
    }
  }

  assert(projectionDimension != -1);

  for (int dimensionIndex = 0; dimensionIndex < DimensionGroupUtil::GetDimensionality(projectedDimensions); dimensionIndex++)
  {
    int32_t dimension = DimensionGroupUtil::GetDimension(projectedDimensions, dimensionIndex);
    projectedDimensionsPair[dimensionIndex] = dimension;

    if (!DimensionGroupUtil::IsDimensionInGroup(volumeDataLayer->GetChunkDimensionGroup(), dimension))
    {
      fmt::print(stderr,"The requested dimension group must contain the dimensions of the projected dimension group.");
      abort();
    }

    if (!volumeDataLayer->IsDimensionLODDecimated(dimension) && lod > 0)
    {
      fmt::print(stderr, "Cannot project subsets with a full-resolution dimension at LOD > 0.");
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

  volumeDataLayer->GetChunksInRegion(boxRequested.min,
                                     boxRequested.max,
                                     &chunksInProjectedRegion);

  for (int iChunk = 0; iChunk < chunksInProjectedRegion.size(); iChunk++)
  {
    int32_t min[Dimensionality_Max];
    int32_t max[Dimensionality_Max];

    chunksInProjectedRegion[iChunk].Layer->GetChunkMinMax(chunksInProjectedRegion[iChunk].Index, min, max, true);

    for (int dimensionIndex = 0; dimensionIndex < 2; dimensionIndex++)
    {
      int32_t dimension = projectedDimensionsPair[dimensionIndex];

      if (min[dimension] < minRequested[dimension]) min[dimension] = minRequested[dimension];
      if (max[dimension] > maxRequested[dimension]) max[dimension] = maxRequested[dimension];
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

    volumeDataLayer->GetChunksInRegion(boxProjected.min,
                                       boxProjected.max,
                                            &chunksIntersectingPlane);

    for (int iChunkInPlane = 0; iChunkInPlane < chunksIntersectingPlane.size(); iChunkInPlane++)
    {
      VolumeDataChunk &chunkInIntersectingPlane = chunksIntersectingPlane[iChunkInPlane];
      auto chunk_it = std::find_if(chunksInRegion.begin(), chunksInRegion.end(), [&chunkInIntersectingPlane] (const VolumeDataChunk &a) { return a.Index == chunkInIntersectingPlane.Index && a.Layer == chunkInIntersectingPlane.Layer; });
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
  return request_processor.AddJob(chunksInRegion, [boxRequested, buffer, projectedDimensions, voxelPlaneSwapped, interpolationMethod, isReplaceNoValue, replacementNoValue](VolumeDataPageImpl* page, VolumeDataChunk dataChunk, Error &error) { return RequestProjectedVolumeSubsetProcessPage(page, dataChunk, boxRequested.min, boxRequested.max, projectedDimensions, voxelPlaneSwapped, interpolationMethod, isReplaceNoValue, replacementNoValue, buffer, error);}, eFormat == VolumeDataChannelDescriptor::Format_1Bit);
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
  const DataBlock &dataBlock = page->GetDataBlock();
  int64_t chunkIndex = page->GetChunkIndex();

  int32_t chunkDimension0 = volumeDataLayer->GetChunkDimension(0);
  int32_t chunkDimension1 = volumeDataLayer->GetChunkDimension(1);
  int32_t chunkDimension2 = volumeDataLayer->GetChunkDimension(2);

  assert(chunkDimension0 >= 0 && chunkDimension1 >= 0);

  VolumeDataChunk volumeDataChunk = { volumeDataLayer, chunkIndex };

  int32_t min[Dimensionality_Max];
  int32_t max[Dimensionality_Max];

  volumeDataLayer->GetChunkMinMax(chunkIndex, min, max, true);

  int32_t lod = volumeDataLayer->GetLOD();

  float lodScale = 1.0f / (1 << lod);

  int32_t fullResolutionDimension = volumeDataLayer->GetLayout()->GetFullResolutionDimension();

  VolumeSampler<T, INTERPMETHOD, isUseNoValue> volumeSampler(dataBlock.Size, dataBlock.Pitch, volumeDataLayer->GetValueRange().Min, volumeDataLayer->GetValueRange().Max,
    volumeDataLayer->GetIntegerScale(), volumeDataLayer->GetIntegerOffset(), noValue, noValue);

  const T*buffer = (const T*)(page->GetRawBufferInternal());

  for (int32_t iSamplePos = iStartSamplePos; iSamplePos < nSamplePos; iSamplePos++)
  {
    const VolumeDataSamplePos &volumeDataSamplePos = volumeSamplePositions[iSamplePos];

    if (volumeDataSamplePos.chunkIndex != chunkIndex) break;

    FloatVector3 pos((volumeDataSamplePos.pos.Data[chunkDimension0] - min[chunkDimension0]) * (chunkDimension0 == fullResolutionDimension ? 1 : lodScale),
                     (volumeDataSamplePos.pos.Data[chunkDimension1] - min[chunkDimension1]) * (chunkDimension1 == fullResolutionDimension ? 1 : lodScale),
                      0);

    if (chunkDimension2 >= 0)
    {
      pos[2] = (volumeDataSamplePos.pos.Data[chunkDimension2] - min[chunkDimension2]) * (chunkDimension2 == fullResolutionDimension ? 1 : lodScale);
    }


    typename InterpolatedRealType<T>::type value = volumeSampler.Sample3D(buffer, pos);

    static_cast<float *>(destBuffer)[volumeDataSamplePos.originalSample] = (float)value;
  }
}

template <typename T>
static void SampleVolumeInit(VolumeDataPageImpl *page, const VolumeDataLayer *volumeDataLayer, const std::vector<VolumeDataSamplePos> &volumeDataSamplePositions, InterpolationMethod interpolationMethod, int32_t iStartSamplePos, int32_t nSamplePos, float noValue, void *destBuffer)
{
  if (volumeDataLayer->IsUseNoValue())
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

static bool RequestVolumeSamplesProcessPage(VolumeDataPageImpl *page, VolumeDataChunk &dataChunk, const std::vector<VolumeDataSamplePos> &volumeDataSamplePositions, InterpolationMethod interpolationMethod, bool isUseNoValue, bool isReplaceNoValue, float replacementNoValue, void *buffer, Error &error)
{
  int32_t  samplePosCount = int32_t(volumeDataSamplePositions.size());

  int32_t iStartSamplePos = 0;
  int32_t iEndSamplePos = samplePosCount - 1;

  // Binary search to find samples within chunk
  while (iStartSamplePos < iEndSamplePos)
  {
    int32_t iSamplePos = (iStartSamplePos + iEndSamplePos) / 2;

    int64_t iSampleChunkIndex = volumeDataSamplePositions[iSamplePos].chunkIndex;

    if (iSampleChunkIndex >= dataChunk.Index)
    {
      iEndSamplePos = iSamplePos;
    }
    else
    {
      iStartSamplePos = iSamplePos + 1;
    }
  }

  assert(volumeDataSamplePositions[iStartSamplePos].chunkIndex == dataChunk.Index &&
    (iStartSamplePos == 0 || volumeDataSamplePositions[size_t(iStartSamplePos) - 1].chunkIndex < dataChunk.Index));

  VolumeDataChannelDescriptor::Format format = page->GetDataBlock().Format;

  switch (format)
  {
  case VolumeDataChannelDescriptor::Format_1Bit:
    SampleVolumeInit<bool>(page, dataChunk.Layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_U8:
    SampleVolumeInit<uint8_t>(page, dataChunk.Layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_U16:
    SampleVolumeInit<uint16_t>(page, dataChunk.Layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_U32:
    SampleVolumeInit<uint32_t>(page, dataChunk.Layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_R32:
    SampleVolumeInit<float>(page, dataChunk.Layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_U64:
    SampleVolumeInit<uint64_t>(page, dataChunk.Layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
    break;
  case VolumeDataChannelDescriptor::Format_R64:
    SampleVolumeInit<double>(page, dataChunk.Layer, volumeDataSamplePositions, interpolationMethod, iStartSamplePos, samplePosCount, replacementNoValue, buffer);
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

    std::copy(&samplePositions[samplePos][0], &samplePositions[samplePos][Dimensionality_Max], volumeDataSamplePos.pos.Data);
    volumeDataSamplePos.chunkIndex = volumeDataLayer->GetChunkIndexFromNDPos(volumeDataSamplePos.pos);
    volumeDataSamplePos.originalSample = samplePos;
  }

  std::sort(volumeDataSamplePositions->begin(), volumeDataSamplePositions->end());

  // Force NEAREST interpolation for discrete volume data
  if (volumeDataLayer->IsDiscrete())
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
      volumeDataChunks.push_back(volumeDataLayer->GetChunkFromIndex(currentChunkIndex));
    }
  }

  return request_processor.AddJob(volumeDataChunks, [buffer, volumeDataSamplePositions, interpolationMethod, isReplaceNoValue, replacementNoValue](VolumeDataPageImpl* page, VolumeDataChunk dataChunk, Error& error)
    {
      return RequestVolumeSamplesProcessPage(page, dataChunk,  *volumeDataSamplePositions, interpolationMethod, dataChunk.Layer->IsUseNoValue(), isReplaceNoValue, isReplaceNoValue ? replacementNoValue : dataChunk.Layer->GetNoValue(), buffer, error);
    });
}

template <typename T, InterpolationMethod INTERPMETHOD, bool isUseNoValue>
void TraceVolume(VolumeDataPageImpl *page, const VolumeDataChunk &chunk, const std::vector<VolumeDataSamplePos> &volumeDataSamplePositions, int32_t traceDimension, float noValue, void *targetBuffer)
{
  int32_t traceSize = chunk.Layer->GetDimensionNumSamples(traceDimension);

  float *traceBuffer = reinterpret_cast<float *>(targetBuffer);

  const DataBlock & dataBlock = page->GetDataBlock();

  const VolumeDataLayer *volumeDataLayer = chunk.Layer;

  int32_t chunkDimension0 = volumeDataLayer->GetChunkDimension(0);
  int32_t chunkDimension1 = volumeDataLayer->GetChunkDimension(1);
  int32_t chunkDimension2 = volumeDataLayer->GetChunkDimension(2);

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

  volumeDataLayer->GetChunkMinMax(chunk.Index, min, max, true);
  volumeDataLayer->GetChunkMinMax(chunk.Index, minExcludingMargin, maxExcludingMargin, false);

  int32_t lod = volumeDataLayer->GetLOD();

  float lodScale = 1.0f / (1 << lod);

  int32_t fullResolutionDimension = volumeDataLayer->GetLayout()->GetFullResolutionDimension();

  VolumeSampler<T, INTERPMETHOD, isUseNoValue> volumeSampler(dataBlock.Size, dataBlock.Pitch, volumeDataLayer->GetValueRange().Min, volumeDataLayer->GetValueRange().Max, volumeDataLayer->GetIntegerScale(), volumeDataLayer->GetIntegerOffset(), noValue, noValue);

  const T* pBuffer = (const T*) page->GetRawBufferInternal();

  int32_t traceDimensionLOD = (traceDimension != fullResolutionDimension) ? lod : 0;
  int32_t overlapCount = GetLODSize(minExcludingMargin[traceDimension], maxExcludingMargin[traceDimension], traceDimensionLOD, maxExcludingMargin[traceDimension] == traceSize);
  int32_t offsetSource = (minExcludingMargin[traceDimension] - min[traceDimension]) >> traceDimensionLOD;
  int32_t offsetTarget = (minExcludingMargin[traceDimension]) >> traceDimensionLOD;

  int32_t traceCount = int32_t(volumeDataSamplePositions.size());

  for (int32_t trace = 0; trace < traceCount; trace++)
  {
    const VolumeDataSamplePos &volumeDataSamplePos = volumeDataSamplePositions[trace];

    bool isInside = true;

    for (int dim = 0; dim < Dimensionality_Max; dim++)
    {
      if (dim != traceDimension &&
        ((int32_t)volumeDataSamplePos.pos.Data[dim] < minExcludingMargin[dim] ||
         (int32_t)volumeDataSamplePos.pos.Data[dim] >= maxExcludingMargin[dim]))
      {
        isInside = false;
        break;
      }
    }

    if (!isInside) continue;

    FloatVector3 pos((volumeDataSamplePos.pos.Data[chunkDimension0] - min[chunkDimension0]) * (chunkDimension0 == fullResolutionDimension ? 1 : lodScale),
      (volumeDataSamplePos.pos.Data[chunkDimension1] - min[chunkDimension1]) * (chunkDimension1 == fullResolutionDimension ? 1 : lodScale),
      0.5f);

    if (chunkDimension2 >= 0)
    {
      pos[2] = (volumeDataSamplePos.pos.Data[chunkDimension2] - min[chunkDimension2]) * (chunkDimension2 == fullResolutionDimension ? 1 : lodScale);
    }

    for (int overlap = 0; overlap < overlapCount; overlap++)
    {
      if (traceDimensionInChunk != -1)
      {
        pos[traceDimensionInChunk] = overlap + offsetSource + 0.5f; // so that we sample the center of the voxel
      }

      typename InterpolatedRealType<T>::type value = volumeSampler.Sample3D(pBuffer, pos);

      traceBuffer[traceSize * volumeDataSamplePos.originalSample + overlap + offsetTarget] = (float)value;
    }
  }
}

template <typename T>
static void TraceVolumeInit(VolumeDataPageImpl *page, const VolumeDataChunk &chunk, const std::vector<VolumeDataSamplePos> &volumeDataSamplePositions, InterpolationMethod interpolationMethod, int32_t traceDimension, float noValue, void *targetBuffer)
{
  if (chunk.Layer->IsUseNoValue())
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

static bool RequestVolumeTracesProcessPage (VolumeDataPageImpl *page, VolumeDataChunk &dataChunk, const std::vector<VolumeDataSamplePos> &volumeDataSamplePositions, InterpolationMethod interpolationMethod, int32_t traceDimension, float noValue, void *buffer, Error &error)
{
  VolumeDataChannelDescriptor::Format format = dataChunk.Layer->GetFormat();
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

    std::copy(&tracePositions[tracePos][0], &tracePositions[tracePos][Dimensionality_Max], volumeDataSamplePos.pos.Data);
    volumeDataSamplePos.chunkIndex = volumeDataLayer->GetChunkIndexFromNDPos(volumeDataSamplePos.pos);
    volumeDataSamplePos.originalSample = tracePos;
  }

  std::sort(volumeDataSamplePositions->begin(), volumeDataSamplePositions->end());

  // Force NEAREST interpolation for discrete volume data
  if (volumeDataLayer->IsDiscrete())
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

      VolumeDataChunk volumeDataChunk(volumeDataLayer->GetChunkFromIndex(currentChunkIndex));
      volumeDataChunks.push_back(volumeDataChunk); 
      
      for (int dim = 0; dim < Dimensionality_Max; dim++)
      {
        traceMin[dim] = (int32_t)volumeDataSamplePos.pos.Data[dim];
        traceMax[dim] = (int32_t)volumeDataSamplePos.pos.Data[dim] + 1;
      }

      traceMin[traceDimension] = 0;
      traceMax[traceDimension] = volumeDataLayer->GetDimensionNumSamples(traceDimension);

      int32_t currentChunksCount = int32_t(volumeDataChunks.size());

      volumeDataLayer->GetChunksInRegion(traceMin, traceMax, &volumeDataChunks, true);

      totalChunks += int32_t(volumeDataChunks.size()) - currentChunksCount;
    }
  }

  return request_processor.AddJob(volumeDataChunks, [buffer, volumeDataSamplePositions, interpolationMethod, traceDimension, replacementNoValue](VolumeDataPageImpl* page, VolumeDataChunk dataChunk, Error& error)
    {
      return RequestVolumeTracesProcessPage(page, dataChunk,  *volumeDataSamplePositions, interpolationMethod, traceDimension, replacementNoValue, buffer, error);
    });
}

int64_t VolumeDataAccessManagerImpl::GetVolumeSubsetBufferSize(VolumeDataLayout const *volumeDataLayout, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, int lod = 0)
{
  const int dimensionality = volumeDataLayout->GetDimensionality();

  for (int32_t dimension = 0; dimension < dimensionality; dimension++)
  {
    if(minVoxelCoordinates[dimension] < 0 || minVoxelCoordinates[dimension] >= maxVoxelCoordinates[dimension])
    {
      fmt::print(stderr, "Illegal volume subset, dimension {} min = {}, max = {}", dimension, minVoxelCoordinates[dimension], maxVoxelCoordinates[dimension]);
      abort();
    }
  }

  int64_t voxelCount = 1;

  for (int dimension = 0; dimension < dimensionality; dimension++)
  {
    if (static_cast<const VolumeDataLayoutImpl *>(volumeDataLayout)->IsDimensionLODDecimated(dimension))
    {
      voxelCount *= GetLODSize(minVoxelCoordinates[dimension], maxVoxelCoordinates[dimension], lod);
    }
    else
    {
      voxelCount *= maxVoxelCoordinates[dimension] - minVoxelCoordinates[dimension];
    }
  }

  if(format == VolumeDataChannelDescriptor::Format_1Bit)
  {
    return (voxelCount + 7) / 8;
  }
  else
  {
    return voxelCount * GetVoxelFormatByteSize(format);
  }
}

int64_t VolumeDataAccessManagerImpl::RequestVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format)
{
  return StaticRequestVolumeSubset(m_requestProcessor, buffer, GetLayer(volumeDataLayout, dimensionsND, lod, channel), minVoxelCoordinates, maxVoxelCoordinates, lod, format, false, 0.0f);
}

int64_t VolumeDataAccessManagerImpl::RequestVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], VolumeDataChannelDescriptor::Format format, float replacementNoValue)
{
  return StaticRequestVolumeSubset(m_requestProcessor, buffer, GetLayer(volumeDataLayout, dimensionsND, lod, channel), minVoxelCoordinates, maxVoxelCoordinates, lod, format, true, replacementNoValue);
}
int64_t VolumeDataAccessManagerImpl::RequestProjectedVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const& voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod)
{
  return StaticRequestProjectedVolumeSubset(m_requestProcessor, buffer, GetLayer(volumeDataLayout, dimensionsND, lod, channel), minVoxelCoordinates, maxVoxelCoordinates, voxelPlane, DimensionGroupUtil::GetDimensionGroupFromDimensionsND(projectedDimensions), lod, format, interpolationMethod, false, 0.0f);
}

int64_t VolumeDataAccessManagerImpl::GetProjectedVolumeSubsetBufferSize(VolumeDataLayout const *volumeDataLayout, const int (&minVoxelCoordinates)[Dimensionality_Max], const int (&maxVoxelCoordinates)[Dimensionality_Max], DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, int lod = 0)
{
  const int dimensionality = volumeDataLayout->GetDimensionality();

  const DimensionGroup projectedDimensionGroup = DimensionGroupUtil::GetDimensionGroupFromDimensionsND(projectedDimensions);

  for (int32_t dimension = 0; dimension < dimensionality; dimension++)
  {
    if(minVoxelCoordinates[dimension] < 0 || minVoxelCoordinates[dimension] >= maxVoxelCoordinates[dimension])
    {
      fmt::print(stderr, "Illegal volume subset, dimension {} min = {}, max = {}", dimension, minVoxelCoordinates[dimension], maxVoxelCoordinates[dimension]);
      abort();
    }
  }

  if (DimensionGroupUtil::GetDimensionality(projectedDimensionGroup) != 2)
  {
    fmt::print(stderr, "The projected dimension group must contain 2 dimensions.");
    abort();
  }

  const int projectedDimension0 = DimensionGroupUtil::GetDimension(projectedDimensionGroup, 0),
            projectedDimension1 = DimensionGroupUtil::GetDimension(projectedDimensionGroup, 1);

  int64_t voxelCount = GetLODSize(minVoxelCoordinates[projectedDimension0], maxVoxelCoordinates[projectedDimension0], lod) *
                       GetLODSize(minVoxelCoordinates[projectedDimension1], maxVoxelCoordinates[projectedDimension1], lod);

  if(format == VolumeDataChannelDescriptor::Format_1Bit)
  {
    return (voxelCount + 7) / 8;
  }
  else
  {
    return voxelCount * GetVoxelFormatByteSize(format);
  }
}

int64_t VolumeDataAccessManagerImpl::RequestProjectedVolumeSubset(void* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const int(&minVoxelCoordinates)[Dimensionality_Max], const int(&maxVoxelCoordinates)[Dimensionality_Max], FloatVector4 const& voxelPlane, DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, InterpolationMethod interpolationMethod, float replacementNoValue)
{
  return StaticRequestProjectedVolumeSubset(m_requestProcessor, buffer, GetLayer(volumeDataLayout, dimensionsND, lod, channel), minVoxelCoordinates, maxVoxelCoordinates, voxelPlane, DimensionGroupUtil::GetDimensionGroupFromDimensionsND(projectedDimensions), lod, format, interpolationMethod, true, replacementNoValue);
}

int64_t VolumeDataAccessManagerImpl::RequestVolumeSamples(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*samplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod)
{
  return StaticRequestVolumeSamples(m_requestProcessor, buffer, GetLayer(volumeDataLayout, dimensionsND, lod, channel), samplePositions, sampleCount, interpolationMethod, false, 0.0f);
}
int64_t VolumeDataAccessManagerImpl::RequestVolumeSamples(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*samplePositions)[Dimensionality_Max], int sampleCount, InterpolationMethod interpolationMethod, float replacementNoValue)
{
  return StaticRequestVolumeSamples(m_requestProcessor, buffer, GetLayer(volumeDataLayout, dimensionsND, lod, channel), samplePositions, sampleCount, interpolationMethod, true, replacementNoValue);
}

int64_t VolumeDataAccessManagerImpl::GetVolumeTracesBufferSize(VolumeDataLayout const *volumeDataLayout, int traceCount, int traceDimension, int lod)
{
  const VolumeDataChannelDescriptor::Format format = VolumeDataChannelDescriptor::Format_R32;

  int effectiveLOD = static_cast<const VolumeDataLayoutImpl *>(volumeDataLayout)->IsDimensionLODDecimated(traceDimension) ? lod : 0;

  int64_t voxelCount = (int64_t)GetLODSize(0, volumeDataLayout->GetDimensionNumSamples(traceDimension), effectiveLOD) * traceCount;

  return voxelCount * GetVoxelFormatByteSize(format);
}

int64_t VolumeDataAccessManagerImpl::RequestVolumeTraces(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int traceDimension)
{
  return StaticRequestVolumeTraces(m_requestProcessor, buffer, GetLayer(volumeDataLayout, dimensionsND, lod, channel), tracePositions, traceCount, lod, interpolationMethod, traceDimension, false, 0.0f);
}
int64_t VolumeDataAccessManagerImpl::RequestVolumeTraces(float* buffer, VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, const float(*tracePositions)[Dimensionality_Max], int traceCount, InterpolationMethod interpolationMethod, int traceDimension, float replacementNoValue)
{
  return StaticRequestVolumeTraces(m_requestProcessor, buffer, GetLayer(volumeDataLayout, dimensionsND, lod, channel), tracePositions, traceCount, lod, interpolationMethod, traceDimension, true, replacementNoValue);
}
int64_t VolumeDataAccessManagerImpl::PrefetchVolumeChunk(VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int64_t chunk)
{
  auto layer = GetLayer(volumeDataLayout, dimensionsND, lod, channel);
  std::vector<VolumeDataChunk> chunks;
  chunks.push_back(layer->GetChunkFromIndex(chunk));
  return m_requestProcessor.AddJob(chunks, [](VolumeDataPageImpl *page, VolumeDataChunk dataChunk, Error &error) {return true;});
}
}
