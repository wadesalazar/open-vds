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

#include <OpenVDS/openvds_export.h>
#include <OpenVDS/VolumeData.h>
#include "DimensionGroup.h"
#include "VolumeDataChannelMapping.h"

#include <assert.h>
#include <cstddef>

namespace OpenVDS
{

template<typename T, size_t N>
constexpr size_t array_size(const T (&)[N])
{
  return N;
}

/// The border mode describes how the voxels outside the input dataset are filled in when using indexing with window
enum class BorderMode
{
  None,   ///< The values in the border cannot be accessed. The reader must check for the border and handle it itself.
  Repeat, ///< The values in the border repeat the last valid value of the dataset
  Mirror, ///< The values in the border are a mirrored copy of the valid values inside the border
  Clear   ///< The values in the border are initialized to zero
};

class VolumeDataPartition
{
public:
  using ChunkVoxelSize = int32_t[4];
private:
  friend class VolumeDataRegion;

  DimensionGroup    m_originalDimensionGroup;

  int32_t           m_lod;

  int32_t           m_fullResolutionDimension;

  IndexArray        m_brickSize;

  IndexArray        m_dimensionFirstSample;
  IndexArray        m_dimensionNumSamples;

  IndexArray        m_negativeMargin;
  IndexArray        m_positiveMargin;

  BorderMode        m_borderMode;

  IndexArray        m_negativeBorder;
  IndexArray        m_positiveBorder;

  int32_t           m_negativeRenderMargin;
  int32_t           m_positiveRenderMargin;

  // Derived member variables
  DimensionGroup    m_chunkDimensionGroup;

  IndexArray        m_chunksInDimension;

  //TODO rename pitch
  uint64_t          m_modulo[Dimensionality_Max];

  int64_t           m_totalChunks;

public:
  VolumeDataPartition(int32_t lod, DimensionGroup dimensionGroup, const IndexArray &firstSample, const IndexArray &numSamples, const IndexArray &brickSize, const IndexArray &negativeMargin, const IndexArray &positiveMargin, BorderMode borderMode, const IndexArray &negativeBorder, const IndexArray &positiveBorder, int32_t negativeRenderMargin, int32_t positiveRenderMargin, int32_t fullResolutionDimension);
  ~VolumeDataPartition()
  {
  }

  bool operator==(const VolumeDataPartition &volumeDataPartion) const;

  int32_t getChunkDimensionality() const;

  int getChunkDimension(int chunkDimension) const;

  int32_t voxelToIndex(int32_t voxel, int32_t dimension) const;

  void chunkIndexToIndexArray(int64_t chunkIndex, IndexArray &indexArray) const;
  int64_t indexArrayToChunkIndex(const IndexArray &indexArray) const;

  int32_t getLod() const { return m_lod; }

  BorderMode getBorderMode() const { return m_borderMode; }

  bool isDimensionChunked(int32_t dimension) const { return DimensionGroupUtil::isDimensionInGroup(m_chunkDimensionGroup, dimension); }
  bool isDimensionLodDecimated(int32_t dimension) const { return DimensionGroupUtil::isDimensionInGroup(m_originalDimensionGroup, dimension) && dimension != m_fullResolutionDimension; }
  int32_t getNumChunksInDimension(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_chunksInDimension[dimension]; }
  int64_t getTotalChunkCount() const { return m_totalChunks; }

  int64_t getParentIndex(int64_t chunk, VolumeDataPartition const &parentPartition, int32_t *child = NULL) const;
  void    getChildIndices(int64_t chunk, VolumeDataPartition const &childPartition, int64_t *childIndices) const;

  int64_t getTotalVoxels(bool isIncludeMargins) const;

  int32_t getDimensionFirstSample(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_dimensionFirstSample[dimension]; }

  int32_t getDimensionNumSamples(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_dimensionNumSamples[dimension]; }

  int32_t getBrickSize(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_brickSize[dimension]; }

  int32_t getNegativeMargin(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_negativeMargin[dimension]; }

  int32_t getPositiveMargin(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_positiveMargin[dimension]; }

  int32_t getNegativeBorder(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_negativeBorder[dimension]; }

  int32_t getPositiveBorder(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_positiveBorder[dimension]; }

  int32_t getNegativeRenderMargin() const { return m_negativeRenderMargin; }
  int32_t getPositiveRenderMargin() const { return m_positiveRenderMargin; }

  DimensionGroup getOriginalDimensionGroup() const { return m_originalDimensionGroup; } // This function returns the dimensiongroup of the layer before the margins were added (NOTE: not the render margins)

  DimensionGroup getChunkDimensionGroup() const { return m_chunkDimensionGroup; }

  void getChunkMinMax(int64_t chunk, int32_t *min, int32_t *max, bool isIncludeMargin) const;

  void getChunkVoxelSize(int64_t chunk, ChunkVoxelSize &size) const;

  static VolumeDataPartition staticMapPartition(VolumeDataPartition const &primaryPartition, const VolumeDataChannelMapping *volumeDataChannelMapping, int32_t mappedValues);

  static VolumeDataPartition staticFindSuperPartition(VolumeDataPartition const &partitionA, VolumeDataPartition const &partitionB);
};
}
