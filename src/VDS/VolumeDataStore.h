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

#ifndef VOLUMEDATASTORE_H
#define VOLUMEDATASOTRE_H

#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/VolumeData.h>
#include <OpenVDS/OpenVDS.h>

#include "MetadataManager.h"

#include <vector>

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

class WaveletAdaptiveLevelsMetadata
{
public:
  uint8_t m_levels[MetadataStatus::WAVELET_ADAPTIVE_LEVELS];

  //void          encode(int32_t totalSize, int32_t const (&adaptiveLevels)[MetadataStatus::WAVELET_ADAPTIVE_LEVELS]);
  //int32_t       decode(int32_t totalSize, int32_t targetLevel) const;
  //void          accumulateSizes(int32_t totalSize, int64_t (&adaptiveLevelSizes)[MetadataStatus::WAVELET_ADAPTIVE_LEVELS], bool isSubtract) const;
  //void          dropLosslessData();
  //void          shift(int32_t nLevels);
  //void          clear() { memset(m_levels, 0x00, sizeof(m_levels)); }
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

struct VolumeDataStore
{
  static bool verify(const VolumeDataChunk& volumeDataChunk, const std::vector<uint8_t>& serializedData, CompressionMethod compressionMethod, bool isFullyRead);
  static bool deserializeVolumeData(const VolumeDataChunk& chunk, const std::vector<uint8_t>& serializedData, const std::vector<uint8_t>& metadata, CompressionMethod compressionMethod, int32_t adaptiveLevel, VolumeDataChannelDescriptor::Format loadFormat, std::vector<uint8_t>& target, Error& error);
};

}

#endif //VOLUMEDATASTORE_H