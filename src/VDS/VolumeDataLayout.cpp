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

#include "VolumeDataLayout.h"

#include "DimensionGroup.h"
#include "VolumeDataLayoutDescriptor.h"

#include "Bitmask.h"

namespace OpenVDS
{
  
VolumeDataLayout::VolumeDataLayout(const VolumeDataLayoutDescriptor &layoutDescriptor,
                   const std::vector<VolumeDataAxisDescriptor> &axisDescriptor,
                   const std::vector<VolumeDataChannelDescriptor> &volumeDataChannelDescriptor,
                   int32_t actualValueRangeChannel,
                   Range<float> const &actualValueRange,
                   VolumeDataHash const &volumeDataHash,
                   CompressionMethod compressionMethod,
                   float compressionTolerance,
                   bool isZipLosslessChannels,
                   int32_t waveletAdaptiveLoadLevel)
  : m_dimensionality(int32_t(axisDescriptor.size()))
  , m_baseBrickSize(int32_t(1) << layoutDescriptor.getBrickSize())
  , m_negativeRenderMargin(layoutDescriptor.getNegativeMargin())
  , m_positiveRenderMargin(layoutDescriptor.getPositiveMargin())
  , m_volumeDataChannelDescriptor(volumeDataChannelDescriptor)
  , m_actualValueRangeChannel(actualValueRangeChannel)
  , m_actualValueRange(actualValueRange)
  , m_contentsHash(volumeDataHash)
  , m_compressionMethod(compressionMethod)
  , m_compressionTolerance(compressionTolerance)
  , m_isZipLosslessChannels(isZipLosslessChannels)
  , m_waveletAdaptiveLoadLevel(waveletAdaptiveLoadLevel)
  , m_fullResolutionDimension(layoutDescriptor.getFullResolutionDimension())
{

  for(int32_t dimension = 0; dimension < array_size(m_dimensionNumSamples); dimension++)
  {
    if(dimension < m_dimensionality)
    {
      assert(axisDescriptor[dimension].getNumSamples() >= 1);
      m_dimensionNumSamples[dimension] = axisDescriptor[dimension].getNumSamples();
      m_dimensionName[dimension] = axisDescriptor[dimension].getName();
      m_dimensionUnit[dimension] = axisDescriptor[dimension].getUnit();
      m_dimensionRange[dimension] = { axisDescriptor[dimension].getCoordinateMin(), axisDescriptor[dimension].getCoordinateMax() };
    }
    else
    {
      m_dimensionNumSamples[dimension] = 1;
    }
  }

  memset(m_primaryBaseLayers, 0, sizeof(m_primaryBaseLayers));
  memset(m_primaryTopLayers, 0, sizeof(m_primaryTopLayers));
}

VolumeDataLayer::VolumeDataLayerID VolumeDataLayout::addDataLayer(VolumeDataLayer *layer)
{
  m_volumeDataLayers.push_back(layer);
  return VolumeDataLayer::VolumeDataLayerID(m_volumeDataLayers.size() - 1);
}

VolumeDataLayer* VolumeDataLayout::getBaseLayer(DimensionGroup dimensionGroup, int32_t channel) const
{
  assert(dimensionGroup >= 0 && dimensionGroup < DimensionGroup_3D_Max);
  assert(channel >= 0 && channel < getChannelCount());

  VolumeDataLayer *volumeDataLayer = m_primaryBaseLayers[dimensionGroup];

  while(channel-- && volumeDataLayer)
  {
    volumeDataLayer = volumeDataLayer->getNextChannelLayer();
  }
  return volumeDataLayer;
}

Range<float> const& VolumeDataLayout::getChannelActualValueRange(int32_t channel) const
{
  return (channel == m_actualValueRangeChannel) ? m_actualValueRange : m_volumeDataChannelDescriptor[channel].getValueRange();
}

int32_t VolumeDataLayout::getMappedValueCount(int32_t channel) const
{
  return m_volumeDataChannelDescriptor[channel].getMappedValueCount();
}

Range<float> const& VolumeDataLayout::getDimensionRange(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < m_dimensionality);
  return m_dimensionRange[dimension];
}

//const VolumeDataChannelMapping* VolumeDataLayout::GetVolumeDataChannelMapping(int32_t channel) const
//{
//  assert(channel >= 0 && channel < m_volumeDataChannelDescriptor.size());
//
//  VolumeDataChannelDescriptor *volumeDataChannelDescriptor = m_volumeDataChannelDescriptor[channel];
//
//  if(volumeDataChannelDescriptor->GetChannelMapping() == OBJ_NONE)
//  {
//    return NULL;
//  }
//  else if(pcVolumeDataChannelDescriptor->GetChannelMapping() == TraceVolumeDataChannelMapping_c::GetInstance().GetObjID())
//  {
//    return &TraceVolumeDataChannelMapping_c::GetInstance();
//  }
//  else
//  {
//    DEBUG_ERROR(("Unknown channel mapping"));
//    return NULL;
//  }
//}

