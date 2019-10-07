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

#include "VolumeDataChannelMapping.h"
#include "VolumeDataPartition.h"

#include <assert.h>

namespace OpenVDS
{

class TraceVolumeDataChannelMapping : public VolumeDataChannelMapping
{
  TraceVolumeDataChannelMapping() {}
  ~TraceVolumeDataChannelMapping() override {}
public:
  DimensionGroup
                getMappedChunkDimensionGroup(VolumeDataPartition const &primaryPartition, int32_t mappedValues) const override;
  int32_t       getMappedPositiveMargin(VolumeDataPartition const &primaryPartition, int32_t dimension) const override;
  int32_t       getMappedNegativeMargin(VolumeDataPartition const &primaryPartition, int32_t dimension) const override;
  int32_t       getMappedPositiveBorder(VolumeDataPartition const &primaryPartition, int32_t dimension) const override;
  int32_t       getMappedNegativeBorder(VolumeDataPartition const &primaryPartition, int32_t dimension) const override;
  int32_t       getMappedDimensionFirstSample(int32_t dimension, int32_t dimensionFirstSample, int32_t mappedValues) const override;
  int32_t       getMappedDimensionNumSamples(int32_t dimension, int32_t dimensionNumSamples, int32_t mappedValues) const override;
  int32_t       getMappedBrickSize(VolumeDataPartition const &primaryPartition, int32_t dimension, int32_t mappedValues) const override;

  int64_t       getMappedChunkIndex(VolumeDataPartition const &primaryPartition, int64_t chunkIndex) const override;
  int32_t       getMappedChunkIndexFromVoxel(VolumeDataPartition const &primaryPartition, int32_t voxel, int32_t dimension) const override;
  void          getLayoutMinMax(VolumeDataPartition const &primaryPartition, int64_t mappedChunkIndex, int32_t *min, int32_t *max, bool isIncludeMargin) const override;
  int64_t       getPrimaryChunkIndex(VolumeDataPartition const &primaryPartition, int64_t mappedChunkIndex) const override;

  static const TraceVolumeDataChannelMapping &
                getInstance();
};

DimensionGroup
TraceVolumeDataChannelMapping::getMappedChunkDimensionGroup(VolumeDataPartition const &primaryPartition, int32_t mappedValues) const
{
  int32_t dimensionCount = 0;
  int32_t dimensions[Dimensionality_Max];

  // If there are more than 1 mapped values, we need to include dimension 0
  if(mappedValues > 1)
  {
    dimensions[dimensionCount++] = 0;
  }

  // Add any dimensions other than 0 to the dimensions for the metadata layer
  DimensionGroup dimensionGroup = primaryPartition.getOriginalDimensionGroup();
  for(int32_t dimension = 1; dimension < Dimensionality_Max; dimension++)
  {
    if(DimensionGroupUtil::isDimensionInGroup(dimensionGroup, dimension))
    {
      dimensions[dimensionCount++] = dimension;
    }
  }
  while(dimensionCount < Dimensionality_Max)
  {
    dimensions[dimensionCount++] = -1;
  }

  return DimensionGroupUtil::getDimensionGroupFromDimensionIndices(dimensions[0], dimensions[1], dimensions[2], dimensions[3], dimensions[4], dimensions[5]);
}

int
TraceVolumeDataChannelMapping::getMappedPositiveMargin(VolumeDataPartition const &primaryPartition, int32_t dimension) const
{
  if(dimension == 0)
  {
    return 0;
  }
  else
  {
    return primaryPartition.getPositiveMargin(dimension);
  }
}

int
TraceVolumeDataChannelMapping::getMappedNegativeMargin(VolumeDataPartition const &primaryPartition, int32_t dimension) const
{
  if(dimension == 0)
  {
    return 0;
  }
  else
  {
    return primaryPartition.getNegativeMargin(dimension);
  }
}

int
TraceVolumeDataChannelMapping::getMappedPositiveBorder(VolumeDataPartition const &primaryPartition, int32_t dimension) const
{
  if(dimension == 0)
  {
    return 0;
  }
  else
  {
    return primaryPartition.getPositiveBorder(dimension);
  }
}

int
TraceVolumeDataChannelMapping::getMappedNegativeBorder(VolumeDataPartition const &primaryPartition, int32_t dimension)const
{
  if(dimension == 0)
  {
    return 0;
  }
  else
  {
    return primaryPartition.getNegativeBorder(dimension);
  }
}

int
TraceVolumeDataChannelMapping::getMappedDimensionFirstSample(int32_t dimension, int32_t dimensionFirstSample, int32_t mappedValues) const
{
  if(dimension == 0)
  {
    return 0;
  }
  else
  {
    return dimensionFirstSample;
  }
}

int
TraceVolumeDataChannelMapping::getMappedDimensionNumSamples(int32_t dimension, int32_t dimensionNumSamples, int32_t mappedValues) const
{
  assert(mappedValues > 0);
  if(dimension == 0)
  {
    return mappedValues;
  }
  else
  {
    return dimensionNumSamples;
  }
}

int
TraceVolumeDataChannelMapping::getMappedBrickSize(VolumeDataPartition const &primaryPartition, int32_t dimension, int32_t mappedValues) const
{
  assert(mappedValues > 0);

  if(dimension == 0)
  {
    return mappedValues;
  }
  else
  {
    return primaryPartition.getBrickSize(dimension);
  }
}

int64_t
TraceVolumeDataChannelMapping::getMappedChunkIndex(VolumeDataPartition const &primaryPartition, int64_t chunkIndex) const
{
  assert(chunkIndex >= 0 && chunkIndex < primaryPartition.getTotalChunkCount());
  return chunkIndex / primaryPartition.getNumChunksInDimension(0);
}

int
TraceVolumeDataChannelMapping::getMappedChunkIndexFromVoxel(VolumeDataPartition const &primaryPartition, int32_t voxel, int32_t dimension) const
{
  if(dimension == 0) return 0;
  else return primaryPartition.voxelToIndex(voxel, dimension);
}

void
TraceVolumeDataChannelMapping::getLayoutMinMax(VolumeDataPartition const &primaryPartition, int64_t mappedChunkIndex, int32_t *min, int32_t *max, bool isIncludeMargin) const
{
  int64_t
    startChunk = mappedChunkIndex * primaryPartition.getNumChunksInDimension(0),
    endChunk   = startChunk + primaryPartition.getNumChunksInDimension(0) - 1;

  int32_t
    startMax[Dimensionality_Max],
    endMin[Dimensionality_Max];

  primaryPartition.getChunkMinMax(startChunk, min, startMax, false);
  primaryPartition.getChunkMinMax(endChunk, endMin, max, false);
}

int64_t
TraceVolumeDataChannelMapping::getPrimaryChunkIndex(VolumeDataPartition const &primaryPartition, int64_t mappedChunkIndex) const
{
  return mappedChunkIndex * primaryPartition.getNumChunksInDimension(0);
}

const TraceVolumeDataChannelMapping &
TraceVolumeDataChannelMapping::getInstance()
{
  static TraceVolumeDataChannelMapping
    instance;

  return instance;
}

const VolumeDataChannelMapping *
VolumeDataChannelMapping::getVolumeDataChannelMapping(VolumeDataMapping volumeDataMapping)
{
  switch(volumeDataMapping)
  {
  case VolumeDataMapping::Direct:   return nullptr;
  case VolumeDataMapping::PerTrace: return &TraceVolumeDataChannelMapping::getInstance();

  default: assert(0 && "Illegal volume data mapping"); return nullptr;
  }
}

}
