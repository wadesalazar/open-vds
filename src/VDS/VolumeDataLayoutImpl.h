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

#ifndef VOLUMEDATALAYOUTIMPL_H
#define VOLUMEDATALAYOUTIMPL_H

#include <OpenVDS/VolumeDataAxisDescriptor.h>
#include <OpenVDS/VolumeDataLayout.h>

#include "VolumeDataLayer.h"
#include "VolumeDataHash.h"

#include <vector>
#include <mutex>

namespace OpenVDS
{
struct VDSHandle;
class VolumeDataLayoutDescriptor;
class VolumeDataLayoutImpl : public VolumeDataLayout
{
private:
  VDSHandle     &m_handle;
  std::vector<VolumeDataLayer *> m_volumeDataLayers;
  std::vector<VolumeDataChannelDescriptor> m_volumeDataChannelDescriptor;
  bool           m_isReadOnly;
  VolumeDataHash m_contentsHash; 
  int32_t        m_dimensionality;
  int32_t        m_baseBrickSize;
  int32_t        m_negativeRenderMargin;
  int32_t        m_positiveRenderMargin;
  int32_t        m_brickSize2DMultiplier;
  int32_t        m_maxLOD;
  bool           m_isCreate2DLODs;

  int32_t        m_pendingWriteRequests;
  int32_t        m_actualValueRangeChannel;
  FloatRange     m_actualValueRange;

  VolumeDataLayer *m_primaryBaseLayers[DimensionGroup_3D_Max];
  VolumeDataLayer *m_primaryTopLayers[DimensionGroup_3D_Max];

  CompressionMethod m_compressionMethod;
  float m_compressionTolerance;
  bool  m_isZipLosslessChannels;
  int32_t m_waveletAdaptiveLoadLevel;
  IndexArray m_dimensionNumSamples;
  const char *m_dimensionName[Dimensionality_Max];
  const char *m_dimensionUnit[Dimensionality_Max];
  FloatRange m_dimensionRange[Dimensionality_Max];
  int32_t m_fullResolutionDimension;

public:
  VolumeDataLayoutImpl(VDSHandle &handle,
                   const VolumeDataLayoutDescriptor &layoutDescriptor,
                   const std::vector<VolumeDataAxisDescriptor> &axisDescriptor,
                   const std::vector<VolumeDataChannelDescriptor> &volumeDataChannelDescriptor,
                   int32_t actualValueRangeChannel, 
                   FloatRange const &actualValueRange, 
                   VolumeDataHash const &volumeDataHash, 
                   CompressionMethod compressionMethod, 
                   float compressionTolerance, 
                   bool isZipLosslessChannels, 
                   int32_t waveletAdaptiveLoadLevel);

  ~VolumeDataLayoutImpl() override;

  VDSHandle       &getHandle() { return m_handle; }

  uint64_t         GetContentsHash() const override { return uint64_t(m_contentsHash); }
  VolumeDataLayer::VolumeDataLayerID addDataLayer(VolumeDataLayer *layer);

  FloatRange const &getChannelValueRange(int32_t channel) const { assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].GetValueRange(); }

  FloatRange const &getChannelActualValueRange(int32_t channel) const;

  const VolumeDataChannelMapping *
                   getVolumeDataChannelMapping(int32_t channel) const;

  int32_t          getChannelMappedValueCount(int32_t channel) const;

  FloatRange const &getDimensionRange(int32_t dimension) const;

  VolumeDataLayer *getVolumeDataLayerFromID(VolumeDataLayer::VolumeDataLayerID volumeDataLayerID) const; 
  VolumeDataLayer *getVolumeDataLayerFromID(VolumeDataLayer::VolumeDataLayerID volumeDataLayerID) { return const_cast<VolumeDataLayer *>(const_cast<const VolumeDataLayoutImpl *>(this)->getVolumeDataLayerFromID(volumeDataLayerID)); }

  VolumeDataLayer *getTopLayer(DimensionGroup dimensionGroup, int32_t channel) const;
  VolumeDataLayer *getTopLayer(DimensionGroup dimensionGroup, int32_t channel) { return const_cast<VolumeDataLayer *>(const_cast<const VolumeDataLayoutImpl *>(this)->getTopLayer(dimensionGroup, channel)); }

  VolumeDataLayer *getBaseLayer(DimensionGroup dimensionGroup, int32_t channel) const;
  VolumeDataLayer *getBaseLayer(DimensionGroup dimensionGroup, int32_t channel) { return const_cast<VolumeDataLayer *>(const_cast<const VolumeDataLayoutImpl *>(this)->getBaseLayer(dimensionGroup, channel)); }

  int32_t getBaseBrickSize() const { return m_baseBrickSize; }
  int32_t getLayoutDimensionNumSamples(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_dimensionNumSamples[dimension]; }
  int32_t getLayerCount() const { return int32_t(m_volumeDataLayers.size()); }

  bool isReadOnly() const { return m_isReadOnly; }
  CompressionMethod getCompressionMethod() const { return m_compressionMethod; }
  float getCompressionTolerance() const { return m_compressionTolerance; }
  bool isZipLosslessChannels() const { return m_isZipLosslessChannels; }
  int32_t getWaveletAdaptiveLoadLevel() const { return m_waveletAdaptiveLoadLevel; }

  const VolumeDataChannelDescriptor &getVolumeDataChannelDescriptor(int32_t channel) const { return m_volumeDataChannelDescriptor[channel]; }

  int32_t changePendingWriteRequestCount(int32_t nDifference);
  void completePendingWriteChunkRequests(int32_t nMaxPendingWriteChunkRequests) const;

  VolumeDataLayoutDescriptor getLayoutDescriptor() const;

