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

#include <OpenVDS/VolumeDataLayoutDescriptor.h>

#include "VolumeDataLayoutImpl.h"
#include "VolumeDataChannelMapping.h"
#include "DimensionGroup.h"
#include "VDS.h"

#include "Bitmask.h"

#include <condition_variable>

namespace OpenVDS
{

static std::mutex &StaticGetPendingRequestCountMutex()
{
  static std::mutex pendingRequestCountMutex;
  return pendingRequestCountMutex;
}

static std::condition_variable &StaticGetPendingRequestCountChangedCondition()
{
  static std::condition_variable pendingRequestCountChangedCondition;

  return pendingRequestCountChangedCondition;
}

VolumeDataLayoutImpl::VolumeDataLayoutImpl(VDS &vds,
                   const VolumeDataLayoutDescriptor &layoutDescriptor,
                   const std::vector<VolumeDataAxisDescriptor> &axisDescriptor,
                   const std::vector<VolumeDataChannelDescriptor> &volumeDataChannelDescriptor,
                   int32_t actualValueRangeChannel,
                   FloatRange const &actualValueRange,
                   VolumeDataHash const &volumeDataHash,
                   CompressionMethod compressionMethod,
                   float compressionTolerance,
                   bool isZipLosslessChannels,
                   int32_t waveletAdaptiveLoadLevel)
  : m_vds(vds)
  , m_volumeDataChannelDescriptor(volumeDataChannelDescriptor)
  , m_isReadOnly(false)
  , m_contentsHash(volumeDataHash)
  , m_dimensionality(int32_t(axisDescriptor.size()))
  , m_baseBrickSize(int32_t(1) << layoutDescriptor.GetBrickSize())
  , m_negativeRenderMargin(layoutDescriptor.GetNegativeMargin())
  , m_positiveRenderMargin(layoutDescriptor.GetPositiveMargin())
  , m_brickSize2DMultiplier(layoutDescriptor.GetBrickSizeMultiplier2D())
  , m_maxLOD(layoutDescriptor.GetLODLevels())
  , m_isCreate2DLODs(layoutDescriptor.IsCreate2DLODs())
  , m_pendingWriteRequests(0)
  , m_actualValueRangeChannel(actualValueRangeChannel)
  , m_actualValueRange(actualValueRange)
  , m_compressionMethod(compressionMethod)
  , m_compressionTolerance(compressionTolerance)
  , m_isZipLosslessChannels(isZipLosslessChannels)
  , m_waveletAdaptiveLoadLevel(waveletAdaptiveLoadLevel)
  , m_fullResolutionDimension(layoutDescriptor.GetFullResolutionDimension())
{

  for(int32_t dimension = 0; dimension < ArraySize(m_dimensionNumSamples); dimension++)
  {
    if (dimension < m_dimensionality)
    {
      assert(axisDescriptor[dimension].GetNumSamples() >= 1);
      m_dimensionNumSamples[dimension] = axisDescriptor[dimension].GetNumSamples();
      m_dimensionName[dimension] = axisDescriptor[dimension].GetName();
      m_dimensionUnit[dimension] = axisDescriptor[dimension].GetUnit();
      m_dimensionCoordinateMin[dimension] = axisDescriptor[dimension].GetCoordinateMin();
      m_dimensionCoordinateMax[dimension] = axisDescriptor[dimension].GetCoordinateMax();
    }
    else
    {
      m_dimensionNumSamples[dimension] = 1;
    }
  }

  memset(m_primaryBaseLayers, 0, sizeof(m_primaryBaseLayers));
  memset(m_primaryTopLayers, 0, sizeof(m_primaryTopLayers));
}

VolumeDataLayoutImpl::~VolumeDataLayoutImpl()
{
}

VolumeDataLayer::VolumeDataLayerID VolumeDataLayoutImpl::AddDataLayer(VolumeDataLayer *layer)
{
  m_volumeDataLayers.push_back(layer);
  return VolumeDataLayer::VolumeDataLayerID(m_volumeDataLayers.size() - 1);
}

VolumeDataLayer* VolumeDataLayoutImpl::GetBaseLayer(DimensionGroup dimensionGroup, int32_t channel) const
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

FloatRange const& VolumeDataLayoutImpl::GetChannelActualValueRange(int32_t channel) const
{
  assert(channel >= 0 && channel < GetChannelCount());
  return (channel == m_actualValueRangeChannel) ? m_actualValueRange : m_volumeDataChannelDescriptor[channel].GetValueRange();
}

VolumeDataMapping VolumeDataLayoutImpl::GetChannelMapping(int32_t channel) const
{
  assert(channel >= 0 && channel < GetChannelCount());
  return m_volumeDataChannelDescriptor[channel].GetMapping();
}

int32_t VolumeDataLayoutImpl::GetChannelMappedValueCount(int32_t channel) const
{
  assert(channel >= 0 && channel < GetChannelCount());
  return m_volumeDataChannelDescriptor[channel].GetMappedValueCount();
}

const VolumeDataChannelMapping* VolumeDataLayoutImpl::GetVolumeDataChannelMapping(int32_t channel) const
{
  assert(channel >= 0 && channel < int32_t(m_volumeDataChannelDescriptor.size()));
  return VolumeDataChannelMapping::GetVolumeDataChannelMapping(m_volumeDataChannelDescriptor[channel].GetMapping());
}

VolumeDataLayer *VolumeDataLayoutImpl::GetVolumeDataLayerFromID(VolumeDataLayer::VolumeDataLayerID volumeDataLayerID) const
{
  assert(volumeDataLayerID >= 0 || volumeDataLayerID == VolumeDataLayer::LayerIdNone);
  if(volumeDataLayerID == VolumeDataLayer::LayerIdNone || volumeDataLayerID >= GetLayerCount())
  {
    return nullptr;
  }
  else
  {
    return m_volumeDataLayers[volumeDataLayerID];
  }
}
  
VolumeDataLayer *VolumeDataLayoutImpl::GetTopLayer(DimensionGroup dimensionGroup, int32_t channel) const
{
  assert(dimensionGroup >= 0 && dimensionGroup < DimensionGroup_3D_Max);
  assert(channel >= 0 && channel < GetChannelCount());

  VolumeDataLayer *volumeDataLayer = m_primaryTopLayers[dimensionGroup];

  while(channel-- && volumeDataLayer)
  {
    volumeDataLayer = volumeDataLayer->GetNextChannelLayer();
  }
  return volumeDataLayer;
}

int32_t VolumeDataLayoutImpl::ChangePendingWriteRequestCount(int32_t difference)
{
  std::unique_lock<std::mutex> pendingRequestCountMutexLock(StaticGetPendingRequestCountMutex());

  assert(m_pendingWriteRequests + difference >= 0);
  m_pendingWriteRequests += difference;
  int32_t ret = m_pendingWriteRequests;
  if(difference)
  {
    pendingRequestCountMutexLock.unlock();
    StaticGetPendingRequestCountChangedCondition().notify_all();
  }
  return ret;
}

void VolumeDataLayoutImpl::CompletePendingWriteChunkRequests(int32_t maxPendingWriteRequests) const
{
  std::unique_lock<std::mutex> pendingRequestCountMutexLock(StaticGetPendingRequestCountMutex());

  StaticGetPendingRequestCountChangedCondition().wait(pendingRequestCountMutexLock, [this, maxPendingWriteRequests] { return m_pendingWriteRequests <= maxPendingWriteRequests; });
}

bool VolumeDataLayoutImpl::IsChannelAvailable(const char *channelName) const
{
  int32_t nChannels = GetChannelCount();

  for(int32_t channel = 0; channel < nChannels; channel++)
  {
    if(strcmp(m_volumeDataChannelDescriptor[channel].GetName(), channelName) == 0) return true;
  }

  return false;
}

int32_t VolumeDataLayoutImpl::GetChannelIndex(const char *channelName) const
{
  int32_t  nChannels = GetChannelCount();

  for(int32_t channel = 0; channel < nChannels; channel++)
  {
    if(strcmp(m_volumeDataChannelDescriptor[channel].GetName(), channelName) == 0) return channel;
  }
  assert(0 && "Should not call this function unless IsChannelAvailable() is true");
  return 0;
}

VolumeDataChannelDescriptor VolumeDataLayoutImpl::GetChannelDescriptor(int32_t channel) const
{
  assert(channel >= 0 && channel < GetChannelCount()); 

  const VolumeDataChannelDescriptor &volumeDataChannelDescriptor = m_volumeDataChannelDescriptor[channel];

  Internal::BitMask<VolumeDataChannelDescriptor::Flags> bFlags = VolumeDataChannelDescriptor::Flags(0);

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
                                       volumeDataChannelDescriptor.GetValueRange().Min,
                                       volumeDataChannelDescriptor.GetValueRange().Max,
                                       GetChannelMapping(channel),
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
                                     volumeDataChannelDescriptor.GetValueRange().Min,
                                     volumeDataChannelDescriptor.GetValueRange().Max,
                                     GetChannelMapping(channel),
                                     volumeDataChannelDescriptor.GetMappedValueCount(),
                                     VolumeDataChannelDescriptor::Flags(bFlags._flags),
                                     volumeDataChannelDescriptor.GetIntegerScale(),
                                     volumeDataChannelDescriptor.GetIntegerOffset());
}

