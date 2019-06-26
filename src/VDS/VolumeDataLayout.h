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
  std::string m_dimensionName[Dimensionality_Max];
  std::string m_dimensionUnit[Dimensionality_Max];
  Range<float> m_dimensionRange[Dimensionality_Max];
  int32_t m_fullResolutionDimension;

public:
  VolumeDataLayout(const VolumeDataLayoutDescriptor &layoutDescriptor,
                   const std::vector<VolumeDataAxisDescriptor> &axisDescriptor,
                   const std::vector<VolumeDataChannelDescriptor> const &volumeDataChannelDescriptor,
                   int32_t actualValueRangeChannel, 
                   Range<float> const &actualValueRange, 
                   VolumeDataHash const &volumeDataHash, 
                   CompressionMethod compressionMethod, 
                   float compressionTolerance, 
                   bool isZipLosslessChannels, 
                   int32_t waveletAdaptiveLoadLevel);

  uint64_t GetContentsHash() const { return m_contentsHash; }
  VolumeDataLayer::VolumeDataLayerID AddDataLayer(VolumeDataLayer *layer);

  Range<float> const &GetChannelValueRange(int32_t iChannel) const { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].GetValueRange(); }

  Range<float> const &GetChannelActualValueRange(int32_t iChannel) const;

  //TODO SHOULD THIS BE REMOVED
  int32_t           GetMappedValueCount(int32_t iChannel) const;

  Range<float> const &GetDimensionRange(int32_t iDimension) const;

  VolumeDataLayer *GetVolumeDataLayerFromID(VolumeDataLayer::VolumeDataLayerID volumeDataLayerID) const; 
  VolumeDataLayer *GetVolumeDataLayerFromID(VolumeDataLayer::VolumeDataLayerID volumeDataLayerID) { return const_cast<VolumeDataLayer *>(const_cast<const VolumeDataLayout *>(this)->GetVolumeDataLayerFromID(volumeDataLayerID)); }

  VolumeDataLayer *GetTopLayer(DimensionGroup dimensionGroup, int32_t iChannel) const;
  VolumeDataLayer *GetTopLayer(DimensionGroup dimensionGroup, int32_t iChannel) { return const_cast<VolumeDataLayer *>(const_cast<const VolumeDataLayout *>(this)->GetTopLayer(dimensionGroup, iChannel)); }

  VolumeDataLayer *GetBaseLayer(DimensionGroup dimensionGroup, int32_t channel) const;
  VolumeDataLayer *GetBaseLayer(DimensionGroup dimensionGroup, int32_t iChannel) { return const_cast<VolumeDataLayer *>(const_cast<const VolumeDataLayout *>(this)->GetBaseLayer(dimensionGroup, iChannel)); }

  int32_t           GetBaseBrickSize() const { return m_baseBrickSize; }
  int32_t           GetLayoutDimensionNumSamples(int32_t iDimension) const { assert(iDimension >= 0 && iDimension < Dimensionality_Max); return m_dimensionNumSamples[iDimension]; }
  int32_t           GetLayerCount() const { return int32_t(m_volumeDataLayers.size()); }

  bool          IsReadOnly() const { return m_isReadOnly; }
  CompressionMethod GetCompressionMethod() const { return m_compressionMethod; }
  float GetCompressionTolerance() const { return m_compressionTolerance; }
  bool isZipLosslessChannels() const { return m_isZipLosslessChannels; }
  int32_t GetWaveletAdaptiveLoadLevel() const { return m_waveletAdaptiveLoadLevel; }

  const VolumeDataChannelDescriptor &GetVolumeDataChannelDescriptor(int32_t iChannel) const { return m_volumeDataChannelDescriptor[iChannel]; }

  //REMOVE VIRTUAL?
//  // Implementation of VolumeDataLayout interface
  virtual int32_t GetChannelCount() const { return int32_t(m_volumeDataChannelDescriptor.size()); }
  virtual bool IsChannelAvailable(const std::string &channelName) const;
  virtual int32_t GetChannelIndex(const std::string &channelName) const;
  virtual VolumeDataChannelDescriptor GetChannelDescriptor(int32_t iChannel) const;
  virtual int32_t GetDimensionality() const { return m_dimensionality; }

  virtual VolumeDataAxisDescriptor GetAxisDescriptor(int32_t iDimension) const;

//  // These convenience functions provide access to the individual elements of the value descriptor
  virtual VolumeDataChannelDescriptor::Format GetChannelFormat(int32_t iChannel) const { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].GetFormat(); }

  virtual VolumeDataChannelDescriptor::Components GetChannelComponents(int32_t iChannel) const { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].GetComponents(); }

  virtual const std::string &GetChannelName(int32_t iChannel) const { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].GetName(); }

  virtual float GetChannelValueRangeMin(int32_t iChannel) const { return GetChannelValueRange(iChannel).min; }
  virtual float GetChannelValueRangeMax(int32_t iChannel) const { return GetChannelValueRange(iChannel).max; }

  virtual const std::string &GetChannelUnit(int32_t iChannel) const { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].GetUnit(); }

  virtual bool  IsChannelDiscrete(int32_t iChannel) const { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].IsDiscrete(); }

  virtual bool  IsChannelRenderable(int32_t iChannel) const { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].IsRenderable(); }

  virtual bool  IsChannelAllowingLossyCompression(int32_t iChannel) const { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].IsAllowLossyCompression(); }

  virtual bool  IsChannelUseZipForLosslessCompression(int32_t iChannel) const { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].IsUseZipForLosslessCompression(); }

  // REMOVE ?
  virtual VolumeDataMapping GetChannelMapping(int32_t iChannel) const;

// These convenience functions provide access to the individual elements of the axis descriptors
  virtual int   GetDimensionNumSamples(int32_t iDimension) const;

  virtual const std::string &GetDimensionName(int32_t iDimension) const;

  virtual const std::string &GetDimensionUnit(int32_t iDimension) const;

  virtual float GetDimensionMin(int32_t iDimension) const { return GetDimensionRange(iDimension).min; }
  virtual float GetDimensionMax(int32_t iDimension) const { return GetDimensionRange(iDimension).max; }

  virtual bool  IsChannelUseNoValue(int32_t iChannel) const { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].IsUseNoValue(); }

  virtual float GetChannelNoValue(int32_t iChannel) const { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].GetNoValue(); }

  virtual float GetChannelIntegerScale(int32_t iChannel) const  { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].GetIntegerScale(); }
  virtual float GetChannelIntegerOffset(int32_t iChannel) const { assert(iChannel >= 0 && iChannel < GetChannelCount()); return m_volumeDataChannelDescriptor[iChannel].GetIntegerOffset(); }

// Mutators
  void          SetContentsHash(VolumeDataHash const &contentsHash);
  void          SetActualValueRange(int32_t actualValueRangeChannel, Range<float> const &actualValueRange); // This should probably be implemented with VDSMetadata

  void          CreateRenderLayers(DimensionGroup dimensions, int32_t nBrickSize, int32_t nPhysicalLODLevels); //-> TODO: rename at some point

  bool          IsDimensionLODDecimated(int32_t iDimension) const { return iDimension != m_fullResolutionDimension; }
  int32_t       GetFullResolutionDimension() const { return m_fullResolutionDimension; }
};
}
#endif //VOLUMEDATALAYOUT_H
