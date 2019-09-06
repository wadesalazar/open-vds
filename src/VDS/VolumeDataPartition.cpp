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

#include "VolumeDataPartition.h"
#include "VolumeDataChannelMapping.h"

#include <algorithm>
#include <cstring>


namespace OpenVDS
{

template<typename T>
const T &clamp(const T &v, const T &low, const T &high)
{
    return std::min(std::max(low, v), high);
}

VolumeDataPartition::VolumeDataPartition(int32_t lod, DimensionGroup dimensionGroup, const IndexArray& firstSample, const IndexArray& numSamples, const IndexArray& brickSize, const IndexArray& negativeMargin, const IndexArray& positiveMargin, BorderMode borderMode, const IndexArray& negativeBorder, const IndexArray& positiveBorder, int32_t negativeRenderMargin, int32_t positiveRenderMargin, int32_t fullResolutionDimension)
  : m_originalDimensionGroup(dimensionGroup)
  , m_lod(lod)
  , m_fullResolutionDimension(fullResolutionDimension)
  , m_borderMode(borderMode)
  , m_negativeRenderMargin(negativeRenderMargin)
  , m_positiveRenderMargin(positiveRenderMargin)
{
  memcpy(m_dimensionFirstSample, firstSample, sizeof(m_dimensionFirstSample));
  memcpy(m_dimensionNumSamples, numSamples, sizeof(m_dimensionNumSamples));
  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    m_brickSize[iDimension] = std::min(brickSize[iDimension], m_dimensionNumSamples[iDimension]);
  }
  memcpy(m_negativeMargin, negativeMargin, sizeof(m_negativeMargin));
  memcpy(m_positiveMargin, positiveMargin, sizeof(m_positiveMargin));
  memcpy(m_negativeBorder, negativeBorder, sizeof(m_negativeBorder));
  memcpy(m_positiveBorder, positiveBorder, sizeof(m_positiveBorder));

  int64_t modulo = 1;

  int32_t chunkedDimensions[Dimensionality_Max];

  int32_t nChunkedDimensions = 0;

  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    int32_t nChunksInDimension;

    if(DimensionGroupUtil::isDimensionInGroup(m_originalDimensionGroup, iDimension))
    {
      chunkedDimensions[nChunkedDimensions++] = iDimension;
      nChunksInDimension = (m_dimensionNumSamples[iDimension] - 1 - m_negativeRenderMargin - m_positiveRenderMargin) / m_brickSize[iDimension] + 1;
    }
    else
    {
      if(m_positiveMargin[iDimension] != 0 || m_negativeMargin[iDimension] != 0)
      {
        chunkedDimensions[nChunkedDimensions++] = iDimension;
      }
      nChunksInDimension = m_dimensionNumSamples[iDimension];
    }

    if (nChunksInDimension < 1)
    {
      nChunksInDimension = 1;
    }

    m_chunksInDimension[iDimension] = nChunksInDimension;
    m_modulo[iDimension] = modulo;
    modulo *= nChunksInDimension;
  }

  m_totalChunks = modulo;

  // Fill in the rest of the chunked dimensions array
  while(nChunkedDimensions < Dimensionality_Max)
  {
    chunkedDimensions[nChunkedDimensions++] = -1;
  }

  m_chunkDimensionGroup = DimensionGroupUtil::getDimensionGroupFromDimensionIndices(chunkedDimensions[0],
                                                                                    chunkedDimensions[1],
                                                                                    chunkedDimensions[2],
                                                                                    chunkedDimensions[3],
                                                                                    chunkedDimensions[4],
                                                                                    chunkedDimensions[5]);
}

bool
VolumeDataPartition::operator==(VolumeDataPartition const &volumeDataPartion) const
{
  return m_originalDimensionGroup == volumeDataPartion.m_originalDimensionGroup &&
         m_lod == volumeDataPartion.m_lod &&
         memcmp(m_brickSize, volumeDataPartion.m_brickSize, sizeof(m_brickSize)) == 0 &&
         memcmp(m_dimensionFirstSample, volumeDataPartion.m_dimensionFirstSample, sizeof(m_dimensionFirstSample)) == 0 &&
         memcmp(m_dimensionNumSamples, volumeDataPartion.m_dimensionNumSamples, sizeof(m_dimensionNumSamples)) == 0 &&
         memcmp(m_negativeMargin, volumeDataPartion.m_negativeMargin, sizeof(m_negativeMargin)) == 0 &&
         memcmp(m_positiveMargin, volumeDataPartion.m_positiveMargin, sizeof(m_positiveMargin)) == 0 &&
         m_borderMode == volumeDataPartion.m_borderMode &&
         (m_borderMode == BorderMode::None || memcmp(m_negativeBorder, volumeDataPartion.m_negativeBorder, sizeof(m_negativeBorder)) == 0) &&
         (m_borderMode == BorderMode::None || memcmp(m_positiveBorder, volumeDataPartion.m_positiveBorder, sizeof(m_positiveBorder)) == 0) &&
         m_negativeRenderMargin == volumeDataPartion.m_negativeRenderMargin &&
         m_positiveRenderMargin == volumeDataPartion.m_positiveRenderMargin &&
         m_fullResolutionDimension == volumeDataPartion.m_fullResolutionDimension;
}

