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

#include "VolumeDataLayer.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include "VolumeDataLayout.h"
#include "VolumeDataRegion.h"

#include "Hash.h"

#include <mutex>
#include <algorithm>
#include <cstdint>
namespace OpenVDS
{

//TODO: should be moved so its not a duplicate
#define WAVELET_MIN_COMPRESSION_TOLERANCE    0.01f
#define WAVELET_ADAPTIVE_LEVELS 16

VolumeDataLayer::VolumeDataLayer(VolumeDataPartition const &volumeDataPartition, VolumeDataLayout *volumeDataLayout, int32_t channel, VolumeDataLayer *primaryChannelLayer, VolumeDataLayer *lowerLOD, VolumeDataLayer::LayerType layerType, const VolumeDataChannelMapping *volumeDataChannelMapping)
  : VolumeDataPartition(volumeDataPartition)
  , m_volumeDataLayout(volumeDataLayout)
  , m_layerID(volumeDataLayout->AddDataLayer(this))
  , m_channel(channel)
  , m_volumeDataChannelMapping(volumeDataChannelMapping)
  , m_layerType(layerType)
  , m_primaryChannelLayer(primaryChannelLayer)
  , m_nextChannelLayer(NULL)
  , m_lowerLOD(lowerLOD)
  , m_higherLOD(NULL)
  , m_remapFromLayer(this)
  , m_produceStatus(ProduceStatusUnavailable)
{
  assert(volumeDataLayout);
  assert((channel == 0 && primaryChannelLayer == NULL) || (channel > 0 && primaryChannelLayer != NULL));


  if(lowerLOD)
  {
    assert(!lowerLOD->m_higherLOD);
    assert(lowerLOD->GetLOD() + 1 == GetLOD());
    lowerLOD->m_higherLOD = this;
  }

  if(primaryChannelLayer)
  {
    assert(primaryChannelLayer->GetLOD() == GetLOD());

    VolumeDataLayer *link = primaryChannelLayer;

    while(link->m_nextChannelLayer)
    {
      link = link->m_nextChannelLayer;
    }
    link->m_nextChannelLayer = this;
  }
}

const VolumeDataChannelMapping* VolumeDataLayer::GetVolumeDataChannelMapping() const
{
  return m_volumeDataChannelMapping; //TODO Verify this!
}

int32_t VolumeDataLayer::GetMappedValueCount() const
{
  return m_volumeDataLayout->GetMappedValueCount(m_channel);
}

void VolumeDataLayer::GetChunkIndexArrayFromVoxel(const IndexArray& voxel, IndexArray& chunk) const
{
  //const VolumeDataChannelMapping *volumeDataChannelMapping = m_volumeDataLayout->GetVolumeDataChannelMapping(GetChannelIndex());

  for(int32_t iDimension = 0; iDimension < array_size(chunk); iDimension++)
  {
    //if(volumeDataChannelMapping)
    //{
    //  chunk[iDimension] = volumeDataChannelMapping->GetMappedChunkIndexFromVoxel(GetPrimaryChannelLayer(), voxel[iDimension], iDimension);
    //}
    //else
    //{
      chunk[iDimension] = VoxelToIndex(voxel[iDimension], iDimension);
    //}
  }
}

int64_t
VolumeDataLayer::GetChunkIndexFromNDPos(const NDPos &ndPos) const
{
  //const VolumeDataChannelMapping *volumeDataChannelMapping = m_volumeDataLayout->GetVolumeDataChannelMapping(GetChannelIndex());

  IndexArray chunk;

  for(int32_t iDimension = 0; iDimension < array_size(chunk); iDimension++)
  {
    //if(volumeDataChannelMapping)
    //{
    //  chunk[iDimension] = volumeDataChannelMapping->GetMappedChunkIndexFromVoxel(GetPrimaryChannelLayer(), (int32_t)floorf(ndPos.data[iDimension]), iDimension);
    //}
    //else
    //{
      chunk[iDimension] = VoxelToIndex((int32_t)floorf(ndPos.data[iDimension]), iDimension);
    //}
  }

  return IndexArrayToChunkIndex(chunk);
}

void VolumeDataLayer::GetChunksInRegion(const IndexArray& min, const IndexArray& max, std::vector<VolumeDataChunk> *volumeDataChunk, bool isAppend) const
{
  VolumeDataRegion volumeDataRegion(*this, min, max);

  volumeDataRegion.GetChunksInRegion(volumeDataChunk, isAppend);
}

void VolumeDataLayer::GetChunksOverlappingChunk(VolumeDataChunk const &cVolumeDataChunk, std::vector<VolumeDataChunk> *volumeDataChunk, bool isAppend) const
{
  static IndexArray null; // static variables are initialized to 0, no need to provide initializer

  VolumeDataRegion
    volumeDataRegion = VolumeDataRegion::VolumeDataRegionOverlappingChunk(*this, cVolumeDataChunk, null);

  volumeDataRegion.GetChunksInRegion(volumeDataChunk, isAppend);
}

const VolumeDataLayer * VolumeDataLayer::GetLayerToRemapFrom() const
{
  return m_remapFromLayer;
}

//VolumeDataLayer::ProduceMethod VolumeDataLayer::GetProduceMethod() const
//{
//  if(m_channel != 0)
//  {
//    return m_primaryChannelLayer->m_produceMethod;
//  }
//  else
//  {
//    return m_produceMethod;
//  }
//}

VolumeDataLayer::ProduceStatus VolumeDataLayer::GetProduceStatus() const
{
  return m_produceStatus;
}


const VolumeDataChannelDescriptor & VolumeDataLayer::GetVolumeDataChannelDescriptor() const
{
  return m_volumeDataLayout->GetVolumeDataChannelDescriptor(m_channel);
}

Range<float> const &VolumeDataLayer::GetValueRange() const
{
  return m_volumeDataLayout->GetChannelValueRange(m_channel);
}

Range<float> const &VolumeDataLayer::GetActualValueRange() const
{
  return m_volumeDataLayout->GetChannelActualValueRange(m_channel);
}

VolumeDataChannelDescriptor::Format VolumeDataLayer::GetFormat() const
{
  return m_volumeDataLayout->GetChannelFormat(m_channel);
}

VolumeDataChannelDescriptor::Components VolumeDataLayer::GetComponents() const
{
  return m_volumeDataLayout->GetChannelComponents(m_channel);
}

bool VolumeDataLayer::IsDiscrete() const
{
  return m_volumeDataLayout->IsChannelDiscrete(m_channel);
}

uint64_t VolumeDataLayer::GetFormatHash(VolumeDataChannelDescriptor::Format actualFormat, bool isReplaceNoValue, float replacementNoValue) const
{
  VolumeDataChannelDescriptor const & volumeDataChannelDescriptor = m_volumeDataLayout->GetVolumeDataChannelDescriptor(m_channel);

  if(actualFormat == VolumeDataChannelDescriptor::FormatAny)
  {
    actualFormat = volumeDataChannelDescriptor.GetFormat();
  }

  HashCombiner hashCombiner(actualFormat);

  hashCombiner.Add(volumeDataChannelDescriptor.GetComponents());

  bool isConvertWithValueRange = (actualFormat == VolumeDataChannelDescriptor::FormatU8 || actualFormat == VolumeDataChannelDescriptor::FormatU16) &&
                                 (volumeDataChannelDescriptor.GetFormat() != VolumeDataChannelDescriptor::FormatU8 && volumeDataChannelDescriptor.GetFormat() != VolumeDataChannelDescriptor::FormatU16);

  if(isConvertWithValueRange)
  {
    hashCombiner.Add(volumeDataChannelDescriptor.GetValueRange());
  }

  if(volumeDataChannelDescriptor.GetFormat() == VolumeDataChannelDescriptor::FormatU8 || volumeDataChannelDescriptor.GetFormat() == VolumeDataChannelDescriptor::FormatU16)
  {
    hashCombiner.Add(volumeDataChannelDescriptor.GetIntegerScale());
    hashCombiner.Add(volumeDataChannelDescriptor.GetIntegerOffset());
  }

  hashCombiner.Add(volumeDataChannelDescriptor.IsUseNoValue());

  if(volumeDataChannelDescriptor.IsUseNoValue() && isReplaceNoValue)
  {
    hashCombiner.Add(replacementNoValue);
  }

  return hashCombiner.GetCombinedHash();
}

bool VolumeDataLayer::IsUseNoValue() const
{
  return m_volumeDataLayout->IsChannelUseNoValue(m_channel);
}

float VolumeDataLayer::GetNoValue() const
{
  return m_volumeDataLayout->GetChannelNoValue(m_channel);
}

static bool CompressionMethod_IsWavelet(CompressionMethod compressionMethod)
{
  return compressionMethod == CompressionMethod::Wavelet ||
         compressionMethod == CompressionMethod::WaveletNormalizeBlock ||
         compressionMethod == CompressionMethod::WaveletLossless ||
         compressionMethod == CompressionMethod::WaveletNormalizeBlockLossless;
}

CompressionMethod VolumeDataLayer::GetEffectiveCompressionMethod() const
{
  auto &channelDescriptor = m_volumeDataLayout->GetVolumeDataChannelDescriptor(m_channel);

  if(!channelDescriptor.IsAllowLossyCompression() && CompressionMethod_IsWavelet(m_volumeDataLayout->GetCompressionMethod()))
  {
    if(m_volumeDataLayout->isZipLosslessChannels() || channelDescriptor.IsUseZipForLosslessCompression())
    {
      return CompressionMethod::Zip;
    }
    else
    {
      return CompressionMethod::Rle;
    }
  }
 
  auto overallCompressionMethod = m_volumeDataLayout->GetCompressionMethod();
  if(GetLOD() > 0)
  {
    if(overallCompressionMethod  == CompressionMethod::WaveletLossless)
    {
      return CompressionMethod::Wavelet;
    }
    if(overallCompressionMethod == CompressionMethod::WaveletNormalizeBlockLossless)
    {
      return CompressionMethod::WaveletNormalizeBlock;
    }
  }
  return overallCompressionMethod;
}

float VolumeDataLayer::GetEffectiveCompressionTolerance() const
{ 
  auto &channelDescriptor = m_volumeDataLayout->GetVolumeDataChannelDescriptor(m_channel);
  //return m_volumeDataLayout->GetVolumeDataChannelDescriptor(m_channel).GetEffectiveCompressionTolerance(m_volumeDataLayout->m_compressionTolerance, GetLOD());
  if(!channelDescriptor.IsAllowLossyCompression())
  {
    return 0.0f;
  }

  float effectiveCompressionTolerance = m_volumeDataLayout->GetCompressionTolerance();

  if(GetLOD() > 0)
  {
    effectiveCompressionTolerance = std::max(effectiveCompressionTolerance, 2.0f);// This means that LOD 1 is never smaller than tolerance 4.0 and LOD 2 8.0 ...... 0.5f);
    effectiveCompressionTolerance *= 1 << (std::min(GetLOD(), 2));
  }

  return effectiveCompressionTolerance;
}

int32_t VolumeDataLayer::GetEffectiveWaveletAdaptiveLoadLevel(float rEffectiveCompressionTolerance, float rCompressionTolerance)
{
  assert(rEffectiveCompressionTolerance >= WAVELET_MIN_COMPRESSION_TOLERANCE);
  assert(rCompressionTolerance >= WAVELET_MIN_COMPRESSION_TOLERANCE);

  int32_t waveletAdaptiveLoadLevel = (int32_t)(log(rEffectiveCompressionTolerance / rCompressionTolerance) / M_LN2);

  return std::max(0, waveletAdaptiveLoadLevel);
}

int32_t VolumeDataLayer::GetEffectiveWaveletAdaptiveLoadLevel() const
{
  if(!CompressionMethod_IsWavelet(GetEffectiveCompressionMethod())) return -1;

  // If we have lower LODs, this layer is not the base layer
  if(m_lowerLOD)
  {
    assert(GetBaseLayer().GetEffectiveCompressionTolerance() == m_volumeDataLayout->GetCompressionTolerance());

    float effectiveCompressionTolerance = GetEffectiveCompressionTolerance();

    assert(effectiveCompressionTolerance >= m_volumeDataLayout->GetCompressionTolerance());

    int iLODAdaptiveDifference = GetEffectiveWaveletAdaptiveLoadLevel(effectiveCompressionTolerance, m_volumeDataLayout->GetCompressionTolerance());

    int iAdaptiveLODLevel = (m_volumeDataLayout->GetWaveletAdaptiveLoadLevel() - iLODAdaptiveDifference);

    // make sure LOD adaptiveness stops at same as max level of LOD0
    iAdaptiveLODLevel = std::min(WAVELET_ADAPTIVE_LEVELS - 1 - iLODAdaptiveDifference, iAdaptiveLODLevel);

    // for now clamp adaptive LOD level to 4, looks so crap otherwise
    iAdaptiveLODLevel = std::min(4, iAdaptiveLODLevel);

    return std::max(0, iAdaptiveLODLevel);
  }

  return m_volumeDataLayout->GetWaveletAdaptiveLoadLevel();
}

float VolumeDataLayer::GetIntegerScale() const
{
  return m_volumeDataLayout->GetChannelIntegerScale(m_channel);
}

float VolumeDataLayer::GetIntegerOffset() const
{
  return m_volumeDataLayout->GetChannelIntegerOffset(m_channel);
}

FloatVec2 VolumeDataLayer::GetQuantizingScaleOffset(VolumeDataChannelDescriptor::Format format) const
{
  VolumeDataChannelDescriptor::Format layerFormat = GetFormat();

  FloatVec2 scaleAndOffset = {GetIntegerScale(), GetIntegerOffset()};

  if (format == VolumeDataChannelDescriptor::FormatU8 || format == VolumeDataChannelDescriptor::FormatU16)
  {
    if ((layerFormat != VolumeDataChannelDescriptor::FormatU8 && layerFormat != VolumeDataChannelDescriptor::FormatU16) ||
        (layerFormat == VolumeDataChannelDescriptor::FormatU16 && format == VolumeDataChannelDescriptor::FormatU8))
    {
      if (format == VolumeDataChannelDescriptor::FormatU8)
      {
        scaleAndOffset[0] = rangeSize(GetValueRange()) / (IsUseNoValue() ? 254.0f : 255.0f);
      }
      else
      {
        scaleAndOffset[0] = rangeSize(GetValueRange()) / (IsUseNoValue() ? 65534.0f : 65535.0f);
      }
      scaleAndOffset[1] = GetValueRange().min;
    }
  }

  return scaleAndOffset;
}

FloatVec2 VolumeDataLayer::GetTextureScaleOffset(VolumeDataChannelDescriptor::Format eDataBlockFormat) const
{
  return StaticGetTextureScaleOffset(GetValueRange(), GetIntegerScale(), GetIntegerOffset(), IsUseNoValue(), GetFormat(), eDataBlockFormat);
}

FloatVec2 VolumeDataLayer::StaticGetTextureScaleOffset(const Range<float> &valueRange, float integerScale, float integerOffset, bool isUseNoValue, VolumeDataChannelDescriptor::Format originalFormat, VolumeDataChannelDescriptor::Format dataBlockFormat)
{
  FloatVec2 scaleAndOffset=  {1.0f, 0.0f};

  if (dataBlockFormat == VolumeDataChannelDescriptor::FormatU8 || dataBlockFormat == VolumeDataChannelDescriptor::FormatU16)
  {
    if (originalFormat == VolumeDataChannelDescriptor::FormatU8)
    {
      scaleAndOffset[0] =  integerScale * 255.0f;
      scaleAndOffset[1] = integerOffset;
    }
    else if (originalFormat == VolumeDataChannelDescriptor::FormatU16 && dataBlockFormat != VolumeDataChannelDescriptor::FormatU8)
    {
      scaleAndOffset[0] = integerScale * 65535.0f;
      scaleAndOffset[1] = integerOffset;
    }
    else
    {
      scaleAndOffset[0] = rangeSize(valueRange);
      scaleAndOffset[1] = valueRange.min;

      if (isUseNoValue)
      {
        if (dataBlockFormat == VolumeDataChannelDescriptor::FormatU8)
        {
          scaleAndOffset[0] = scaleAndOffset[0] * 255.0f / 254.0f;
        }
        else if (dataBlockFormat == VolumeDataChannelDescriptor::FormatU16)
        {
          scaleAndOffset[0] = scaleAndOffset[0] * 65535.0f / 65534.0f;
        }
      }
    }
  }

  return scaleAndOffset;
}

void VolumeDataLayer::SetProduceStatus(ProduceStatus produceStatus)
{
  m_produceStatus = produceStatus;
}

}
