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

#include "VolumeDataLayoutImpl.h"

#include "Wavelet.h"
#include "VolumeDataHash.h"
#include "Rle.h"
#include "DataBlock.h"
#include <OpenVDS/ValueConversion.h>


#include <stdlib.h>
#include <assert.h>

#include <zlib.h>

namespace OpenVDS
{
static bool CompressionMethodIsWavelet(CompressionMethod compressionMethod)
{
  return compressionMethod == CompressionMethod::Wavelet ||
         compressionMethod == CompressionMethod::WaveletNormalizeBlock ||
         compressionMethod == CompressionMethod::WaveletLossless ||
         compressionMethod == CompressionMethod::WaveletNormalizeBlockLossless;
}

static uint32_t GetByteSize(const DataBlockDescriptor &descriptor)
{
  int32_t size[DataStoreDimensionality_Max];
  size[0] = descriptor.SizeX;
  size[1] = descriptor.SizeY;
  size[2] = descriptor.SizeZ;
  size[3] = 1;
  return GetByteSize(size, descriptor.Format, descriptor.Components);
}


bool VolumeDataStore::Verify(const VolumeDataChunk &volumeDataChunk, const std::vector<uint8_t> &serializedData, CompressionMethod compressionMethod, bool isFullyRead)
{
  bool isValid = false;

  int32_t voxelSize[DataStoreDimensionality_Max];

  volumeDataChunk.Layer->GetChunkVoxelSize(volumeDataChunk.Index, voxelSize);

  if(serializedData.empty())
  {
    isValid = true;
  }
  else if(CompressionMethodIsWavelet(compressionMethod))
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
      isValid = serializedDescriptor->IsValid(voxelSize);
    }
  }

  return isValid;
}

static void CopyLinearBufferIntoDataBlock(const void *sourceBuffer, const DataBlock &dataBlock, std::vector<uint8_t> &targetBuffer)
{

  int32_t sizeX = dataBlock.Size[0];
  int32_t sizeY = dataBlock.Size[1];
  int32_t sizeZ = dataBlock.Size[2];

  int32_t allocatedSizeX = dataBlock.AllocatedSize[0];
  int32_t allocatedSizeY = dataBlock.AllocatedSize[1];

  if(dataBlock.Format == VolumeDataChannelDescriptor::Format_1Bit)
  {
    sizeX = ((sizeX * dataBlock.Components) + 7) / 8;
  }

  uint32_t elementSize = GetElementSize(dataBlock);

  for(int32_t iZ = 0; iZ < sizeZ; iZ++)
  {
    for(int32_t iY = 0; iY < sizeY; iY++)
    {
      uint8_t *target = targetBuffer.data()                             +  (iZ * allocatedSizeY + iY) * allocatedSizeX * elementSize;
      const uint8_t *source = static_cast<const uint8_t*>(sourceBuffer) +  (iZ * sizeY          + iY) * sizeX          * elementSize;
      memcpy(target, source, size_t(sizeX) * elementSize);
    }
  }
}

