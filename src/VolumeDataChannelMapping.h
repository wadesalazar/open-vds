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
#include "DimensionGroup.h"

namespace OpenVDS
{

class VolumeDataPartition;

class OPENVDS_EXPORT VolumeDataChannelMapping
{
public:
  virtual      ~VolumeDataChannelMapping() {}

  virtual DimensionGroup
                    GetMappedChunkDimensionGroup(VolumeDataPartition const &cPrimaryPartition, int32_t nMappedValues) const = 0;
  virtual int32_t   GetMappedPositiveMargin(VolumeDataPartition const &cPrimaryPartition, int32_t iDimension) const = 0;
  virtual int32_t   GetMappedNegativeMargin(VolumeDataPartition const &cPrimaryPartition, int32_t iDimension) const = 0;
  virtual int32_t   GetMappedPositiveBorder(VolumeDataPartition const &cPrimaryPartition, int32_t iDimension) const = 0;
  virtual int32_t   GetMappedNegativeBorder(VolumeDataPartition const &cPrimaryPartition, int32_t iDimension) const = 0;
  virtual int32_t   GetMappedDimensionFirstSample(int32_t iDimension, int32_t nDimensionFirstSample, int32_t nMappedValues) const = 0;
  virtual int32_t   GetMappedDimensionNumSamples(int32_t iDimension, int32_t nDimensionNumSamples, int32_t nMappedValues) const = 0;
  virtual int32_t   GetMappedBrickSize(VolumeDataPartition const &cPrimaryPartition, int32_t iDimension, int32_t nMappedValues) const = 0;

  virtual int64_t   GetMappedChunkIndex(VolumeDataPartition const &cPrimaryPartition, int64_t iChunkIndex) const = 0;
  virtual int32_t   GetMappedChunkIndexFromVoxel(VolumeDataPartition const &cPrimaryPartition, int32_t nVoxel, int32_t iDimension) const = 0;
  virtual void      GetLayoutMinMax(VolumeDataPartition const &cPrimaryPartition, int64_t iMappedChunkIndex, int32_t *pnMin, int32_t *pnMax, bool isIncludeMargin) const = 0;
  virtual int64_t   GetPrimaryChunkIndex(VolumeDataPartition const &cPrimaryPartition, int64_t iMappedChunkIndex) const = 0;
};
}
