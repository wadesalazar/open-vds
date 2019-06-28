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

#ifndef VOLUMEDATALAYOUT_H
#define VOLUMEDATALAYOUT_H

#include "VolumeDataLayer.h"
#include "VolumeDataHash.h"
#include "VolumeDataAxisDescriptor.h"

#include <vector>
#include <mutex>

namespace OpenVDS
{

class VolumeDataLayoutDescriptor;
class VolumeDataLayout
{
private:
  std::vector<VolumeDataLayer *> m_volumeDataLayers;
  std::vector<VolumeDataChannelDescriptor> m_volumeDataChannelDescriptor;
  bool          m_isReadOnly;
  VolumeDataHash m_contentsHash; 
  int32_t        m_dimensionality;
  int32_t        m_baseBrickSize;
  int32_t        m_negativeRenderMargin;
  int32_t        m_positiveRenderMargin;
  int32_t        m_actualValueRangeChannel;
  Range<float>   m_actualValueRange;

  VolumeDataLayer *m_primaryBaseLayers[DimensionGroup_3D_Max];
  VolumeDataLayer *m_primaryTopLayers[DimensionGroup_3D_Max];

  CompressionMethod m_compressionMethod;
  float m_compressionTolerance;
  bool  m_isZipLosslessChannels;
  int32_t m_waveletAdaptiveLoadLevel;
  IndexArray m_dimensionNumSamples;
  const char *m_dimensionName[Dimensionality_Max];
  const char *m_dimensionUnit[Dimensionality_Max];
  Range<float> m_dimensionRange[Dimensionality_Max];
  int32_t m_fullResolutionDimension;

public:
  VolumeDataLayout(const VolumeDataLayoutDescriptor &layoutDescriptor,
                   const std::vector<VolumeDataAxisDescriptor> &axisDescriptor,
                   const std::vector<VolumeDataChannelDescriptor> &volumeDataChannelDescriptor,
                   int32_t actualValueRangeChannel, 
                   Range<float> const &actualValueRange, 
                   VolumeDataHash const &volumeDataHash, 
                   CompressionMethod compressionMethod, 
                   float compressionTolerance, 
                   bool isZipLosslessChannels, 
                   int32_t waveletAdaptiveLoadLevel);

  uint64_t getContentsHash() const { return m_contentsHash; }
  VolumeDataLayer::VolumeDataLayerID addDataLayer(VolumeDataLayer *layer);

  Range<float> const &getChannelValueRange(int32_t channel) const { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].getValueRange(); }

  Range<float> const &getChannelActualValueRange(int32_t channel) const;

  //TODO SHOULD THIS BE REMOVED
  int32_t getMappedValueCount(int32_t channel) const;

  Range<float> const &getDimensionRange(int32_t dimension) const;

  VolumeDataLayer *getVolumeDataLayerFromID(VolumeDataLayer::VolumeDataLayerID volumeDataLayerID) const; 
  VolumeDataLayer *getVolumeDataLayerFromID(VolumeDataLayer::VolumeDataLayerID volumeDataLayerID) { return const_cast<VolumeDataLayer *>(const_cast<const VolumeDataLayout *>(this)->getVolumeDataLayerFromID(volumeDataLayerID)); }

  VolumeDataLayer *getTopLayer(DimensionGroup dimensionGroup, int32_t channel) const;
  VolumeDataLayer *getTopLayer(DimensionGroup dimensionGroup, int32_t channel) { return const_cast<VolumeDataLayer *>(const_cast<const VolumeDataLayout *>(this)->getTopLayer(dimensionGroup, channel)); }

  VolumeDataLayer *getBaseLayer(DimensionGroup dimensionGroup, int32_t channel) const;
  VolumeDataLayer *getBaseLayer(DimensionGroup dimensionGroup, int32_t channel) { return const_cast<VolumeDataLayer *>(const_cast<const VolumeDataLayout *>(this)->getBaseLayer(dimensionGroup, channel)); }

  int32_t getBaseBrickSize() const { return m_baseBrickSize; }
  int32_t getLayoutDimensionNumSamples(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_dimensionNumSamples[dimension]; }
  int32_t getLayerCount() const { return int32_t(m_volumeDataLayers.size()); }