VolumeDataLayoutDescriptor VolumeDataLayoutImpl::GetLayoutDescriptor() const
{
  VolumeDataLayoutDescriptor::BrickSize
    brickSize;

  switch(m_baseBrickSize)
  {
  case   32: brickSize = VolumeDataLayoutDescriptor::BrickSize_32; break;
  case   64: brickSize = VolumeDataLayoutDescriptor::BrickSize_64; break;
  case  128: brickSize = VolumeDataLayoutDescriptor::BrickSize_128; break;
  case  256: brickSize = VolumeDataLayoutDescriptor::BrickSize_256; break;
  case  512: brickSize = VolumeDataLayoutDescriptor::BrickSize_512; break;
  case 1024: brickSize = VolumeDataLayoutDescriptor::BrickSize_1024; break;
  case 2048: brickSize = VolumeDataLayoutDescriptor::BrickSize_2048; break;
  case 4096: brickSize = VolumeDataLayoutDescriptor::BrickSize_4096; break;

  default: assert(0 && "illegal bricksize"); brickSize = VolumeDataLayoutDescriptor::BrickSize();
  }

  VolumeDataLayoutDescriptor::LODLevels
    lodLevels = (VolumeDataLayoutDescriptor::LODLevels)m_maxLOD;

  VolumeDataLayoutDescriptor::Options
    options = VolumeDataLayoutDescriptor::Options_None;

  if(m_isCreate2DLODs)                options = options | VolumeDataLayoutDescriptor::Options_Create2DLODs;
  if(m_fullResolutionDimension != -1) options = options | VolumeDataLayoutDescriptor::Options_ForceFullResolutionDimension;

  return VolumeDataLayoutDescriptor(brickSize,
                                    m_negativeRenderMargin, m_positiveRenderMargin,
                                    m_brickSize2DMultiplier,
                                    lodLevels,
                                    options,
                                    m_fullResolutionDimension);
}

