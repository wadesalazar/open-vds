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
  case DimensionsND::Group012:
    return DimensionGroup_012;
  case DimensionsND::Group013:
    return DimensionGroup_013;
  case DimensionsND::Group014:
    return DimensionGroup_014;
  case DimensionsND::Group015:
    return DimensionGroup_015;
  case DimensionsND::Group023:
    return DimensionGroup_023;
  case DimensionsND::Group024:
    return DimensionGroup_024;
  case DimensionsND::Group025:
    return DimensionGroup_025;
  case DimensionsND::Group034:
    return DimensionGroup_034;
  case DimensionsND::Group035:
    return DimensionGroup_035;
  case DimensionsND::Group045:
    return DimensionGroup_045;
  case DimensionsND::Group123:
    return DimensionGroup_123;
  case DimensionsND::Group124:
    return DimensionGroup_124;
  case DimensionsND::Group125:
    return DimensionGroup_125;
  case DimensionsND::Group134:
    return DimensionGroup_134;
  case DimensionsND::Group135:
    return DimensionGroup_135;
  case DimensionsND::Group145:
    return DimensionGroup_145;
  case DimensionsND::Group234:
    return DimensionGroup_234;
  case DimensionsND::Group235:
    return DimensionGroup_235;
  case DimensionsND::Group245:
    return DimensionGroup_245;
  case DimensionsND::Group345:
    return DimensionGroup_345;
  case DimensionsND::Group01:
    return DimensionGroup_01;
  case DimensionsND::Group02:
    return DimensionGroup_02;
  case DimensionsND::Group03:
    return DimensionGroup_03;
  case DimensionsND::Group04:
    return DimensionGroup_04;
  case DimensionsND::Group05:
    return DimensionGroup_05;
  case DimensionsND::Group12:
    return DimensionGroup_12;
  case DimensionsND::Group13:
    return DimensionGroup_13;
  case DimensionsND::Group14:
    return DimensionGroup_14;
  case DimensionsND::Group15:
    return DimensionGroup_15;
  case DimensionsND::Group23:
    return DimensionGroup_23;
  case DimensionsND::Group24:
    return DimensionGroup_24;
  case DimensionsND::Group25:
    return DimensionGroup_25;
  case DimensionsND::Group34:
    return DimensionGroup_34;
  case DimensionsND::Group35:
    return DimensionGroup_35;
  case DimensionsND::Group45:
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
    return DimensionsND::Group012;
  case DimensionGroup_013:
    return DimensionsND::Group013;
  case DimensionGroup_014:
    return DimensionsND::Group014;
  case DimensionGroup_015:
    return DimensionsND::Group015;
  case DimensionGroup_023:
    return DimensionsND::Group023;
  case DimensionGroup_024:
    return DimensionsND::Group024;
  case DimensionGroup_025:
    return DimensionsND::Group025;
  case DimensionGroup_034:
    return DimensionsND::Group034;
  case DimensionGroup_035:
    return DimensionsND::Group035;
  case DimensionGroup_045:
    return DimensionsND::Group045;
  case DimensionGroup_123:
    return DimensionsND::Group123;
  case DimensionGroup_124:
    return DimensionsND::Group124;
  case DimensionGroup_125:
    return DimensionsND::Group125;
  case DimensionGroup_134:
    return DimensionsND::Group134;
  case DimensionGroup_135:
    return DimensionsND::Group135;
  case DimensionGroup_145:
    return DimensionsND::Group145;
  case DimensionGroup_234:
    return DimensionsND::Group234;
  case DimensionGroup_235:
    return DimensionsND::Group235;
  case DimensionGroup_245:
    return DimensionsND::Group245;
  case DimensionGroup_345:
    return DimensionsND::Group345;
  case DimensionGroup_01:
    return DimensionsND::Group01;
  case DimensionGroup_02:
    return DimensionsND::Group02;
  case DimensionGroup_03:
    return DimensionsND::Group03;
  case DimensionGroup_04:
    return DimensionsND::Group04;
  case DimensionGroup_05:
    return DimensionsND::Group05;
  case DimensionGroup_12:
    return DimensionsND::Group12;
  case DimensionGroup_13:
    return DimensionsND::Group13;
  case DimensionGroup_14:
    return DimensionsND::Group14;
  case DimensionGroup_15:
    return DimensionsND::Group15;
  case DimensionGroup_23:
    return DimensionsND::Group23;
  case DimensionGroup_24:
    return DimensionsND::Group24;
  case DimensionGroup_25:
    return DimensionsND::Group25;
  case DimensionGroup_34:
    return DimensionsND::Group34;
  case DimensionGroup_35:
    return DimensionsND::Group35;
  case DimensionGroup_45:
    return DimensionsND::Group45;
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
