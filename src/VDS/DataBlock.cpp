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

#include "DataBlock.h"

namespace OpenVDS
{

bool initializeDataBlock(const DataBlockDescriptor &descriptor, DataBlock &dataBlock, Error &error)
{
  int32_t size[DataStoreDimensionality_Max];
  size[0] = descriptor.sizeX;
  size[1] = descriptor.sizeY;
  size[2] = descriptor.sizeZ;
  return initializeDataBlock(descriptor.format, descriptor.components, Dimensionality(descriptor.dimensionality), size, dataBlock, error);
}

bool initializeDataBlock(VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, Dimensionality dimensionality, int32_t (&size)[DataStoreDimensionality_Max], DataBlock &dataBlock, Error &error)
{
  dataBlock.components = components;
  dataBlock.format = format;
  dataBlock.dimensionality = dimensionality;

  switch(dimensionality)
  {
  case 1:
    dataBlock.size[0] = size[0];
    dataBlock.size[1] = 1;
    dataBlock.size[2] = 1;
    dataBlock.size[3] = 1;
    break;
  case 2:
    dataBlock.size[0] = size[0];
    dataBlock.size[1] = size[1];
    dataBlock.size[2] = 1;
    dataBlock.size[3] = 1;
    break;
  case 3:
    dataBlock.size[0] = size[0];
    dataBlock.size[1] = size[1];
    dataBlock.size[2] = size[2];
    dataBlock.size[3] = 1;
    break;
  default:
    error.string = "Serialized datablock has illegal dimensionality";
    error.code = -1;
    return false;
  }

  dataBlock.allocatedSize[0] = (format == VolumeDataChannelDescriptor::Format_1Bit) ? ((dataBlock.size[0] * components) + 7) / 8 : getAllocatedByteSizeForSize(dataBlock.size[0]);
  dataBlock.allocatedSize[1] = getAllocatedByteSizeForSize(dataBlock.size[1]);
  dataBlock.allocatedSize[2] = getAllocatedByteSizeForSize(dataBlock.size[2]);
  dataBlock.allocatedSize[3] = getAllocatedByteSizeForSize(dataBlock.size[3]);
  dataBlock.pitch[0] = 1;
  for (int i = 1; i < DataStoreDimensionality_Max; i++)
  {
    dataBlock.pitch[i] = dataBlock.pitch[i - 1] * dataBlock.allocatedSize[i - 1];
  }

  uint64_t allocatedByteSize = dataBlock.allocatedSize[0];

  for (int i = 1; i < DataStoreDimensionality_Max; i++)
  {
    allocatedByteSize *= dataBlock.allocatedSize[i];
  }

  if (allocatedByteSize * getElementSize(dataBlock.format, dataBlock.components) > 0x7FFFFFFF)
  {
    char buffer[4096];
    snprintf(buffer, sizeof(buffer), "Datablock is too big (%d x %d x %d x %d x %d bytes)", dataBlock.allocatedSize[0], dataBlock.allocatedSize[1], dataBlock.allocatedSize[2], dataBlock.allocatedSize[3], getElementSize(dataBlock.format, dataBlock.components));
    error.string = buffer;
    error.code = -1;
    return false;
  }
  return true;
}

}