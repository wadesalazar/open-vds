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
#include <OpenVDS/Range.h>
#include <OpenVDS/Vector.h>

#include "VolumeDataPartition.h"
#include "VolumeDataChunk.h"

#include <vector>

namespace OpenVDS
{
class VolumeDataLayoutImpl;

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
    ProduceStatus_Unavailable,
    ProduceStatus_Remapped,
    ProduceStatus_Normal
  };

private:
  VolumeDataLayoutImpl * m_volumeDataLayout;

  VolumeDataLayerID m_layerID;
  int32_t m_channel;

  const VolumeDataChannelMapping * m_volumeDataChannelMapping;

  LayerType   m_layerType;

  VolumeDataLayer * m_primaryChannelLayer;
  VolumeDataLayer * m_nextChannelLayer;
  VolumeDataLayer * m_lowerLOD;
  VolumeDataLayer * m_higherLOD;

 mutable const VolumeDataLayer * m_remapFromLayer;
 //bool m_isAllowRemapFromRemap;

  ProduceStatus m_produceStatus;

  ~VolumeDataLayer()
  {
  }

  uint64_t getFormatHash(VolumeDataChannelDescriptor::Format actualFormat, bool isReplaceNoValue, float replacementNoValue) const;

public:
  VolumeDataLayer(VolumeDataPartition const& volumeDataPartition, VolumeDataLayoutImpl* volumeDataLayout, int32_t channel, VolumeDataLayer* primaryChannelLayer, VolumeDataLayer* lowerLOD, LayerType layerType, const VolumeDataChannelMapping* volumeDataChannelMapping);

  VolumeDataLayoutImpl *getLayout() const
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
    VolumeDataLayer* baseLayer = this; while (baseLayer->m_lowerLOD) baseLayer = baseLayer->m_lowerLOD; return *baseLayer;
  }

  const VolumeDataLayer& getBaseLayer() const
  {
    VolumeDataLayer const* baseLayer = this; while (baseLayer->m_lowerLOD) baseLayer = baseLayer->m_lowerLOD; return *baseLayer;
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
    return m_higherLOD;
  }

  const VolumeDataLayer* getParentLayer() const
  {
    return m_higherLOD;
  }

  VolumeDataLayer* getChildLayer()
  {
    return m_lowerLOD;
  }

  const VolumeDataLayer* getChildLayer() const
  {
    return m_lowerLOD;
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

  uint64_t getFormatHash(VolumeDataChannelDescriptor::Format actualFormat = VolumeDataChannelDescriptor::Format_Any) const
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

  void setProduceStatus(ProduceStatus produceStatus);
};
}
#endif //VOLUMEDATALAYER_H
