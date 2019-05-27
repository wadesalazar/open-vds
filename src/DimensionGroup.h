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

#ifndef DIMENSIONGROUP_H
#define DIMENSIONGROUP_H

#include "VolumeData.h"
#include <cstdint>

namespace OpenVDS
{

enum DimensionGroup
{
  DimensionGroup_0,
  DimensionGroup_1,
  DimensionGroup_2,
  DimensionGroup_3,
  DimensionGroup_4,
  DimensionGroup_5,

  DimensionGroup_01,
  DimensionGroup_02,
  DimensionGroup_03,
  DimensionGroup_04,
  DimensionGroup_05,
  DimensionGroup_12,
  DimensionGroup_13,
  DimensionGroup_14,
  DimensionGroup_15,
  DimensionGroup_23,
  DimensionGroup_24,
  DimensionGroup_25,
  DimensionGroup_34,
  DimensionGroup_35,
  DimensionGroup_45,

  DimensionGroup_012,
  DimensionGroup_013,
  DimensionGroup_014,
  DimensionGroup_015,
  DimensionGroup_023,
  DimensionGroup_024,
  DimensionGroup_025,
  DimensionGroup_034,
  DimensionGroup_035,
  DimensionGroup_045,
  DimensionGroup_123,
  DimensionGroup_124,
  DimensionGroup_125,
  DimensionGroup_134,
  DimensionGroup_135,
  DimensionGroup_145,
  DimensionGroup_234,
  DimensionGroup_235,
  DimensionGroup_245,
  DimensionGroup_345,

  DimensionGroup_0123,
  DimensionGroup_0124,
  DimensionGroup_0125,
  DimensionGroup_0134,
  DimensionGroup_0135,
  DimensionGroup_0145,
  DimensionGroup_0234,
  DimensionGroup_0235,
  DimensionGroup_0245,
  DimensionGroup_0345,
  DimensionGroup_1234,
  DimensionGroup_1235,
  DimensionGroup_1245,
  DimensionGroup_1345,
  DimensionGroup_2345,

  DimensionGroup_01234,
  DimensionGroup_01235,
  DimensionGroup_01245,
  DimensionGroup_01345,
  DimensionGroup_02345,
  DimensionGroup_12345,

  DimensionGroup_012345,

  DimensionGroup_Max,

  DimensionGroup_1D_Max = DimensionGroup_5 + 1,
  DimensionGroup_2D_Max = DimensionGroup_45 + 1,
  DimensionGroup_3D_Max = DimensionGroup_345 + 1,
  DimensionGroup_4D_Max = DimensionGroup_2345 + 1,
  DimensionGroup_5D_Max = DimensionGroup_12345 + 1,
  DimensionGroup_6D_Max = DimensionGroup_012345 + 1
};

enum Dimensionality
{
  Dimensionality_1 = 1,
  Dimensionality_2,
  Dimensionality_3,
  Dimensionality_4,
  Dimensionality_5,
  Dimensionality_6,
  Dimensionality_Max = Dimensionality_6
};

namespace DimensionGroupUtil
{
  bool       IsDimensionInGroup(DimensionGroup eDimensionGroup, int32_t iDimension);

  int32_t    GetDimensionality(DimensionGroup eDimensionGroup);

  int32_t    GetDimension(DimensionGroup eDimensionGroup, int32_t iIndexInGroup);

  DimensionGroup GetDimensionGroupFromDimensionIndices(int iDimension0);
  DimensionGroup GetDimensionGroupFromDimensionIndices(int iDimension0, int iDimension1);
  DimensionGroup GetDimensionGroupFromDimensionIndices(int iDimension0, int iDimension1, int iDimension2);
  DimensionGroup GetDimensionGroupFromDimensionIndices(int iDimension0, int iDimension1, int iDimension2, int iDimension3);
  DimensionGroup GetDimensionGroupFromDimensionIndices(int iDimension0, int iDimension1, int iDimension2, int iDimension3, int iDimension4);
  DimensionGroup GetDimensionGroupFromDimensionIndices(int iDimension0, int iDimension1, int iDimension2, int iDimension3, int iDimension4, int iDimension5);

  DimensionGroup GetDimensionGroupFromDimensionsND(DimensionsND eDimensionsND);

  DimensionsND GetDimensionsNDFromDimensionGroup(DimensionGroup eDimensionGroup);

  const char *GetDimensionGroupShortName(DimensionGroup eDimensionGroup);

  const char *GetDimensionGroupName(DimensionGroup eDimensionGroup);

  bool IsRemappingPossible(DimensionGroup eDimensionGroupA, DimensionGroup eDimensionGroupB);

  DimensionGroup Union(DimensionGroup eDimensionGroupA, DimensionGroup eDimensionGroupB);
}
}
#endif
