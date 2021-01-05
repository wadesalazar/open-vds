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

bool InitializeDataBlock(const DataBlockDescriptor &descriptor, DataBlock &dataBlock, Error &error)
{
  int32_t size[DataBlock::Dimensionality_Max];
  size[0] = descriptor.SizeX;
  size[1] = descriptor.SizeY;
  size[2] = descriptor.SizeZ;
  return InitializeDataBlock(descriptor.Format, descriptor.Components, enum DataBlock::Dimensionality(descriptor.Dimensionality), size, dataBlock, error);
}

bool InitializeDataBlock(VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, enum DataBlock::Dimensionality dimensionality, int32_t (&size)[DataBlock::Dimensionality_Max], DataBlock &dataBlock, Error &error)
{
  dataBlock.Components = components;
  dataBlock.Format = format;
  dataBlock.Dimensionality = dimensionality;

  switch(dimensionality)
  {
  case 1:
    dataBlock.Size[0] = size[0];
    dataBlock.Size[1] = 1;
    dataBlock.Size[2] = 1;
    dataBlock.Size[3] = 1;
    break;
  case 2:
    dataBlock.Size[0] = size[0];
    dataBlock.Size[1] = size[1];
    dataBlock.Size[2] = 1;
    dataBlock.Size[3] = 1;
    break;
  case 3:
    dataBlock.Size[0] = size[0];
    dataBlock.Size[1] = size[1];
    dataBlock.Size[2] = size[2];
    dataBlock.Size[3] = 1;
    break;
  default:
    error.string = "Serialized datablock has illegal dimensionality";
    error.code = -1;
    return false;
  }

  dataBlock.AllocatedSize[0] = (format == VolumeDataChannelDescriptor::Format_1Bit) ? ((dataBlock.Size[0] * components) + 7) / 8 : GetAllocatedByteSizeForSize(dataBlock.Size[0]);
  dataBlock.AllocatedSize[1] = GetAllocatedByteSizeForSize(dataBlock.Size[1]);
  dataBlock.AllocatedSize[2] = GetAllocatedByteSizeForSize(dataBlock.Size[2]);
  dataBlock.AllocatedSize[3] = GetAllocatedByteSizeForSize(dataBlock.Size[3]);
  dataBlock.Pitch[0] = 1;
  for (int i = 1; i < DataBlock::Dimensionality_Max; i++)
  {
    dataBlock.Pitch[i] = dataBlock.Pitch[i - 1] * dataBlock.AllocatedSize[i - 1];
  }

  uint64_t allocatedByteSize = dataBlock.AllocatedSize[0];

  for (int i = 1; i < DataBlock::Dimensionality_Max; i++)
  {
    allocatedByteSize *= dataBlock.AllocatedSize[i];
  }

  if (allocatedByteSize * GetElementSize(dataBlock.Format, dataBlock.Components) > 0x7FFFFFFF)
  {
    char buffer[4096];
    snprintf(buffer, sizeof(buffer), "Datablock is too big (%d x %d x %d x %d x %d bytes)", dataBlock.AllocatedSize[0], dataBlock.AllocatedSize[1], dataBlock.AllocatedSize[2], dataBlock.AllocatedSize[3], GetElementSize(dataBlock.Format, dataBlock.Components));
    error.string = buffer;
    error.code = -1;
    return false;
  }
  return true;
}

}
