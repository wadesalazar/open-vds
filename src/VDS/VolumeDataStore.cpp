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

#include "VolumeDataStore.h"

#include "VolumeDataLayout.h"
#include "Wavelet.h"
#include "VolumeDataHash.h"
#include "Rle.h"
#include "DataBlock.h"
#include "ValueConversion.h"


#include <stdlib.h>
#include <assert.h>

#include <zlib.h>

namespace OpenVDS
{
static bool compressionMethodIsWavelet(CompressionMethod compressionMethod)
{
  return compressionMethod == CompressionMethod::Wavelet ||
         compressionMethod == CompressionMethod::WaveletNormalizeBlock ||
         compressionMethod == CompressionMethod::WaveletLossless ||
         compressionMethod == CompressionMethod::WaveletNormalizeBlockLossless;
}

static uint32_t getByteSize(const DataBlockDescriptor &descriptor)
{
  int32_t size[DataStoreDimensionality_Max];
  size[0] = descriptor.sizeX;
  size[1] = descriptor.sizeY;
  size[2] = descriptor.sizeZ;
  size[3] = 1;
  return getByteSize(size, descriptor.format, descriptor.components);
}


bool VolumeDataStore::verify(const VolumeDataChunk &volumeDataChunk, const std::vector<uint8_t> &serializedData, CompressionMethod compressionMethod, bool isFullyRead)
{
  bool isValid = false;

  int32_t voxelSize[DataStoreDimensionality_Max];

  volumeDataChunk.layer->getChunkVoxelSize(volumeDataChunk.chunkIndex, voxelSize);

  if(serializedData.empty())
  {
    isValid = true;
  }
  else if(compressionMethodIsWavelet(compressionMethod))
  {
    if(serializedData.size() >= sizeof(int32_t) * 6)
    {
      const int32_t *waveletHeader = (const int32_t *)serializedData.data();

      int32_t dataVersion    = waveletHeader[0];
      int32_t compressedSize = waveletHeader[1];
      int32_t createSizeX    = waveletHeader[2];
      int32_t createSizeY    = waveletHeader[3];
      int32_t createSizeZ    = waveletHeader[4];
      int32_t dimensions     = waveletHeader[5];

      isValid = dataVersion == WAVELET_DATA_VERSION_1_4 &&
                (compressedSize <= serializedData.size() || !isFullyRead) &&
                (createSizeX == voxelSize[0]                  ) &&
                (createSizeY == voxelSize[1] || dimensions < 2) &&
                (createSizeZ == voxelSize[2] || dimensions < 3) &&
                dimensions >= 1 &&
                dimensions <= 3;
    }
  }
  else if(compressionMethod == CompressionMethod::None ||
          compressionMethod == CompressionMethod::RLE ||
          compressionMethod == CompressionMethod::Zip)
  {
    if(serializedData.size() > sizeof(DataBlockDescriptor))
    {
      DataBlockDescriptor *serializedDescriptor = (DataBlockDescriptor*)serializedData.data();
      isValid = serializedDescriptor->isValid(voxelSize);
    }
  }

  return isValid;
}

static void copyLinearBufferIntoDataBlock(const void *sourceBuffer, const DataBlock &dataBlock, std::vector<uint8_t> &targetBuffer)
{

  int32_t sizeX = dataBlock.size[0];
  int32_t sizeY = dataBlock.size[1];
  int32_t sizeZ = dataBlock.size[2];

  int32_t allocatedSizeX = dataBlock.allocatedSize[0];
  int32_t allocatedSizeY = dataBlock.allocatedSize[1];
  int32_t allocatedSizeZ = dataBlock.allocatedSize[2];

  if(dataBlock.format == VolumeDataChannelDescriptor::Format_1Bit)
  {
    sizeX = ((sizeX * dataBlock.components) + 7) / 8;
  }

  uint32_t elementSize = getElementSize(dataBlock);
  uint32_t byteSize = sizeX * sizeY * sizeZ * elementSize;

  for(int32_t iZ = 0; iZ < sizeZ; iZ++)
  {
    for(int32_t iY = 0; iY < sizeY; iY++)
    {
      uint8_t *target = targetBuffer.data()                       +  (iZ * allocatedSizeY + iY) * allocatedSizeX * elementSize;
      const uint8_t *source = static_cast<const uint8_t*>(sourceBuffer) +  (iZ * sizeY          + iY) * sizeX          * elementSize;
      memcpy(target, source, sizeX * elementSize);
    }
  }
}

bool deserializeVolumeData(const std::vector<uint8_t> &serializedData, VolumeDataChannelDescriptor::Format format, CompressionMethod compressionMethod, const FloatRange &valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, int32_t adaptiveLevel, DataBlock &dataBlock, std::vector<uint8_t> &destination, Error &error)
{
  if(compressionMethodIsWavelet(compressionMethod))
  {
    const void *data = serializedData.data();

    int32_t dataVersion = ((int32_t *)data)[0];
    int32_t compressedSize = ((int32_t *)data)[1];

    assert(dataVersion == WAVELET_DATA_VERSION_1_4);

    bool isNormalize = false;
    bool isLossless = false;

    if((compressionMethod == CompressionMethod::WaveletNormalizeBlock) ||
       (compressionMethod == CompressionMethod::WaveletNormalizeBlockLossless))
    {
      isNormalize = true;
    }

    if((compressionMethod == CompressionMethod::WaveletLossless) ||
       (compressionMethod == CompressionMethod::WaveletNormalizeBlockLossless))
    {
      isLossless = true;
    }

    // if adaptive level is 0 or more, don't use lossless data
    if (adaptiveLevel >= 0)
    {
      isLossless = false;
    }
    else
    {
      assert(adaptiveLevel == -1);
      // Now we can use lossless if there is lossless data - set iAdaptivelevel to max adaptive.
      adaptiveLevel = 0;
    }

    if (!Wavelet_Decompress(data, int32_t(serializedData.size()), format, valueRange, integerScale, integerOffset, isUseNoValue, noValue, isNormalize, adaptiveLevel, isLossless, dataBlock, destination, error))
      return false;
  }
  else if(compressionMethod == CompressionMethod::RLE)
  {
    DataBlockDescriptor *dataBlockDescriptor = (DataBlockDescriptor *)serializedData.data();

    if(!dataBlockDescriptor->isValid())
    {
      error.code = -1;
      error.string = "Failed to decode DataBlockDescriptor";
      return false;
    }

    if (!initializeDataBlock(*dataBlockDescriptor, dataBlock, error))
      return false;

    void * source = dataBlockDescriptor + 1;

    int32_t byteSize = getByteSize(*dataBlockDescriptor);
    std::unique_ptr<uint8_t[]>buffer(new uint8_t[byteSize]);

    int32_t decompressedSize = rle_Decompress((uint8_t *)buffer.get(), byteSize, (uint8_t *)source);
    assert(decompressedSize == byteSize);

    int allocatedSize = getAllocatedByteSize(dataBlock);
    destination.resize(allocatedSize);
    copyLinearBufferIntoDataBlock(buffer.get(), dataBlock, destination);
  }
  else if(compressionMethod == CompressionMethod::Zip)
  {
    DataBlockDescriptor *dataBlockDescriptor = (DataBlockDescriptor *)serializedData.data();

    if(!dataBlockDescriptor->isValid())
    {
      error.code = -1;
      error.string = "Failed to decode DataBlockDescriptor";
      return false;
    }
    if (!initializeDataBlock(*dataBlockDescriptor, dataBlock, error))
      return false;

    void * source = dataBlockDescriptor + 1;

    int32_t byteSize = getByteSize(*dataBlockDescriptor);
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[byteSize]);

    unsigned long destLen = byteSize;

    int status = uncompress(buffer.get(), &destLen, (uint8_t *)source, uint32_t(serializedData.size() - sizeof(DataBlockDescriptor)));

    if (status != Z_OK)
    {
      fprintf(stderr, "zlib uncompress failed (status %d) in VolumeDataStore_c::DeSerialize\n", status);
      return NULL;
    }

    int allocatedSize = getAllocatedByteSize(dataBlock);
    destination.resize(allocatedSize);
    copyLinearBufferIntoDataBlock(buffer.get(), dataBlock, destination);
  }
  else if(compressionMethod == CompressionMethod::None)
  {
    DataBlockDescriptor *dataBlockDescriptor = (DataBlockDescriptor *)serializedData.data();

    if(!dataBlockDescriptor->isValid())
    {
      error.code = -1;
      error.string = "Failed to decode DataBlockDescriptor";
      return false;
    }
    if (!initializeDataBlock(*dataBlockDescriptor, dataBlock, error))
      return false;

    void * source = dataBlockDescriptor + 1;

    int32_t byteSize = getAllocatedByteSize(dataBlock);
    destination.resize(byteSize);
    copyLinearBufferIntoDataBlock(source, dataBlock, destination);
  }

