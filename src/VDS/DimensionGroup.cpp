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

#include "DimensionGroup.h"
#include <assert.h>

namespace OpenVDS
{
namespace DimensionGroupUtil
{
bool isDimensionInGroup(DimensionGroup dimensionGroup, int32_t dimension)
{
  switch(dimension)
  {
  case 0:
    switch(dimensionGroup)
    {
    case DimensionGroup_0:
    case DimensionGroup_01:
    case DimensionGroup_02:
    case DimensionGroup_03:
    case DimensionGroup_04:
    case DimensionGroup_05:
    case DimensionGroup_012:
    case DimensionGroup_013:
    case DimensionGroup_014:
    case DimensionGroup_015:
    case DimensionGroup_023:
    case DimensionGroup_024:
    case DimensionGroup_025:
    case DimensionGroup_034:
    case DimensionGroup_035:
    case DimensionGroup_045:
    case DimensionGroup_0123:
    case DimensionGroup_0124:
    case DimensionGroup_0125:
    case DimensionGroup_0134:
    case DimensionGroup_0135:
    case DimensionGroup_0145:
    case DimensionGroup_0234:
    case DimensionGroup_0235:
    case DimensionGroup_0245:
    case DimensionGroup_0345:
    case DimensionGroup_01234:
    case DimensionGroup_01235:
    case DimensionGroup_01245:
    case DimensionGroup_01345:
    case DimensionGroup_02345:
    case DimensionGroup_012345:
      return true;
    default:
      return false;
    }
  case 1:
    switch(dimensionGroup)
    {
    case DimensionGroup_1:
    case DimensionGroup_01:
    case DimensionGroup_12:
    case DimensionGroup_13:
    case DimensionGroup_14:
    case DimensionGroup_15:
    case DimensionGroup_012:
    case DimensionGroup_013:
    case DimensionGroup_014:
    case DimensionGroup_015:
    case DimensionGroup_123:
    case DimensionGroup_124:
    case DimensionGroup_125:
    case DimensionGroup_134:
    case DimensionGroup_135:
    case DimensionGroup_145:
    case DimensionGroup_0123:
    case DimensionGroup_0124:
    case DimensionGroup_0125:
    case DimensionGroup_0134:
    case DimensionGroup_0135:
    case DimensionGroup_0145:
    case DimensionGroup_1234:
    case DimensionGroup_1235:
    case DimensionGroup_1245:
    case DimensionGroup_1345:
    case DimensionGroup_01234:
    case DimensionGroup_01235:
    case DimensionGroup_01245:
    case DimensionGroup_01345:
    case DimensionGroup_12345:
    case DimensionGroup_012345:
      return true;
    default:
      return false;
    }
  case 2:
    switch(dimensionGroup)
    {
    case DimensionGroup_2:
    case DimensionGroup_02:
    case DimensionGroup_12:
    case DimensionGroup_23:
    case DimensionGroup_24:
    case DimensionGroup_25:

    case DimensionGroup_012:
    case DimensionGroup_023:
    case DimensionGroup_024:
    case DimensionGroup_025:
    case DimensionGroup_123:
    case DimensionGroup_124:
    case DimensionGroup_125:
    case DimensionGroup_234:
    case DimensionGroup_235:
    case DimensionGroup_245:
    case DimensionGroup_0123:
    case DimensionGroup_0124:
    case DimensionGroup_0125:
    case DimensionGroup_0234:
    case DimensionGroup_0235:
    case DimensionGroup_0245:
    case DimensionGroup_1234:
    case DimensionGroup_1235:
    case DimensionGroup_1245:
    case DimensionGroup_2345:
    case DimensionGroup_01234:
    case DimensionGroup_01235:
    case DimensionGroup_01245:
    case DimensionGroup_02345:
    case DimensionGroup_12345:
    case DimensionGroup_012345:
      return true;
    default:
      return false;
    }
  case 3:
    switch(dimensionGroup)
    {
    case DimensionGroup_3:
    case DimensionGroup_03:
    case DimensionGroup_13:
    case DimensionGroup_23:
    case DimensionGroup_34:
    case DimensionGroup_35:
    case DimensionGroup_013:
    case DimensionGroup_023:
    case DimensionGroup_034:
    case DimensionGroup_035:
    case DimensionGroup_123:
    case DimensionGroup_134:
    case DimensionGroup_135:
    case DimensionGroup_234:
    case DimensionGroup_235:
    case DimensionGroup_345:
    case DimensionGroup_0123:
    case DimensionGroup_0134:
    case DimensionGroup_0135:
    case DimensionGroup_0234:
    case DimensionGroup_0235:
    case DimensionGroup_0345:
    case DimensionGroup_1234:
    case DimensionGroup_1235:
    case DimensionGroup_1345:
    case DimensionGroup_2345:
    case DimensionGroup_01234:
    case DimensionGroup_01235:
    case DimensionGroup_01345:
    case DimensionGroup_02345:
    case DimensionGroup_12345:
    case DimensionGroup_012345:
      return true;
    default:
      return false;
    }
  case 4:
    switch(dimensionGroup)
    {
    case DimensionGroup_4:
    case DimensionGroup_04:
    case DimensionGroup_14:
    case DimensionGroup_24:
    case DimensionGroup_34:
    case DimensionGroup_45:
    case DimensionGroup_014:
    case DimensionGroup_024:
    case DimensionGroup_034:
    case DimensionGroup_045:
    case DimensionGroup_124:
    case DimensionGroup_134:
    case DimensionGroup_145:
    case DimensionGroup_234:
    case DimensionGroup_245:
    case DimensionGroup_345:
    case DimensionGroup_0124:
    case DimensionGroup_0134:
    case DimensionGroup_0145:
    case DimensionGroup_0234:
    case DimensionGroup_0245:
    case DimensionGroup_0345:
    case DimensionGroup_1234:
    case DimensionGroup_1245:
    case DimensionGroup_1345:
    case DimensionGroup_2345:
    case DimensionGroup_01234:
    case DimensionGroup_01245:
    case DimensionGroup_01345:
    case DimensionGroup_02345:
    case DimensionGroup_12345:
    case DimensionGroup_012345:
      return true;
    default:
      return false;
    }
  case 5:
    switch(dimensionGroup)
    {
    case DimensionGroup_5:
    case DimensionGroup_05:
    case DimensionGroup_15:
    case DimensionGroup_25:
    case DimensionGroup_35:
    case DimensionGroup_45:
    case DimensionGroup_015:
    case DimensionGroup_025:
    case DimensionGroup_035:
    case DimensionGroup_045:
    case DimensionGroup_125:
    case DimensionGroup_135:
    case DimensionGroup_145:
    case DimensionGroup_235:
    case DimensionGroup_245:
    case DimensionGroup_345:
    case DimensionGroup_0125:
    case DimensionGroup_0135:
    case DimensionGroup_0145:
    case DimensionGroup_0235:
    case DimensionGroup_0245:
    case DimensionGroup_0345:
    case DimensionGroup_1235:
    case DimensionGroup_1245:
    case DimensionGroup_1345:
    case DimensionGroup_2345:
    case DimensionGroup_01235:
    case DimensionGroup_01245:
    case DimensionGroup_01345:
    case DimensionGroup_02345:
    case DimensionGroup_12345:
    case DimensionGroup_012345:
      return true;
    default:
      return false;
    }
  default:
    return false;
  }
}

int32_t getDimensionality(DimensionGroup dimensionGroup)
{
  switch(dimensionGroup)
  {
  case DimensionGroup_0:
  case DimensionGroup_1:
  case DimensionGroup_2:
  case DimensionGroup_3:
  case DimensionGroup_4:
  case DimensionGroup_5:
    return 1;

  case DimensionGroup_01:
  case DimensionGroup_02:
  case DimensionGroup_03:
  case DimensionGroup_04:
  case DimensionGroup_05:
  case DimensionGroup_12:
  case DimensionGroup_13:
  case DimensionGroup_14:
  case DimensionGroup_15:
  case DimensionGroup_23:
  case DimensionGroup_24:
  case DimensionGroup_25:
  case DimensionGroup_34:
  case DimensionGroup_35:
  case DimensionGroup_45:
    return 2;

  case DimensionGroup_012:
  case DimensionGroup_013:
  case DimensionGroup_014:
  case DimensionGroup_015:
  case DimensionGroup_023:
  case DimensionGroup_024:
  case DimensionGroup_025:
  case DimensionGroup_034:
  case DimensionGroup_035:
  case DimensionGroup_045:
  case DimensionGroup_123:
  case DimensionGroup_124:
  case DimensionGroup_125:
  case DimensionGroup_134:
  case DimensionGroup_135:
  case DimensionGroup_145:
  case DimensionGroup_234:
  case DimensionGroup_235:
  case DimensionGroup_245:
  case DimensionGroup_345:
    return 3;

  case DimensionGroup_0123:
  case DimensionGroup_0124:
  case DimensionGroup_0125:
  case DimensionGroup_0134:
  case DimensionGroup_0135:
  case DimensionGroup_0145:
  case DimensionGroup_0234:
  case DimensionGroup_0235:
  case DimensionGroup_0245:
  case DimensionGroup_0345:
  case DimensionGroup_1234:
  case DimensionGroup_1235:
  case DimensionGroup_1245:
  case DimensionGroup_1345:
  case DimensionGroup_2345:
    return 4;

  case DimensionGroup_01234:
  case DimensionGroup_01235:
  case DimensionGroup_01245:
  case DimensionGroup_01345:
  case DimensionGroup_02345:
  case DimensionGroup_12345:
    return 5;

  case DimensionGroup_012345:
    return 6;

  default:
    assert(0 && "Illegal dimensiongroup");
    return 0;
  }
}

int32_t getDimension(DimensionGroup dimensionGroup, int32_t indexInGroup)
{
  assert(dimensionGroup >= 0 && dimensionGroup < DimensionGroup_Max);

  switch(indexInGroup)
  {
  case 0:
    switch(dimensionGroup)
    {
    case DimensionGroup_0:
    case DimensionGroup_01:
    case DimensionGroup_02:
    case DimensionGroup_03:
    case DimensionGroup_04:
    case DimensionGroup_05:
    case DimensionGroup_012:
    case DimensionGroup_013:
    case DimensionGroup_014:
    case DimensionGroup_015:
    case DimensionGroup_023:
    case DimensionGroup_024:
    case DimensionGroup_025:
    case DimensionGroup_034:
    case DimensionGroup_035:
    case DimensionGroup_045:
    case DimensionGroup_0123:
    case DimensionGroup_0124:
    case DimensionGroup_0125:
    case DimensionGroup_0134:
    case DimensionGroup_0135:
    case DimensionGroup_0145:
    case DimensionGroup_0234:
    case DimensionGroup_0235:
    case DimensionGroup_0245:
    case DimensionGroup_0345:
    case DimensionGroup_01234:
    case DimensionGroup_01235:
    case DimensionGroup_01245:
    case DimensionGroup_01345:
    case DimensionGroup_02345:
    case DimensionGroup_012345:
      return 0;

    case DimensionGroup_1:
    case DimensionGroup_12:
    case DimensionGroup_13:
    case DimensionGroup_14:
    case DimensionGroup_15:
    case DimensionGroup_123:
    case DimensionGroup_124:
    case DimensionGroup_125:
    case DimensionGroup_134:
    case DimensionGroup_135:
    case DimensionGroup_145:
    case DimensionGroup_1234:
    case DimensionGroup_1235:
    case DimensionGroup_1245:
    case DimensionGroup_1345:
    case DimensionGroup_12345:
      return 1;

    case DimensionGroup_2:
    case DimensionGroup_23:
    case DimensionGroup_24:
    case DimensionGroup_25:
    case DimensionGroup_234:
    case DimensionGroup_235:
    case DimensionGroup_245:
    case DimensionGroup_2345:
      return 2;

    case DimensionGroup_3:
    case DimensionGroup_34:
    case DimensionGroup_35:
    case DimensionGroup_345:
      return 3;

    case DimensionGroup_4:
    case DimensionGroup_45:
      return 4;

    case DimensionGroup_5:
      return 5;

    default:
      return -1;
    }
  case 1:
    switch(dimensionGroup)
    {
    case DimensionGroup_01:
    case DimensionGroup_012:
    case DimensionGroup_013:
    case DimensionGroup_014:
    case DimensionGroup_015:
    case DimensionGroup_0123:
    case DimensionGroup_0124:
    case DimensionGroup_0125:
    case DimensionGroup_0134:
    case DimensionGroup_0135:
    case DimensionGroup_0145:
    case DimensionGroup_01234:
    case DimensionGroup_01235:
    case DimensionGroup_01245:
    case DimensionGroup_01345:
    case DimensionGroup_012345:
      return 1;

    case DimensionGroup_02:
    case DimensionGroup_12:
    case DimensionGroup_023:
    case DimensionGroup_024:
    case DimensionGroup_025:
    case DimensionGroup_123:
    case DimensionGroup_124:
    case DimensionGroup_125:
    case DimensionGroup_0234:
    case DimensionGroup_0235:
    case DimensionGroup_0245:
    case DimensionGroup_1234:
    case DimensionGroup_1235:
    case DimensionGroup_1245:
    case DimensionGroup_02345:
    case DimensionGroup_12345:
      return 2;

    case DimensionGroup_03:
    case DimensionGroup_13:
    case DimensionGroup_23:
    case DimensionGroup_034:
    case DimensionGroup_035:
    case DimensionGroup_134:
    case DimensionGroup_135:
    case DimensionGroup_234:
    case DimensionGroup_235:
    case DimensionGroup_0345:
    case DimensionGroup_1345:
    case DimensionGroup_2345:
      return 3;

    case DimensionGroup_04:
    case DimensionGroup_14:
    case DimensionGroup_24:
    case DimensionGroup_34:
    case DimensionGroup_045:
    case DimensionGroup_145:
    case DimensionGroup_245:
    case DimensionGroup_345:
      return 4;

    case DimensionGroup_05:
    case DimensionGroup_15:
    case DimensionGroup_25:
    case DimensionGroup_35:
    case DimensionGroup_45:
      return 5;

    default:
      return -1;
    }
  case 2:
    switch(dimensionGroup)
    {
    case DimensionGroup_012:
    case DimensionGroup_0123:
    case DimensionGroup_0124:
    case DimensionGroup_0125:
    case DimensionGroup_01234:
    case DimensionGroup_01235:
    case DimensionGroup_01245:
    case DimensionGroup_012345:
      return 2;

    case DimensionGroup_013:
    case DimensionGroup_023:
    case DimensionGroup_123:
    case DimensionGroup_0134:
    case DimensionGroup_0135:
    case DimensionGroup_0234:
    case DimensionGroup_0235:
    case DimensionGroup_1234:
    case DimensionGroup_1235:
    case DimensionGroup_01345:
    case DimensionGroup_02345:
    case DimensionGroup_12345:
      return 3;

    case DimensionGroup_014:
    case DimensionGroup_024:
    case DimensionGroup_034:
    case DimensionGroup_124:
    case DimensionGroup_134:
    case DimensionGroup_234:
    case DimensionGroup_0145:
    case DimensionGroup_0245:
    case DimensionGroup_0345:
    case DimensionGroup_1245:
    case DimensionGroup_1345:
    case DimensionGroup_2345:
      return 4;

    case DimensionGroup_015:
    case DimensionGroup_025:
    case DimensionGroup_035:
    case DimensionGroup_045:
    case DimensionGroup_125:
    case DimensionGroup_135:
    case DimensionGroup_145:
    case DimensionGroup_235:
    case DimensionGroup_245:
    case DimensionGroup_345:
      return 5;

    default:
      return -1;
    }
  case 3:
    switch(dimensionGroup)
    {
    case DimensionGroup_0123:
    case DimensionGroup_01234:
    case DimensionGroup_01235:
    case DimensionGroup_012345:
      return 3;

    case DimensionGroup_0124:
    case DimensionGroup_0134:
    case DimensionGroup_0234:
    case DimensionGroup_1234:
    case DimensionGroup_01245:
    case DimensionGroup_01345:
    case DimensionGroup_02345:
    case DimensionGroup_12345:
      return 4;

    case DimensionGroup_0125:
    case DimensionGroup_0135:
    case DimensionGroup_0145:
    case DimensionGroup_0235:
    case DimensionGroup_0245:
    case DimensionGroup_0345:
    case DimensionGroup_1235:
    case DimensionGroup_1245:
    case DimensionGroup_1345:
    case DimensionGroup_2345:
      return 5;

    default:
      return -1;
    }
  case 4:
    switch(dimensionGroup)
    {
    case DimensionGroup_01234:
      return 4;
    case DimensionGroup_01235:
    case DimensionGroup_01245:
    case DimensionGroup_01345:
    case DimensionGroup_02345:
    case DimensionGroup_12345:
      return 5;
    default:
      return -1;
    }
  case 5:
    switch(dimensionGroup)
    {
    case DimensionGroup_012345:
      return 5;
    default:
      return -1;
    }
  default:
    return -1;
  }
}

DimensionGroup getDimensionGroupFromDimensionIndices(int dimension0)
{
  switch(dimension0)
  {
  case 0:
    return DimensionGroup_0;
  case 1:
    return DimensionGroup_1;
  case 2:
    return DimensionGroup_2;
  case 3:
    return DimensionGroup_3;
  case 4:
    return DimensionGroup_4;
  case 5:
    return DimensionGroup_5;
  default:
    return DimensionGroup_Max;
  }
}

DimensionGroup getDimensionGroupFromDimensionIndices(int dimension0, int dimension1)
{
  if(dimension1 == -1) return getDimensionGroupFromDimensionIndices(dimension0);

  switch(dimension0)
  {
  case 0:
    switch(dimension1)
    {
    case 1:
      return DimensionGroup_01;
    case 2:
      return DimensionGroup_02;
    case 3:
      return DimensionGroup_03;
    case 4:
      return DimensionGroup_04;
    case 5:
      return DimensionGroup_05;
    default:
      return DimensionGroup_Max;
    }
  case 1:
    switch(dimension1)
    {
    case 2:
      return DimensionGroup_12;
    case 3:
      return DimensionGroup_13;
    case 4:
      return DimensionGroup_14;
    case 5:
      return DimensionGroup_15;
    default:
      return DimensionGroup_Max;
    }
  case 2:
    switch(dimension1)
    {
    case 3:
      return DimensionGroup_23;
    case 4:
      return DimensionGroup_24;
    case 5:
      return DimensionGroup_25;
    default:
      return DimensionGroup_Max;
    }
  case 3:
    switch(dimension1)
    {
    case 4:
      return DimensionGroup_34;
    case 5:
      return DimensionGroup_35;
    default:
      return DimensionGroup_Max;
    }
  case 4:
    switch(dimension1)
    {
    case 5:
      return DimensionGroup_45;
    default:
      return DimensionGroup_Max;
    }
  case 5:
    return DimensionGroup_Max;
  default:
    return DimensionGroup_Max;
  }
}

DimensionGroup getDimensionGroupFromDimensionIndices(int dimension0, int dimension1, int dimension2)
{
  if(dimension2 == -1) return getDimensionGroupFromDimensionIndices(dimension0, dimension1);

  switch(dimension0)
  {
  case 0:
    switch(dimension1)
    {
    case 1:
      switch(dimension2)
      {
      case 2:
        return DimensionGroup_012;
      case 3:
        return DimensionGroup_013;
      case 4:
        return DimensionGroup_014;
      case 5:
        return DimensionGroup_015;
      default:
        return DimensionGroup_Max;
      }
    case 2:
      switch(dimension2)
      {
      case 3:
        return DimensionGroup_023;
      case 4:
        return DimensionGroup_024;
      case 5:
        return DimensionGroup_025;
      default:
        return DimensionGroup_Max;
      }
    case 3:
      switch(dimension2)
      {
      case 4:
        return DimensionGroup_034;
      case 5:
        return DimensionGroup_035;
      default:
        return DimensionGroup_Max;
      }
    case 4:
      if(dimension2 == 5)
      {
        return DimensionGroup_045;
      }
      else
      {
        return DimensionGroup_Max;
      }
    default:
      return DimensionGroup_Max;
    }
  case 1:
    switch(dimension1)
    {
    case 2:
      switch(dimension2)
      {
      case 3:
        return DimensionGroup_123;
      case 4:
        return DimensionGroup_124;
      case 5:
        return DimensionGroup_125;
      default:
        return DimensionGroup_Max;
      }
    case 3:
      switch(dimension2)
      {
      case 4:
        return DimensionGroup_134;
      case 5:
        return DimensionGroup_135;
      default:
        return DimensionGroup_Max;
      }
    case 4:
      if(dimension2 == 5)
      {
        return DimensionGroup_145;
      }
      else
      {
        return DimensionGroup_Max;
      }
    default:
      return DimensionGroup_Max;
    }
  case 2:
    switch(dimension1)
    {
    case 3:
      if(dimension2 == 4)
      {
        return DimensionGroup_234;
      }
      else if(dimension2 == 5)
      {
        return DimensionGroup_235;
      }
      else
      {
        return DimensionGroup_Max;
      }
    case 4:
      if(dimension2 == 5)
      {
        return DimensionGroup_245;
      }
      else
      {
        return DimensionGroup_Max;
      }
    default:
      return DimensionGroup_Max;
    }
  case 3:
    if(dimension1 == 4 && dimension2 == 5)
    {
      return DimensionGroup_345;
    }
    else
    {
      return DimensionGroup_Max;
    }
  default:
    return DimensionGroup_Max;
  }
}

DimensionGroup getDimensionGroupFromDimensionIndices(int dimension0, int dimension1, int dimension2, int dimension3)
{
  if(dimension3 == -1) return getDimensionGroupFromDimensionIndices(dimension0, dimension1, dimension2);

  switch(dimension0)
  {
  case 0:
    switch(dimension1)
    {
    case 1:
      switch(dimension2)
      {
      case 2:
        switch(dimension3)
        {
        case 3:
          return DimensionGroup_0123;
        case 4:
          return DimensionGroup_0124;
        case 5:
          return DimensionGroup_0125;
        default:
          return DimensionGroup_Max;
        }
      case 3:
        switch(dimension3)
        {
        case 4:
          return DimensionGroup_0134;
        case 5:
          return DimensionGroup_0135;
        default:
          return DimensionGroup_Max;
        }
      case 4:
        if(dimension3 == 5)
        {
          return DimensionGroup_0145;
        }
        else
        {
          return DimensionGroup_Max;
        }
      default:
        return DimensionGroup_0345;
      }
    case 2:
    case 3:
      if(dimension2 == 4 && dimension3 == 5)
      {
        return DimensionGroup_0345;
      }
      else
      {
        return DimensionGroup_Max;
      }
    default:
      return DimensionGroup_Max;
    }
  case 1:
    switch(dimension1)
    {
    case 2:
      switch(dimension2)
      {
      case 3:
        switch(dimension3)
        {
        case 4:
          return DimensionGroup_1234;
        case 5:
          return DimensionGroup_1235;
        default:
          return DimensionGroup_Max;
        }
      case 4:
        if(dimension3 == 5)
        {
          return DimensionGroup_1245;
        }
        else
        {
          return DimensionGroup_Max;
        }
      default:
        return DimensionGroup_Max;
      }
    case 3:
      if(dimension2 == 4 && dimension3 == 5)
      {
        return DimensionGroup_1345;
      }
      else
      {
        return DimensionGroup_Max;
      }
    default:
      return DimensionGroup_Max;
    }
  case 2:
    if(dimension1 == 3 && dimension2 == 4 && dimension3 == 5)
    {
      return DimensionGroup_2345;
    }
    else
    {
      return DimensionGroup_Max;
    }
  default:
    return DimensionGroup_Max;
  }
}

DimensionGroup getDimensionGroupFromDimensionIndices(int dimension0, int dimension1, int dimension2, int dimension3, int dimension4)
{
  if(dimension4 == -1) return getDimensionGroupFromDimensionIndices(dimension0, dimension1, dimension2, dimension3);
  if(dimension0 == 0)
  {
    if(dimension1 == 1)
    {
      if(dimension2 == 2)
      {
        if(dimension3 == 3)
        {
          if(dimension4 == 4)
          {
            return DimensionGroup_01234;
          }
          else if(dimension4 == 5)
          {
            return DimensionGroup_01235;
          }
          else
          {
            return DimensionGroup_Max;
          }
        }
        else if(dimension3 == 4 && dimension4 == 5)
        {
          return DimensionGroup_01245;
        }
        else
        {
          return DimensionGroup_Max;
        }
      }
      else if(dimension2 == 3 && dimension3 == 4 && dimension4 == 5)
      {
        return DimensionGroup_01345;
      }
      else
      {
        return DimensionGroup_Max;
      }
    }
    else if(dimension1 == 2 && dimension2 == 3 && dimension3 == 4 && dimension4 == 5)
    {
      return DimensionGroup_02345;
    }
    else
    {
      return DimensionGroup_Max;
    }
  }
  else if(dimension0 == 1 && dimension1 == 2 && dimension2 == 3 && dimension3 == 4 && dimension4 == 5)
  {
    return DimensionGroup_12345;
  }
  else
  {
    return DimensionGroup_Max;
  }
}

DimensionGroup getDimensionGroupFromDimensionIndices(int dimension0, int dimension1, int dimension2, int dimension3, int dimension4, int dimension5)
{
  if(dimension5 == -1) return getDimensionGroupFromDimensionIndices(dimension0, dimension1, dimension2, dimension3, dimension4);
  if(dimension0 == 0 && dimension1 == 1 && dimension2 == 2 && dimension3 == 3 && dimension4 == 4 && dimension5 == 5)
  {
    return DimensionGroup_012345;
  }
  else
  {
    return DimensionGroup_Max;
  }
}

DimensionGroup getDimensionGroupFromDimensionsND(DimensionsND dimensionsND)
{
  switch(dimensionsND)
  {
  default:
    assert(false && ("Illegal dimensions ND"));
  case Dimensions_012:
    return DimensionGroup_012;
  case Dimensions_013:
    return DimensionGroup_013;
  case Dimensions_014:
    return DimensionGroup_014;
  case Dimensions_015:
    return DimensionGroup_015;
  case Dimensions_023:
    return DimensionGroup_023;
  case Dimensions_024:
    return DimensionGroup_024;
  case Dimensions_025:
    return DimensionGroup_025;
  case Dimensions_034:
    return DimensionGroup_034;
  case Dimensions_035:
    return DimensionGroup_035;
  case Dimensions_045:
    return DimensionGroup_045;
  case Dimensions_123:
    return DimensionGroup_123;
  case Dimensions_124:
    return DimensionGroup_124;
  case Dimensions_125:
    return DimensionGroup_125;
  case Dimensions_134:
    return DimensionGroup_134;
  case Dimensions_135:
    return DimensionGroup_135;
  case Dimensions_145:
    return DimensionGroup_145;
  case Dimensions_234:
    return DimensionGroup_234;
  case Dimensions_235:
    return DimensionGroup_235;
  case Dimensions_245:
    return DimensionGroup_245;
  case Dimensions_345:
    return DimensionGroup_345;
  case Dimensions_01:
    return DimensionGroup_01;
  case Dimensions_02:
    return DimensionGroup_02;
  case Dimensions_03:
    return DimensionGroup_03;
  case Dimensions_04:
    return DimensionGroup_04;
  case Dimensions_05:
    return DimensionGroup_05;
  case Dimensions_12:
    return DimensionGroup_12;
  case Dimensions_13:
    return DimensionGroup_13;
  case Dimensions_14:
    return DimensionGroup_14;
  case Dimensions_15:
    return DimensionGroup_15;
  case Dimensions_23:
    return DimensionGroup_23;
  case Dimensions_24:
    return DimensionGroup_24;
  case Dimensions_25:
    return DimensionGroup_25;
  case Dimensions_34:
    return DimensionGroup_34;
  case Dimensions_35:
    return DimensionGroup_35;
  case Dimensions_45:
    return DimensionGroup_45;
  }
}

DimensionsND getDimensionsNDFromDimensionGroup(DimensionGroup dimensionGroup)
{
  switch(dimensionGroup)
  {
  default:
    assert(false && ("Illegal dimensiongroup"));
  case DimensionGroup_012:
    return Dimensions_012;
  case DimensionGroup_013:
    return Dimensions_013;
  case DimensionGroup_014:
    return Dimensions_014;
  case DimensionGroup_015:
    return Dimensions_015;
  case DimensionGroup_023:
    return Dimensions_023;
  case DimensionGroup_024:
    return Dimensions_024;
  case DimensionGroup_025:
    return Dimensions_025;
  case DimensionGroup_034:
    return Dimensions_034;
  case DimensionGroup_035:
    return Dimensions_035;
  case DimensionGroup_045:
    return Dimensions_045;
  case DimensionGroup_123:
    return Dimensions_123;
  case DimensionGroup_124:
    return Dimensions_124;
  case DimensionGroup_125:
    return Dimensions_125;
  case DimensionGroup_134:
    return Dimensions_134;
  case DimensionGroup_135:
    return Dimensions_135;
  case DimensionGroup_145:
    return Dimensions_145;
  case DimensionGroup_234:
    return Dimensions_234;
  case DimensionGroup_235:
    return Dimensions_235;
  case DimensionGroup_245:
    return Dimensions_245;
  case DimensionGroup_345:
    return Dimensions_345;
  case DimensionGroup_01:
    return Dimensions_01;
  case DimensionGroup_02:
    return Dimensions_02;
  case DimensionGroup_03:
    return Dimensions_03;
  case DimensionGroup_04:
    return Dimensions_04;
  case DimensionGroup_05:
    return Dimensions_05;
  case DimensionGroup_12:
    return Dimensions_12;
  case DimensionGroup_13:
    return Dimensions_13;
  case DimensionGroup_14:
    return Dimensions_14;
  case DimensionGroup_15:
    return Dimensions_15;
  case DimensionGroup_23:
    return Dimensions_23;
  case DimensionGroup_24:
    return Dimensions_24;
  case DimensionGroup_25:
    return Dimensions_25;
  case DimensionGroup_34:
    return Dimensions_34;
  case DimensionGroup_35:
    return Dimensions_35;
  case DimensionGroup_45:
    return Dimensions_45;
  }
}

const char * getDimensionGroupShortName(DimensionGroup dimensionGroup)
{
  static const char *apzDimensionNames[DimensionGroup_Max] = 
  {
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",

    "0/1",
    "0/2",
    "0/3",
    "0/4",
    "0/5",
    "1/2",
    "1/3",
    "1/4",
    "1/5",
    "2/3",
    "2/4",
    "2/5",
    "3/4",
    "3/5",
    "4/5",

    "0/1/2",
    "0/1/3",
    "0/1/4",
    "0/1/5",
    "0/2/3",
    "0/2/4",
    "0/2/5",
    "0/3/4",
    "0/3/5",
    "0/4/5",
    "1/2/3",
    "1/2/4",
    "1/2/5",
    "1/3/4",
    "1/3/5",
    "1/4/5",
    "2/3/4",
    "2/3/5",
    "2/4/5",
    "3/4/5",

    "0/1/2/3",
    "0/1/2/4",
    "0/1/2/5",
    "0/1/3/4",
    "0/1/3/5",
    "0/1/4/5",
    "0/2/3/4",
    "0/2/3/5",
    "0/2/4/5",
    "0/3/4/5",
    "1/2/3/4",
    "1/2/3/5",
    "1/2/4/5",
    "1/3/4/5",
    "2/3/4/5",

    "0/1/2/3/4",
    "0/1/2/3/5",
    "0/1/2/4/5",
    "0/1/3/4/5",
    "0/2/3/4/5",
    "1/2/3/4/5",

    "0/1/2/3/4/5"
  };

  assert(dimensionGroup >= 0 && dimensionGroup < DimensionGroup_Max);
  return apzDimensionNames[dimensionGroup];
}

const char * getDimensionGroupName(DimensionGroup dimensionGroup)
{
  static const char *apzDimensionNames[DimensionGroup_Max] = 
  {
    "Dimensions_0",
    "Dimensions_1",
    "Dimensions_2",
    "Dimensions_3",
    "Dimensions_4",
    "Dimensions_5",

    "Dimensions_01",
    "Dimensions_02",
    "Dimensions_03",
    "Dimensions_04",
    "Dimensions_05",
    "Dimensions_12",
    "Dimensions_13",
    "Dimensions_14",
    "Dimensions_15",
    "Dimensions_23",
    "Dimensions_24",
    "Dimensions_25",
    "Dimensions_34",
    "Dimensions_35",
    "Dimensions_45",

    "Dimensions_012",
    "Dimensions_013",
    "Dimensions_014",
    "Dimensions_015",
    "Dimensions_023",
    "Dimensions_024",
    "Dimensions_025",
    "Dimensions_034",
    "Dimensions_035",
    "Dimensions_045",
    "Dimensions_123",
    "Dimensions_124",
    "Dimensions_125",
    "Dimensions_134",
    "Dimensions_135",
    "Dimensions_145",
    "Dimensions_234",
    "Dimensions_235",
    "Dimensions_245",
    "Dimensions_345",

    "Dimensions_0123",
    "Dimensions_0124",
    "Dimensions_0125",
    "Dimensions_0134",
    "Dimensions_0135",
    "Dimensions_0145",
    "Dimensions_0234",
    "Dimensions_0235",
    "Dimensions_0245",
    "Dimensions_0345",
    "Dimensions_1234",
    "Dimensions_1235",
    "Dimensions_1245",
    "Dimensions_1345",
    "Dimensions_2345",

    "Dimensions_01234",
    "Dimensions_01235",
    "Dimensions_01245",
    "Dimensions_01345",
    "Dimensions_02345",
    "Dimensions_12345",

    "Dimensions_012345"
  };

  assert(dimensionGroup >= 0 && dimensionGroup < DimensionGroup_Max);
  return apzDimensionNames[dimensionGroup];
}
const char* getDimensionsGroupString(DimensionsND dimensions)
{
  switch(dimensions)
  {
  case Dimensions_012: return "012";
  case Dimensions_013: return "013";
  case Dimensions_014: return "014";
  case Dimensions_015: return "015";
  case Dimensions_023: return "023";
  case Dimensions_024: return "024";
  case Dimensions_025: return "025";
  case Dimensions_034: return "034";
  case Dimensions_035: return "035";
  case Dimensions_045: return "045";
  case Dimensions_123: return "123";
  case Dimensions_124: return "124";
  case Dimensions_125: return "125";
  case Dimensions_134: return "134";
  case Dimensions_135: return "135";
  case Dimensions_145: return "145";
  case Dimensions_234: return "234";
  case Dimensions_235: return "235";
  case Dimensions_245: return "245";
  case Dimensions_345: return "345";

  case Dimensions_01: return "01";
  case Dimensions_02: return "02";
  case Dimensions_03: return "03";
  case Dimensions_04: return "04";
  case Dimensions_05: return "05";
  case Dimensions_12: return "12";
  case Dimensions_13: return "13";
  case Dimensions_14: return "14";
  case Dimensions_15: return "15";
  case Dimensions_23: return "23";
  case Dimensions_24: return "24";
  case Dimensions_25: return "25";
  case Dimensions_34: return "34";
  case Dimensions_35: return "35";
  case Dimensions_45: return "45";
  default:
    ;
  }
  return "";
}

bool isRemappingPossible(DimensionGroup dimensionGroupA, DimensionGroup dimensionGroupB)
{
  int32_t nDimensionalityA = getDimensionality(dimensionGroupA);
  int32_t nDimensionalityB = getDimensionality(dimensionGroupB);

  if(nDimensionalityA > nDimensionalityB)
  {
    // Normalize the problem
    return isRemappingPossible(dimensionGroupB, dimensionGroupA);
  }
  else if(nDimensionalityA == nDimensionalityB)
  {
    return dimensionGroupA == dimensionGroupB;
  }
  else if(nDimensionalityA == nDimensionalityB - 1)
  {
    for(int32_t iIndexInGroup = 0; iIndexInGroup < nDimensionalityA; iIndexInGroup++)
    {
      if(!isDimensionInGroup(dimensionGroupB, getDimension(dimensionGroupA, iIndexInGroup)))
      {
        return false;
      }
    }
    return true;
  }
  else
  {
    return false;
  }
}

DimensionGroup unionGroups(DimensionGroup dimensionGroupA, DimensionGroup dimensionGroupB)
{
 int32_t nDimensions = 0;

 int32_t aiDimension[Dimensionality_Max];

  for(int32_t iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    aiDimension[iDimension] = -1;
    if(isDimensionInGroup(dimensionGroupA, iDimension) || isDimensionInGroup(dimensionGroupB, iDimension))
    {
      aiDimension[nDimensions++] = iDimension;
    }
  }
  return getDimensionGroupFromDimensionIndices(aiDimension[0], aiDimension[1], aiDimension[2], aiDimension[3], aiDimension[4], aiDimension[5]);
}

} //namespace DimensionGroupUtil
} //namespace OpenVDS
