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
#include "OpenVDSHandle.h"

#include "Bitmask.h"

#include <condition_variable>

namespace OpenVDS
{

static std::mutex &staticGetPendingRequestCountMutex()
{
  static std::mutex pendingRequestCountMutex;
  return pendingRequestCountMutex;
}

static std::condition_variable &staticGetPendingRequestCountChangedCondition()
{
  static std::condition_variable pendingRequestCountChangedCondition;

  return pendingRequestCountChangedCondition;
}

VolumeDataLayoutImpl::VolumeDataLayoutImpl(VDSHandle &handle,
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
  : m_handle(handle)
  , m_dimensionality(int32_t(axisDescriptor.size()))
  , m_baseBrickSize(int32_t(1) << layoutDescriptor.getBrickSize())
  , m_negativeRenderMargin(layoutDescriptor.getNegativeMargin())
  , m_positiveRenderMargin(layoutDescriptor.getPositiveMargin())
  , m_brickSize2DMultiplier(layoutDescriptor.getBrickSizeMultiplier2D())
  , m_maxLOD(layoutDescriptor.getLODLevels())
  , m_volumeDataChannelDescriptor(volumeDataChannelDescriptor)
  , m_isCreate2DLODs(layoutDescriptor.isCreate2DLODs())
  , m_actualValueRangeChannel(actualValueRangeChannel)
  , m_contentsHash(volumeDataHash)
  , m_pendingWriteRequests(0)
  , m_actualValueRange(actualValueRange)
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

VolumeDataLayoutImpl::~VolumeDataLayoutImpl()
{
}

VolumeDataLayer::VolumeDataLayerID VolumeDataLayoutImpl::addDataLayer(VolumeDataLayer *layer)
{
  m_volumeDataLayers.push_back(layer);
  return VolumeDataLayer::VolumeDataLayerID(m_volumeDataLayers.size() - 1);
}

VolumeDataLayer* VolumeDataLayoutImpl::getBaseLayer(DimensionGroup dimensionGroup, int32_t channel) const
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

FloatRange const& VolumeDataLayoutImpl::getChannelActualValueRange(int32_t channel) const
{
  assert(channel >= 0 && channel < getChannelCount());
  return (channel == m_actualValueRangeChannel) ? m_actualValueRange : m_volumeDataChannelDescriptor[channel].getValueRange();
}

VolumeDataMapping VolumeDataLayoutImpl::getChannelMapping(int32_t channel) const
{
  assert(channel >= 0 && channel < getChannelCount());
  return m_volumeDataChannelDescriptor[channel].getMapping();
}

int32_t VolumeDataLayoutImpl::getChannelMappedValueCount(int32_t channel) const
{
  assert(channel >= 0 && channel < getChannelCount());
  return m_volumeDataChannelDescriptor[channel].getMappedValueCount();
}

FloatRange const& VolumeDataLayoutImpl::getDimensionRange(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < m_dimensionality);
  return m_dimensionRange[dimension];
}

const VolumeDataChannelMapping* VolumeDataLayoutImpl::getVolumeDataChannelMapping(int32_t channel) const
{
  assert(channel >= 0 && channel < m_volumeDataChannelDescriptor.size());
  return VolumeDataChannelMapping::getVolumeDataChannelMapping(m_volumeDataChannelDescriptor[channel].getMapping());
}

VolumeDataLayer *VolumeDataLayoutImpl::getVolumeDataLayerFromID(VolumeDataLayer::VolumeDataLayerID volumeDataLayerID) const
{
  assert(volumeDataLayerID >= 0 || volumeDataLayerID == VolumeDataLayer::LayerIdNone);
  if(volumeDataLayerID == VolumeDataLayer::LayerIdNone || volumeDataLayerID >= getLayerCount())
  {
    return nullptr;
  }
  else
  {
    return m_volumeDataLayers[volumeDataLayerID];
  }
}
  
VolumeDataLayer *VolumeDataLayoutImpl::getTopLayer(DimensionGroup dimensionGroup, int32_t channel) const
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

int32_t VolumeDataLayoutImpl::changePendingWriteRequestCount(int32_t difference)
{
  std::unique_lock<std::mutex> pendingRequestCountMutexLock(staticGetPendingRequestCountMutex());

  assert(m_pendingWriteRequests + difference >= 0);
  m_pendingWriteRequests += difference;
  int32_t ret = m_pendingWriteRequests;
  if(difference)
  {
    pendingRequestCountMutexLock.unlock();
    staticGetPendingRequestCountChangedCondition().notify_all();
  }
  return ret;
}

void VolumeDataLayoutImpl::completePendingWriteChunkRequests(int32_t maxPendingWriteRequests) const
{
  std::unique_lock<std::mutex> pendingRequestCountMutexLock(staticGetPendingRequestCountMutex());

  staticGetPendingRequestCountChangedCondition().wait(pendingRequestCountMutexLock, [this, maxPendingWriteRequests] { return m_pendingWriteRequests <= maxPendingWriteRequests; });
}

bool VolumeDataLayoutImpl::isChannelAvailable(const char *channelName) const
{
  int32_t nChannels = getChannelCount();

  for(int32_t channel = 0; channel < nChannels; channel++)
  {
    if(strcmp(m_volumeDataChannelDescriptor[channel].getName(), channelName) == 0) return true;
  }

  return false;
}

int32_t VolumeDataLayoutImpl::getChannelIndex(const char *channelName) const
{
  int32_t  nChannels = getChannelCount();

  for(int32_t channel = 0; channel < nChannels; channel++)
  {
    if(strcmp(m_volumeDataChannelDescriptor[channel].getName(), channelName) == 0) return channel;
  }
  assert(0 && "Should not call this function unless IsChannelAvailable() is true");
  return 0;
}

VolumeDataChannelDescriptor VolumeDataLayoutImpl::getChannelDescriptor(int32_t channel) const
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

VolumeDataLayoutDescriptor VolumeDataLayoutImpl::getLayoutDescriptor() const
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

VolumeDataAxisDescriptor VolumeDataLayoutImpl::getAxisDescriptor(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < m_dimensionality);
  return VolumeDataAxisDescriptor(getDimensionNumSamples(dimension),
                                  getDimensionName(dimension),
                                  getDimensionUnit(dimension),
                                  getDimensionRange(dimension));
}