  bool isReadOnly() const { return m_isReadOnly; }
  CompressionMethod getCompressionMethod() const { return m_compressionMethod; }
  float getCompressionTolerance() const { return m_compressionTolerance; }
  bool isZipLosslessChannels() const { return m_isZipLosslessChannels; }
  int32_t getWaveletAdaptiveLoadLevel() const { return m_waveletAdaptiveLoadLevel; }

  const VolumeDataChannelDescriptor &getVolumeDataChannelDescriptor(int32_t channel) const { return m_volumeDataChannelDescriptor[channel]; }

  //REMOVE VIRTUAL?
  // Implementation of VolumeDataLayout interface
  virtual int32_t getChannelCount() const { return int32_t(m_volumeDataChannelDescriptor.size()); }
  virtual bool isChannelAvailable(const char *channelName) const;
  virtual int32_t getChannelIndex(const char *channelName) const;
  virtual VolumeDataChannelDescriptor getChannelDescriptor(int32_t channel) const;
  virtual int32_t getDimensionality() const { return m_dimensionality; }

  virtual VolumeDataAxisDescriptor getAxisDescriptor(int32_t dimension) const;

//  // These convenience functions provide access to the individual elements of the value descriptor
  virtual VolumeDataChannelDescriptor::Format getChannelFormat(int32_t channel) const { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].getFormat(); }

  virtual VolumeDataChannelDescriptor::Components getChannelComponents(int32_t channel) const { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].getComponents(); }

  virtual const const char *getChannelName(int32_t channel) const { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].getName(); }

  virtual float getChannelValueRangeMin(int32_t channel) const { return getChannelValueRange(channel).min; }
  virtual float getChannelValueRangeMax(int32_t channel) const { return getChannelValueRange(channel).max; }

  virtual const const char *getChannelUnit(int32_t channel) const { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].getUnit(); }

  virtual bool isChannelDiscrete(int32_t channel) const { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].isDiscrete(); }
  virtual bool isChannelRenderable(int32_t channel) const { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].isRenderable(); }
  virtual bool isChannelAllowingLossyCompression(int32_t channel) const { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].isAllowLossyCompression(); }
  virtual bool isChannelUseZipForLosslessCompression(int32_t channel) const { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].isUseZipForLosslessCompression(); }

  // REMOVE ?
  virtual VolumeDataMapping getChannelMapping(int32_t channel) const;

// These convenience functions provide access to the individual elements of the axis descriptors
  virtual int getDimensionNumSamples(int32_t dimension) const;

  virtual const char *getDimensionName(int32_t dimension) const;

  virtual const char *getDimensionUnit(int32_t dimension) const;

  virtual float getDimensionMin(int32_t dimension) const { return getDimensionRange(dimension).min; }
  virtual float getDimensionMax(int32_t dimension) const { return getDimensionRange(dimension).max; }

  virtual bool  isChannelUseNoValue(int32_t channel) const { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].isUseNoValue(); }

  virtual float getChannelNoValue(int32_t channel) const { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].getNoValue(); }

  virtual float getChannelIntegerScale(int32_t channel) const  { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].getIntegerScale(); }
  virtual float getChannelIntegerOffset(int32_t channel) const { assert(channel >= 0 && channel < getChannelCount()); return m_volumeDataChannelDescriptor[channel].getIntegerOffset(); }

// Mutators
  void setContentsHash(VolumeDataHash const &contentsHash);
  void setActualValueRange(int32_t actualValueRangeChannel, Range<float> const &actualValueRange); // This should probably be implemented with VDSMetadata

  void createRenderLayers(DimensionGroup dimensions, int32_t brickSize, int32_t physicalLodLevels); //-> TODO: rename at some point

  bool isDimensionLodDecimated(int32_t dimension) const { return dimension != m_fullResolutionDimension; }
  int32_t getFullResolutionDimension() const { return m_fullResolutionDimension; }
};
}
#endif //VOLUMEDATALAYOUT_H