  int32_t GetChannelCount() const override { return int32_t(m_volumeDataChannelDescriptor.size()); }
  bool IsChannelAvailable(const char *channelName) const override;
  int32_t GetChannelIndex(const char *channelName) const override;
  VolumeDataChannelDescriptor GetChannelDescriptor(int32_t channel) const override;
  int32_t GetDimensionality() const override { return m_dimensionality; }

  VolumeDataAxisDescriptor GetAxisDescriptor(int32_t dimension) const override;

//  // These convenience functions provide access to the individual elements of the value descriptor
  VolumeDataChannelDescriptor::Format GetChannelFormat(int32_t channel) const override { assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].GetFormat(); }

  VolumeDataChannelDescriptor::Components GetChannelComponents(int32_t channel) const override { assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].GetComponents(); }

  const char *GetChannelName(int32_t channel) const override { assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].GetName(); }

  float GetChannelValueRangeMin(int32_t channel) const override{ return getChannelValueRange(channel).Min; }
  float GetChannelValueRangeMax(int32_t channel) const override{ return getChannelValueRange(channel).Max; }

  const char *GetChannelUnit(int32_t channel) const override { assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].GetUnit(); }

  bool IsChannelDiscrete(int32_t channel) const override{ assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].IsDiscrete(); }
  bool IsChannelRenderable(int32_t channel) const override { assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].IsRenderable(); }
  bool IsChannelAllowingLossyCompression(int32_t channel) const override { assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].IsAllowLossyCompression(); }
  bool IsChannelUseZipForLosslessCompression(int32_t channel) const override { assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].IsUseZipForLosslessCompression(); }

  VolumeDataMapping GetChannelMapping(int32_t channel) const override;

// These convenience functions provide access to the individual elements of the axis descriptors
  int GetDimensionNumSamples(int32_t dimension) const override;

  const char *GetDimensionName(int32_t dimension) const override;

  const char *GetDimensionUnit(int32_t dimension) const override;

  float GetDimensionMin(int32_t dimension) const override { return getDimensionRange(dimension).Min; }
  float GetDimensionMax(int32_t dimension) const override { return getDimensionRange(dimension).Max; }

  bool  IsChannelUseNoValue(int32_t channel) const override { assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].IsUseNoValue(); }

  float GetChannelNoValue(int32_t channel) const override { assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].GetNoValue(); }

  float GetChannelIntegerScale(int32_t channel) const  override { assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].GetIntegerScale(); }
  float GetChannelIntegerOffset(int32_t channel) const override { assert(channel >= 0 && channel < GetChannelCount()); return m_volumeDataChannelDescriptor[channel].GetIntegerOffset(); }

// Mutators
  void setContentsHash(VolumeDataHash const &contentsHash);
  void setActualValueRange(int32_t actualValueRangeChannel, FloatRange const &actualValueRange); // This should probably be implemented with VDSMetadata

  void createLayers(DimensionGroup dimensionGroup, int32_t brickSize, int32_t physicalLODLevels, VolumeDataLayer::ProduceStatus produceStatus);

  bool isDimensionLODDecimated(int32_t dimension) const { return dimension != m_fullResolutionDimension; }
  int32_t getFullResolutionDimension() const { return m_fullResolutionDimension; }

  bool        IsMetadataIntAvailable(const char* category, const char* name) const override;
  bool        IsMetadataIntVector2Available(const char* category, const char* name) const override;
  bool        IsMetadataIntVector3Available(const char* category, const char* name) const override;
  bool        IsMetadataIntVector4Available(const char* category, const char* name) const override;
  bool        IsMetadataFloatAvailable(const char* category, const char* name) const override;
  bool        IsMetadataFloatVector2Available(const char* category, const char* name) const override;
  bool        IsMetadataFloatVector3Available(const char* category, const char* name) const override;
  bool        IsMetadataFloatVector4Available(const char* category, const char* name) const override;
  bool        IsMetadataDoubleAvailable(const char* category, const char* name) const override;
  bool        IsMetadataDoubleVector2Available(const char* category, const char* name) const override;
  bool        IsMetadataDoubleVector3Available(const char* category, const char* name) const override;
  bool        IsMetadataDoubleVector4Available(const char* category, const char* name) const override;
  bool        IsMetadataStringAvailable(const char* category, const char* name) const override;
  bool        IsMetadataBLOBAvailable(const char* category, const char* name) const override;

  int         GetMetadataInt(const char* category, const char* name) const override;
  IntVector2  GetMetadataIntVector2(const char* category, const char* name) const override;
  IntVector3  GetMetadataIntVector3(const char* category, const char* name) const override;
  IntVector4  GetMetadataIntVector4(const char* category, const char* name) const override;
  float        GetMetadataFloat(const char* category, const char* name) const override;
  FloatVector2 GetMetadataFloatVector2(const char* category, const char* name) const override;
  FloatVector3 GetMetadataFloatVector3(const char* category, const char* name) const override;
  FloatVector4 GetMetadataFloatVector4(const char* category, const char* name) const override;
  double        GetMetadataDouble(const char* category, const char* name) const override;
  DoubleVector2 GetMetadataDoubleVector2(const char* category, const char* name) const override;
  DoubleVector3 GetMetadataDoubleVector3(const char* category, const char* name) const override;
  DoubleVector4 GetMetadataDoubleVector4(const char* category, const char* name) const override;
  const char* GetMetadataString(const char* category, const char* name) const override;
  void        GetMetadataBLOB(const char* category, const char* name, const void **data, size_t *size)  const override;
};
}
#endif //VOLUMEDATALAYOUTIMPL_H
