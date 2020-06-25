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

  uint64_t GetFormatHash(VolumeDataChannelDescriptor::Format actualFormat, bool isReplaceNoValue, float replacementNoValue) const;

public:
  VolumeDataLayer(VolumeDataPartition const& volumeDataPartition, VolumeDataLayoutImpl* volumeDataLayout, int32_t channel, VolumeDataLayer* primaryChannelLayer, VolumeDataLayer* lowerLOD, LayerType layerType, const VolumeDataChannelMapping* volumeDataChannelMapping);

  VolumeDataLayoutImpl *GetLayout() const
  {
    return m_volumeDataLayout;
  }

  VolumeDataLayerID GetLayerID() const
  {
    return m_layerID;
  }

  int32_t GetChannelIndex() const
  {
    return m_channel;
  }

  const VolumeDataChannelMapping* GetVolumeDataChannelMapping() const;

  int32_t GetMappedValueCount() const;

  LayerType GetLayerType() const
  {
    return m_layerType;
  }

  VolumeDataLayer& GetPrimaryChannelLayer()
  {
    return m_primaryChannelLayer ? *m_primaryChannelLayer : *this;
  }

  const VolumeDataLayer& GetPrimaryChannelLayer() const
  {
    return m_primaryChannelLayer ? *m_primaryChannelLayer : *this;
  }

  VolumeDataLayer& GetBaseLayer()
  {
    VolumeDataLayer* baseLayer = this; while (baseLayer->m_lowerLOD) baseLayer = baseLayer->m_lowerLOD; return *baseLayer;
  }

  const VolumeDataLayer& GetBaseLayer() const
  {
    VolumeDataLayer const* baseLayer = this; while (baseLayer->m_lowerLOD) baseLayer = baseLayer->m_lowerLOD; return *baseLayer;
  }

  VolumeDataLayer* GetNextChannelLayer()
  {
    return m_nextChannelLayer;
  }

  const VolumeDataLayer* GetNextChannelLayer() const
  {
    return m_nextChannelLayer;
  }

  VolumeDataLayer* GetParentLayer()
  {
    return m_higherLOD;
  }

  const VolumeDataLayer* GetParentLayer() const
  {
    return m_higherLOD;
  }

  VolumeDataLayer* GetChildLayer()
  {
    return m_lowerLOD;
  }

  const VolumeDataLayer* GetChildLayer() const
  {
    return m_lowerLOD;
  }

  const VolumeDataLayer* GetLayerToRemapFrom() const;

  void          GetChunkIndexArrayFromVoxel(const IndexArray & voxel, IndexArray &chunk) const;

  int64_t       GetChunkIndexFromNDPos(const NDPos & cNDPos) const;

  void          GetChunksInRegion(const IndexArray& min, const IndexArray& max, std::vector<VolumeDataChunk>* volumeDataChunk, bool isAppend = false) const;

  void          GetChunksOverlappingChunk(VolumeDataChunk const& cVolumeDataChunk, std::vector<VolumeDataChunk>* volumeDataChunk, bool isAppend = false) const;

  VolumeDataChunk GetChunkFromIndex(int64_t chunk) const { assert(chunk >= 0 && chunk < GetTotalChunkCount()); return {this, chunk}; }

  ProduceStatus GetProduceStatus() const;

  const VolumeDataChannelDescriptor& GetVolumeDataChannelDescriptor() const;

  FloatRange const& GetValueRange() const;

  FloatRange const& GetActualValueRange() const;

  VolumeDataChannelDescriptor::Format GetFormat() const;

  VolumeDataChannelDescriptor::Components GetComponents() const;

  bool IsDiscrete() const;

  uint64_t GetFormatHash(VolumeDataChannelDescriptor::Format actualFormat = VolumeDataChannelDescriptor::Format_Any) const
  {
    return GetFormatHash(actualFormat, false, 0);
  }

  uint64_t GetFormatHash(VolumeDataChannelDescriptor::Format actualFormat, float replacementNoValue) const
  {
    return GetFormatHash(actualFormat, true, replacementNoValue);
  }

  bool IsUseNoValue() const;

  float GetNoValue() const;

  CompressionMethod GetEffectiveCompressionMethod() const;

  float GetEffectiveCompressionTolerance() const;

  static int32_t GetEffectiveWaveletAdaptiveLoadLevel(float effectiveCompressionTolerance, float compressionTolerance);

  int32_t GetEffectiveWaveletAdaptiveLoadLevel() const;

  float GetIntegerScale() const;

  float GetIntegerOffset() const;

  void SetProduceStatus(ProduceStatus produceStatus);
};
}
#endif //VOLUMEDATALAYER_H
