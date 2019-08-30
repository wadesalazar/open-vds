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
#include "VolumeDataChunk.h"
#include "VolumeDataLayer.h"
#include <OpenVDS/VolumeDataChannelDescriptor.h>

#include <algorithm>

namespace OpenVDS
{

template <typename T>
inline T dataBlock_ReadElement(const T *ptBuffer, size_t iElement) { return ptBuffer[iElement]; }
template <>
inline bool dataBlock_ReadElement(const bool *ptBuffer, size_t iElement) { return (reinterpret_cast<const unsigned char *>(ptBuffer)[iElement / 8] & (1 << (iElement % 8))) != 0; }

template <typename T>
inline void dataBlock_WriteElement(T *ptBuffer, size_t iElement, T tValue) { ptBuffer[iElement] = tValue; }
template <>
inline void dataBlock_WriteElement(bool *ptBuffer, size_t iElement, bool tValue) { if(tValue) { reinterpret_cast<unsigned char *>(ptBuffer)[iElement / 8] |= (1 << (iElement % 8)); } else { reinterpret_cast<unsigned char *>(ptBuffer)[iElement / 8] &= ~(1 << (iElement % 8)); } }

template <bool isTargetDim0Contigous, bool isSourceDim0Contigous, typename T>
static void dataBlock_BlockCopyWithExplicitContiguity(T * __restrict ptTarget, const T * __restrict ptSource, int32_t iTargetIndex, int32_t iSourceIndex, int32_t (&targetPitch)[4], int32_t (&sourcePitch)[4], int32_t (&size)[4])
{
  assert(!isTargetDim0Contigous || targetPitch[0] == 1);
  assert(!isSourceDim0Contigous || sourcePitch[0] == 1);

  int32_t
    iSourceIndex3 = iSourceIndex,
    iTargetIndex3 = iTargetIndex;

  for(int32_t iDim3 = 0; iDim3 < size[3]; iDim3++, iSourceIndex3 += sourcePitch[3], iTargetIndex3 += targetPitch[3])
  {
    int32_t
      iSourceIndex2 = iSourceIndex3,
      iTargetIndex2 = iTargetIndex3;

    for(int32_t iDim2 = 0; iDim2 < size[2]; iDim2++, iSourceIndex2 += sourcePitch[2], iTargetIndex2 += targetPitch[2])
    {
      int32_t
        iSourceIndex1 = iSourceIndex2,
        iTargetIndex1 = iTargetIndex2;

      for(int32_t iDim1 = 0; iDim1 < size[1]; iDim1++, iSourceIndex1 += sourcePitch[1], iTargetIndex1 += targetPitch[1])
      {
        int32_t
          iSourceIndex0 = iSourceIndex1,
          iTargetIndex0 = iTargetIndex1;

        if(isTargetDim0Contigous && isSourceDim0Contigous)
        {
          for(int32_t iDim0 = 0; iDim0 < size[0]; iDim0++, iSourceIndex0++, iTargetIndex0++)
          {
            dataBlock_WriteElement(ptTarget, iTargetIndex0, dataBlock_ReadElement(ptSource, iSourceIndex0));
          }
        }
        else if(isTargetDim0Contigous)
        {
          for(int32_t iDim0 = 0; iDim0 < size[0]; iDim0++, iSourceIndex0 += sourcePitch[0], iTargetIndex0++)
          {
            dataBlock_WriteElement(ptTarget, iTargetIndex0, dataBlock_ReadElement(ptSource, iSourceIndex0));
          }
        }
        else if(isSourceDim0Contigous)
        {
          for(int32_t iDim0 = 0; iDim0 < size[0]; iDim0++, iSourceIndex0++, iTargetIndex0 += targetPitch[0])
          {
            dataBlock_WriteElement(ptTarget, iTargetIndex0, dataBlock_ReadElement(ptSource, iSourceIndex0));
          }
        }
        else
        {
          for(int32_t iDim0 = 0; iDim0 < size[0]; iDim0++, iSourceIndex0 += sourcePitch[0], iTargetIndex0 += targetPitch[0])
          {
            dataBlock_WriteElement(ptTarget, iTargetIndex0, dataBlock_ReadElement(ptSource, iSourceIndex0));
          }
        }
      }
    }
  }
}

VolumeDataPageImpl::VolumeDataPageImpl(VolumeDataPageAccessorImpl* volumeDataPageAccessor, int64_t chunk)
  : m_volumeDataPageAccessor(volumeDataPageAccessor)
  , m_chunk(chunk)
  , m_blob()
  , m_pins(1)
  , m_buffer(nullptr)
  , m_isDirty(false)
  , m_chunksCopiedTo(0)
{
  for (int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    m_pitch[iDimension] = 0;
    m_writtenMin[iDimension] = 0;
    m_writtenMax[iDimension] = 0;
  }

  memset(m_copiedToChunkIndexes, 0, sizeof(m_copiedToChunkIndexes));
}

VolumeDataPageImpl::~VolumeDataPageImpl()
{
  if(m_pins > 0)
  {
    fprintf(stderr, "OpenVDS: VolumeDataPage was not released before VolumeDataPageAccessor was destructed");
  }

  m_buffer = NULL;
}