static bool CopyDataBlockIntoLinearBuffer(const DataBlock &dataBlock, const void *sourceBuffer, void *targetBuffer, int32_t bufferSize)
{
  int32_t size[DataStoreDimensionality_Max];
  memcpy(size, dataBlock.Size, sizeof(size));
  int32_t allocatedSize[DataStoreDimensionality_Max];
  memcpy(allocatedSize, dataBlock.AllocatedSize, sizeof(allocatedSize));

  int32_t elementSize = int32_t(GetElementSize(dataBlock));

  if(dataBlock.Format == VolumeDataChannelDescriptor::Format_1Bit)
  {
    size[0] = ((size[0] * dataBlock.Components) + 7) / 8;
  }

  // Check if first row is constant
  bool isConstant = true;
  switch(dataBlock.Format)
  {
  default:
    fprintf(stderr, "Illegal format\n");
  case VolumeDataChannelDescriptor::Format_1Bit:
    isConstant = (reinterpret_cast<const uint8_t*>(sourceBuffer)[0] == 0x00 || reinterpret_cast<const uint8_t*>(sourceBuffer)[0] == 0xff);
    // Fall through
  case VolumeDataChannelDescriptor::Format_U8:
    for(int32_t iX = 1; isConstant && iX < size[0]; iX++)
    {
      isConstant = reinterpret_cast<const uint8_t *>(sourceBuffer)[0] == reinterpret_cast<const uint8_t *>(sourceBuffer)[iX];
    }
    break;
  case VolumeDataChannelDescriptor::Format_U16:
    for(int32_t iX = 1; isConstant && iX < size[0]; iX++)
    {
      isConstant = reinterpret_cast<const uint16_t *>(sourceBuffer)[0] == reinterpret_cast<const uint16_t *>(sourceBuffer)[iX];
    }
    break;

  case VolumeDataChannelDescriptor::Format_R32:
  case VolumeDataChannelDescriptor::Format_U32:
    for(int32_t iX = 1; isConstant && iX < size[0]; iX++)
    {
      isConstant = reinterpret_cast<const uint32_t *>(sourceBuffer)[0] == reinterpret_cast<const uint32_t *>(sourceBuffer)[iX];
    }
    break;

  case VolumeDataChannelDescriptor::Format_U64:
  case VolumeDataChannelDescriptor::Format_R64:
    for(int32_t iX = 1; isConstant && iX < size[0]; iX++)
    {
      isConstant = reinterpret_cast<const uint64_t *>(sourceBuffer)[0] == reinterpret_cast<const uint64_t *>(sourceBuffer)[iX];
    }
    break;
  }

  assert(bufferSize >= size[0] * size[1] * size[2] * elementSize);

  for(int32_t iZ = 0; iZ < size[2]; iZ++)
  {
    for(int32_t iY = 0; iY < size[1]; iY++)
    {
      uint8_t *puSource = (uint8_t *)sourceBuffer;
      uint8_t *puTarget = (uint8_t *)targetBuffer;

      puSource += (iZ * allocatedSize[1] + iY) * allocatedSize[0] * elementSize;
      puTarget += (iZ * size[1]          + iY) * size[0]          * elementSize;

      if(isConstant)
      {
        isConstant = (memcmp(sourceBuffer, puSource, size[0] * elementSize) == 0);
      }

      memcpy(puTarget, puSource, size[0] * elementSize);
    }
  }

  return isConstant;
}

template<typename T>
static uint64_t GetConstantValueVolumeDataHash(T value, const Range<float> &valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue)
{
  if (isUseNoValue)
  {
    if(value == ConvertNoValue<T>(noValue))
    {
      return VolumeDataHash::NOVALUE;
    }
    else
    {
      QuantizingValueConverterWithNoValue<float, T, true> converter(valueRange.Min, valueRange.Max, integerScale, integerOffset, noValue, noValue, false);
      return VolumeDataHash(converter.ConvertValue(value)).CalculateHash();
    }
  }
  else
  {
    QuantizingValueConverterWithNoValue<float, T, false> converter(valueRange.Min, valueRange.Max, integerScale, integerOffset, noValue, noValue, false);
    return VolumeDataHash(converter.ConvertValue(value)).CalculateHash();
  }
}

static uint64_t GetConstantValueVolumeDataHash(const DataBlock  &dataBlock, const uint8_t *sourceBuffer, const Range<float> &valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue)
{
  switch(dataBlock.Format)
  {
  case VolumeDataChannelDescriptor::Format_1Bit:
    assert(reinterpret_cast<const uint8_t *>(sourceBuffer)[0] == 0x00 || reinterpret_cast<const uint8_t *>(sourceBuffer)[0] == 0xff);
    return VolumeDataHash((float)(reinterpret_cast<const uint8_t *>(sourceBuffer)[0] != 0)).CalculateHash();
  case VolumeDataChannelDescriptor::Format_U8:  return GetConstantValueVolumeDataHash(reinterpret_cast<const uint8_t  *>(sourceBuffer)[0], valueRange, integerScale, integerOffset, isUseNoValue, noValue);
  case VolumeDataChannelDescriptor::Format_U16: return GetConstantValueVolumeDataHash(reinterpret_cast<const uint16_t *>(sourceBuffer)[0], valueRange, integerScale, integerOffset, isUseNoValue, noValue);
  case VolumeDataChannelDescriptor::Format_R32: return GetConstantValueVolumeDataHash(reinterpret_cast<const float *>(sourceBuffer)[0], valueRange, integerScale, integerOffset, isUseNoValue, noValue);
  case VolumeDataChannelDescriptor::Format_U32: return GetConstantValueVolumeDataHash(reinterpret_cast<const uint32_t *>(sourceBuffer)[0], valueRange, integerScale, integerOffset, isUseNoValue, noValue);
  case VolumeDataChannelDescriptor::Format_R64: return GetConstantValueVolumeDataHash(reinterpret_cast<const double *>(sourceBuffer)[0], valueRange, integerScale, integerOffset, isUseNoValue, noValue);
  case VolumeDataChannelDescriptor::Format_U64: return GetConstantValueVolumeDataHash(reinterpret_cast<const uint64_t *>(sourceBuffer)[0], valueRange, integerScale, integerOffset, isUseNoValue, noValue);

  default:
    assert(0 && "Unknown format");
    return VolumeDataHash::UNKNOWN;
  }
}

