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

#ifndef DATABLOCK_H
#define DATABLOCK_H

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>

namespace OpenVDS
{

enum DataStoreDimensionality
{
  DataStoreDimensionality_1 = 1,
  DataStoreDimensionality_2 = 2,
  DataStoreDimensionality_3 = 3,
  DataStoreDimensionality_4 = 4,
  DataStoreDimensionality_Max = DataStoreDimensionality_4 
};

struct DataBlock
{
  VolumeDataChannelDescriptor::Format format;
  VolumeDataChannelDescriptor::Components components;
  Dimensionality dimensionality;
  int32_t size[DataStoreDimensionality_Max];
  int32_t allocatedSize[DataStoreDimensionality_Max];
  int32_t pitch[DataStoreDimensionality_Max];
};

class DataBlockDescriptor
{
public:
  //This layout is fixed
  int32_t dimensionality;

  int32_t sizeX;
  int32_t sizeY;
  int32_t sizeZ;

  VolumeDataChannelDescriptor::Format format;
  VolumeDataChannelDescriptor::Components components;


  bool isValid(const int32_t (&voxelSize)[DataStoreDimensionality_Max]) const
  {
    return dimensionality >= 1 &&
           dimensionality <= 3 &&
           (sizeX == voxelSize[0]                        ) &&
           (sizeY == voxelSize[1] || dimensionality < 2) &&
           (sizeZ == voxelSize[2] || dimensionality < 3) &&
           (format == VolumeDataChannelDescriptor::Format_1Bit ||
            format == VolumeDataChannelDescriptor::Format_U8   ||
            format == VolumeDataChannelDescriptor::Format_U16  ||
            format == VolumeDataChannelDescriptor::Format_U32  ||
            format == VolumeDataChannelDescriptor::Format_U64  ||
            format == VolumeDataChannelDescriptor::Format_R32  ||
            format == VolumeDataChannelDescriptor::Format_R64) &&
           (components == VolumeDataChannelDescriptor::Components_1 ||
            components == VolumeDataChannelDescriptor::Components_2 ||
            components == VolumeDataChannelDescriptor::Components_4);
  }

  bool isValid() const { int32_t voxelSize[DataStoreDimensionality_Max] = {sizeX, sizeY, sizeZ}; return isValid(voxelSize); }
};

bool initializeDataBlock(const DataBlockDescriptor &descriptor, DataBlock &dataBlock, Error &error);
bool initializeDataBlock(VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, Dimensionality dimensionality, int32_t (&size)[DataStoreDimensionality_Max], DataBlock &dataBlock, Error &error);

static uint32_t getElementSize(VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components)
{
  switch(format)
  {
  default:
    fprintf(stderr, "Illegal format");
    abort();
  case VolumeDataChannelDescriptor::Format_1Bit:
    return 1;
  case VolumeDataChannelDescriptor::Format_U8:
    return 1 * components;
  case VolumeDataChannelDescriptor::Format_U16:
    return 2 * components;
  case VolumeDataChannelDescriptor::Format_R32:
  case VolumeDataChannelDescriptor::Format_U32:
    return 4 * components;
  case VolumeDataChannelDescriptor::Format_U64:
  case VolumeDataChannelDescriptor::Format_R64:
    return 8 * components;
  }
}

static uint32_t getElementSize(const DataBlock &datablock)
{
  return getElementSize(datablock.format, datablock.components);
}

static uint32_t getByteSize(const int32_t (&size)[DataStoreDimensionality_Max], VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, bool isBitSize = true)
{
  int byteSize = size[0] * getElementSize(format, components);

  if(format == VolumeDataChannelDescriptor::Format_1Bit && isBitSize)
  {
    byteSize = (byteSize + 7) / 8;
  }

  for (int i = 1; i < DataStoreDimensionality_Max; i++)
  {
    byteSize *= size[i];
  }

  return byteSize;
}

static uint32_t getByteSize(const DataBlock &block)
{
  return getByteSize(block.size, block.format, block.components);
}

static uint32_t getAllocatedByteSize(const DataBlock &block)
{
  return getByteSize(block.allocatedSize, block.format, block.components, false);
}

}

#endif //DATABLOCK_H