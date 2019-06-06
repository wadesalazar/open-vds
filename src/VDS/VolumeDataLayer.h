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

#include "VolumeDataPartition.h"
#include "VolumeDataChunk.h"
#include "VolumeDataChannelDescriptor.h"

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
  VolumeDataLayer(VolumeDataPartition const& volumeDataPartition, VolumeDataLayout* volumeDataLayout, int32_t channel, VolumeDataLayer* primaryChannelLayer, VolumeDataLayer* lowerLOD, LayerType layerType, const VolumeDataChannelMapping* volumeDataChannelMapping);

  VolumeDataLayout *GetLayout() const
  {
    return m_volumeDataLayout;
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

  VolumeDataChunk GetChunkFromIndex(int64_t iChunk) const;

  //ProduceMethod GetProduceMethod() const;

  ProduceStatus GetProduceStatus() const;

  const VolumeDataChannelDescriptor& GetVolumeDataChannelDescriptor() const;

  Range<float> const& GetValueRange() const;

  Range<float> const& GetActualValueRange() const;

  VolumeDataChannelDescriptor::Format GetFormat() const;

  VolumeDataChannelDescriptor::Components GetComponents() const;

  bool          IsDiscrete() const;

  uint64_t           GetFormatHash(VolumeDataChannelDescriptor::Format actualFormat = VolumeDataChannelDescriptor::FormatAny) const
  {
    return GetFormatHash(actualFormat, false, 0);
  }

  uint64_t           GetFormatHash(VolumeDataChannelDescriptor::Format actualFormat, float replacementNoValue) const
  {
    return GetFormatHash(actualFormat, true, replacementNoValue);
  }

  bool          IsUseNoValue() const;

  float           GetNoValue() const;

  CompressionMethod GetEffectiveCompressionMethod() const;

  float           GetEffectiveCompressionTolerance() const;

  static int32_t    GetEffectiveWaveletAdaptiveLoadLevel(float effectiveCompressionTolerance, float compressionTolerance);

  int32_t           GetEffectiveWaveletAdaptiveLoadLevel() const;

  float           GetIntegerScale() const;

  float           GetIntegerOffset() const;

  // in cases where we have an integer data block for a non-integer layer, use this scale/offset to get back to the original values
  FloatVec2 GetQuantizingScaleOffset(VolumeDataChannelDescriptor::Format dataBlockFormat) const;

  FloatVec2 GetTextureScaleOffset(VolumeDataChannelDescriptor::Format dataBlockFormat) const;

  void          SetProduceStatus(ProduceStatus produceStatus);

  // Static methods
  static FloatVec2 StaticGetTextureScaleOffset(const Range<float> &valueRange, float integerScale, float integerOffset, bool isUseNoValue, VolumeDataChannelDescriptor::Format originalFormat, VolumeDataChannelDescriptor::Format dataBlockFormat);
};
}
#endif //VOLUMEDATALAYER_H
