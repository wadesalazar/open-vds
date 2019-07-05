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

#define _USE_MATH_DEFINES
#include "VolumeDataLayer.h"

#include <cmath>

#include "VolumeDataLayout.h"
#include "VolumeDataRegion.h"

#include <OpenVDS/VolumeDataAccess.h>

#include "Hash.h"

#include <mutex>
#include <algorithm>
#include <cstdint>
namespace OpenVDS
{

//TODO: should be moved so its not a duplicate
#define WAVELET_MIN_COMPRESSION_TOLERANCE    0.01f
#define WAVELET_ADAPTIVE_LEVELS 16

VolumeDataLayer::VolumeDataLayer(VolumeDataPartition const &volumeDataPartition, VolumeDataLayout *volumeDataLayout, int32_t channel, VolumeDataLayer *primaryChannelLayer, VolumeDataLayer *lowerLod, VolumeDataLayer::LayerType layerType, const VolumeDataChannelMapping *volumeDataChannelMapping)
  : VolumeDataPartition(volumeDataPartition)
  , m_volumeDataLayout(volumeDataLayout)
  , m_layerID(volumeDataLayout->addDataLayer(this))
  , m_channel(channel)
  , m_volumeDataChannelMapping(volumeDataChannelMapping)
  , m_layerType(layerType)
  , m_primaryChannelLayer(primaryChannelLayer)
  , m_nextChannelLayer(NULL)
  , m_lowerLod(lowerLod)
  , m_higherLod(NULL)
  , m_remapFromLayer(this)
  , m_produceStatus(ProduceStatusUnavailable)
{
  assert(volumeDataLayout);
  assert((channel == 0 && primaryChannelLayer == NULL) || (channel > 0 && primaryChannelLayer != NULL));


  if(lowerLod)
  {
    assert(!lowerLod->m_higherLod);
    assert(lowerLod->getLod() + 1 == getLod());
    lowerLod->m_higherLod = this;
  }

  if(primaryChannelLayer)
  {
    assert(primaryChannelLayer->getLod() == getLod());

    VolumeDataLayer *link = primaryChannelLayer;

    while(link->m_nextChannelLayer)
    {
      link = link->m_nextChannelLayer;
    }
    link->m_nextChannelLayer = this;
  }
}

const VolumeDataChannelMapping* VolumeDataLayer::getVolumeDataChannelMapping() const
{
  return m_volumeDataChannelMapping; //TODO Verify this!
}

int32_t VolumeDataLayer::getMappedValueCount() const
{
  return m_volumeDataLayout->getMappedValueCount(m_channel);
}

void VolumeDataLayer::getChunkIndexArrayFromVoxel(const IndexArray& voxel, IndexArray& chunk) const
{
  //const VolumeDataChannelMapping *volumeDataChannelMapping = m_volumeDataLayout->getVolumeDataChannelMapping(getChannelIndex());

  for(int32_t iDimension = 0; iDimension < array_size(chunk); iDimension++)
  {
    //if(volumeDataChannelMapping)
    //{
    //  chunk[iDimension] = volumeDataChannelMapping->getMappedChunkIndexFromVoxel(getPrimaryChannelLayer(), voxel[iDimension], iDimension);
    //}
    //else
    //{
      chunk[iDimension] = voxelToIndex(voxel[iDimension], iDimension);
    //}
  }
}

int64_t
VolumeDataLayer::getChunkIndexFromNDPos(const NDPos &ndPos) const
{
  //const VolumeDataChannelMapping *volumeDataChannelMapping = m_volumeDataLayout->getVolumeDataChannelMapping(getChannelIndex());

  IndexArray chunk;

  for(int32_t iDimension = 0; iDimension < array_size(chunk); iDimension++)
  {
    //if(volumeDataChannelMapping)
    //{
    //  chunk[iDimension] = volumeDataChannelMapping->getMappedChunkIndexFromVoxel(getPrimaryChannelLayer(), (int32_t)floorf(ndPos.data[iDimension]), iDimension);
    //}
    //else
    //{
      chunk[iDimension] = voxelToIndex((int32_t)floorf(ndPos.data[iDimension]), iDimension);
    //}
  }

  return indexArrayToChunkIndex(chunk);
}

void VolumeDataLayer::getChunksInRegion(const IndexArray& min, const IndexArray& max, std::vector<VolumeDataChunk> *volumeDataChunk, bool isAppend) const
{
  VolumeDataRegion volumeDataRegion(*this, min, max);

  volumeDataRegion.getChunksInRegion(volumeDataChunk, isAppend);
}

void VolumeDataLayer::getChunksOverlappingChunk(VolumeDataChunk const &cVolumeDataChunk, std::vector<VolumeDataChunk> *volumeDataChunk, bool isAppend) const
{
  static IndexArray null; // static variables are initialized to 0, no need to provide initializer

  VolumeDataRegion
    volumeDataRegion = VolumeDataRegion::VolumeDataRegionOverlappingChunk(*this, cVolumeDataChunk, null);

  volumeDataRegion.getChunksInRegion(volumeDataChunk, isAppend);
}

const VolumeDataLayer * VolumeDataLayer::getLayerToRemapFrom() const
{
  return m_remapFromLayer;
}

//VolumeDataLayer::ProduceMethod VolumeDataLayer::getProduceMethod() const
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

VolumeDataLayer::ProduceStatus VolumeDataLayer::getProduceStatus() const
{
  return m_produceStatus;
}


const VolumeDataChannelDescriptor & VolumeDataLayer::getVolumeDataChannelDescriptor() const
{
  return m_volumeDataLayout->getVolumeDataChannelDescriptor(m_channel);
}

FloatRange const &VolumeDataLayer::getValueRange() const
{
  return m_volumeDataLayout->getChannelValueRange(m_channel);
}

FloatRange const &VolumeDataLayer::getActualValueRange() const
{
  return m_volumeDataLayout->getChannelActualValueRange(m_channel);
}

VolumeDataChannelDescriptor::Format VolumeDataLayer::getFormat() const
{
  return m_volumeDataLayout->getChannelFormat(m_channel);
}

VolumeDataChannelDescriptor::Components VolumeDataLayer::getComponents() const
{
  return m_volumeDataLayout->getChannelComponents(m_channel);
}

bool VolumeDataLayer::isDiscrete() const
{
  return m_volumeDataLayout->isChannelDiscrete(m_channel);
}

uint64_t VolumeDataLayer::getFormatHash(VolumeDataChannelDescriptor::Format actualFormat, bool isReplaceNoValue, float replacementNoValue) const
{
  VolumeDataChannelDescriptor const & volumeDataChannelDescriptor = m_volumeDataLayout->getVolumeDataChannelDescriptor(m_channel);

  if(actualFormat == VolumeDataChannelDescriptor::FormatAny)
  {
    actualFormat = volumeDataChannelDescriptor.getFormat();
  }

  HashCombiner hashCombiner(actualFormat);

  hashCombiner.add(volumeDataChannelDescriptor.getComponents());

  bool isConvertWithValueRange = (actualFormat == VolumeDataChannelDescriptor::FormatU8 || actualFormat == VolumeDataChannelDescriptor::FormatU16) &&
                                 (volumeDataChannelDescriptor.getFormat() != VolumeDataChannelDescriptor::FormatU8 && volumeDataChannelDescriptor.getFormat() != VolumeDataChannelDescriptor::FormatU16);

  if(isConvertWithValueRange)
  {
    hashCombiner.add(volumeDataChannelDescriptor.getValueRange());
  }

  if(volumeDataChannelDescriptor.getFormat() == VolumeDataChannelDescriptor::FormatU8 || volumeDataChannelDescriptor.getFormat() == VolumeDataChannelDescriptor::FormatU16)
  {
    hashCombiner.add(volumeDataChannelDescriptor.getIntegerScale());
    hashCombiner.add(volumeDataChannelDescriptor.getIntegerOffset());
  }

  hashCombiner.add(volumeDataChannelDescriptor.isUseNoValue());

  if(volumeDataChannelDescriptor.isUseNoValue() && isReplaceNoValue)
  {
    hashCombiner.add(replacementNoValue);
  }

  return hashCombiner.getCombinedHash();
}

bool VolumeDataLayer::isUseNoValue() const
{
  return m_volumeDataLayout->isChannelUseNoValue(m_channel);
}

float VolumeDataLayer::getNoValue() const
{
  return m_volumeDataLayout->getChannelNoValue(m_channel);
}

static bool CompressionMethod_IsWavelet(CompressionMethod compressionMethod)
{
  return compressionMethod == CompressionMethod::Wavelet ||
         compressionMethod == CompressionMethod::WaveletNormalizeBlock ||
         compressionMethod == CompressionMethod::WaveletLossless ||
         compressionMethod == CompressionMethod::WaveletNormalizeBlockLossless;
}

CompressionMethod VolumeDataLayer::getEffectiveCompressionMethod() const
{
  auto &channelDescriptor = m_volumeDataLayout->getVolumeDataChannelDescriptor(m_channel);

  if(!channelDescriptor.isAllowLossyCompression() && CompressionMethod_IsWavelet(m_volumeDataLayout->getCompressionMethod()))
  {
    if(m_volumeDataLayout->isZipLosslessChannels() || channelDescriptor.isUseZipForLosslessCompression())
    {
      return CompressionMethod::Zip;
    }
    else
    {
      return CompressionMethod::Rle;
    }
  }
 
  auto overallCompressionMethod = m_volumeDataLayout->getCompressionMethod();
  if(getLod() > 0)
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

float VolumeDataLayer::getEffectiveCompressionTolerance() const
{ 
  auto &channelDescriptor = m_volumeDataLayout->getVolumeDataChannelDescriptor(m_channel);
  //return m_volumeDataLayout->getVolumeDataChannelDescriptor(m_channel).getEffectiveCompressionTolerance(m_volumeDataLayout->m_compressionTolerance, getLod());
  if(!channelDescriptor.isAllowLossyCompression())
  {
    return 0.0f;
  }

  float effectiveCompressionTolerance = m_volumeDataLayout->getCompressionTolerance();

  if(getLod() > 0)
  {
    effectiveCompressionTolerance = std::max(effectiveCompressionTolerance, 2.0f);// This means that lod 1 is never smaller than tolerance 4.0 and lod 2 8.0 ...... 0.5f);
    effectiveCompressionTolerance *= 1 << (std::min(getLod(), 2));
  }

  return effectiveCompressionTolerance;
}

int32_t VolumeDataLayer::getEffectiveWaveletAdaptiveLoadLevel(float rEffectiveCompressionTolerance, float rCompressionTolerance)
{
  assert(rEffectiveCompressionTolerance >= WAVELET_MIN_COMPRESSION_TOLERANCE);
  assert(rCompressionTolerance >= WAVELET_MIN_COMPRESSION_TOLERANCE);

  int32_t waveletAdaptiveLoadLevel = (int32_t)(log(rEffectiveCompressionTolerance / rCompressionTolerance) / M_LN2);

  return std::max(0, waveletAdaptiveLoadLevel);
}

int32_t VolumeDataLayer::getEffectiveWaveletAdaptiveLoadLevel() const
{
  if(!CompressionMethod_IsWavelet(getEffectiveCompressionMethod())) return -1;

  // If we have lower lods, this layer is not the base layer
  if(m_lowerLod)
  {
    assert(getBaseLayer().getEffectiveCompressionTolerance() == m_volumeDataLayout->getCompressionTolerance());

    float effectiveCompressionTolerance = getEffectiveCompressionTolerance();

    assert(effectiveCompressionTolerance >= m_volumeDataLayout->getCompressionTolerance());

    int lodAdaptiveDifference = getEffectiveWaveletAdaptiveLoadLevel(effectiveCompressionTolerance, m_volumeDataLayout->getCompressionTolerance());

    int adaptiveLodLevel = (m_volumeDataLayout->getWaveletAdaptiveLoadLevel() - lodAdaptiveDifference);

    // make sure lod adaptiveness stops at same as max level of lod0
    adaptiveLodLevel = std::min(WAVELET_ADAPTIVE_LEVELS - 1 - lodAdaptiveDifference, adaptiveLodLevel);

    // for now clamp adaptive lod level to 4, looks so crap otherwise
    adaptiveLodLevel = std::min(4, adaptiveLodLevel);

    return std::max(0, adaptiveLodLevel);
  }

  return m_volumeDataLayout->getWaveletAdaptiveLoadLevel();
}

float VolumeDataLayer::getIntegerScale() const
{
  return m_volumeDataLayout->getChannelIntegerScale(m_channel);
}

float VolumeDataLayer::getIntegerOffset() const
{
  return m_volumeDataLayout->getChannelIntegerOffset(m_channel);
}

FloatVec2 VolumeDataLayer::getQuantizingScaleOffset(VolumeDataChannelDescriptor::Format format) const
{
  VolumeDataChannelDescriptor::Format layerFormat = getFormat();

  FloatVec2 scaleAndOffset = {getIntegerScale(), getIntegerOffset()};

  if (format == VolumeDataChannelDescriptor::FormatU8 || format == VolumeDataChannelDescriptor::FormatU16)
  {
    if ((layerFormat != VolumeDataChannelDescriptor::FormatU8 && layerFormat != VolumeDataChannelDescriptor::FormatU16) ||
        (layerFormat == VolumeDataChannelDescriptor::FormatU16 && format == VolumeDataChannelDescriptor::FormatU8))
    {
      if (format == VolumeDataChannelDescriptor::FormatU8)
      {
        scaleAndOffset[0] = rangeSize(getValueRange()) / (isUseNoValue() ? 254.0f : 255.0f);
      }
      else
      {
        scaleAndOffset[0] = rangeSize(getValueRange()) / (isUseNoValue() ? 65534.0f : 65535.0f);
      }
      scaleAndOffset[1] = getValueRange().min;
    }
  }

  return scaleAndOffset;
}

FloatVec2 VolumeDataLayer::getTextureScaleOffset(VolumeDataChannelDescriptor::Format eDataBlockFormat) const
{
  return staticGetTextureScaleOffset(getValueRange(), getIntegerScale(), getIntegerOffset(), isUseNoValue(), getFormat(), eDataBlockFormat);
}

FloatVec2 VolumeDataLayer::staticGetTextureScaleOffset(const FloatRange &valueRange, float integerScale, float integerOffset, bool isUseNoValue, VolumeDataChannelDescriptor::Format originalFormat, VolumeDataChannelDescriptor::Format dataBlockFormat)
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

void VolumeDataLayer::setProduceStatus(ProduceStatus produceStatus)
{
  m_produceStatus = produceStatus;
}

}
