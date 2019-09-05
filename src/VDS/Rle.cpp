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

#include "Rle.h"

#include <assert.h>
#include <string.h>

#define HUERLE_RUN_LENGTH_FIRST_BYTE_SHIFT    6
#define HUERLE_RUN_LENGTH_FIRST_BYTE_MASK     (0x3f)
#define HUERLE_EXTENDED_RUN_FLAG              (1 << 7)
#define HUERLE_UNIQUE_RUN_FLAG                (1 << 6)
#define HUERLE_MAX_RUN_LENGTH                 (1 << (HUERLE_RUN_LENGTH_FIRST_BYTE_SHIFT + 8))

namespace OpenVDS
{

struct RLEHeader
{
  int32_t compressedSize;
  int32_t originalSize;
  int32_t rleUnitSize;
};

template<class T>
static uint64_t rle_Pack(T value)
{
  uint16_t packed16;
  uint32_t packed32;
  uint64_t packed64;

  if(sizeof(T) < 2)
  {
    packed16 = ((uint16_t)value << 8) | (uint16_t)value;
  }
  else if(sizeof(T) == 2)
  {
    packed16 = (uint16_t)value;
  }

  if(sizeof(T) < 4)
  {
    packed32 = ((uint32_t)packed16 << 16) | (uint32_t)packed16;
  }
  else if(sizeof(T) == 4)
  {
    packed32 = (uint32_t)value;
  }

  if(sizeof(T) < 8)
  {
    packed64 = ((uint64_t)packed32 << 32) | (uint64_t)packed32;
  }
  else if(sizeof(T) == 8)
  {
    packed64 = (uint64_t)value;
  }

  return packed64;
}


template <typename T>
int32_t RLE_Decompress(uint8_t *target_parameter, int32_t nTargetSize, uint8_t *source)
{
  RLEHeader * rleHeader = (RLEHeader *)source;

  source += sizeof(RLEHeader);

  T *target = (T *)target_parameter;

  uint8_t *end = target_parameter + rleHeader->originalSize;
     
  int32_t jafs = sizeof(uint64_t) / sizeof(T);

  while ((uint8_t *)target != (uint8_t *)end)
  {
    // Get the run length (one or two bytes) from the source
    uint8_t u0 = *source++;

    int32_t runLength;

    if (u0 & HUERLE_EXTENDED_RUN_FLAG)
    {
      uint8_t u1 = *source++;

      runLength = (u0 & HUERLE_RUN_LENGTH_FIRST_BYTE_MASK) |
                   (u1 << HUERLE_RUN_LENGTH_FIRST_BYTE_SHIFT);
    }
    else
    {
      runLength = (u0 & HUERLE_RUN_LENGTH_FIRST_BYTE_MASK);
    }

    // We don't need to encode runs of 0 length, so we store one less than the actual run length
    runLength++;

    bool isUnique = u0 & HUERLE_UNIQUE_RUN_FLAG;

    if(isUnique)
    {
      memcpy(target, source, runLength * sizeof(T));
      target += runLength;
      source += runLength * sizeof(T);
    }
    else
    {
      // Get the value from the source
      T t0 = *((T *)source);

      source += sizeof(T);

      assert(!((int64_t)target & (sizeof(T) - 1)));

      // Check for long runs
      if (runLength >= 2 * jafs)
      {
        // Write data up to an aligned boundary
        while ((int64_t)target & 7)
        {
          *target++ = t0;
          runLength--;
        }

        assert(!((int64_t)target & (sizeof(T) - 1)));

        uint64_t packedValue = rle_Pack(t0);

        while (runLength >= jafs)
        {
          *(uint64_t *)target = packedValue;
          target += jafs;
          runLength -= jafs;
        }
      }

      // Write tail data (or, in short runs, ALL the data):
      while (runLength > 0)
      {
        assert((uint8_t *)target != (uint8_t *)end);

        *target++ = t0;
        runLength--;
      }
    }
  }

  return rleHeader->originalSize;
}
int32_t rle_Decompress(uint8_t *target, int32_t targetSize, uint8_t *source)
{
  RLEHeader *rleHeader = (RLEHeader *)source;

  assert(targetSize == rleHeader->originalSize);

  switch (rleHeader->rleUnitSize)
  {
    case 1:
      return RLE_Decompress<uint8_t>(target, targetSize, source);

    case 2:
      return RLE_Decompress<uint16_t>(target, targetSize, source);

    case 4:
      return RLE_Decompress<uint32_t>(target, targetSize, source);

    case 8:
      return RLE_Decompress<uint64_t>(target, targetSize, source);

    default:
      assert(("Unsupported unit size"));
  }
  return 0;
}
}