VolumeDataLayer *VolumeDataLayout::getVolumeDataLayerFromID(VolumeDataLayer::VolumeDataLayerID volumeDataLayerID) const
{
  assert(volumeDataLayerID >= 0 || volumeDataLayerID == VolumeDataLayer::LayerIdNone);
  if(volumeDataLayerID == VolumeDataLayer::LayerIdNone || volumeDataLayerID >= getLayerCount())
  {
    return NULL;
  }
  else
  {
    return m_volumeDataLayers[volumeDataLayerID];
  }
}
  
VolumeDataLayer *VolumeDataLayout::getTopLayer(DimensionGroup dimensionGroup, int32_t channel) const
{
  assert(dimensionGroup >= 0 && dimensionGroup < DimensionGroup_3D_Max);
  assert(channel >= 0 && channel < getChannelCount());

  VolumeDataLayer *volumeDataLayer = m_primaryTopLayers[dimensionGroup];

  while(channel-- && volumeDataLayer)
  {
    volumeDataLayer = volumeDataLayer->getNextChannelLayer();
  }
  return volumeDataLayer;
}

bool VolumeDataLayout::isChannelAvailable(const char *channelName) const
{
  int32_t nChannels = getChannelCount();

  for(int32_t channel = 0; channel < nChannels; channel++)
  {
    if(m_volumeDataChannelDescriptor[channel].getName() == channelName) return true;
  }

  return false;
}

int32_t VolumeDataLayout::getChannelIndex(const char *channelName) const
{
  int32_t  nChannels = getChannelCount();

  for(int32_t channel = 0; channel < nChannels; channel++)
  {
    if(m_volumeDataChannelDescriptor[channel].getName() == channelName) return channel;
  }
  assert(0 && "Should not call this function unless IsChannelAvailable() is true");
  return 0;
}

VolumeDataChannelDescriptor VolumeDataLayout::getChannelDescriptor(int32_t channel) const
{
  assert(channel >= 0 && channel < getChannelCount()); 

  const VolumeDataChannelDescriptor &volumeDataChannelDescriptor = m_volumeDataChannelDescriptor[channel];

  Internal::BitMask<VolumeDataChannelDescriptor::Flags> bFlags = VolumeDataChannelDescriptor::Flags(0);

  if(volumeDataChannelDescriptor.isDiscrete())                      bFlags = bFlags | VolumeDataChannelDescriptor::DiscreteData;
  if(!volumeDataChannelDescriptor.isAllowLossyCompression())        bFlags = bFlags | VolumeDataChannelDescriptor::NoLossyCompression;
  if(volumeDataChannelDescriptor.isUseZipForLosslessCompression())  bFlags = bFlags | VolumeDataChannelDescriptor::NoLossyCompressionUseZip;
  if(!volumeDataChannelDescriptor.isRenderable())                   bFlags = bFlags | VolumeDataChannelDescriptor::NotRenderable;

  if (volumeDataChannelDescriptor.isUseNoValue())
  {
    return VolumeDataChannelDescriptor(volumeDataChannelDescriptor.getFormat(),
                                       volumeDataChannelDescriptor.getComponents(),
                                       volumeDataChannelDescriptor.getName(),
                                       volumeDataChannelDescriptor.getUnit(),
                                       volumeDataChannelDescriptor.getValueRange().min,
                                       volumeDataChannelDescriptor.getValueRange().max,
                                       getChannelMapping(channel),
                                       volumeDataChannelDescriptor.getMappedValueCount(),
                                       VolumeDataChannelDescriptor::Flags(bFlags._flags),
                                       volumeDataChannelDescriptor.getNoValue(),
                                       volumeDataChannelDescriptor.getIntegerScale(),
                                       volumeDataChannelDescriptor.getIntegerOffset());
  }

  return VolumeDataChannelDescriptor(volumeDataChannelDescriptor.getFormat(),
                                     volumeDataChannelDescriptor.getComponents(),
                                     volumeDataChannelDescriptor.getName(),
                                     volumeDataChannelDescriptor.getUnit(),
                                     volumeDataChannelDescriptor.getValueRange().min,
                                     volumeDataChannelDescriptor.getValueRange().max,
                                     getChannelMapping(channel),
                                     volumeDataChannelDescriptor.getMappedValueCount(),
                                     VolumeDataChannelDescriptor::Flags(bFlags._flags),
                                     volumeDataChannelDescriptor.getIntegerScale(),
                                     volumeDataChannelDescriptor.getIntegerOffset());
}

VolumeDataAxisDescriptor VolumeDataLayout::getAxisDescriptor(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < m_dimensionality);
  return VolumeDataAxisDescriptor(getDimensionNumSamples(dimension),
                                       getDimensionName(dimension),
                                       getDimensionUnit(dimension),
                                       getDimensionRange(dimension));
}