  if(dataBlock.format != format)
  {
    error.string = "Formats doesn't match in deserialization\n";
    error.code = -2;
    return false;
  }
  return true;
}

static float getConvertedConstantValue(VolumeDataChannelDescriptor const &volumeDataChannelDescriptor, VolumeDataChannelDescriptor::Format format, float noValue, VolumeDataHash const &constantValueVolumeDataHash)
{
  if(format == VolumeDataChannelDescriptor::Format_1Bit)
  {
    return float(constantValueVolumeDataHash.getConstantValue(0) != 0);
  }
  else if(format == VolumeDataChannelDescriptor::Format_U8 || format == VolumeDataChannelDescriptor::Format_U16)
  {
    if(constantValueVolumeDataHash.isNoValue())
    {
      if(format == VolumeDataChannelDescriptor::Format_U8)
      {
        return convertNoValue<uint8_t>(noValue);
      }
      else
      {
        assert(format == VolumeDataChannelDescriptor::Format_U16);
        return convertNoValue<uint16_t>(noValue);
      }
    }

    int32_t buckets = (format == VolumeDataChannelDescriptor::Format_U8 ? 256 : 65536) - volumeDataChannelDescriptor.isUseNoValue();

    float reciprocalScale;
    float offset;

    if(volumeDataChannelDescriptor.getFormat() != VolumeDataChannelDescriptor::Format_U8 && volumeDataChannelDescriptor.getFormat() != VolumeDataChannelDescriptor::Format_U16)
    {
      offset = volumeDataChannelDescriptor.getValueRange().min;
      reciprocalScale = float(buckets - 1) / rangeSize(volumeDataChannelDescriptor.getValueRange());
    }
    else
    {
      offset = volumeDataChannelDescriptor.getIntegerOffset();
      reciprocalScale = 1.0f / volumeDataChannelDescriptor.getIntegerScale();
    }

    return (float)quantizeValueWithReciprocalScale(constantValueVolumeDataHash.getConstantValue(noValue), offset, reciprocalScale, buckets);
  }
  else
  {
    assert(format == VolumeDataChannelDescriptor::Format_R32 || format == VolumeDataChannelDescriptor::Format_U32 || format == VolumeDataChannelDescriptor::Format_R64 || format == VolumeDataChannelDescriptor::Format_U64);
    return constantValueVolumeDataHash.getConstantValue(noValue);
  }
}

