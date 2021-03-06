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

#ifndef VOLUMEDATAPARTITION
#define VOLUMEDATAPARTITION

#include <OpenVDS/openvds_export.h>
#include <OpenVDS/VolumeData.h>

#include "DimensionGroup.h"

#include <assert.h>
#include <cstddef>

namespace OpenVDS
{
class VolumeDataChannelMapping;

template<typename T, size_t N>
constexpr int ArraySize(const T (&)[N])
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

  int32_t GetChunkDimensionality() const;

  int32_t GetChunkDimension(int chunkDimension) const;

  int32_t VoxelToIndex(int32_t voxel, int32_t dimension) const;

  void ChunkIndexToIndexArray(int64_t chunkIndex, IndexArray &indexArray) const;
  int64_t IndexArrayToChunkIndex(const IndexArray &indexArray) const;

  int32_t GetLOD() const { return m_lod; }

  BorderMode GetBorderMode() const { return m_borderMode; }

  bool IsDimensionChunked(int32_t dimension) const { return DimensionGroupUtil::IsDimensionInGroup(m_chunkDimensionGroup, dimension); }
  bool IsDimensionLODDecimated(int32_t dimension) const { return DimensionGroupUtil::IsDimensionInGroup(m_originalDimensionGroup, dimension) && dimension != m_fullResolutionDimension; }
  int32_t GetNumChunksInDimension(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_chunksInDimension[dimension]; }
  int64_t GetTotalChunkCount() const { return m_totalChunks; }

  int64_t GetParentIndex(int64_t chunk, VolumeDataPartition const &parentPartition, int32_t *child = nullptr) const;
  void    GetChildIndices(int64_t chunk, VolumeDataPartition const &childPartition, int64_t *childIndices) const;

  int64_t GetTotalVoxels(bool isIncludeMargins) const;

  int32_t GetDimensionFirstSample(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_dimensionFirstSample[dimension]; }

  int32_t GetDimensionNumSamples(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_dimensionNumSamples[dimension]; }

  int32_t GetBrickSize(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_brickSize[dimension]; }

  int32_t GetNegativeMargin(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_negativeMargin[dimension]; }

  int32_t GetPositiveMargin(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_positiveMargin[dimension]; }

  int32_t GetNegativeBorder(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_negativeBorder[dimension]; }

  int32_t GetPositiveBorder(int32_t dimension) const { assert(dimension >= 0 && dimension < Dimensionality_Max); return m_positiveBorder[dimension]; }

  int32_t GetNegativeRenderMargin() const { return m_negativeRenderMargin; }
  int32_t GetPositiveRenderMargin() const { return m_positiveRenderMargin; }

  DimensionGroup GetOriginalDimensionGroup() const { return m_originalDimensionGroup; } // This function returns the dimensiongroup of the layer before the margins were added (NOTE: not the render margins)

  DimensionGroup GetChunkDimensionGroup() const { return m_chunkDimensionGroup; }

  void GetChunkMinMax(int64_t chunk, int32_t *min, int32_t *max, bool isIncludeMargin) const;

  void GetChunkVoxelSize(int64_t chunk, ChunkVoxelSize &size) const;

  static VolumeDataPartition StaticMapPartition(VolumeDataPartition const &primaryPartition, const VolumeDataChannelMapping *volumeDataChannelMapping, int32_t mappedValues);

  static VolumeDataPartition StaticFindSuperPartition(VolumeDataPartition const &partitionA, VolumeDataPartition const &partitionB);
};
}
#endif //VOLUMEDATAPARTITION
