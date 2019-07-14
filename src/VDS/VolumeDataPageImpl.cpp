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

#include "VolumeDataPageImpl.h"
#include "VolumeDataPageAccessorImpl.h"

namespace OpenVDS
{
//VolumeDataPageImpl::VolumeDataPageImpl(VolumeDataPageAccessor* volumeDataPageAccessor, int64_t chunk)
//  : m_volumeDataPageAccessor(volumeDataPageAccessor)
//  , m_chunk(chunk)
//  , m_blob()
//  , m_pins(1)
//  , m_buffer(nullptr)
//  , m_isDirty(false)
//  , m_chunksCopiedTo(0)
//{
//  for (int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
//  {
//    m_pitch[iDimension] = 0;
//    m_writtenMin[iDimension] = 0;
//    m_writtenMax[iDimension] = 0;
//  }
//
//  memset(m_copiedToChunkIndexes, 0, sizeof(m_copiedToChunkIndexes));
//}

VolumeDataPageImpl::VolumeDataPageImpl(VolumeDataPageAccessorImpl* volumeDataPageAccessor, int64_t chunk)
{
}
VolumeDataPageImpl::~VolumeDataPageImpl()
{
}

  // All these methods require the caller to hold a lock
bool VolumeDataPageImpl::isPinned()
{
  return true;
}
void VolumeDataPageImpl::pin()
{
}
void VolumeDataPageImpl::unPin()
{}

bool VolumeDataPageImpl::isEmpty()
{
  return true;
}
bool VolumeDataPageImpl::isDirty()
{
  return true;
}
bool VolumeDataPageImpl::isWritten()
{
  return true;
}

void VolumeDataPageImpl::makeDirty()
{}

void VolumeDataPageImpl::setBufferData(std::vector<uint8_t>&& blob, const int(&pitch)[Dimensionality_Max])
{

}
void VolumeDataPageImpl::writeBack(VolumeDataLayer* volumeDataLayer, std::unique_lock<std::mutex>& pageListMutexLock)
{
}
void* VolumeDataPageImpl::getBufferInternal(int(&anPitch)[Dimensionality_Max], bool isReadWrite)
{
  return nullptr;
}
bool VolumeDataPageImpl::isCopyMarginNeeded(VolumeDataPage* targetPage)
{
  return true;
}
void VolumeDataPageImpl::copyMargin(VolumeDataPage* targetPage)
{
}

// Implementation of Hue::HueSpaceLib::VolumeDataPage interface, these methods aquire a lock (except the GetMinMax methods which don't need to)
void  VolumeDataPageImpl::getMinMax(int(&min)[Dimensionality_Max], int(&max)[Dimensionality_Max]) const
{
}
void  VolumeDataPageImpl::getMinMaxExcludingMargin(int(&minExcludingMargin)[Dimensionality_Max], int(&maxExcludingMargin)[Dimensionality_Max]) const
{
}
const void* VolumeDataPageImpl::getBuffer(int(&pitch)[Dimensionality_Max])
{
  return nullptr;
}
void* VolumeDataPageImpl::getWritableBuffer(int(&pitch)[Dimensionality_Max])
{
  return nullptr;
}
void VolumeDataPageImpl::updateWrittenRegion(const int(&writtenMin)[Dimensionality_Max], const int(&writtenMax)[Dimensionality_Max])
{
}
void VolumeDataPageImpl::release()
{
}
}