int32_t VolumeDataPartition::getChunkDimensionality() const
{
  return DimensionGroupUtil::getDimensionality(getChunkDimensionGroup());
}

int32_t VolumeDataPartition::getChunkDimension(int chunkDimension) const
{
  return DimensionGroupUtil::getDimension(getChunkDimensionGroup(), chunkDimension);
}

//////////////////////////////////////////////////////////////////////////////
// StaticMapPartition

VolumeDataPartition
VolumeDataPartition::staticMapPartition(VolumeDataPartition const &primaryPartition, const VolumeDataChannelMapping *volumeDataChannelMapping, int32_t mappedValues)
{
  if(!volumeDataChannelMapping)
  {
    return primaryPartition;
  }

  DimensionGroup eMappedDimensionGroup = volumeDataChannelMapping->getMappedChunkDimensionGroup(primaryPartition, mappedValues);

  int32_t anMappedDimensionFirstSample[Dimensionality_Max];
  int32_t anMappedDimensionNumSamples[Dimensionality_Max];
  int32_t anMappedBrickSize[Dimensionality_Max];
  int32_t anMappedNegativeMargin[Dimensionality_Max];
  int32_t anMappedPositiveMargin[Dimensionality_Max];
  int32_t anMappedNegativeBorder[Dimensionality_Max];
  int32_t anMappedPositiveBorder[Dimensionality_Max];

  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    anMappedDimensionFirstSample[iDimension] = volumeDataChannelMapping->getMappedDimensionFirstSample(iDimension, primaryPartition.m_dimensionFirstSample[iDimension], mappedValues);
    anMappedDimensionNumSamples[iDimension] = volumeDataChannelMapping->getMappedDimensionNumSamples(iDimension, primaryPartition.m_dimensionNumSamples[iDimension], mappedValues);
    anMappedBrickSize[iDimension] = volumeDataChannelMapping->getMappedBrickSize(primaryPartition, iDimension, mappedValues);
    anMappedNegativeMargin[iDimension] = volumeDataChannelMapping->getMappedNegativeMargin(primaryPartition, iDimension);
    anMappedPositiveMargin[iDimension] = volumeDataChannelMapping->getMappedPositiveMargin(primaryPartition, iDimension);
    anMappedNegativeBorder[iDimension] = volumeDataChannelMapping->getMappedNegativeBorder(primaryPartition, iDimension);
    anMappedPositiveBorder[iDimension] = volumeDataChannelMapping->getMappedPositiveBorder(primaryPartition, iDimension);
  }

  return VolumeDataPartition(primaryPartition.m_lod, eMappedDimensionGroup, anMappedDimensionFirstSample, anMappedDimensionNumSamples, anMappedBrickSize, anMappedNegativeMargin, anMappedPositiveMargin, primaryPartition.m_borderMode, anMappedNegativeBorder, anMappedPositiveBorder, primaryPartition.m_negativeRenderMargin, primaryPartition.m_positiveRenderMargin, primaryPartition.m_fullResolutionDimension);
}

//////////////////////////////////////////////////////////////////////////////
// StaticFindSuperPartition

VolumeDataPartition
VolumeDataPartition::staticFindSuperPartition(VolumeDataPartition const &partitionA, VolumeDataPartition const &partitionB)
{
  int32_t anSuperBrickSize[Dimensionality_Max];

  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    assert(partitionA.m_dimensionFirstSample[iDimension] == partitionB.m_dimensionFirstSample[iDimension]);
    assert(partitionA.m_dimensionNumSamples[iDimension] == partitionB.m_dimensionNumSamples[iDimension]);

    anSuperBrickSize[iDimension] = std::max(partitionA.m_brickSize[iDimension], partitionB.m_brickSize[iDimension]);
  }

  DimensionGroup dimensionGroup = DimensionGroupUtil::unionGroups(partitionA.m_originalDimensionGroup, partitionB.m_originalDimensionGroup);

  return VolumeDataPartition(partitionA.m_lod, dimensionGroup, partitionA.m_dimensionFirstSample, partitionA.m_dimensionNumSamples, anSuperBrickSize, partitionA.m_negativeMargin, partitionA.m_positiveMargin, partitionA.m_borderMode, partitionA.m_negativeBorder, partitionA.m_positiveBorder, partitionA.m_negativeRenderMargin, partitionA.m_positiveRenderMargin, partitionA.m_fullResolutionDimension);
}