VolumeDataAxisDescriptor VolumeDataLayoutImpl::GetAxisDescriptor(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < m_dimensionality);
  return VolumeDataAxisDescriptor(GetDimensionNumSamples(dimension),
                                  GetDimensionName(dimension),
                                  GetDimensionUnit(dimension),
                                  GetDimensionMin(dimension),
                                  GetDimensionMax(dimension));
}

int VolumeDataLayoutImpl::GetDimensionNumSamples(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < ArraySize(m_dimensionNumSamples));
  return m_dimensionNumSamples[dimension];
}

const char *VolumeDataLayoutImpl::GetDimensionName(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < ArraySize(m_dimensionName));
  return m_dimensionName[dimension];
}

const char *VolumeDataLayoutImpl::GetDimensionUnit(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < ArraySize(m_dimensionUnit));
  return m_dimensionUnit[dimension];
}
  
void VolumeDataLayoutImpl::SetContentsHash(VolumeDataHash const &contentsHash)
{
  m_contentsHash = contentsHash;
}

void VolumeDataLayoutImpl::SetActualValueRange(int32_t actualValueRangeChannel, FloatRange const& actualValueRange)
{
  m_actualValueRangeChannel = actualValueRangeChannel;
  m_actualValueRange = actualValueRange;
}


void VolumeDataLayoutImpl::CreateLayers(DimensionGroup dimensionGroup, int32_t brickSize, int32_t physicalLODLevels, VolumeDataLayer::ProduceStatus produceStatus)
{
  assert(physicalLODLevels > 0);

  int32_t channels = GetChannelCount();

  IndexArray brickSizeArray;

  static IndexArray null;

  VolumeDataLayer **lowerLOD = (VolumeDataLayer **)alloca(channels * sizeof(VolumeDataLayer*));

  memset(lowerLOD, 0, channels * sizeof(VolumeDataLayer*));

  bool isCreateMoreLODs = true;

  for(int32_t lod = 0; isCreateMoreLODs; lod++)
  {
    isCreateMoreLODs = (lod < physicalLODLevels - 1); // Always create all physical lods even if we get only one cube before the top level;

    for(int32_t dimension = 0; dimension < ArraySize(brickSizeArray); dimension++)
    {
      brickSizeArray[dimension] = DimensionGroupUtil::IsDimensionInGroup(dimensionGroup, dimension) ? (brickSize << (dimension != m_fullResolutionDimension ? lod : 0)) : 1;
    }

    VolumeDataPartition primaryPartition(lod, dimensionGroup, null, m_dimensionNumSamples, brickSizeArray, null, null, BorderMode::None, null, null, m_negativeRenderMargin, m_positiveRenderMargin, m_fullResolutionDimension);

    VolumeDataLayer *primaryChannelLayer = nullptr;

    for(int32_t channel = 0; channel < channels; channel++)
    {
      if(lod > 0 && !IsChannelRenderable(channel))
      {
        continue;
      }

      VolumeDataLayer::LayerType layerType = (lod < physicalLODLevels) ? VolumeDataLayer::Renderable : VolumeDataLayer::Virtual;

      if(!IsChannelRenderable(channel))
      {
        assert(channel != 0);
        layerType = VolumeDataLayer::Auxiliary;
      }

      VolumeDataChannelMapping const *volumeDataChannelMapping = GetVolumeDataChannelMapping(channel);

      VolumeDataLayer *volumeDataLayer = new VolumeDataLayer(VolumeDataPartition::StaticMapPartition(primaryPartition, volumeDataChannelMapping, GetChannelMappedValueCount(channel)),
                                                            this, channel,primaryChannelLayer, lowerLOD[channel], layerType, volumeDataChannelMapping);

      if(channel == 0)
      {
        primaryChannelLayer = volumeDataLayer;
      }

      assert(volumeDataLayer->GetLOD() == lod);

      lowerLOD[channel] = volumeDataLayer;

      for(int32_t dimension = 0; dimension < Dimensionality_Max; dimension++)
      {
        if(volumeDataLayer->IsDimensionChunked(dimension) && volumeDataLayer->GetNumChunksInDimension(dimension) > 1 && dimension != m_fullResolutionDimension)
        {
          isCreateMoreLODs = true;
          break;
        }
      }
    }

    m_primaryTopLayers[dimensionGroup] = primaryChannelLayer;
    if(lod == 0)
    {
      m_primaryBaseLayers[dimensionGroup] = primaryChannelLayer;
    }

    if(lod < physicalLODLevels)
    {
      for(VolumeDataLayer *volumeDataLayer = m_primaryTopLayers[dimensionGroup]; volumeDataLayer; volumeDataLayer = volumeDataLayer->GetNextChannelLayer())
      {
        if(volumeDataLayer->GetLayerType() != VolumeDataLayer::Virtual)
        {
          volumeDataLayer->SetProduceStatus(produceStatus);
        }
      }
    }
  }
}

