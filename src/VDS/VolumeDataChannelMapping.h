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

#ifndef VOLUMEDATACHANNELMAPPING_H
#define VOLUMEDATACHANNELMAPPING_H

#include "VolumeDataPartition.h"

#include <cstdint>

namespace OpenVDS
{
class VolumeDataChannelMapping
{
protected:
  virtual      ~VolumeDataChannelMapping() {}
public:
  virtual DimensionGroup GetMappedChunkDimensionGroup(VolumeDataPartition const &primaryPartition, int32_t mappedValues) const = 0;
  virtual int32_t   GetMappedPositiveMargin(VolumeDataPartition const &primaryPartition, int32_t dimension) const = 0;
  virtual int32_t   GetMappedNegativeMargin(VolumeDataPartition const &primaryPartition, int32_t dimension) const = 0;
  virtual int32_t   GetMappedPositiveBorder(VolumeDataPartition const &primaryPartition, int32_t dimension) const = 0;
  virtual int32_t   GetMappedNegativeBorder(VolumeDataPartition const &primaryPartition, int32_t dimension) const = 0;
  virtual int32_t   GetMappedDimensionFirstSample(int32_t dimension, int32_t dimensionFirstSample, int32_t mappedValues) const = 0;
  virtual int32_t   GetMappedDimensionNumSamples(int32_t dimension, int32_t dimensionNumSamples, int32_t mappedValues) const = 0;
  virtual int32_t   GetMappedBrickSize(VolumeDataPartition const &primaryPartition, int32_t dimension, int32_t mappedValues) const = 0;

  virtual int64_t   GetMappedChunkIndex(VolumeDataPartition const &primaryPartition, int64_t chunkIndex) const = 0;
  virtual int32_t   GetMappedChunkIndexFromVoxel(VolumeDataPartition const &primaryPartition, int32_t voxel, int32_t dimension) const = 0;
  virtual void      GetLayoutMinMax(VolumeDataPartition const &primaryPartition, int64_t mappedChunkIndex, int32_t *min, int32_t *max, bool isIncludeMargin) const = 0;
  virtual int64_t   GetPrimaryChunkIndex(VolumeDataPartition const &primaryPartition, int64_t mappedChunkIndex) const = 0;

  static  const VolumeDataChannelMapping *
                    GetVolumeDataChannelMapping(VolumeDataMapping volumeDataMapping);

  static VolumeDataPartition
                    StaticMapPartition(VolumeDataPartition const &primaryPartition, const VolumeDataChannelMapping *volumeDataChannelMapping, int32_t mappedValues);
};
}
#endif //VOLUMEDATACHANNELMAPPING_H