  // All these methods require the caller to hold a lock
bool VolumeDataPageImpl::isPinned()
{
  //assert(m_volumeDataPageAccessor->m_pageListMutex.isLockedByCurrentThread());
  assert(m_pins >= 0);
  return m_pins > 0;
}
void VolumeDataPageImpl::pin()
{
  //assert(m_volumeDataPageAccessor->m_pageListMutex.isLockedByCurrentThread());
  assert(m_pins >= 0);
  m_pins++;
}
void VolumeDataPageImpl::unPin()
{
  //assert(m_volumeDataPageAccessor->m_pageListMutex.isLockedByCurrentThread());
  m_pins--;
  assert(m_pins >= 0);
}

bool VolumeDataPageImpl::isEmpty()
{
  //assert(m_volumeDataPageAccessor->m_pageListMutex.isLockedByCurrentThread());
  return m_blob.empty();
}

bool VolumeDataPageImpl::isDirty()
{
  //assert(m_volumeDataPageAccessor->m_pageListMutex.isLockedByCurrentThread());
  return m_isDirty;
}

bool VolumeDataPageImpl::isWritten()
{
  //assert(m_volumeDataPageAccessor->m_pageListMutex.isLockedByCurrentThread());
  return !(m_writtenMin[0] == 0 && m_writtenMax[0] == 0);
}

void VolumeDataPageImpl::makeDirty()
{
  //assert(m_volumeDataPageAccessor->m_pageListMutex.isLockedByCurrentThread());
  m_isDirty = true;
}

void VolumeDataPageImpl::setBufferData(std::vector<uint8_t>&& blob, const int(&pitch)[Dimensionality_Max])
{
  //assert(m_volumeDataPageAccessor->m_pageListMutex.isLockedByCurrentThread());

  m_blob = blob;

  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    m_pitch[iDimension] = pitch[iDimension];
  }
}

void VolumeDataPageImpl::writeBack(VolumeDataLayer* volumeDataLayer, std::unique_lock<std::mutex>& pageListMutexLock)
{
  assert(false);
}


void* VolumeDataPageImpl::getBufferInternal(int(&anPitch)[Dimensionality_Max], bool isReadWrite)
{
  //assert(m_volumeDataPageAccessor->m_pageListMutex.isLockedByCurrentThread());

  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    anPitch[iDimension] = m_pitch[iDimension];
  }

  if(!m_buffer)
  {
    m_buffer = m_blob.data();
  }

  return m_buffer;
}

bool VolumeDataPageImpl::isCopyMarginNeeded(VolumeDataPageImpl* targetPage)
{
  if(targetPage == this) return false;

  int32_t targetMin[Dimensionality_Max];
  int32_t targetMax[Dimensionality_Max];

  targetPage->getMinMax(targetMin, targetMax);

  int32_t overlapMin[Dimensionality_Max];
  int32_t overlapMax[Dimensionality_Max];
  int32_t overlapSize[Dimensionality_Max];

  // Check if there is any overlap between the written region and the target page
  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    overlapMin[iDimension] = m_writtenMin[iDimension] >= targetMin[iDimension] ? m_writtenMin[iDimension] : targetMin[iDimension];
    overlapMax[iDimension] = m_writtenMax[iDimension] <= targetMax[iDimension] ? m_writtenMax[iDimension] : targetMax[iDimension];
    overlapSize[iDimension] = overlapMax[iDimension] - overlapMin[iDimension];
    if(overlapSize[iDimension] <= 0)
    {
      return false;
    }
  }

  // Check if the margins have already been copied
  for(int32_t copiedToChunk = 0; copiedToChunk < m_chunksCopiedTo; copiedToChunk++)
  {
    if(targetPage->getChunkIndex() == m_copiedToChunkIndexes[copiedToChunk])
    {
      return false;
    }
  }

  return true;
}