bool DeserializeVolumeData(const std::vector<uint8_t> &serializedData, VolumeDataChannelDescriptor::Format format, CompressionMethod compressionMethod, const FloatRange &valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, int32_t adaptiveLevel, DataBlock &dataBlock, std::vector<uint8_t> &destination, Error &error)
{
  if(CompressionMethodIsWavelet(compressionMethod))
  {
    const void *data = serializedData.data();

    int32_t dataVersion = ((int32_t *)data)[0];

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

    if(!dataBlockDescriptor->IsValid())
    {
      error.code = -1;
      error.string = "Failed to decode DataBlockDescriptor";
      return false;
    }

    if (!InitializeDataBlock(*dataBlockDescriptor, dataBlock, error))
      return false;

    void * source = dataBlockDescriptor + 1;

    int32_t byteSize = GetByteSize(*dataBlockDescriptor);
    std::unique_ptr<uint8_t[]>buffer(new uint8_t[byteSize]);

    int32_t decompressedSize = RleDecompress((uint8_t *)buffer.get(), byteSize, (uint8_t *)source);
    assert(decompressedSize == byteSize);

    int allocatedSize = GetAllocatedByteSize(dataBlock);
    destination.resize(allocatedSize);
    CopyLinearBufferIntoDataBlock(buffer.get(), dataBlock, destination);
  }
  else if(compressionMethod == CompressionMethod::Zip)
  {
    DataBlockDescriptor *dataBlockDescriptor = (DataBlockDescriptor *)serializedData.data();

    if(!dataBlockDescriptor->IsValid())
    {
      error.code = -1;
      error.string = "Failed to decode DataBlockDescriptor";
      return false;
    }
    if (!InitializeDataBlock(*dataBlockDescriptor, dataBlock, error))
      return false;

    void * source = dataBlockDescriptor + 1;

    int32_t byteSize = GetByteSize(*dataBlockDescriptor);
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[byteSize]);

    unsigned long destLen = byteSize;

    int status = uncompress(buffer.get(), &destLen, (uint8_t *)source, uint32_t(serializedData.size() - sizeof(DataBlockDescriptor)));

    if (status != Z_OK)
    {
      fprintf(stderr, "zlib uncompress failed (status %d) in VolumeDataStore_c::DeSerialize\n", status);
      return false;
    }

    int allocatedSize = GetAllocatedByteSize(dataBlock);
    destination.resize(allocatedSize);
    CopyLinearBufferIntoDataBlock(buffer.get(), dataBlock, destination);
  }
  else if(compressionMethod == CompressionMethod::None)
  {
    DataBlockDescriptor *dataBlockDescriptor = (DataBlockDescriptor *)serializedData.data();

    if(!dataBlockDescriptor->IsValid())
    {
      error.code = -1;
      error.string = "Failed to decode DataBlockDescriptor";
      return false;
    }
    if (!InitializeDataBlock(*dataBlockDescriptor, dataBlock, error))
      return false;

    void * source = dataBlockDescriptor + 1;

    int32_t byteSize = GetAllocatedByteSize(dataBlock);
    destination.resize(byteSize);
    CopyLinearBufferIntoDataBlock(source, dataBlock, destination);
  }

  if(dataBlock.Format != format)
  {
    error.string = "Formats doesn't match in deserialization\n";
    error.code = -2;
    return false;
  }
  return true;
}