//////////////////////////////////////////////////////////////////////////////
// ChunkIndexToIndexArray

void
VolumeDataPartition::chunkIndexToIndexArray(int64_t chunkIndex, IndexArray &indexArray) const
{
  for(int32_t iDimension = Dimensionality_Max - 1; iDimension >= 0; iDimension--)
  {
    indexArray[iDimension] = (int32_t)(chunkIndex / m_modulo[iDimension]);
    chunkIndex %= m_modulo[iDimension];
  }
}

//////////////////////////////////////////////////////////////////////////////
// IndexArrayToChunkIndex

int64_t VolumeDataPartition::indexArrayToChunkIndex(const IndexArray &indexArray) const
{
  int64_t iChunkIndex = 0;
  for(int32_t iDimension = Dimensionality_Max - 1; iDimension >= 0; iDimension--)
  {
    iChunkIndex += indexArray[iDimension] * m_modulo[iDimension];
  }
  return iChunkIndex;
}

//////////////////////////////////////////////////////////////////////////////
// GetParentIndex

int64_t VolumeDataPartition::getParentIndex(int64_t chunk, VolumeDataPartition const &parentPartition, int32_t *child) const
{
  int32_t
    iChild = 0;

  int64_t iChunkedDimension = 0;

  int32_t
    aiIndexArray[Dimensionality_Max];

  chunkIndexToIndexArray(chunk, aiIndexArray);
  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    if(isDimensionChunked(iDimension))
    {
      iChild |= (aiIndexArray[iDimension] & 1) << (iChunkedDimension++);
      aiIndexArray[iDimension] = aiIndexArray[iDimension] / (iDimension != m_fullResolutionDimension ? 2 : 1);
    }
  }
  assert(iChunkedDimension <= 3);
  if(child) *child = iChild;

  return parentPartition.indexArrayToChunkIndex(aiIndexArray);
}

//////////////////////////////////////////////////////////////////////////////
// GetChildIndices