template <typename T>
static void fillConstantValueBuffer(std::vector<uint8_t> &buffer, int32_t allocatedElements, float value)
{
  T v = convertValue<T>(value);
  T *b = reinterpret_cast<T *>(buffer.data());
  for(int32_t element = 0; element < allocatedElements; element++)
  {
    b[element] = v;
  }
}

bool VolumeDataStore::createConstantValueDataBlock(VolumeDataChunk const &volumeDataChunk, VolumeDataChannelDescriptor::Format format, float noValue, VolumeDataChannelDescriptor::Components components, VolumeDataHash const &constantValueVolumeDataHash, DataBlock &dataBlock, std::vector<uint8_t> &buffer, Error &error)
{
  int32_t size[4];
  volumeDataChunk.layer->getChunkVoxelSize(volumeDataChunk.chunkIndex, size);
  int32_t dimensionality = volumeDataChunk.layer->getChunkDimensionality();
  if (!initializeDataBlock(format, components, Dimensionality(dimensionality), size, dataBlock, error))
    return false;

 
  int32_t allocatedSize = getAllocatedByteSize(dataBlock);
  buffer.resize(allocatedSize);

  int32_t allocatedElements = dataBlock.allocatedSize[0] * dataBlock.allocatedSize[1] * dataBlock.allocatedSize[2] * dataBlock.allocatedSize[3] * dataBlock.components;

  float convertedConstantValue = getConvertedConstantValue(volumeDataChunk.layer->getVolumeDataChannelDescriptor(), format, noValue, constantValueVolumeDataHash);

  assert(dataBlock.format == format);

  VolumeDataChannelDescriptor::Format effectiveFormat = format;

  // Use U8 format fill methods for 1-bit
  if(format == VolumeDataChannelDescriptor::Format_1Bit)
  {
    effectiveFormat = VolumeDataChannelDescriptor::Format_U8;
    convertedConstantValue = convertValue<bool>(convertedConstantValue) ? 255.0f : 0.0f;
  }

  switch (effectiveFormat)
  {
  default:
    error.code = -3;
    error.string = "Invalid format in createConstantValuedataBlock";
    return false;
  case VolumeDataChannelDescriptor::Format_U8:  fillConstantValueBuffer<uint8_t>(buffer, allocatedElements, convertedConstantValue); break;
  case VolumeDataChannelDescriptor::Format_U16: fillConstantValueBuffer<uint16_t>(buffer, allocatedElements, convertedConstantValue); break;
  case VolumeDataChannelDescriptor::Format_R32: fillConstantValueBuffer<float>(buffer, allocatedElements, convertedConstantValue); break;
  case VolumeDataChannelDescriptor::Format_U32: fillConstantValueBuffer<uint32_t>(buffer, allocatedElements, convertedConstantValue); break;
  case VolumeDataChannelDescriptor::Format_R64: fillConstantValueBuffer<double>(buffer, allocatedElements, convertedConstantValue); break;
  case VolumeDataChannelDescriptor::Format_U64: fillConstantValueBuffer<uint64_t>(buffer, allocatedElements, convertedConstantValue); break;
  }

  return true;
}