static float GetConvertedConstantValue(VolumeDataChannelDescriptor const &volumeDataChannelDescriptor, VolumeDataChannelDescriptor::Format format, float noValue, VolumeDataHash const &constantValueVolumeDataHash)
{
  if(format == VolumeDataChannelDescriptor::Format_1Bit)
  {
    return float(constantValueVolumeDataHash.GetConstantValue(0) != 0);
  }
  else if(format == VolumeDataChannelDescriptor::Format_U8 || format == VolumeDataChannelDescriptor::Format_U16)
  {
    if(constantValueVolumeDataHash.IsNoValue())
    {
      if(format == VolumeDataChannelDescriptor::Format_U8)
      {
        return ConvertNoValue<uint8_t>(noValue);
      }
      else
      {
        assert(format == VolumeDataChannelDescriptor::Format_U16);
        return ConvertNoValue<uint16_t>(noValue);
      }
    }

    int32_t buckets = (format == VolumeDataChannelDescriptor::Format_U8 ? 256 : 65536) - volumeDataChannelDescriptor.IsUseNoValue();

    float reciprocalScale;
    float offset;

    if(volumeDataChannelDescriptor.GetFormat() != VolumeDataChannelDescriptor::Format_U8 && volumeDataChannelDescriptor.GetFormat() != VolumeDataChannelDescriptor::Format_U16)
    {
      offset = volumeDataChannelDescriptor.GetValueRange().Min;
      reciprocalScale = float(buckets - 1) / rangeSize(volumeDataChannelDescriptor.GetValueRange());
    }
    else
    {
      offset = volumeDataChannelDescriptor.GetIntegerOffset();
      reciprocalScale = 1.0f / volumeDataChannelDescriptor.GetIntegerScale();
    }

    return (float)QuantizeValueWithReciprocalScale(constantValueVolumeDataHash.GetConstantValue(noValue), offset, reciprocalScale, buckets);
  }
  else
  {
    assert(format == VolumeDataChannelDescriptor::Format_R32 || format == VolumeDataChannelDescriptor::Format_U32 || format == VolumeDataChannelDescriptor::Format_R64 || format == VolumeDataChannelDescriptor::Format_U64);
    return constantValueVolumeDataHash.GetConstantValue(noValue);
  }
}

template <typename T>
static void FillConstantValueBuffer(std::vector<uint8_t> &buffer, int32_t allocatedElements, float value)
{
  T v = ConvertValue<T>(value);
  T *b = reinterpret_cast<T *>(buffer.data());
  for(int32_t element = 0; element < allocatedElements; element++)
  {
    b[element] = v;
  }
}

bool VolumeDataStore::CreateConstantValueDataBlock(VolumeDataChunk const &volumeDataChunk, VolumeDataChannelDescriptor::Format format, float noValue, VolumeDataChannelDescriptor::Components components, VolumeDataHash const &constantValueVolumeDataHash, DataBlock &dataBlock, std::vector<uint8_t> &buffer, Error &error)
{
  int32_t size[4];
  volumeDataChunk.Layer->GetChunkVoxelSize(volumeDataChunk.Index, size);
  int32_t dimensionality = volumeDataChunk.Layer->GetChunkDimensionality();
  if (!InitializeDataBlock(format, components, Dimensionality(dimensionality), size, dataBlock, error))
    return false;

 
  int32_t allocatedSize = GetAllocatedByteSize(dataBlock);
  buffer.resize(allocatedSize);

  int32_t allocatedElements = dataBlock.AllocatedSize[0] * dataBlock.AllocatedSize[1] * dataBlock.AllocatedSize[2] * dataBlock.AllocatedSize[3] * dataBlock.Components;

  float convertedConstantValue = GetConvertedConstantValue(volumeDataChunk.Layer->GetVolumeDataChannelDescriptor(), format, noValue, constantValueVolumeDataHash);

  assert(dataBlock.Format == format);

  VolumeDataChannelDescriptor::Format effectiveFormat = format;

  // Use U8 format fill methods for 1-bit
  if(format == VolumeDataChannelDescriptor::Format_1Bit)
  {
    effectiveFormat = VolumeDataChannelDescriptor::Format_U8;
    convertedConstantValue = ConvertValue<bool>(convertedConstantValue) ? 255.0f : 0.0f;
  }

  switch (effectiveFormat)
  {
  default:
    error.code = -3;
    error.string = "Invalid format in createConstantValuedataBlock";
    return false;
  case VolumeDataChannelDescriptor::Format_U8:  FillConstantValueBuffer<uint8_t>(buffer, allocatedElements, convertedConstantValue); break;
  case VolumeDataChannelDescriptor::Format_U16: FillConstantValueBuffer<uint16_t>(buffer, allocatedElements, convertedConstantValue); break;
  case VolumeDataChannelDescriptor::Format_R32: FillConstantValueBuffer<float>(buffer, allocatedElements, convertedConstantValue); break;
  case VolumeDataChannelDescriptor::Format_U32: FillConstantValueBuffer<uint32_t>(buffer, allocatedElements, convertedConstantValue); break;
  case VolumeDataChannelDescriptor::Format_R64: FillConstantValueBuffer<double>(buffer, allocatedElements, convertedConstantValue); break;
  case VolumeDataChannelDescriptor::Format_U64: FillConstantValueBuffer<uint64_t>(buffer, allocatedElements, convertedConstantValue); break;
  }

  return true;
}

