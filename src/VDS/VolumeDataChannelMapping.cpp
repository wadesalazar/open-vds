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
                GetMappedChunkDimensionGroup(VolumeDataPartition const &primaryPartition, int32_t mappedValues) const override;
  int32_t       GetMappedPositiveMargin(VolumeDataPartition const &primaryPartition, int32_t dimension) const override;
  int32_t       GetMappedNegativeMargin(VolumeDataPartition const &primaryPartition, int32_t dimension) const override;
  int32_t       GetMappedPositiveBorder(VolumeDataPartition const &primaryPartition, int32_t dimension) const override;
  int32_t       GetMappedNegativeBorder(VolumeDataPartition const &primaryPartition, int32_t dimension) const override;
  int32_t       GetMappedDimensionFirstSample(int32_t dimension, int32_t dimensionFirstSample, int32_t mappedValues) const override;
  int32_t       GetMappedDimensionNumSamples(int32_t dimension, int32_t dimensionNumSamples, int32_t mappedValues) const override;
  int32_t       GetMappedBrickSize(VolumeDataPartition const &primaryPartition, int32_t dimension, int32_t mappedValues) const override;

  int64_t       GetMappedChunkIndex(VolumeDataPartition const &primaryPartition, int64_t chunkIndex) const override;
  int32_t       GetMappedChunkIndexFromVoxel(VolumeDataPartition const &primaryPartition, int32_t voxel, int32_t dimension) const override;
  void          GetLayoutMinMax(VolumeDataPartition const &primaryPartition, int64_t mappedChunkIndex, int32_t *min, int32_t *max, bool isIncludeMargin) const override;
  int64_t       GetPrimaryChunkIndex(VolumeDataPartition const &primaryPartition, int64_t mappedChunkIndex) const override;

  static const TraceVolumeDataChannelMapping &
                GetInstance();
};

DimensionGroup
TraceVolumeDataChannelMapping::GetMappedChunkDimensionGroup(VolumeDataPartition const &primaryPartition, int32_t mappedValues) const
{
  int32_t dimensionCount = 0;
  int32_t dimensions[Dimensionality_Max];

  // If there are more than 1 mapped values, we need to include dimension 0
  if(mappedValues > 1)
  {
    dimensions[dimensionCount++] = 0;
  }

  // Add any dimensions other than 0 to the dimensions for the metadata layer
  DimensionGroup dimensionGroup = primaryPartition.GetOriginalDimensionGroup();
  for(int32_t dimension = 1; dimension < Dimensionality_Max; dimension++)
  {
    if(DimensionGroupUtil::IsDimensionInGroup(dimensionGroup, dimension))
    {
      dimensions[dimensionCount++] = dimension;
    }
  }
  while(dimensionCount < Dimensionality_Max)
  {
    dimensions[dimensionCount++] = -1;
  }

  return DimensionGroupUtil::GetDimensionGroupFromDimensionIndices(dimensions[0], dimensions[1], dimensions[2], dimensions[3], dimensions[4], dimensions[5]);
}

int
TraceVolumeDataChannelMapping::GetMappedPositiveMargin(VolumeDataPartition const &primaryPartition, int32_t dimension) const
{
  if(dimension == 0)
  {
    return 0;
  }
  else
  {
    return primaryPartition.GetPositiveMargin(dimension);
  }
}

int
TraceVolumeDataChannelMapping::GetMappedNegativeMargin(VolumeDataPartition const &primaryPartition, int32_t dimension) const
{
  if(dimension == 0)
  {
    return 0;
  }
  else
  {
    return primaryPartition.GetNegativeMargin(dimension);
  }
}

int
TraceVolumeDataChannelMapping::GetMappedPositiveBorder(VolumeDataPartition const &primaryPartition, int32_t dimension) const
{
  if(dimension == 0)
  {
    return 0;
  }
  else
  {
    return primaryPartition.GetPositiveBorder(dimension);
  }
}

int
TraceVolumeDataChannelMapping::GetMappedNegativeBorder(VolumeDataPartition const &primaryPartition, int32_t dimension)const
{
  if(dimension == 0)
  {
    return 0;
  }
  else
  {
    return primaryPartition.GetNegativeBorder(dimension);
  }
}

int
TraceVolumeDataChannelMapping::GetMappedDimensionFirstSample(int32_t dimension, int32_t dimensionFirstSample, int32_t mappedValues) const
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
TraceVolumeDataChannelMapping::GetMappedDimensionNumSamples(int32_t dimension, int32_t dimensionNumSamples, int32_t mappedValues) const
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
TraceVolumeDataChannelMapping::GetMappedBrickSize(VolumeDataPartition const &primaryPartition, int32_t dimension, int32_t mappedValues) const
{
  assert(mappedValues > 0);

  if(dimension == 0)
  {
    return mappedValues;
  }
  else
  {
    return primaryPartition.GetBrickSize(dimension);
  }
}

int64_t
TraceVolumeDataChannelMapping::GetMappedChunkIndex(VolumeDataPartition const &primaryPartition, int64_t chunkIndex) const
{
  assert(chunkIndex >= 0 && chunkIndex < primaryPartition.GetTotalChunkCount());
  return chunkIndex / primaryPartition.GetNumChunksInDimension(0);
}

int
TraceVolumeDataChannelMapping::GetMappedChunkIndexFromVoxel(VolumeDataPartition const &primaryPartition, int32_t voxel, int32_t dimension) const
{
  if(dimension == 0) return 0;
  else return primaryPartition.VoxelToIndex(voxel, dimension);
}

void
TraceVolumeDataChannelMapping::GetLayoutMinMax(VolumeDataPartition const &primaryPartition, int64_t mappedChunkIndex, int32_t *min, int32_t *max, bool isIncludeMargin) const
{
  int64_t
    startChunk = mappedChunkIndex * primaryPartition.GetNumChunksInDimension(0),
    endChunk   = startChunk + primaryPartition.GetNumChunksInDimension(0) - 1;

  int32_t
    startMax[Dimensionality_Max],
    endMin[Dimensionality_Max];

  primaryPartition.GetChunkMinMax(startChunk, min, startMax, false);
  primaryPartition.GetChunkMinMax(endChunk, endMin, max, false);
}

int64_t
TraceVolumeDataChannelMapping::GetPrimaryChunkIndex(VolumeDataPartition const &primaryPartition, int64_t mappedChunkIndex) const
{
  return mappedChunkIndex * primaryPartition.GetNumChunksInDimension(0);
}

const TraceVolumeDataChannelMapping &
TraceVolumeDataChannelMapping::GetInstance()
{
  static TraceVolumeDataChannelMapping
    instance;

  return instance;
}

const VolumeDataChannelMapping *
VolumeDataChannelMapping::GetVolumeDataChannelMapping(VolumeDataMapping volumeDataMapping)
{
  switch(volumeDataMapping)
  {
  case VolumeDataMapping::Direct:   return nullptr;
  case VolumeDataMapping::PerTrace: return &TraceVolumeDataChannelMapping::GetInstance();

  default: assert(0 && "Illegal volume data mapping"); return nullptr;
  }
}

}
