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
int64_t VolumeDataRegion::getNumChunksInRegion() const
{
  return m_chunksInRegion;
}

int64_t VolumeDataRegion::getChunkIndexInRegion(int64_t chunkInRegion) const
{
  assert(chunkInRegion >= 0 && chunkInRegion < m_chunksInRegion);

  int64_t iChunkIndex = 0;

  for(int32_t iDimension = int32_t(array_size(m_chunkMin)) - 1; iDimension >= 0; iDimension--)
  {
    iChunkIndex += (chunkInRegion / m_modulo[iDimension] + m_chunkMin[iDimension]) * m_layerModulo[iDimension];
    chunkInRegion %= m_modulo[iDimension];
  }

  return iChunkIndex;
}

void VolumeDataRegion::getChunksInRegion(std::vector<VolumeDataChunk>* volumeDataChunk, bool isAppend) const
{
  if (!isAppend)
  {
    volumeDataChunk->clear();
  }

  int32_t nChunksInRegion = (int32_t)getNumChunksInRegion();
  if(!nChunksInRegion) return;

  volumeDataChunk->reserve(nChunksInRegion);

  for(int32_t iChunkInRegion = 0; iChunkInRegion < nChunksInRegion; iChunkInRegion++)
  {
    volumeDataChunk->push_back(m_volumeDataLayer->getChunkFromIndex(getChunkIndexInRegion(iChunkInRegion)));
  }
}

bool VolumeDataRegion::isChunkInRegion(VolumeDataChunk const &volumeDataChunk) const
{
  if(//volumeDataChunk.GetVDS() == _gVDS &&
     volumeDataChunk.layer == m_volumeDataLayer)
  {
    IndexArray indexArray;

    m_volumeDataLayer->chunkIndexToIndexArray(volumeDataChunk.chunkIndex, indexArray);
    for(int32_t iDimension = 0; iDimension < array_size(indexArray); iDimension++)
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

  for(int32_t iDimension = 0; iDimension < array_size(m_chunkMin); iDimension++)
  {
    m_chunkMin[iDimension] = volumeDataLayer.voxelToIndex(min[iDimension], iDimension);
    m_chunkMax[iDimension] = volumeDataLayer.voxelToIndex(max[iDimension] - 1, iDimension);

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

  assert(volumeDataLayer.getVolumeDataChannelMapping() == targetLayer->getVolumeDataChannelMapping() && "VolumeDataRegionOverlappingChunk() doesn't work between layers with different mappings");

  targetLayer->getChunkMinMax(volumeDataChunk.chunkIndex, min, max, false);

  IndexArray validMin;
  IndexArray validMax;

  for (int dimension = 0; dimension < array_size(validMin); dimension++)
  {
    int neededExtraValidVoxelsNegative = 0,
        neededExtraValidVoxelsPositive = 0;

    // Do we have a render margin in this dimension?
    if (DimensionGroupUtil::isDimensionInGroup(targetLayer->getOriginalDimensionGroup(), dimension))
    {
      // Can we copy from source's render margin in this dimension?
      if (DimensionGroupUtil::isDimensionInGroup(volumeDataLayer.getOriginalDimensionGroup(), dimension))
      {
        neededExtraValidVoxelsNegative = std::max(0, targetLayer->getNegativeRenderMargin() - volumeDataLayer.getNegativeRenderMargin());
        neededExtraValidVoxelsPositive = std::max(0, targetLayer->getPositiveRenderMargin() - volumeDataLayer.getPositiveRenderMargin());
      }
      else
      {
        neededExtraValidVoxelsNegative = targetLayer->getNegativeRenderMargin();
        neededExtraValidVoxelsPositive = targetLayer->getPositiveRenderMargin();
      }
    }

    neededExtraValidVoxelsNegative += targetLayer->getNegativeMargin(dimension) - volumeDataLayer.getNegativeMargin(dimension);
    neededExtraValidVoxelsPositive += targetLayer->getPositiveMargin(dimension) - volumeDataLayer.getPositiveMargin(dimension);

    validMin[dimension] = min[dimension] - offset[dimension] - std::max(0, neededExtraValidVoxelsNegative),
    validMax[dimension] = max[dimension] - offset[dimension] + std::max(0, neededExtraValidVoxelsPositive);

    // Limit the valid area so it doesn't extend into the border
    validMin[dimension] = std::max(validMin[dimension], targetLayer->getDimensionFirstSample(dimension));
    validMax[dimension] = std::min(validMax[dimension], targetLayer->getDimensionFirstSample(dimension) + targetLayer->getDimensionNumSamples(dimension));
  }

  return VolumeDataRegion(volumeDataLayer, validMin, validMax);
}

}