bool VolumeDataStore::DeserializeVolumeData(const VolumeDataChunk &volumeDataChunk, const std::vector<uint8_t>& serializedData, const std::vector<uint8_t>& metadata, CompressionMethod compressionMethod, int32_t adaptiveLevel, VolumeDataChannelDescriptor::Format loadFormat, DataBlock &dataBlock, std::vector<uint8_t>& target, Error& error)
{
  uint64_t volumeDataHashValue = VolumeDataHash::UNKNOWN;

  bool isWaveletAdaptive = false;

  if (CompressionMethodIsWavelet(compressionMethod) && metadata.size() == sizeof(uint64_t) + sizeof(uint8_t[WAVELET_ADAPTIVE_LEVELS]) && VolumeDataStore::Verify(volumeDataChunk, serializedData, compressionMethod, false))
  {
    isWaveletAdaptive = true;
  }
  else if (metadata.size() != sizeof(uint64_t) || !Verify(volumeDataChunk, serializedData, compressionMethod, true))
  {
    return false;
  }

  memcpy(&volumeDataHashValue, metadata.data(), sizeof(uint64_t));
  
  VolumeDataHash volumeDataHash(volumeDataHashValue);

  const VolumeDataLayer* volumeDataLayer = volumeDataChunk.Layer;

  if (volumeDataHash.IsConstant())
  {
    if (!CreateConstantValueDataBlock(volumeDataChunk, volumeDataLayer->GetFormat(), volumeDataLayer->GetNoValue(), volumeDataLayer->GetComponents(), volumeDataHash, dataBlock, target, error))
      return false;
  }
  else
  {
    volumeDataHash = uint64_t(volumeDataHash) ^ (uint64_t(adaptiveLevel) + 1) * 0x4068934683409867ULL;

    {
      //create a value range from scale and offset so that conversion to 8 or 16 bit is done correctly inside deserialization
      FloatRange deserializeValueRange = volumeDataLayer->GetValueRange();

      if (volumeDataLayer->GetFormat() == VolumeDataChannelDescriptor::Format_U16 || volumeDataLayer->GetFormat() == VolumeDataChannelDescriptor::Format_U8)
      {
        if (loadFormat == VolumeDataChannelDescriptor::Format_U16)
        {
          deserializeValueRange.Min = volumeDataLayer->GetIntegerOffset();
          deserializeValueRange.Max = volumeDataLayer->GetIntegerScale() * (volumeDataLayer->IsUseNoValue() ? 65534.0f : 65535.0f) + volumeDataLayer->GetIntegerOffset();
        }
        else if (loadFormat == VolumeDataChannelDescriptor::Format_U8 && volumeDataLayer->GetFormat() != VolumeDataChannelDescriptor::Format_U16)
        {
          deserializeValueRange.Min = volumeDataLayer->GetIntegerOffset();
          deserializeValueRange.Max = volumeDataLayer->GetIntegerScale() * (volumeDataLayer->IsUseNoValue() ? 254.0f : 255.0f) + volumeDataLayer->GetIntegerOffset();
        }
      }

      if (!OpenVDS::DeserializeVolumeData(serializedData, loadFormat, compressionMethod, deserializeValueRange, volumeDataLayer->GetIntegerScale(), volumeDataLayer->GetIntegerOffset(), volumeDataLayer->IsUseNoValue(), volumeDataLayer->GetNoValue(), adaptiveLevel, dataBlock, target, error))
        return false;

    }
  }

  return true;
}