void VolumeDataPageImpl::copyMargin(VolumeDataPageImpl* targetPage)
{
  //assert(m_volumeDataPageAccessor->m_pageListMutex.isLockedByCurrentThread());
  assert(isDirty());
  assert(!targetPage->isEmpty() && "The caller have to ensure the target page is finished reading");

  int32_t sourceMin[Dimensionality_Max];
  int32_t sourceMax[Dimensionality_Max];

  getMinMax(sourceMin, sourceMax);

  int32_t targetMin[Dimensionality_Max];
  int32_t targetMax[Dimensionality_Max];

  targetPage->getMinMax(targetMin, targetMax);

  int32_t overlapMin[Dimensionality_Max];
  int32_t overlapMax[Dimensionality_Max];
  int32_t overlapSize[Dimensionality_Max];

  bool isEmpty = false;

  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    overlapMin[iDimension] = m_writtenMin[iDimension] >= targetMin[iDimension] ? m_writtenMin[iDimension] : targetMin[iDimension];
    overlapMax[iDimension] = m_writtenMax[iDimension] <= targetMax[iDimension] ? m_writtenMax[iDimension] : targetMax[iDimension];
    overlapSize[iDimension] = overlapMax[iDimension] - overlapMin[iDimension];
    if(overlapSize[iDimension] <= 0)
    {
      isEmpty = true;
    }
  }

  assert(!isEmpty);

  int32_t sourcePitch[Dimensionality_Max];
  int32_t targetPitch[Dimensionality_Max];

  const void * sourceBuffer = getBufferInternal(sourcePitch, false);

  void *targetBuffer = static_cast<VolumeDataPageImpl*>(targetPage)->getBufferInternal(targetPitch, true);

  int32_t iSourceIndex = 0;
  int32_t iTargetIndex = 0;

  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    iSourceIndex += (overlapMin[iDimension] - sourceMin[iDimension]) * sourcePitch[iDimension];
    iTargetIndex += (overlapMin[iDimension] - targetMin[iDimension]) * targetPitch[iDimension];
  }

  int32_t remappedSourcePitch[4];
  int32_t remappedTargetPitch[4];
  int32_t remappedOverlapSize[4];

  for(int32_t iChunkDimension = 0; iChunkDimension < 4; iChunkDimension++)
  {
    int32_t iDimension = m_volumeDataPageAccessor->getLayer()->getChunkDimension(iChunkDimension);

    if(iDimension == -1)
    {
      remappedSourcePitch[iChunkDimension] = 0;
      remappedTargetPitch[iChunkDimension] = 0;
      remappedOverlapSize[iChunkDimension] = 1;
    }
    else
    {
      remappedSourcePitch[iChunkDimension] = sourcePitch[iDimension];
      remappedTargetPitch[iChunkDimension] = targetPitch[iDimension];
      remappedOverlapSize[iChunkDimension] = overlapSize[iDimension];
    }
  }

  switch(m_volumeDataPageAccessor->getChannelDescriptor().getFormat())
  {
  default:
    fprintf(stderr,"Unsupported format");
    break;
  case VolumeDataChannelDescriptor::Format_1Bit:
    dataBlock_BlockCopyWithExplicitContiguity<true, true>((bool *)targetBuffer, (bool *)sourceBuffer, iTargetIndex, iSourceIndex, remappedTargetPitch, remappedSourcePitch, remappedOverlapSize);
    break;

  case VolumeDataChannelDescriptor::Format_U8:
    dataBlock_BlockCopyWithExplicitContiguity<true, true>((uint8_t *)targetBuffer, (uint8_t *)sourceBuffer, iTargetIndex, iSourceIndex, remappedTargetPitch, remappedSourcePitch, remappedOverlapSize);
    break;

  case VolumeDataChannelDescriptor::Format_U16:
    dataBlock_BlockCopyWithExplicitContiguity<true, true>((uint16_t *)targetBuffer, (uint16_t *)sourceBuffer, iTargetIndex, iSourceIndex, remappedTargetPitch, remappedSourcePitch, remappedOverlapSize);
    break;

  case VolumeDataChannelDescriptor::Format_R32:
  case VolumeDataChannelDescriptor::Format_U32:
    dataBlock_BlockCopyWithExplicitContiguity<true, true>((uint32_t *)targetBuffer, (uint32_t *)sourceBuffer, iTargetIndex, iSourceIndex, remappedTargetPitch, remappedSourcePitch, remappedOverlapSize);
    break;

  case VolumeDataChannelDescriptor::Format_R64:
  case VolumeDataChannelDescriptor::Format_U64:
    dataBlock_BlockCopyWithExplicitContiguity<true, true>((uint64_t *)targetBuffer, (uint64_t *)sourceBuffer, iTargetIndex, iSourceIndex, remappedTargetPitch, remappedSourcePitch, remappedOverlapSize);
    break;
  }

  targetPage->makeDirty();

  assert(m_chunksCopiedTo < array_size(m_copiedToChunkIndexes));
  m_copiedToChunkIndexes[m_chunksCopiedTo++] = targetPage->getChunkIndex();
}