bool        VolumeDataLayoutImpl::IsMetadataIntAvailable(const char* category, const char* name) const           { return m_vds.metadataContainer.IsMetadataIntAvailable(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataIntVector2Available(const char* category, const char* name) const    { return m_vds.metadataContainer.IsMetadataIntVector2Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataIntVector3Available(const char* category, const char* name) const    { return m_vds.metadataContainer.IsMetadataIntVector3Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataIntVector4Available(const char* category, const char* name) const    { return m_vds.metadataContainer.IsMetadataIntVector4Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataFloatAvailable(const char* category, const char* name) const         { return m_vds.metadataContainer.IsMetadataFloatAvailable(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataFloatVector2Available(const char* category, const char* name) const  { return m_vds.metadataContainer.IsMetadataFloatVector2Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataFloatVector3Available(const char* category, const char* name) const  { return m_vds.metadataContainer.IsMetadataFloatVector3Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataFloatVector4Available(const char* category, const char* name) const  { return m_vds.metadataContainer.IsMetadataFloatVector4Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataDoubleAvailable(const char* category, const char* name) const        { return m_vds.metadataContainer.IsMetadataDoubleAvailable(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataDoubleVector2Available(const char* category, const char* name) const { return m_vds.metadataContainer.IsMetadataDoubleVector2Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataDoubleVector3Available(const char* category, const char* name) const { return m_vds.metadataContainer.IsMetadataDoubleVector3Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataDoubleVector4Available(const char* category, const char* name) const { return m_vds.metadataContainer.IsMetadataDoubleVector4Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataStringAvailable(const char* category, const char* name) const        { return m_vds.metadataContainer.IsMetadataStringAvailable(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataBLOBAvailable(const char* category, const char* name) const          { return m_vds.metadataContainer.IsMetadataBLOBAvailable(category, name); }

int           VolumeDataLayoutImpl::GetMetadataInt(const char* category, const char* name) const             { return m_vds.metadataContainer.GetMetadataInt(category, name); }
IntVector2    VolumeDataLayoutImpl::GetMetadataIntVector2(const char* category, const char* name) const      { return m_vds.metadataContainer.GetMetadataIntVector2(category, name); }
IntVector3    VolumeDataLayoutImpl::GetMetadataIntVector3(const char* category, const char* name) const      { return m_vds.metadataContainer.GetMetadataIntVector3(category, name); }
IntVector4    VolumeDataLayoutImpl::GetMetadataIntVector4(const char* category, const char* name) const      { return m_vds.metadataContainer.GetMetadataIntVector4(category, name); }
float         VolumeDataLayoutImpl::GetMetadataFloat(const char* category, const char* name) const          { return m_vds.metadataContainer.GetMetadataFloat(category, name); }
FloatVector2  VolumeDataLayoutImpl::GetMetadataFloatVector2(const char* category, const char* name) const   { return m_vds.metadataContainer.GetMetadataFloatVector2(category, name); }
FloatVector3  VolumeDataLayoutImpl::GetMetadataFloatVector3(const char* category, const char* name) const   { return m_vds.metadataContainer.GetMetadataFloatVector3(category, name); }
FloatVector4  VolumeDataLayoutImpl::GetMetadataFloatVector4(const char* category, const char* name) const   { return m_vds.metadataContainer.GetMetadataFloatVector4(category, name); }
double        VolumeDataLayoutImpl::GetMetadataDouble(const char* category, const char* name) const        { return m_vds.metadataContainer.GetMetadataDouble(category, name); }
DoubleVector2 VolumeDataLayoutImpl::GetMetadataDoubleVector2(const char* category, const char* name) const { return m_vds.metadataContainer.GetMetadataDoubleVector2(category, name); }
DoubleVector3 VolumeDataLayoutImpl::GetMetadataDoubleVector3(const char* category, const char* name) const { return m_vds.metadataContainer.GetMetadataDoubleVector3(category, name); }
DoubleVector4 VolumeDataLayoutImpl::GetMetadataDoubleVector4(const char* category, const char* name) const { return m_vds.metadataContainer.GetMetadataDoubleVector4(category, name); }
const char*   VolumeDataLayoutImpl::GetMetadataString(const char* category, const char* name) const          { return m_vds.metadataContainer.GetMetadataString(category, name); }
void          VolumeDataLayoutImpl::GetMetadataBLOB(const char* category, const char* name, const void **data, size_t *size)  const { return m_vds.metadataContainer.GetMetadataBLOB(category, name, data, size); }
MetadataKeyRange
              VolumeDataLayoutImpl::GetMetadataKeys() const { return m_vds.metadataContainer.GetMetadataKeys(); }

}