bool VolumeDataStore::SerializeVolumeData(const VolumeDataChunk& chunk, const DataBlock& dataBlock, const std::vector<uint8_t>& chunkData, CompressionMethod compressionMethod, std::vector<uint8_t>& destinationBuffer, uint64_t& outputHash, Error& error)
{
  DataBlockDescriptor dataBlockHeader;
  dataBlockHeader.Components = dataBlock.Components;
  dataBlockHeader.Dimensionality = dataBlock.Dimensionality;
  dataBlockHeader.Format = dataBlock.Format;
  dataBlockHeader.SizeX = dataBlock.Size[0];
  dataBlockHeader.SizeY = dataBlock.Size[1];
  dataBlockHeader.SizeZ = dataBlock.Size[2];

  switch (compressionMethod)
  {
  case CompressionMethod::None:
  {
    int32_t byteSize = GetByteSize(dataBlock) + sizeof(DataBlockDescriptor);

    destinationBuffer.resize(byteSize);

    void *targetBuffer = destinationBuffer.data();
    memcpy(targetBuffer, &dataBlockHeader, sizeof(dataBlockHeader));
    targetBuffer = ((uint8_t *)targetBuffer) + sizeof(dataBlockHeader);

    bool isConstant = CopyDataBlockIntoLinearBuffer(dataBlock, chunkData.data(), targetBuffer, byteSize - sizeof(dataBlockHeader));

    if(isConstant)
    {
      destinationBuffer.resize(0);
      auto& layer = *chunk.Layer;
      outputHash = GetConstantValueVolumeDataHash(dataBlock, (const uint8_t *) targetBuffer, layer.GetValueRange(), layer.GetIntegerScale(), layer.GetIntegerOffset(), layer.IsUseNoValue(), layer.GetNoValue());
      return true;
    }
    break;
  }
  case CompressionMethod::Zip:
  {
    uint32_t tmpbuffersize = GetByteSize(dataBlock);
    std::unique_ptr<uint8_t[]> tmpdata(new uint8_t[tmpbuffersize]);
    bool isConstant = CopyDataBlockIntoLinearBuffer(dataBlock, chunkData.data(), tmpdata.get(), tmpbuffersize);

    if (isConstant)
    {
      destinationBuffer.resize(0);
      auto& layer = *chunk.Layer;
      outputHash = GetConstantValueVolumeDataHash(dataBlock, tmpdata.get(), layer.GetValueRange(), layer.GetIntegerScale(), layer.GetIntegerOffset(), layer.IsUseNoValue(), layer.GetNoValue());
      return true;
    }
    unsigned long compressedMaxSize = compressBound(tmpbuffersize);
    destinationBuffer.resize(compressedMaxSize + sizeof(dataBlockHeader));
    void *targetBuffer = destinationBuffer.data();
    memcpy(targetBuffer, &dataBlockHeader, sizeof(dataBlockHeader));
    targetBuffer = ((uint8_t *)targetBuffer) + sizeof(dataBlockHeader);
    unsigned long compressedSize = compressedMaxSize;
    int status = compress((uint8_t *)targetBuffer, &compressedSize, tmpdata.get(), tmpbuffersize);
    destinationBuffer.resize(compressedSize + sizeof(dataBlockHeader));

    if (status != Z_OK)
    {
      error.code = -100;
      error.string = "zlib compression failed";
      return false;
    }
    break;
  }
  default:
    error.code = -2;
    error.string = "Invalid compression method specified when serializing a VolumeDataChunk";
    return false;
  }
  outputHash = VolumeDataHash::UNKNOWN;
  return true;
}

}
