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

#include "VolumeDataChunk.h"
#include "VolumeDataLayout.h"

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

  for(int32_t iDimension = int32_t(array_size(m_chunkMin)) - 1; iDimension >= 0; iDimension--)
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
    volumeDataChunk->push_back({m_volumeDataLayer, int64_t(GetChunkIndexInRegion(iChunkInRegion))});
  }
}

bool VolumeDataRegion::IsChunkInRegion(VolumeDataChunk const &volumeDataChunk) const
{
  if(//volumeDataChunk.GetVDS() == _gVDS &&
     volumeDataChunk.layer == m_volumeDataLayer)
  {
    IndexArray indexArray;

    m_volumeDataLayer->ChunkIndexToIndexArray(volumeDataChunk.chunkIndex, indexArray);
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

VolumeDataRegion::VolumeDataRegion(VolumeDataLayer const &volumeDataLayer, const IndexArray &anMin, const IndexArray &anMax)
  : m_volumeDataLayer(&volumeDataLayer)
{
  VolumeDataLayout *volumeDataLayout = volumeDataLayer.GetLayout();

  int64_t nModulo = 1;

  for(int32_t iDimension = 0; iDimension < array_size(m_chunkMin); iDimension++)
  {
    m_chunkMin[iDimension] = volumeDataLayer.VoxelToIndex(anMin[iDimension], iDimension);
    m_chunkMax[iDimension] = volumeDataLayer.VoxelToIndex(anMax[iDimension] - 1, iDimension);

    m_layerModulo[iDimension] = volumeDataLayer.m_modulo[iDimension];
    m_modulo[iDimension] = nModulo;
    nModulo *= m_chunkMax[iDimension] - m_chunkMin[iDimension] + 1;

    assert(m_chunkMin[iDimension] <= m_chunkMax[iDimension]);
  }

  m_chunksInRegion = nModulo;
}

VolumeDataRegion VolumeDataRegion::VolumeDataRegionOverlappingChunk(VolumeDataLayer const &volumeDataLayer, VolumeDataChunk const &volumeDataChunk, const IndexArray &anOffset)
{
  IndexArray min;
  IndexArray max;

  const VolumeDataLayer *targetLayer = volumeDataChunk.layer;

  assert(volumeDataLayer.GetVolumeDataChannelMapping() == targetLayer->GetVolumeDataChannelMapping() && "VolumeDataRegionOverlappingChunk() doesn't work between layers with different mappings");

  targetLayer->GetChunkMinMax(volumeDataChunk.chunkIndex, min, max, false);

  IndexArray validMin;
  IndexArray validMax;

  for(int32_t iDimension = 0; iDimension < array_size(validMin); iDimension++)
  {
    int32_t nNeededExtraValidVoxelsNegative = 0;
    int32_t nNeededExtraValidVoxelsPositive = 0;

    if(DimensionGroupUtil::IsDimensionInGroup(targetLayer->m_originalDimensionGroup, iDimension))
    {
      nNeededExtraValidVoxelsNegative += volumeDataLayer.GetNegativeRenderMargin();
      nNeededExtraValidVoxelsPositive += volumeDataLayer.GetPositiveRenderMargin();

      if(DimensionGroupUtil::IsDimensionInGroup(volumeDataLayer.GetOriginalDimensionGroup(), iDimension))
      {
        // How much can we copy from render margin to render margin?
        nNeededExtraValidVoxelsNegative -= volumeDataLayer.GetNegativeRenderMargin();
        nNeededExtraValidVoxelsPositive -= volumeDataLayer.GetPositiveRenderMargin();
      }
    }

    nNeededExtraValidVoxelsNegative += targetLayer->GetNegativeMargin(iDimension) - volumeDataLayer.GetNegativeMargin(iDimension);
    nNeededExtraValidVoxelsNegative += targetLayer->GetNegativeMargin(iDimension) - volumeDataLayer.GetNegativeMargin(iDimension);
    nNeededExtraValidVoxelsPositive += targetLayer->GetPositiveMargin(iDimension) - volumeDataLayer.GetPositiveMargin(iDimension);

    validMin[iDimension] = min[iDimension] - anOffset[iDimension] - std::max(0, nNeededExtraValidVoxelsNegative),
    validMax[iDimension] = max[iDimension] - anOffset[iDimension] + std::max(0, nNeededExtraValidVoxelsPositive);

    // Limit the valid area so it doesn't extend into the border
    validMin[iDimension] = std::max(validMin[iDimension], targetLayer->GetDimensionFirstSample(iDimension));
    validMax[iDimension] = std::min(validMax[iDimension], targetLayer->GetDimensionFirstSample(iDimension) + targetLayer->GetDimensionNumSamples(iDimension));
  }

  return VolumeDataRegion(volumeDataLayer, validMin, validMax);
}

}