void
VolumeDataPartition::getChildIndices(int64_t chunk, VolumeDataPartition const &childPartition, int64_t *childIndices) const
{
  for(int32_t iChild = 0; iChild < 8; iChild++)
  {
    childIndices[iChild] = -1;
  }

  int32_t
    aiIndexArray[Dimensionality_Max];

  int32_t
    aiChunkedIndex[3];

  int32_t
    aiChunkedDimensions[3];

  int32_t
    nChunkedDimensions = 0;

  chunkIndexToIndexArray(chunk, aiIndexArray);
  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    if(isDimensionChunked(iDimension))
    {
      aiChunkedIndex[nChunkedDimensions] = aiIndexArray[iDimension] * (iDimension != m_fullResolutionDimension ? 2 : 1);
      aiChunkedDimensions[nChunkedDimensions++] = iDimension;
    }
  }
  assert(nChunkedDimensions <= 3);

  for(int32_t iChild = 0; iChild < (1 << nChunkedDimensions); iChild++)
  {
    bool isValid = true;
    for(int32_t iChunkedDimension = 0; iChunkedDimension < nChunkedDimensions; iChunkedDimension++)
    {
      int32_t iDimension = aiChunkedDimensions[iChunkedDimension];

      int32_t
        iPos;

      if (m_fullResolutionDimension == iDimension)
      {
        if (iChild & (1 << iChunkedDimension))
        {
          // avoid duplicates
          isValid = false;
          break;
        }

        iPos = aiChunkedIndex[iChunkedDimension];
      }
      else
      {
        iPos = aiChunkedIndex[iChunkedDimension] + (iChild & (1 << iChunkedDimension) ? 1 : 0);
      }

      if(iPos >= childPartition.m_chunksInDimension[iDimension])
      {
        isValid = false;
        break;
      }
      aiIndexArray[iDimension] = iPos;
    }

    if(isValid)
    {
      childIndices[iChild] = childPartition.indexArrayToChunkIndex(aiIndexArray);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// GetChunkMinMax

void
VolumeDataPartition::getChunkMinMax(int64_t chunk, int32_t *min, int32_t *max, bool isIncludeMargin) const
{
  for(int32_t iDimension = Dimensionality_Max - 1; iDimension >= 0; iDimension--)
  {
    assert((DimensionGroupUtil::isDimensionInGroup(m_originalDimensionGroup, iDimension) || m_negativeBorder[iDimension] <= m_negativeMargin[iDimension]) && "We can't have layers with border and no margin in dimensions that are not chunked");
    assert((DimensionGroupUtil::isDimensionInGroup(m_originalDimensionGroup, iDimension) || m_positiveBorder[iDimension] <= m_positiveMargin[iDimension]) && "We can't have layers with border and no margin in dimensions that are not chunked");

    int32_t iChunkInDimension = (int32_t)(chunk / m_modulo[iDimension]);

    int32_t nMin;
    int32_t nMax;

    if(DimensionGroupUtil::isDimensionInGroup(m_originalDimensionGroup, iDimension))
    {
      nMin = iChunkInDimension * m_brickSize[iDimension];
      nMax = (iChunkInDimension + 1) * m_brickSize[iDimension];

      nMin += m_negativeRenderMargin;
      nMax += m_negativeRenderMargin;

      if(isIncludeMargin || iChunkInDimension == 0)
      {
        nMin -= m_negativeRenderMargin;
      }

      if(isIncludeMargin || iChunkInDimension == m_chunksInDimension[iDimension] - 1)
      {
        nMax += m_positiveRenderMargin;
      }

      assert(nMin >= 0);
      nMax = std::min(nMax, m_dimensionNumSamples[iDimension]);
    }
    else
    {
      assert(m_brickSize[iDimension] == 1);

      nMin = (int32_t)(chunk / m_modulo[iDimension]);
      nMax = nMin + 1;
    }

    // Add the positive and negative margins that can go outside the dataset (but not outside the border area)
    if(isIncludeMargin)
    {
      if(iChunkInDimension == 0)
      {
        nMin -= m_negativeBorder[iDimension];
      }
      else
      {
        nMin = std::max(nMin - m_negativeMargin[iDimension], -m_negativeBorder[iDimension]);
      }

      if(iChunkInDimension == m_chunksInDimension[iDimension] - 1)
      {
        nMax += m_positiveBorder[iDimension];
      }
      else
      {
        nMax = std::max(nMax + m_positiveMargin[iDimension], m_dimensionNumSamples[iDimension] + m_positiveBorder[iDimension]);
      }
    }

    // Adjust for subset layers
    nMin += m_dimensionFirstSample[iDimension];
    nMax += m_dimensionFirstSample[iDimension];

    assert(nMax > nMin);
    min[iDimension] = nMin;
    max[iDimension] = nMax;

    chunk %= m_modulo[iDimension];
  }
}

void VolumeDataPartition::getChunkVoxelSize(int64_t chunk, ChunkVoxelSize &size) const
{
  assert(DimensionGroupUtil::getDimensionality(m_chunkDimensionGroup) <= array_size(size));

  int32_t
    anMin[Dimensionality_Max], anMax[Dimensionality_Max];

  getChunkMinMax(chunk, anMin, anMax, true);

  for(int32_t iDataBlockDimension = 0; iDataBlockDimension < array_size(size); iDataBlockDimension++)
  {
    int32_t
      iDimension = DimensionGroupUtil::getDimension(m_chunkDimensionGroup, iDataBlockDimension);

    if(iDimension == -1)
    {
      size[iDataBlockDimension] = 1;
    }
    else if (isDimensionLODDecimated(iDimension))
    {
      size[iDataBlockDimension] = (anMax[iDimension] - anMin[iDimension] + (1 << m_lod) - 1) >> m_lod;
    }
    else
    {
      size[iDataBlockDimension] = anMax[iDimension] - anMin[iDimension];
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// VoxelToIndex

int32_t VolumeDataPartition::voxelToIndex(int32_t voxel, int32_t dimension) const
{
  // Adjust for subset layers
  voxel -= m_dimensionFirstSample[dimension];

  if(DimensionGroupUtil::isDimensionInGroup(m_originalDimensionGroup, dimension))
  {
    voxel -= m_negativeRenderMargin;
  }
  int32_t iIndex = voxel / m_brickSize[dimension];
  return clamp(iIndex, 0, m_chunksInDimension[dimension] - 1);
}

int64_t VolumeDataPartition::getTotalVoxels(bool isIncludeMargins) const
{
  int64_t nTotalLayerSize = 1;

  int32_t lod = getLOD();

  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    int32_t nNumChunksInDimension = getNumChunksInDimension(iDimension);
    int32_t nDimensionNumSamples = getDimensionNumSamples(iDimension);
    if(DimensionGroupUtil::isDimensionInGroup(m_originalDimensionGroup, iDimension))
    {
      if(isIncludeMargins)
      {
        nDimensionNumSamples += (nNumChunksInDimension - 1) * (m_positiveRenderMargin + m_negativeRenderMargin);
      }

      if (iDimension != m_fullResolutionDimension)
      {
        nDimensionNumSamples = (nDimensionNumSamples + (1 << lod) - 1) >> lod;
      }
    }
    nTotalLayerSize = nTotalLayerSize * nDimensionNumSamples;
  }
  return nTotalLayerSize;
}
}