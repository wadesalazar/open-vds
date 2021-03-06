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

#ifndef RLE_H
#define RLE_H

#include <stdint.h>

namespace OpenVDS
{

struct RLEHeader
{
  int32_t compressedSize;
  int32_t originalSize;
  int32_t rleUnitSize;
};

int32_t RleDecompress(uint8_t *target, int32_t targetSize, uint8_t* source);

}

#endif //RLE_H