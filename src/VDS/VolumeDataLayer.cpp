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

#include "VolumeDataLayoutImpl.h"
#include "VolumeDataRegion.h"
#include "VolumeDataChannelMapping.h"

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

VolumeDataLayer::VolumeDataLayer(VolumeDataPartition const &volumeDataPartition, VolumeDataLayoutImpl *volumeDataLayout, int32_t channel, VolumeDataLayer *primaryChannelLayer, VolumeDataLayer *lowerLOD, VolumeDataLayer::LayerType layerType, const VolumeDataChannelMapping *volumeDataChannelMapping)
  : VolumeDataPartition(volumeDataPartition)
  , m_volumeDataLayout(volumeDataLayout)
  , m_layerID(volumeDataLayout->addDataLayer(this))
  , m_channel(channel)
  , m_volumeDataChannelMapping(volumeDataChannelMapping)
  , m_layerType(layerType)
  , m_primaryChannelLayer(primaryChannelLayer)
  , m_nextChannelLayer(nullptr)
  , m_lowerLOD(lowerLOD)
  , m_higherLOD(nullptr)
  , m_remapFromLayer(this)
  , m_produceStatus(ProduceStatus_Unavailable)
{
  assert(volumeDataLayout);
  assert((channel == 0 && primaryChannelLayer == nullptr) || (channel > 0 && primaryChannelLayer != nullptr));
  assert(volumeDataChannelMapping == volumeDataLayout->getVolumeDataChannelMapping(channel));

  if(lowerLOD)
  {
    assert(!lowerLOD->m_higherLOD);
    assert(lowerLOD->getLOD() + 1 == getLOD());
    lowerLOD->m_higherLOD = this;
  }

  if(primaryChannelLayer)
  {
    assert(primaryChannelLayer->getLOD() == getLOD());

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
  return m_volumeDataChannelMapping;
}

int32_t VolumeDataLayer::getMappedValueCount() const
{
  return m_volumeDataLayout->getChannelMappedValueCount(m_channel);
}

void VolumeDataLayer::getChunkIndexArrayFromVoxel(const IndexArray& voxel, IndexArray& chunk) const
{
  for(int32_t iDimension = 0; iDimension < array_size(chunk); iDimension++)
  {
    if(m_volumeDataChannelMapping)
    {
      chunk[iDimension] = m_volumeDataChannelMapping->getMappedChunkIndexFromVoxel(getPrimaryChannelLayer(), voxel[iDimension], iDimension);
    }
    else
    {
      chunk[iDimension] = voxelToIndex(voxel[iDimension], iDimension);
    }
  }
}

int64_t
VolumeDataLayer::getChunkIndexFromNDPos(const NDPos &ndPos) const
{
  IndexArray chunk;

  for(int32_t iDimension = 0; iDimension < array_size(chunk); iDimension++)
  {
    if(m_volumeDataChannelMapping)
    {
      chunk[iDimension] = m_volumeDataChannelMapping->getMappedChunkIndexFromVoxel(getPrimaryChannelLayer(), (int32_t)floorf(ndPos.data[iDimension]), iDimension);
    }
    else
    {
      chunk[iDimension] = voxelToIndex((int32_t)floorf(ndPos.data[iDimension]), iDimension);
    }
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
  return m_volumeDataLayout->GetChannelFormat(m_channel);
}

VolumeDataChannelDescriptor::Components VolumeDataLayer::getComponents() const
{
  return m_volumeDataLayout->GetChannelComponents(m_channel);
}

bool VolumeDataLayer::isDiscrete() const
{
  return m_volumeDataLayout->IsChannelDiscrete(m_channel);
}

uint64_t VolumeDataLayer::getFormatHash(VolumeDataChannelDescriptor::Format actualFormat, bool isReplaceNoValue, float replacementNoValue) const
{
  VolumeDataChannelDescriptor const & volumeDataChannelDescriptor = m_volumeDataLayout->getVolumeDataChannelDescriptor(m_channel);

  if(actualFormat == VolumeDataChannelDescriptor::Format_Any)
  {
    actualFormat = volumeDataChannelDescriptor.GetFormat();
  }

  HashCombiner hashCombiner(actualFormat);

  hashCombiner.add(volumeDataChannelDescriptor.GetComponents());

  bool isConvertWithValueRange = (actualFormat == VolumeDataChannelDescriptor::Format_U8 || actualFormat == VolumeDataChannelDescriptor::Format_U16) &&
                                 (volumeDataChannelDescriptor.GetFormat() != VolumeDataChannelDescriptor::Format_U8 && volumeDataChannelDescriptor.GetFormat() != VolumeDataChannelDescriptor::Format_U16);

  if(isConvertWithValueRange)
  {
    hashCombiner.add(volumeDataChannelDescriptor.GetValueRange());
  }

  if(volumeDataChannelDescriptor.GetFormat() == VolumeDataChannelDescriptor::Format_U8 || volumeDataChannelDescriptor.GetFormat() == VolumeDataChannelDescriptor::Format_U16)
  {
    hashCombiner.add(volumeDataChannelDescriptor.GetIntegerScale());
    hashCombiner.add(volumeDataChannelDescriptor.GetIntegerOffset());
  }

  hashCombiner.add(volumeDataChannelDescriptor.IsUseNoValue());

  if(volumeDataChannelDescriptor.IsUseNoValue() && isReplaceNoValue)
  {
    hashCombiner.add(replacementNoValue);
  }

  return hashCombiner.getCombinedHash();
}

bool VolumeDataLayer::isUseNoValue() const
{
  return m_volumeDataLayout->IsChannelUseNoValue(m_channel);
}

float VolumeDataLayer::getNoValue() const
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

CompressionMethod VolumeDataLayer::getEffectiveCompressionMethod() const
{
  auto &channelDescriptor = m_volumeDataLayout->getVolumeDataChannelDescriptor(m_channel);

  if(!channelDescriptor.IsAllowLossyCompression() && CompressionMethod_IsWavelet(m_volumeDataLayout->getCompressionMethod()))
  {
    if(m_volumeDataLayout->isZipLosslessChannels() || channelDescriptor.IsUseZipForLosslessCompression())
    {
      return CompressionMethod::Zip;
    }
    else
    {
      return CompressionMethod::RLE;
    }
  }
 
  auto overallCompressionMethod = m_volumeDataLayout->getCompressionMethod();
  if(getLOD() > 0)
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
  //return m_volumeDataLayout->getVolumeDataChannelDescriptor(m_channel).getEffectiveCompressionTolerance(m_volumeDataLayout->m_compressionTolerance, getLOD());
  if(!channelDescriptor.IsAllowLossyCompression())
  {
    return 0.0f;
  }

  float effectiveCompressionTolerance = m_volumeDataLayout->getCompressionTolerance();

  if(getLOD() > 0)
  {
    effectiveCompressionTolerance = std::max(effectiveCompressionTolerance, 2.0f);// This means that lod 1 is never smaller than tolerance 4.0 and lod 2 8.0 ...... 0.5f);
    effectiveCompressionTolerance *= 1 << (std::min(getLOD(), 2));
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
  if(m_lowerLOD)
  {
    assert(getBaseLayer().getEffectiveCompressionTolerance() == m_volumeDataLayout->getCompressionTolerance());

    float effectiveCompressionTolerance = getEffectiveCompressionTolerance();

    assert(effectiveCompressionTolerance >= m_volumeDataLayout->getCompressionTolerance());

    int lodAdaptiveDifference = getEffectiveWaveletAdaptiveLoadLevel(effectiveCompressionTolerance, m_volumeDataLayout->getCompressionTolerance());

    int adaptiveLODLevel = (m_volumeDataLayout->getWaveletAdaptiveLoadLevel() - lodAdaptiveDifference);

    // make sure lod adaptiveness stops at same as max level of lod0
    adaptiveLODLevel = std::min(WAVELET_ADAPTIVE_LEVELS - 1 - lodAdaptiveDifference, adaptiveLODLevel);

    // for now clamp adaptive lod level to 4, looks so crap otherwise
    adaptiveLODLevel = std::min(4, adaptiveLODLevel);

    return std::max(0, adaptiveLODLevel);
  }

  return m_volumeDataLayout->getWaveletAdaptiveLoadLevel();
}

float VolumeDataLayer::getIntegerScale() const
{
  return m_volumeDataLayout->GetChannelIntegerScale(m_channel);
}

float VolumeDataLayer::getIntegerOffset() const
{
  return m_volumeDataLayout->GetChannelIntegerOffset(m_channel);
}

void VolumeDataLayer::setProduceStatus(ProduceStatus produceStatus)
{
  m_produceStatus = produceStatus;
}

}