int VolumeDataLayoutImpl::getDimensionNumSamples(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < array_size(m_dimensionNumSamples));
  return m_dimensionNumSamples[dimension];
}

const char *VolumeDataLayoutImpl::getDimensionName(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < array_size(m_dimensionName));
  return m_dimensionName[dimension];
}

const char *VolumeDataLayoutImpl::getDimensionUnit(int32_t dimension) const
{
  assert(dimension >= 0 && dimension < array_size(m_dimensionUnit));
  return m_dimensionUnit[dimension];
}
  
void VolumeDataLayoutImpl::setContentsHash(VolumeDataHash const &contentsHash)
{
  m_contentsHash = contentsHash;
}

void VolumeDataLayoutImpl::setActualValueRange(int32_t actualValueRangeChannel, FloatRange const& actualValueRange)
{
  m_actualValueRangeChannel = actualValueRangeChannel;
  m_actualValueRange = actualValueRange;
}


void VolumeDataLayoutImpl::createLayers(DimensionGroup dimensionGroup, int32_t brickSize, int32_t physicalLODLevels, VolumeDataLayer::ProduceStatus produceStatus)
{
  assert(physicalLODLevels > 0);

  int32_t channels = getChannelCount();

  IndexArray brickSizeArray;

  static IndexArray null;

  VolumeDataLayer **lowerLOD = (VolumeDataLayer **)alloca(channels * sizeof(VolumeDataLayer*));

  memset(lowerLOD, 0, channels * sizeof(VolumeDataLayer*));

  bool isCreateMoreLODs = true;

  for(int32_t lod = 0; isCreateMoreLODs; lod++)
  {
    isCreateMoreLODs = (lod < physicalLODLevels - 1); // Always create all physical lods even if we get only one cube before the top level;

    for(int32_t dimension = 0; dimension < array_size(brickSizeArray); dimension++)
    {
      brickSizeArray[dimension] = DimensionGroupUtil::isDimensionInGroup(dimensionGroup, dimension) ? (brickSize << (dimension != m_fullResolutionDimension ? lod : 0)) : 1;
    }

    VolumeDataPartition primaryPartition(lod, dimensionGroup, null, m_dimensionNumSamples, brickSizeArray, null, null, BorderMode::None, null, null, m_negativeRenderMargin, m_positiveRenderMargin, m_fullResolutionDimension);

    VolumeDataLayer *primaryChannelLayer = nullptr;

    for(int32_t channel = 0; channel < channels; channel++)
    {
      if(lod > 0 && !isChannelRenderable(channel))
      {
        continue;
      }

      VolumeDataLayer::LayerType layerType = (lod < physicalLODLevels) ? VolumeDataLayer::Renderable : VolumeDataLayer::Virtual;

      if(!isChannelRenderable(channel))
      {
        assert(channel != 0);
        layerType = VolumeDataLayer::Auxiliary;
      }

      VolumeDataChannelMapping const *volumeDataChannelMapping = getVolumeDataChannelMapping(channel);

      VolumeDataLayer *volumeDataLayer = new VolumeDataLayer(VolumeDataPartition::staticMapPartition(primaryPartition, volumeDataChannelMapping, getChannelMappedValueCount(channel)),
                                                            this, channel,primaryChannelLayer, lowerLOD[channel], layerType, volumeDataChannelMapping);

      if(channel == 0)
      {
        primaryChannelLayer = volumeDataLayer;
      }

      assert(volumeDataLayer->getLOD() == lod);

      lowerLOD[channel] = volumeDataLayer;

      for(int32_t dimension = 0; dimension < Dimensionality_Max; dimension++)
      {
        if(volumeDataLayer->isDimensionChunked(dimension) && volumeDataLayer->getNumChunksInDimension(dimension) > 1 && dimension != m_fullResolutionDimension)
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
      for(VolumeDataLayer *volumeDataLayer = m_primaryTopLayers[dimensionGroup]; volumeDataLayer; volumeDataLayer = volumeDataLayer->getNextChannelLayer())
      {
        if(volumeDataLayer->getLayerType() != VolumeDataLayer::Virtual)
        {
          volumeDataLayer->setProduceStatus(produceStatus);
        }
      }
    }
  }
}

bool        VolumeDataLayoutImpl::IsMetadataIntAvailable(const char* category, const char* name) const           { return m_handle.metadataContainer.IsMetadataIntAvailable(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataIntVector2Available(const char* category, const char* name) const    { return m_handle.metadataContainer.IsMetadataIntVector2Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataIntVector3Available(const char* category, const char* name) const    { return m_handle.metadataContainer.IsMetadataIntVector3Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataIntVector4Available(const char* category, const char* name) const    { return m_handle.metadataContainer.IsMetadataIntVector4Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataFloatAvailable(const char* category, const char* name) const         { return m_handle.metadataContainer.IsMetadataFloatAvailable(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataFloatVector2Available(const char* category, const char* name) const  { return m_handle.metadataContainer.IsMetadataFloatVector2Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataFloatVector3Available(const char* category, const char* name) const  { return m_handle.metadataContainer.IsMetadataFloatVector3Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataFloatVector4Available(const char* category, const char* name) const  { return m_handle.metadataContainer.IsMetadataFloatVector4Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataDoubleAvailable(const char* category, const char* name) const        { return m_handle.metadataContainer.IsMetadataDoubleAvailable(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataDoubleVector2Available(const char* category, const char* name) const { return m_handle.metadataContainer.IsMetadataDoubleVector2Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataDoubleVector3Available(const char* category, const char* name) const { return m_handle.metadataContainer.IsMetadataDoubleVector3Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataDoubleVector4Available(const char* category, const char* name) const { return m_handle.metadataContainer.IsMetadataDoubleVector4Available(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataStringAvailable(const char* category, const char* name) const        { return m_handle.metadataContainer.IsMetadataStringAvailable(category, name); }
bool        VolumeDataLayoutImpl::IsMetadataBLOBAvailable(const char* category, const char* name) const          { return m_handle.metadataContainer.IsMetadataBLOBAvailable(category, name); }

int           VolumeDataLayoutImpl::GetMetadataInt(const char* category, const char* name) const             { return m_handle.metadataContainer.GetMetadataInt(category, name); }
IntVector2    VolumeDataLayoutImpl::GetMetadataIntVector2(const char* category, const char* name) const      { return m_handle.metadataContainer.GetMetadataIntVector2(category, name); }
IntVector3    VolumeDataLayoutImpl::GetMetadataIntVector3(const char* category, const char* name) const      { return m_handle.metadataContainer.GetMetadataIntVector3(category, name); }
IntVector4    VolumeDataLayoutImpl::GetMetadataIntVector4(const char* category, const char* name) const      { return m_handle.metadataContainer.GetMetadataIntVector4(category, name); }
float         VolumeDataLayoutImpl::GetMetadataFloat(const char* category, const char* name) const          { return m_handle.metadataContainer.GetMetadataFloat(category, name); }
FloatVector2  VolumeDataLayoutImpl::GetMetadataFloatVector2(const char* category, const char* name) const   { return m_handle.metadataContainer.GetMetadataFloatVector2(category, name); }
FloatVector3  VolumeDataLayoutImpl::GetMetadataFloatVector3(const char* category, const char* name) const   { return m_handle.metadataContainer.GetMetadataFloatVector3(category, name); }
FloatVector4  VolumeDataLayoutImpl::GetMetadataFloatVector4(const char* category, const char* name) const   { return m_handle.metadataContainer.GetMetadataFloatVector4(category, name); }
double        VolumeDataLayoutImpl::GetMetadataDouble(const char* category, const char* name) const        { return m_handle.metadataContainer.GetMetadataDouble(category, name); }
DoubleVector2 VolumeDataLayoutImpl::GetMetadataDoubleVector2(const char* category, const char* name) const { return m_handle.metadataContainer.GetMetadataDoubleVector2(category, name); }
DoubleVector3 VolumeDataLayoutImpl::GetMetadataDoubleVector3(const char* category, const char* name) const { return m_handle.metadataContainer.GetMetadataDoubleVector3(category, name); }
DoubleVector4 VolumeDataLayoutImpl::GetMetadataDoubleVector4(const char* category, const char* name) const { return m_handle.metadataContainer.GetMetadataDoubleVector4(category, name); }
const char*   VolumeDataLayoutImpl::GetMetadataString(const char* category, const char* name) const          { return m_handle.metadataContainer.GetMetadataString(category, name); }
void          VolumeDataLayoutImpl::GetMetadataBLOB(const char* category, const char* name, const void **data, size_t *size)  const { return m_handle.metadataContainer.GetMetadataBLOB(category, name, data, size); }

}
