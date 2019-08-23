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

#include <stdlib.h>
#include <assert.h>

namespace OpenVDS
{
static bool compressionMethodIsWavelet(CompressionMethod compressionMethod)
{
  return compressionMethod == CompressionMethod::Wavelet ||
         compressionMethod == CompressionMethod::WaveletNormalizeBlock ||
         compressionMethod == CompressionMethod::WaveletLossless ||
         compressionMethod == CompressionMethod::WaveletNormalizeBlockLossless;
}
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

  for (int i = 1; i < Dimensionality_Max; i++)
  {
    byteSize *= size[i];
  }

  return byteSize;
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

static bool createDataBlock(const DataBlockDescriptor &descriptor, DataBlock &dataBlock, Error &error)
{
  dataBlock.components = descriptor.components;
  dataBlock.format = descriptor.format;

  if (descriptor.dimensionality == 1)
  {
    dataBlock.dimensionality = Dimensionality_1;
    dataBlock.size[0] = descriptor.sizeX;
    dataBlock.size[1] = 1;
    dataBlock.size[2] = 1;
    dataBlock.size[3] = 1;
  }
  else if (descriptor.dimensionality == 2)
  {
    dataBlock.dimensionality = Dimensionality_2;
    dataBlock.size[0] = descriptor.sizeX;
    dataBlock.size[1] = descriptor.sizeY;
    dataBlock.size[2] = 1;
    dataBlock.size[3] = 1;
  }
  else if (descriptor.dimensionality == 3)
  {
    dataBlock.dimensionality = Dimensionality_3;
    dataBlock.size[0] = descriptor.sizeX;
    dataBlock.size[1] = descriptor.sizeY;
    dataBlock.size[2] = descriptor.sizeZ;
    dataBlock.size[3] = 1;
  }
  else
  {
    error.string = "Serialized datablock has illegal dimensionality";
    error.code = -1;
    return false;
  }
  dataBlock.allocatedSize[0] = (descriptor.format == VolumeDataChannelDescriptor::Format_1Bit) ? ((dataBlock.size[0] * descriptor.components) + 7) / 8 : dataBlock.size[0];
  dataBlock.allocatedSize[1] = dataBlock.size[1];
  dataBlock.allocatedSize[2] = dataBlock.size[2];
  dataBlock.allocatedSize[3] = dataBlock.size[3];
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

      isValid = dataVersion >= WAVELET_OLD_DATA_VERSION &&
                dataVersion <= WAVELET_DATA_VERSION_1_4 &&
                (compressedSize <= serializedData.size() || (dataVersion >= WAVELET_DATA_VERSION_1_4 && !isFullyRead)) &&
                (createSizeX == voxelSize[0]                  ) &&
                (createSizeY == voxelSize[1] || dimensions < 2) &&
                (createSizeZ == voxelSize[2] || dimensions < 3) &&
                dimensions >= 1 &&
                dimensions <= 3;
    }
  }
  else if(compressionMethod == CompressionMethod::None ||
          compressionMethod == CompressionMethod::Rle ||
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

static bool deserialize(const std::vector<uint8_t> &serializedData, VolumeDataChannelDescriptor::Format format, CompressionMethod compressionMethod, bool isRenderable, const FloatRange &valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, int32_t adaptiveLevel, std::vector<uint8_t> &destination, Error &error)
{
  DataBlock dataBlock;
  if(compressionMethodIsWavelet(compressionMethod))
  {
    assert(isRenderable);

    const void *data = serializedData.data();

    int32_t dataVersion = ((int32_t *)data)[0];
    int32_t compressedSize = ((int32_t *)data)[1];

    assert(dataVersion >= WAVELET_OLD_DATA_VERSION && dataVersion <= WAVELET_DATA_VERSION_1_4);
    assert(compressedSize <= serializedData.size() || dataVersion >= WAVELET_DATA_VERSION_1_4);

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

    //pcDataBlock = Wavelet_Decompress(const_cast<void_px>(pxData), (int)cBLOB.GetSize(), eFormat, cValueRange, rIntegerScale, rIntegerOffset, isUseNoValue, rNoValue, isNormalize, ConfigMemoryManagement_Get()->_cCurrent._eBypassCompressionMode, iAdaptiveLevel, isLossless);
  }
  else if(compressionMethod == CompressionMethod::Rle)
  {
    DataBlockDescriptor *dataBlockDescriptor = (DataBlockDescriptor *)serializedData.data();

    if(!dataBlockDescriptor->isValid())
    {
      error.code = -1;
      error.string = "Failed to decode DataBlockDescriptor";
      return false;
    }

    if (!createDataBlock(*dataBlockDescriptor, dataBlock, error))
      return false;

    void * source = dataBlockDescriptor + 1;

    int32_t byteSize = getByteSize(*dataBlockDescriptor);
    std::unique_ptr<uint8_t[]>buffer(new uint8_t[byteSize]);

    //int32_t decompressedSize = RleDecompress((uint8_t *)buffer, byteSize, (uint8_t *)source);
    assert(decompressedSize == byteSize);
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
    if (!createDataBlock(*dataBlockDescriptor, dataBlock, error))
      return false;

    void * source = dataBlockDescriptor + 1;

    int32_t byteSize = getByteSize(*dataBlockDescriptor);
    void *buffer = malloc(byteSize);

    unsigned long destLen = byteSize;

    //int status = uncompress((U8_pu)pxBuffer, &uDestLen, (U8_pu)pxSource, (unsigned long)cBLOB.GetSize() - sizeof(DataBlockDescriptor_c));

    //if (status != Z_OK)
    //{
    //  Log_Printf(LOG_GROUP_ERROR, "Space", "zlib uncompress failed (status %d) in VolumeDataStore_c::DeSerialize\n", status);
    //  delete pcDataBlock;
    //  return NULL;
    //}

    //assert(uDestLen == nByteSize);
    copyLinearBufferIntoDataBlock(buffer, dataBlock, destination);
    free(buffer);
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
    if (!createDataBlock(*dataBlockDescriptor, dataBlock, error))
      return false;

    void * source = dataBlockDescriptor + 1;

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

bool VolumeDataStore::deserializeVolumeData(const VolumeDataChunk &chunk, const std::vector<uint8_t>& serializedData, const std::vector<uint8_t>& metadata, CompressionMethod compressionMethod, int32_t adaptiveLevel, VolumeDataChannelDescriptor::Format loadFormat, std::vector<uint8_t>& target, Error& error)
{
  uint64_t volumeDataHashValue = VolumeDataHash::UNKNOWN;

  bool isWaveletAdaptive = false;

  if (compressionMethodIsWavelet(compressionMethod) && metadata.size() == sizeof(uint64_t) + sizeof(WaveletAdaptiveLevelsMetadata) && VolumeDataStore::verify(chunk, serializedData, compressionMethod, false))
  {
    isWaveletAdaptive = true;
  }
  else if (metadata.size() != sizeof(uint64_t) || !verify(chunk, serializedData, compressionMethod, true))
  {
    return false;
  }

  WaveletAdaptiveLevelsMetadata waveletAdaptiveLevelsMetadata;

  memcpy(&volumeDataHashValue, metadata.data(), sizeof(uint64_t));

  if (isWaveletAdaptive)
  {
    memcpy(&waveletAdaptiveLevelsMetadata, (char*)metadata.data() + sizeof(uint64_t), sizeof(waveletAdaptiveLevelsMetadata));
  }

  VolumeDataHash volumeDataHash(volumeDataHashValue);

  const VolumeDataLayer* volumeDataLayer = chunk.layer;

  if (volumeDataHash.isConstant())
  {
    //pcTargetCacheItem->SetConstant(cHueVolumeDataHash, eLoadFormat, eNewStatus);
  }
  else
  {
    volumeDataHash = volumeDataHash ^ (adaptiveLevel + 1) * 0x4068934683409867ULL;

    //if(!pcTargetCacheItem->LookupDataBlock(eLoadFormat, cHueVolumeDataHash, eNewStatus))
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

      if (!deserialize(serializedData, loadFormat, compressionMethod, false, deserializeValueRange, volumeDataLayer->getIntegerScale(), volumeDataLayer->getIntegerOffset(), volumeDataLayer->isUseNoValue(), volumeDataLayer->getNoValue(), adaptiveLevel, target, error))
        return false;

      //pcTargetCacheItem->SetDataBlock(pcHueDataBlock, eLoadFormat, NULL, cHueVolumeDataHash, eNewStatus);
    }
  }

  return true;
}
}
