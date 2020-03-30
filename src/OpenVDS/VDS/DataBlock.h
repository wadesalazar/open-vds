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
  VolumeDataChannelDescriptor::Format Format;
  VolumeDataChannelDescriptor::Components Components;
  enum Dimensionality Dimensionality;
  int32_t Size[DataStoreDimensionality_Max];
  int32_t AllocatedSize[DataStoreDimensionality_Max];
  int32_t Pitch[DataStoreDimensionality_Max];
};

class DataBlockDescriptor
{
public:
  //This layout is fixed
  int32_t Dimensionality;

  int32_t SizeX;
  int32_t SizeY;
  int32_t SizeZ;

  VolumeDataChannelDescriptor::Format Format;
  VolumeDataChannelDescriptor::Components Components;


  bool IsValid(const int32_t (&voxelSize)[DataStoreDimensionality_Max]) const
  {
    return Dimensionality >= 1 &&
           Dimensionality <= 3 &&
           (SizeX == voxelSize[0]                        ) &&
           (SizeY == voxelSize[1] || Dimensionality < 2) &&
           (SizeZ == voxelSize[2] || Dimensionality < 3) &&
           (Format == VolumeDataChannelDescriptor::Format_1Bit ||
            Format == VolumeDataChannelDescriptor::Format_U8   ||
            Format == VolumeDataChannelDescriptor::Format_U16  ||
            Format == VolumeDataChannelDescriptor::Format_U32  ||
            Format == VolumeDataChannelDescriptor::Format_U64  ||
            Format == VolumeDataChannelDescriptor::Format_R32  ||
            Format == VolumeDataChannelDescriptor::Format_R64) &&
           (Components == VolumeDataChannelDescriptor::Components_1 ||
            Components == VolumeDataChannelDescriptor::Components_2 ||
            Components == VolumeDataChannelDescriptor::Components_4);
  }

  bool IsValid() const { int32_t voxelSize[DataStoreDimensionality_Max] = {SizeX, SizeY, SizeZ}; return IsValid(voxelSize); }
};

bool InitializeDataBlock(const DataBlockDescriptor &descriptor, DataBlock &dataBlock, Error &error);
bool InitializeDataBlock(VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, Dimensionality dimensionality, int32_t (&size)[DataStoreDimensionality_Max], DataBlock &dataBlock, Error &error);

static int32_t GetVoxelFormatByteSize(VolumeDataChannelDescriptor::Format format)
{
  int32_t iRetval = -1;
  switch (format) {
  case VolumeDataChannelDescriptor::Format_R64:
  case VolumeDataChannelDescriptor::Format_U64:
    iRetval = 8;
    break;
  case VolumeDataChannelDescriptor::Format_R32:
  case VolumeDataChannelDescriptor::Format_U32:
    iRetval = 4;
    break;
  case VolumeDataChannelDescriptor::Format_U16:
    iRetval = 2;
    break;
  case VolumeDataChannelDescriptor::Format_U8:
  case VolumeDataChannelDescriptor::Format_1Bit:
    iRetval =1;
    break;
  default:
    fprintf(stderr, "Unknown voxel format");
    abort();
  }

  return iRetval;
}

static uint32_t GetElementSize(VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components)
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

inline uint32_t GetElementSize(const DataBlock &datablock)
{
  return GetElementSize(datablock.Format, datablock.Components);
}

inline uint32_t GetByteSize(const int32_t (&size)[DataStoreDimensionality_Max], VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, bool isBitSize = true)
{
  int byteSize = size[0] * GetElementSize(format, components);

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

inline uint32_t GetByteSize(const DataBlock &block)
{
  return GetByteSize(block.Size, block.Format, block.Components);
}

inline uint32_t GetAllocatedByteSize(const DataBlock &block)
{
  return GetByteSize(block.AllocatedSize, block.Format, block.Components, false);
}

inline int32_t GetAllocatedByteSizeForSize(const int32_t size)
{
  return size == 1 ? 1 : (size + 7) & -8;
}

}

#endif //DATABLOCK_H