// Implementation of Hue::HueSpaceLib::VolumeDataPage interface, these methods aquire a lock (except the GetMinMax methods which don't need to)
void  VolumeDataPageImpl::getMinMax(int(&min)[Dimensionality_Max], int(&max)[Dimensionality_Max]) const
{
  m_volumeDataPageAccessor->getLayer()->getChunkMinMax(m_chunk, min, max, true);
}
void  VolumeDataPageImpl::getMinMaxExcludingMargin(int(&minExcludingMargin)[Dimensionality_Max], int(&maxExcludingMargin)[Dimensionality_Max]) const
{
  m_volumeDataPageAccessor->getLayer()->getChunkMinMax(m_chunk, minExcludingMargin, maxExcludingMargin, false);
}
const void* VolumeDataPageImpl::getBuffer(int(&pitch)[Dimensionality_Max])
{
 std::unique_lock<std::mutex>  pageListMutexLock(const_cast<VolumeDataPageAccessorImpl *>(m_volumeDataPageAccessor)->m_pagesMutex);

  return getBufferInternal(pitch, m_volumeDataPageAccessor->isReadWrite());
}
void* VolumeDataPageImpl::getWritableBuffer(int(&pitch)[Dimensionality_Max])
{
 std::unique_lock<std::mutex>  pageListMutexLock(const_cast<VolumeDataPageAccessorImpl *>(m_volumeDataPageAccessor)->m_pagesMutex);

  if(!m_volumeDataPageAccessor->isReadWrite())
  {
    fprintf(stderr, "Trying to get a writable buffer from a read-only volume data page accessor");
  }

  return getBufferInternal(pitch, true);
}
void VolumeDataPageImpl::updateWrittenRegion(const int(&writtenMin)[Dimensionality_Max], const int(&writtenMax)[Dimensionality_Max])
{
 std::unique_lock<std::mutex>  pageListMutexLock(const_cast<VolumeDataPageAccessorImpl *>(m_volumeDataPageAccessor)->m_pagesMutex);

  //if(!m_blob.getIsLocked())
  //{
    //fprintf(stderr, "Cannot update the written area when of a volume data page which isn't writable"));
  //}

  // Expand written area
  if(m_writtenMin[0] == 0 && m_writtenMax[0] == 0)
  {
    for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
    {
      m_writtenMin[iDimension] = writtenMin[iDimension];
      m_writtenMax[iDimension] = writtenMax[iDimension];
    }
  }
  else
  {
    for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
    {
      m_writtenMin[iDimension] = std::min(m_writtenMin[iDimension], writtenMin[iDimension]);
      m_writtenMax[iDimension] = std::max(m_writtenMax[iDimension], writtenMax[iDimension]);
    }
  }

  // Clear the copied to chunk list
  memset(m_copiedToChunkIndexes, 0, sizeof(m_copiedToChunkIndexes));
  m_chunksCopiedTo = 0;

  makeDirty();
}
void VolumeDataPageImpl::release()
{
 std::unique_lock<std::mutex>  pageListMutexLock(const_cast<VolumeDataPageAccessorImpl *>(m_volumeDataPageAccessor)->m_pagesMutex);
 unPin();
}

}