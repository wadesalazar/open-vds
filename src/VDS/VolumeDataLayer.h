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

#ifndef VOLUMEDATALAYER_H
#define VOLUMEDATALAYER_H

#include <OpenVDS/VolumeDataChannelDescriptor.h>

#include "VolumeDataPartition.h"
#include "VolumeDataChunk.h"

#include <Math/Range.h>
#include <Math/Vector.h>

#include <vector>

namespace OpenVDS
{
class VolumeDataLayout;

class VolumeDataLayer : public VolumeDataPartition
{
public:

  enum VolumeDataLayerID
  {
    LayerIdNone = -1,
    CachableLayeridMax = 1024
  };

  enum LayerType
  {
    Renderable,
    Auxiliary,
    AuxiliaryNoRenderMargin, // This layer type is only used by the VirtualTextureViewContext! It doesn't have larger bricks at the start and end of the volume.
    Virtual
  };

  enum ProduceMethod
  {
    // These are ordered, so greater values mean easier to produce
    AlwaysRemap,
    RemapFromCachedIfPossible,
    NeverRemap
  };

  enum ProduceStatus
  {
    // These are ordered, so greater values mean easier to produce
    ProduceStatusUnavailable,
    ProduceStatusRemapped,
    ProduceStatusNormal
  };

private:
  VolumeDataLayout * m_volumeDataLayout;                                          

  VolumeDataLayerID m_layerID;
  int32_t m_channel;

  const VolumeDataChannelMapping * m_volumeDataChannelMapping;

  LayerType   m_layerType;

  VolumeDataLayer * m_primaryChannelLayer;
  VolumeDataLayer * m_nextChannelLayer;
  VolumeDataLayer * m_lowerLod;
  VolumeDataLayer * m_higherLod;

 mutable const VolumeDataLayer * m_remapFromLayer;
 //bool m_isAllowRemapFromRemap;

  ProduceStatus m_produceStatus;

  ~VolumeDataLayer()
  {
  }

  uint64_t getFormatHash(VolumeDataChannelDescriptor::Format actualFormat, bool isReplaceNoValue, float replacementNoValue) const;

public:
  VolumeDataLayer(VolumeDataPartition const& volumeDataPartition, VolumeDataLayout* volumeDataLayout, int32_t channel, VolumeDataLayer* primaryChannelLayer, VolumeDataLayer* lowerLod, LayerType layerType, const VolumeDataChannelMapping* volumeDataChannelMapping);

  VolumeDataLayout *getLayout() const
  {
    return m_volumeDataLayout;
  }

  int32_t getChannelIndex() const
  {
    return m_channel;
  }

  const VolumeDataChannelMapping* getVolumeDataChannelMapping() const;

  int32_t getMappedValueCount() const;

  LayerType getLayerType() const
  {
    return m_layerType;
  }

  VolumeDataLayer& getPrimaryChannelLayer()
  {
    return m_primaryChannelLayer ? *m_primaryChannelLayer : *this;
  }

  const VolumeDataLayer& getPrimaryChannelLayer() const
  {
    return m_primaryChannelLayer ? *m_primaryChannelLayer : *this;
  }

  VolumeDataLayer& getBaseLayer()
  {
    VolumeDataLayer* baseLayer = this; while (baseLayer->m_lowerLod) baseLayer = baseLayer->m_lowerLod; return *baseLayer;
  }

  const VolumeDataLayer& getBaseLayer() const
  {
    VolumeDataLayer const* baseLayer = this; while (baseLayer->m_lowerLod) baseLayer = baseLayer->m_lowerLod; return *baseLayer;
  }

  VolumeDataLayer* getNextChannelLayer()
  {
    return m_nextChannelLayer;
  }

  const VolumeDataLayer* getNextChannelLayer() const
  {
    return m_nextChannelLayer;
  }

  VolumeDataLayer* getParentLayer()
  {
    return m_higherLod;
  }

  const VolumeDataLayer* getParentLayer() const
  {
    return m_higherLod;
  }

  VolumeDataLayer* getChildLayer()
  {
    return m_lowerLod;
  }

  const VolumeDataLayer* getChildLayer() const
  {
    return m_lowerLod;
  }

  const VolumeDataLayer* getLayerToRemapFrom() const;

  void          getChunkIndexArrayFromVoxel(const IndexArray & voxel, IndexArray &chunk) const;

  int64_t       getChunkIndexFromNDPos(const NDPos & cNDPos) const;

  void          getChunksInRegion(const IndexArray& min, const IndexArray& max, std::vector<VolumeDataChunk>* volumeDataChunk, bool isAppend = false) const;

  void          getChunksOverlappingChunk(VolumeDataChunk const& cVolumeDataChunk, std::vector<VolumeDataChunk>* volumeDataChunk, bool isAppend = false) const;

  VolumeDataChunk getChunkFromIndex(int64_t chunk) const { assert(chunk >= 0 && chunk < getTotalChunkCount()); return {this, chunk}; }

  ProduceStatus getProduceStatus() const;

  const VolumeDataChannelDescriptor& getVolumeDataChannelDescriptor() const;

  FloatRange const& getValueRange() const;

  FloatRange const& getActualValueRange() const;

  VolumeDataChannelDescriptor::Format getFormat() const;

  VolumeDataChannelDescriptor::Components getComponents() const;

  bool isDiscrete() const;

  uint64_t getFormatHash(VolumeDataChannelDescriptor::Format actualFormat = VolumeDataChannelDescriptor::FormatAny) const
  {
    return getFormatHash(actualFormat, false, 0);
  }

  uint64_t getFormatHash(VolumeDataChannelDescriptor::Format actualFormat, float replacementNoValue) const
  {
    return getFormatHash(actualFormat, true, replacementNoValue);
  }

  bool isUseNoValue() const;

  float getNoValue() const;

  CompressionMethod getEffectiveCompressionMethod() const;

  float getEffectiveCompressionTolerance() const;

  static int32_t getEffectiveWaveletAdaptiveLoadLevel(float effectiveCompressionTolerance, float compressionTolerance);

  int32_t getEffectiveWaveletAdaptiveLoadLevel() const;

  float getIntegerScale() const;

  float getIntegerOffset() const;

  // in cases where we have an integer data block for a non-integer layer, use this scale/offset to get back to the original values
  FloatVector2 getQuantizingScaleOffset(VolumeDataChannelDescriptor::Format dataBlockFormat) const;

  FloatVector2 getTextureScaleOffset(VolumeDataChannelDescriptor::Format dataBlockFormat) const;

  void setProduceStatus(ProduceStatus produceStatus);

  static FloatVector2 staticGetTextureScaleOffset(const FloatRange &valueRange, float integerScale, float integerOffset, bool isUseNoValue, VolumeDataChannelDescriptor::Format originalFormat, VolumeDataChannelDescriptor::Format dataBlockFormat);
};
}
#endif //VOLUMEDATALAYER_H
