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

#include "VolumeDataRegion.h"

#include <OpenVDS/VolumeDataLayout.h>

#include "VolumeDataChunk.h"

#include <assert.h>
#include <algorithm>

namespace OpenVDS
{
int64_t VolumeDataRegion::GetNumChunksInRegion() const
{
  return m_chunksInRegion;
}

int64_t VolumeDataRegion::GetChunkIndexInRegion(int64_t chunkInRegion) const
{
  assert(chunkInRegion >= 0 && chunkInRegion < m_chunksInRegion);

  int64_t iChunkIndex = 0;

  for(int32_t iDimension = int32_t(ArraySize(m_chunkMin)) - 1; iDimension >= 0; iDimension--)
  {
    iChunkIndex += (chunkInRegion / m_modulo[iDimension] + m_chunkMin[iDimension]) * m_layerModulo[iDimension];
    chunkInRegion %= m_modulo[iDimension];
  }

  return iChunkIndex;
}

void VolumeDataRegion::GetChunksInRegion(std::vector<VolumeDataChunk>* volumeDataChunk, bool isAppend) const
{
  if (!isAppend)
  {
    volumeDataChunk->clear();
  }

  int32_t nChunksInRegion = (int32_t)GetNumChunksInRegion();
  if(!nChunksInRegion) return;

  volumeDataChunk->reserve(nChunksInRegion);

  for(int32_t iChunkInRegion = 0; iChunkInRegion < nChunksInRegion; iChunkInRegion++)
  {
    volumeDataChunk->push_back(m_volumeDataLayer->GetChunkFromIndex(GetChunkIndexInRegion(iChunkInRegion)));
  }
}

bool VolumeDataRegion::IsChunkInRegion(VolumeDataChunk const &volumeDataChunk) const
{
  if(//volumeDataChunk.GetVDS() == _gVDS &&
     volumeDataChunk.layer == m_volumeDataLayer)
  {
    IndexArray indexArray;

    m_volumeDataLayer->ChunkIndexToIndexArray(volumeDataChunk.index, indexArray);
    for(int32_t iDimension = 0; iDimension < ArraySize(indexArray); iDimension++)
    {
      if(indexArray[iDimension] < m_chunkMin[iDimension] ||
         indexArray[iDimension] > m_chunkMax[iDimension])
      {
        return false;
      }
    }
    return true;
  }
  return false;
}

VolumeDataRegion::VolumeDataRegion(VolumeDataLayer const &volumeDataLayer, const IndexArray &min, const IndexArray &max)
  : m_volumeDataLayer(&volumeDataLayer)
{
  int64_t modulo = 1;

  for(int32_t iDimension = 0; iDimension < ArraySize(m_chunkMin); iDimension++)
  {
    m_chunkMin[iDimension] = volumeDataLayer.VoxelToIndex(min[iDimension], iDimension);
    m_chunkMax[iDimension] = volumeDataLayer.VoxelToIndex(max[iDimension] - 1, iDimension);

    m_layerModulo[iDimension] = volumeDataLayer.m_modulo[iDimension];
    m_modulo[iDimension] = modulo;
    modulo *= m_chunkMax[iDimension] - m_chunkMin[iDimension] + 1;

    assert(m_chunkMin[iDimension] <= m_chunkMax[iDimension]);
  }

  m_chunksInRegion = modulo;
}

VolumeDataRegion VolumeDataRegion::VolumeDataRegionOverlappingChunk(VolumeDataLayer const &volumeDataLayer, VolumeDataChunk const &volumeDataChunk, const IndexArray &offset)
{
  IndexArray min;
  IndexArray max;

  const VolumeDataLayer *targetLayer = volumeDataChunk.layer;

  assert(volumeDataLayer.GetVolumeDataChannelMapping() == targetLayer->GetVolumeDataChannelMapping() && "VolumeDataRegionOverlappingChunk() doesn't work between layers with different mappings");

  targetLayer->GetChunkMinMax(volumeDataChunk.index, min, max, false);

  IndexArray validMin;
  IndexArray validMax;

  for (int dimension = 0; dimension < ArraySize(validMin); dimension++)
  {
    int neededExtraValidVoxelsNegative = 0,
        neededExtraValidVoxelsPositive = 0;

    // Do we have a render margin in this dimension?
    if (DimensionGroupUtil::IsDimensionInGroup(targetLayer->GetOriginalDimensionGroup(), dimension))
    {
      // Can we copy from source's render margin in this dimension?
      if (DimensionGroupUtil::IsDimensionInGroup(volumeDataLayer.GetOriginalDimensionGroup(), dimension))
      {
        neededExtraValidVoxelsNegative = std::max(0, targetLayer->GetNegativeRenderMargin() - volumeDataLayer.GetNegativeRenderMargin());
        neededExtraValidVoxelsPositive = std::max(0, targetLayer->GetPositiveRenderMargin() - volumeDataLayer.GetPositiveRenderMargin());
      }
      else
      {
        neededExtraValidVoxelsNegative = targetLayer->GetNegativeRenderMargin();
        neededExtraValidVoxelsPositive = targetLayer->GetPositiveRenderMargin();
      }
    }

    neededExtraValidVoxelsNegative += targetLayer->GetNegativeMargin(dimension) - volumeDataLayer.GetNegativeMargin(dimension);
    neededExtraValidVoxelsPositive += targetLayer->GetPositiveMargin(dimension) - volumeDataLayer.GetPositiveMargin(dimension);

    validMin[dimension] = min[dimension] - offset[dimension] - std::max(0, neededExtraValidVoxelsNegative),
    validMax[dimension] = max[dimension] - offset[dimension] + std::max(0, neededExtraValidVoxelsPositive);

    // Limit the valid area so it doesn't extend into the border
    validMin[dimension] = std::max(validMin[dimension], targetLayer->GetDimensionFirstSample(dimension));
    validMax[dimension] = std::min(validMax[dimension], targetLayer->GetDimensionFirstSample(dimension) + targetLayer->GetDimensionNumSamples(dimension));
  }

  return VolumeDataRegion(volumeDataLayer, validMin, validMax);
}

}
