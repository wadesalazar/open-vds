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

#include "Bitmask.h"

namespace OpenVDS
{
  
VolumeDataLayout::VolumeDataLayout(int32_t dimensionality,
                                  const IndexArray &size,
                                  int32_t baseBrickSize,
                                  int32_t negativeRenderMargin,
                                  int32_t positiveRenderMargin, 
                                  std::vector<VolumeDataChannelDescriptor> const &volumeDataChannelDescriptor, 
                                  int32_t actualValueRangeChannel, 
                                  Range<float> const &actualValueRange, 
                                  VolumeDataHash const &volumeDataHash, 
                                  CompressionMethod compressionMethod, 
                                  float compressionTolerance, 
                                  bool isZipLosslessChannels, 
                                  int32_t waveletAdaptiveLoadLevel, 
                                  int32_t fullResolutionDimension)
  : m_dimensionality(dimensionality)
  , m_baseBrickSize(baseBrickSize)
  , m_negativeRenderMargin(negativeRenderMargin)
  , m_positiveRenderMargin(positiveRenderMargin)
  , m_volumeDataChannelDescriptor(volumeDataChannelDescriptor)
  , m_actualValueRangeChannel(actualValueRangeChannel)
  , m_actualValueRange(actualValueRange)
  , m_contentsHash(volumeDataHash)
  , m_compressionMethod(compressionMethod)
  , m_compressionTolerance(compressionTolerance)
  , m_isZipLosslessChannels(isZipLosslessChannels)
  , m_waveletAdaptiveLoadLevel(waveletAdaptiveLoadLevel)
  , m_fullResolutionDimension(fullResolutionDimension)
{

  for(int32_t iDimension = 0; iDimension < array_size(m_dimensionNumSamples); iDimension++)
  {
    if(iDimension < dimensionality)
    {
      assert(size[iDimension] >= 1);
      m_dimensionNumSamples[iDimension] = size[iDimension];
    }
    else
    {
      m_dimensionNumSamples[iDimension] = 1;
    }
  }

  memset(m_primaryBaseLayers, 0, sizeof(m_primaryBaseLayers));
  memset(m_primaryTopLayers, 0, sizeof(m_primaryTopLayers));
}

VolumeDataLayer::VolumeDataLayerID VolumeDataLayout::AddDataLayer(VolumeDataLayer *layer)
{
  m_volumeDataLayers.push_back(layer);
  return VolumeDataLayer::VolumeDataLayerID(m_volumeDataLayers.size() - 1);
}

VolumeDataLayer* VolumeDataLayout::GetBaseLayer(DimensionGroup dimensionGroup, int32_t channel) const
{
  assert(dimensionGroup >= 0 && dimensionGroup < DimensionGroup_3D_Max);
  assert(channel >= 0 && channel < GetChannelCount());

  VolumeDataLayer *volumeDataLayer = m_primaryBaseLayers[dimensionGroup];

  while(channel-- && volumeDataLayer)
  {
    volumeDataLayer = volumeDataLayer->GetNextChannelLayer();
  }
  return volumeDataLayer;
}

Range<float> const& VolumeDataLayout::GetChannelActualValueRange(int32_t iChannel) const
{
  return (iChannel == m_actualValueRangeChannel) ? m_actualValueRange : m_volumeDataChannelDescriptor[iChannel].GetValueRange();
}

int32_t VolumeDataLayout::GetMappedValueCount(int32_t iChannel) const
{
  return m_volumeDataChannelDescriptor[iChannel].GetMappedValueCount();
}

Range<float> const& VolumeDataLayout::GetDimensionRange(int32_t iDimension) const
{
  assert(iDimension >= 0 && iDimension < m_dimensionality);
  return m_dimensionRange[iDimension];
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

VolumeDataLayer *VolumeDataLayout::GetVolumeDataLayerFromID(VolumeDataLayer::VolumeDataLayerID volumeDataLayerID) const
{
  assert(volumeDataLayerID >= 0 || volumeDataLayerID == VolumeDataLayer::LayerIdNone);
  if(volumeDataLayerID == VolumeDataLayer::LayerIdNone || volumeDataLayerID >= GetLayerCount())
  {
    return NULL;
  }
  else
  {
    return m_volumeDataLayers[volumeDataLayerID];
  }
}
  
VolumeDataLayer *VolumeDataLayout::GetTopLayer(DimensionGroup dimensionGroup, int32_t iChannel) const
{
  assert(dimensionGroup >= 0 && dimensionGroup < DimensionGroup_3D_Max);
  assert(iChannel >= 0 && iChannel < GetChannelCount());

  VolumeDataLayer *volumeDataLayer = m_primaryTopLayers[dimensionGroup];

  while(iChannel-- && volumeDataLayer)
  {
    volumeDataLayer = volumeDataLayer->GetNextChannelLayer();
  }
  return volumeDataLayer;
}

bool VolumeDataLayout::IsChannelAvailable(const std::string &channelName) const
{
  int32_t nChannels = GetChannelCount();

  for(int32_t iChannel = 0; iChannel < nChannels; iChannel++)
  {
    if(m_volumeDataChannelDescriptor[iChannel].GetName() == channelName) return true;
  }

  return false;
}

int32_t VolumeDataLayout::GetChannelIndex(const std::string& channelName) const
{
  int32_t  nChannels = GetChannelCount();

  for(int32_t iChannel = 0; iChannel < nChannels; iChannel++)
  {
    if(m_volumeDataChannelDescriptor[iChannel].GetName() == channelName) return iChannel;
  }
  assert(0 && "Should not call this function unless IsChannelAvailable() is true");
  return 0;
}

VolumeDataChannelDescriptor VolumeDataLayout::GetChannelDescriptor(int32_t iChannel) const
{
  assert(iChannel >= 0 && iChannel < GetChannelCount()); 

  const VolumeDataChannelDescriptor &volumeDataChannelDescriptor = m_volumeDataChannelDescriptor[iChannel];

  Internal::bit_mask<VolumeDataChannelDescriptor::Flags> bFlags = VolumeDataChannelDescriptor::Flags(0);

  if(volumeDataChannelDescriptor.IsDiscrete())                      bFlags = bFlags | VolumeDataChannelDescriptor::DiscreteData;
  if(!volumeDataChannelDescriptor.IsAllowLossyCompression())        bFlags = bFlags | VolumeDataChannelDescriptor::NoLossyCompression;
  if(volumeDataChannelDescriptor.IsUseZipForLosslessCompression())  bFlags = bFlags | VolumeDataChannelDescriptor::NoLossyCompressionUseZip;
  if(!volumeDataChannelDescriptor.IsRenderable())                   bFlags = bFlags | VolumeDataChannelDescriptor::NotRenderable;

  if (volumeDataChannelDescriptor.IsUseNoValue())
  {
    return VolumeDataChannelDescriptor(volumeDataChannelDescriptor.GetFormat(),
                                       volumeDataChannelDescriptor.GetComponents(),
                                       volumeDataChannelDescriptor.GetName(),
                                       volumeDataChannelDescriptor.GetUnit(),
                                       volumeDataChannelDescriptor.GetValueRange().min,
                                       volumeDataChannelDescriptor.GetValueRange().max,
                                       GetChannelMapping(iChannel),
                                       volumeDataChannelDescriptor.GetMappedValueCount(),
                                       VolumeDataChannelDescriptor::Flags(bFlags._flags),
                                       volumeDataChannelDescriptor.GetNoValue(),
                                       volumeDataChannelDescriptor.GetIntegerScale(),
                                       volumeDataChannelDescriptor.GetIntegerOffset());
  }

  return VolumeDataChannelDescriptor(volumeDataChannelDescriptor.GetFormat(),
                                     volumeDataChannelDescriptor.GetComponents(),
                                     volumeDataChannelDescriptor.GetName(),
                                     volumeDataChannelDescriptor.GetUnit(),
                                     volumeDataChannelDescriptor.GetValueRange().min,
                                     volumeDataChannelDescriptor.GetValueRange().max,
                                     GetChannelMapping(iChannel),
                                     volumeDataChannelDescriptor.GetMappedValueCount(),
                                     VolumeDataChannelDescriptor::Flags(bFlags._flags),
                                     volumeDataChannelDescriptor.GetIntegerScale(),
                                     volumeDataChannelDescriptor.GetIntegerOffset());
}

VolumeDataAxisDescriptor VolumeDataLayout::GetAxisDescriptor(int32_t iDimension) const
{
  assert(iDimension >= 0 && iDimension < m_dimensionality);
  return VolumeDataAxisDescriptor(GetDimensionNumSamples(iDimension),
                                       GetDimensionName(iDimension),
                                       GetDimensionUnit(iDimension),
                                       GetDimensionRange(iDimension).min,
                                       GetDimensionRange(iDimension).max);
}

VolumeDataMapping VolumeDataLayout::GetChannelMapping(int32_t iChannel) const
{
  assert(iChannel >= 0 && iChannel < GetChannelCount());
  assert(0); 
//  const VolumeDataChannelDescriptor &volumeDataChannelDescriptor = m_volumeDataChannelDescriptor[iChannel];
//
//  if (volumeDataChannelDescriptor.GetChannelMapping() == nullptr)
//  {
//    return VOLUMEDATAMAPPING_DIRECT;
//  }
//  else if (pcVolumeDataChannelDescriptor->GetChannelMapping() == TraceVolumeDataChannelMapping_c::GetInstance().GetObjID())
//  {
//    return ::SpaceLib::VOLUMEDATAMAPPING_PER_TRACE;
//  }
//  else
//  {
//    DEBUG_ERROR(("Unknown channel mapping"));
//    return ::SpaceLib::VOLUMEDATAMAPPING_DIRECT;
//  }
  return VolumeDataMapping::Direct;
}

int VolumeDataLayout::GetDimensionNumSamples(int32_t iDimension) const
{
  assert(iDimension >= 0 && iDimension < array_size(m_dimensionNumSamples));
  return m_dimensionNumSamples[iDimension];
}

const std::string& VolumeDataLayout::GetDimensionName(int32_t iDimension) const
{
  assert(iDimension >= 0 && iDimension < array_size(m_dimensionName));
  return m_dimensionName[iDimension];
}

const std::string& VolumeDataLayout::GetDimensionUnit(int32_t iDimension) const
{
  assert(iDimension >= 0 && iDimension < array_size(m_dimensionUnit));
  return m_dimensionUnit[iDimension];
}
  
void VolumeDataLayout::SetContentsHash(VolumeDataHash const &contentsHash)
{
  m_contentsHash = contentsHash;
}

void VolumeDataLayout::SetActualValueRange(int32_t actualValueRangeChannel, Range<float> const& actualValueRange)
{
  m_actualValueRangeChannel = actualValueRangeChannel;
  m_actualValueRange = actualValueRange;
}


void VolumeDataLayout::CreateRenderLayers(DimensionGroup dimensionGroup, int32_t nBrickSize, int32_t nPhysicalLODLevels)
{
  assert(nPhysicalLODLevels > 0);

  int32_t nChannels = GetChannelCount();

  IndexArray brickSize;

  static IndexArray null;

  VolumeDataLayer **lowerLOD = (VolumeDataLayer **)alloca(nChannels * sizeof(VolumeDataLayer*));

  memset(lowerLOD, 0, nChannels * sizeof(VolumeDataLayer*));

  bool isCreateMoreLODs = true;

  for(int32_t iLOD = 0; isCreateMoreLODs; iLOD++)
  {
    isCreateMoreLODs = (iLOD < nPhysicalLODLevels - 1); // Always create all physical LODs even if we get only one cube before the top level;

    for(int32_t iDimension = 0; iDimension < array_size(brickSize); iDimension++)
    {
      brickSize[iDimension] = DimensionGroupUtil::IsDimensionInGroup(dimensionGroup, iDimension) ? (nBrickSize << (iDimension != m_fullResolutionDimension ? iLOD : 0)) : 1;
    }

    VolumeDataPartition primaryPartition(iLOD, dimensionGroup, null, m_dimensionNumSamples, brickSize, null, null, BorderMode::None, null, null, m_negativeRenderMargin, m_positiveRenderMargin, m_fullResolutionDimension);

    VolumeDataLayer *primaryChannelLayer = NULL;

    for(int32_t iChannel = 0; iChannel < nChannels; iChannel++)
    {
      if(iLOD > 0 && !IsChannelRenderable(iChannel))
      {
        continue;
      }

      VolumeDataLayer::LayerType layerType = (iLOD < nPhysicalLODLevels) ? VolumeDataLayer::Renderable : VolumeDataLayer::Virtual;

      if(!IsChannelRenderable(iChannel))
      {
        assert(iChannel != 0);
        layerType = VolumeDataLayer::Auxiliary;
      }

      VolumeDataChannelMapping const * volumeDataChannelMapping = nullptr;// = GetVolumeDataChannelMapping(iChannel);

      VolumeDataLayer *volumeDataLayer = new VolumeDataLayer(VolumeDataPartition::StaticMapPartition(primaryPartition, volumeDataChannelMapping, GetMappedValueCount(iChannel)),
                                                            this, iChannel,primaryChannelLayer, lowerLOD[iChannel], layerType, volumeDataChannelMapping);

      if(iChannel == 0)
      {
        primaryChannelLayer = volumeDataLayer;
      }

      assert(volumeDataLayer->GetLOD() == iLOD);

      lowerLOD[iChannel] = volumeDataLayer;

      for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
      {
        if(volumeDataLayer->IsDimensionChunked(iDimension) && volumeDataLayer->GetNumChunksInDimension(iDimension) > 1 && iDimension != m_fullResolutionDimension)
        {
          isCreateMoreLODs = true;
          break;
        }
      }
    }

    m_primaryTopLayers[dimensionGroup] = primaryChannelLayer;
    if(iLOD == 0)
    {
      m_primaryBaseLayers[dimensionGroup] = primaryChannelLayer;
    }

    //// Default physical layers to NEVER_REMAP
    //if(iLOD < nPhysicalLODLevels) _apcPrimaryTopLayers[eDimensionGroup]->SetProduceMethod(VolumeDataLayer_c::NEVER_REMAP, 0);
    if(iLOD < nPhysicalLODLevels) m_primaryTopLayers[dimensionGroup]->SetProduceStatus(VolumeDataLayer::ProduceStatusNormal);
  }
}

}