VolumeDataMapping VolumeDataLayout::getChannelMapping(int32_t channel) const
{
  assert(channel >= 0 && channel < getChannelCount());
  assert(0); 
//  const VolumeDataChannelDescriptor &volumeDataChannelDescriptor = m_volumeDataChannelDescriptor[channel];
//
//  if (volumeDataChannelDescriptor.GetChannelMapping() == nullptr)
//  {
//    return VOLUMEDATAMAPPING_DIRECT;
//  }
//  else if (pcVolumeDataChannelDescriptor->GetChannelMapping() == TraceVolumeDataChannelMapping_c::GetInstance().GetObjID())
//  {
//    return VOLUMEDATAMAPPING_PER_TRACE;
//  }
//  else
//  {
//    DEBUG_ERROR(("Unknown channel mapping"));
//    return ::SpaceLib::VOLUMEDATAMAPPING_DIRECT;
//  }
  return VolumeDataMapping::Direct;
}

int VolumeDataLayout::getDimensionNumSamples(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < array_size(m_dimensionNumSamples));
  return m_dimensionNumSamples[dimension];
}

const char *VolumeDataLayout::getDimensionName(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < array_size(m_dimensionName));
  return m_dimensionName[dimension];
}

const char *VolumeDataLayout::getDimensionUnit(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < array_size(m_dimensionUnit));
  return m_dimensionUnit[dimension];
}
  
void VolumeDataLayout::setContentsHash(VolumeDataHash const &contentsHash)
{
  m_contentsHash = contentsHash;
}

void VolumeDataLayout::setActualValueRange(int32_t actualValueRangeChannel, Range<float> const& actualValueRange)
{
  m_actualValueRangeChannel = actualValueRangeChannel;
  m_actualValueRange = actualValueRange;
}


void VolumeDataLayout::createRenderLayers(DimensionGroup dimensionGroup, int32_t brickSize, int32_t physicalLodLevels)
{
  assert(physicalLodLevels > 0);

  int32_t channels = getChannelCount();

  IndexArray brickSizeArray;

  static IndexArray null;

  VolumeDataLayer **lowerLod = (VolumeDataLayer **)alloca(channels * sizeof(VolumeDataLayer*));

  memset(lowerLod, 0, channels * sizeof(VolumeDataLayer*));

  bool isCreateMoreLods = true;

  for(int32_t lod = 0; isCreateMoreLods; lod++)
  {
    isCreateMoreLods = (lod < physicalLodLevels - 1); // Always create all physical lods even if we get only one cube before the top level;

    for(int32_t dimension = 0; dimension < array_size(brickSizeArray); dimension++)
    {
      brickSizeArray[dimension] = DimensionGroupUtil::isDimensionInGroup(dimensionGroup, dimension) ? (brickSize << (dimension != m_fullResolutionDimension ? lod : 0)) : 1;
    }

    VolumeDataPartition primaryPartition(lod, dimensionGroup, null, m_dimensionNumSamples, brickSizeArray, null, null, BorderMode::None, null, null, m_negativeRenderMargin, m_positiveRenderMargin, m_fullResolutionDimension);

    VolumeDataLayer *primaryChannelLayer = NULL;

    for(int32_t channel = 0; channel < channels; channel++)
    {
      if(lod > 0 && !isChannelRenderable(channel))
      {
        continue;
      }

      VolumeDataLayer::LayerType layerType = (lod < physicalLodLevels) ? VolumeDataLayer::Renderable : VolumeDataLayer::Virtual;

      if(!isChannelRenderable(channel))
      {
        assert(channel != 0);
        layerType = VolumeDataLayer::Auxiliary;
      }

      VolumeDataChannelMapping const *volumeDataChannelMapping = nullptr;// = GetVolumeDataChannelMapping(channel);

      VolumeDataLayer *volumeDataLayer = new VolumeDataLayer(VolumeDataPartition::staticMapPartition(primaryPartition, volumeDataChannelMapping, getMappedValueCount(channel)),
                                                            this, channel,primaryChannelLayer, lowerLod[channel], layerType, volumeDataChannelMapping);

      if(channel == 0)
      {
        primaryChannelLayer = volumeDataLayer;
      }

      assert(volumeDataLayer->getLod() == lod);

      lowerLod[channel] = volumeDataLayer;

      for(int32_t dimension = 0; dimension < Dimensionality_Max; dimension++)
      {
        if(volumeDataLayer->isDimensionChunked(dimension) && volumeDataLayer->getNumChunksInDimension(dimension) > 1 && dimension != m_fullResolutionDimension)
        {
          isCreateMoreLods = true;
          break;
        }
      }
    }

    m_primaryTopLayers[dimensionGroup] = primaryChannelLayer;
    if(lod == 0)
    {
      m_primaryBaseLayers[dimensionGroup] = primaryChannelLayer;
    }

    //// Default physical layers to NEVER_REMAP
    //if(lod < physicalLodLevels) _apcPrimaryTopLayers[eDimensionGroup]->SetProduceMethod(VolumeDataLayer_c::NEVER_REMAP, 0);
    if(lod < physicalLodLevels) m_primaryTopLayers[dimensionGroup]->setProduceStatus(VolumeDataLayer::ProduceStatusNormal);
  }
}

}