bool VolumeDataStore::deserializeVolumeData(const VolumeDataChunk &volumeDataChunk, const std::vector<uint8_t>& serializedData, const std::vector<uint8_t>& metadata, CompressionMethod compressionMethod, int32_t adaptiveLevel, VolumeDataChannelDescriptor::Format loadFormat, DataBlock &dataBlock, std::vector<uint8_t>& target, Error& error)
{
  uint64_t volumeDataHashValue = VolumeDataHash::UNKNOWN;

  bool isWaveletAdaptive = false;

  if (compressionMethodIsWavelet(compressionMethod) && metadata.size() == sizeof(uint64_t) + sizeof(uint8_t[WAVELET_ADAPTIVE_LEVELS]) && VolumeDataStore::verify(volumeDataChunk, serializedData, compressionMethod, false))
  {
    isWaveletAdaptive = true;
  }
  else if (metadata.size() != sizeof(uint64_t) || !verify(volumeDataChunk, serializedData, compressionMethod, true))
  {
    return false;
  }

  memcpy(&volumeDataHashValue, metadata.data(), sizeof(uint64_t));
  
  VolumeDataHash volumeDataHash(volumeDataHashValue);

  const VolumeDataLayer* volumeDataLayer = volumeDataChunk.layer;

  if (volumeDataHash.isConstant())
  {
    if (!createConstantValueDataBlock(volumeDataChunk, volumeDataLayer->getFormat(), volumeDataLayer->getNoValue(), volumeDataLayer->getComponents(), volumeDataHash, dataBlock, target, error))
      return false;
  }
  else
  {
    volumeDataHash = uint64_t(volumeDataHash) ^ (uint64_t(adaptiveLevel) + 1) * 0x4068934683409867ULL;

    {
      //create a value range from scale and offset so that conversion to 8 or 16 bit is done correctly inside deserialization
      FloatRange deserializeValueRange = volumeDataLayer->getValueRange();

      if (volumeDataLayer->getFormat() == VolumeDataChannelDescriptor::Format_U16 || volumeDataLayer->getFormat() == VolumeDataChannelDescriptor::Format_U8)
      {
        if (loadFormat == VolumeDataChannelDescriptor::Format_U16)
        {
          deserializeValueRange.min = volumeDataLayer->getIntegerOffset();
          deserializeValueRange.max = volumeDataLayer->getIntegerScale() * (volumeDataLayer->isUseNoValue() ? 65534.0f : 65535.0f) + volumeDataLayer->getIntegerOffset();
        }
        else if (loadFormat == VolumeDataChannelDescriptor::Format_U8 && volumeDataLayer->getFormat() != VolumeDataChannelDescriptor::Format_U16)
        {
          deserializeValueRange.min = volumeDataLayer->getIntegerOffset();
          deserializeValueRange.max = volumeDataLayer->getIntegerScale() * (volumeDataLayer->isUseNoValue() ? 254.0f : 255.0f) + volumeDataLayer->getIntegerOffset();
        }
      }

      if (!OpenVDS::deserializeVolumeData(serializedData, loadFormat, compressionMethod, deserializeValueRange, volumeDataLayer->getIntegerScale(), volumeDataLayer->getIntegerOffset(), volumeDataLayer->isUseNoValue(), volumeDataLayer->getNoValue(), adaptiveLevel, dataBlock, target, error))
        return false;

    }
  }

  return true;
}

