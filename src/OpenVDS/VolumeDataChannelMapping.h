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

#include <cstdint>
#include <OpenVDS/openvds_export.h>
#include <VDS/DimensionGroup.h>

namespace OpenVDS
{

class VolumeDataPartition;

class VolumeDataChannelMapping
{
public:
  virtual      ~VolumeDataChannelMapping() {}

  virtual DimensionGroup getMappedChunkDimensionGroup(VolumeDataPartition const &primaryPartition, int32_t mappedValues) const = 0;
  virtual int32_t   getMappedPositiveMargin(VolumeDataPartition const &primaryPartition, int32_t dimension) const = 0;
  virtual int32_t   getMappedNegativeMargin(VolumeDataPartition const &primaryPartition, int32_t dimension) const = 0;
  virtual int32_t   getMappedPositiveBorder(VolumeDataPartition const &primaryPartition, int32_t dimension) const = 0;
  virtual int32_t   getMappedNegativeBorder(VolumeDataPartition const &primaryPartition, int32_t dimension) const = 0;
  virtual int32_t   getMappedDimensionFirstSample(int32_t dimension, int32_t dimensionFirstSample, int32_t mappedValues) const = 0;
  virtual int32_t   getMappedDimensionNumSamples(int32_t dimension, int32_t dimensionNumSamples, int32_t mappedValues) const = 0;
  virtual int32_t   getMappedBrickSize(VolumeDataPartition const &primaryPartition, int32_t dimension, int32_t mappedValues) const = 0;

  virtual int64_t   getMappedChunkIndex(VolumeDataPartition const &primaryPartition, int64_t chunkIndex) const = 0;
  virtual int32_t   getMappedChunkIndexFromVoxel(VolumeDataPartition const &primaryPartition, int32_t voxel, int32_t dimension) const = 0;
  virtual void      getLayoutMinMax(VolumeDataPartition const &primaryPartition, int64_t mappedChunkIndex, int32_t *min, int32_t *max, bool isIncludeMargin) const = 0;
  virtual int64_t   getPrimaryChunkIndex(VolumeDataPartition const &primaryPartition, int64_t mappedChunkIndex) const = 0;
};
}