bool VolumeDataStore::serialize(const VolumeDataChunk& chunk, const std::vector<uint8_t>& chunkData, CompressionMethod compressionMethod, std::vector<uint8_t>& destinationBuffer, Error& error)
{
  destinationBuffer = chunkData;
//    DataBlockDescriptor dataBlockDescriptor;
//
//    if(!dataBlockDescriptor->isValid())
//    {
//      error.code = -1;
//      error.string = "Failed to decode DataBlockDescriptor";
//      return false;
//    }
//
//    if (!initializeDataBlock(*dataBlockDescriptor, dataBlock, error))
//      return false;
//
//    void * source = dataBlockDescriptor + 1;
//
//    int32_t byteSize = getByteSize(*dataBlockDescriptor);
//    std::unique_ptr<uint8_t[]>buffer(new uint8_t[byteSize]);
//
//    int32_t decompressedSize = rle_Decompress((uint8_t *)buffer.get(), byteSize, (uint8_t *)source);
//    assert(decompressedSize == byteSize);
//
//    int allocatedSize = getAllocatedByteSize(dataBlock);
//    destination.resize(allocatedSize);
//    copyLinearBufferIntoDataBlock(buffer.get(), dataBlock, destination);
//  switch(compressionMethod)
//  {
//  case CompressionMethod::None:
//    destinationBuffer = layerData;
//    return true;
//  case CompressionMethod::Zip:
//    return true;
//  default:
//    error.code = -20;
//    error.string = "Not implemented compresssion algorithm";
//    return false;
//  }
  return true;
}

}
