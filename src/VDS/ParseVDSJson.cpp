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

#include "ParseVDSJson.h"
#include "VolumeDataLayer.h"
#include "VolumeDataLayout.h"
#include "VolumeDataHash.h"
#include "MetadataManager.h"
#include <IO/S3_Downloader.h>

#include <json/json.h>

#include "Bitmask.h"

#include "Base64.h"

namespace OpenVDS
{

static DimensionsND dimensionsNDFromJson(Json::Value const &jsonDimensionsND)
{
  std::string
    dimensionsNDString = jsonDimensionsND.asString();

  if (dimensionsNDString == "Dimensions_012")
  {
    return Dimensions_012;
  }
  else if (dimensionsNDString == "Dimensions_013")
  {
    return Dimensions_013;
  }
  else if (dimensionsNDString == "Dimensions_014")
  {
    return Dimensions_014;
  }
  else if (dimensionsNDString == "Dimensions_015")
  {
    return Dimensions_015;
  }
  else if (dimensionsNDString == "Dimensions_023")
  {
    return Dimensions_023;
  }
  else if (dimensionsNDString == "Dimensions_024")
  {
    return Dimensions_024;
  }
  else if (dimensionsNDString == "Dimensions_025")
  {
    return Dimensions_025;
  }
  else if (dimensionsNDString == "Dimensions_034")
  {
    return Dimensions_034;
  }
  else if (dimensionsNDString == "Dimensions_035")
  {
    return Dimensions_035;
  }
  else if (dimensionsNDString == "Dimensions_045")
  {
    return Dimensions_045;
  }
  else if (dimensionsNDString == "Dimensions_123")
  {
    return Dimensions_123;
  }
  else if (dimensionsNDString == "Dimensions_124")
  {
    return Dimensions_124;
  }
  else if (dimensionsNDString == "Dimensions_125")
  {
    return Dimensions_125;
  }
  else if (dimensionsNDString == "Dimensions_134")
  {
    return Dimensions_134;
  }
  else if (dimensionsNDString == "Dimensions_135")
  {
    return Dimensions_135;
  }
  else if (dimensionsNDString == "Dimensions_145")
  {
    return Dimensions_145;
  }
  else if (dimensionsNDString == "Dimensions_234")
  {
    return Dimensions_234;
  }
  else if (dimensionsNDString == "Dimensions_235")
  {
    return Dimensions_235;
  }
  else if (dimensionsNDString == "Dimensions_245")
  {
    return Dimensions_245;
  }
  else if (dimensionsNDString == "Dimensions_345")
  {
    return Dimensions_345;
  }
  else if (dimensionsNDString == "Dimensions_01")
  {
    return Dimensions_01;
  }
  else if (dimensionsNDString == "Dimensions_02")
  {
    return Dimensions_02;
  }
  else if (dimensionsNDString == "Dimensions_03")
  {
    return Dimensions_03;
  }
  else if (dimensionsNDString == "Dimensions_04")
  {
    return Dimensions_04;
  }
  else if (dimensionsNDString == "Dimensions_05")
  {
    return Dimensions_05;
  }
  else if (dimensionsNDString == "Dimensions_12")
  {
    return Dimensions_12;
  }
  else if (dimensionsNDString == "Dimensions_13")
  {
    return Dimensions_13;
  }
  else if (dimensionsNDString == "Dimensions_14")
  {
    return Dimensions_14;
  }
  else if (dimensionsNDString == "Dimensions_15")
  {
    return Dimensions_15;
  }
  else if (dimensionsNDString == "Dimensions_23")
  {
    return Dimensions_23;
  }
  else if (dimensionsNDString == "Dimensions_24")
  {
    return Dimensions_24;
  }
  else if (dimensionsNDString == "Dimensions_25")
  {
    return Dimensions_25;
  }
  else if (dimensionsNDString == "Dimensions_34")
  {
    return Dimensions_34;
  }
  else if (dimensionsNDString == "Dimensions_35")
  {
    return Dimensions_35;
  }
  else if (dimensionsNDString == "Dimensions_45")
  {
    return Dimensions_45;
  }

  throw Json::Exception("Illegal dimension group");
}

static enum VolumeDataLayoutDescriptor::BrickSize brickSizeFromJson(Json::Value const &jsonBrickSize)
{
  std::string brickSizeString = jsonBrickSize.asString();

  if(brickSizeString == "BrickSize_32")
  {
    return VolumeDataLayoutDescriptor::BrickSize_32;
  }
  else if(brickSizeString == "BrickSize_64")
  {
    return VolumeDataLayoutDescriptor::BrickSize_64;
  }
  else if(brickSizeString == "BrickSize_128")
  {
    return VolumeDataLayoutDescriptor::BrickSize_128;
  }
  else if(brickSizeString == "BrickSize_256")
  {
    return VolumeDataLayoutDescriptor::BrickSize_256;
  }
  else if(brickSizeString == "BrickSize_512")
  {
    return VolumeDataLayoutDescriptor::BrickSize_512;
  }
  else if(brickSizeString == "BrickSize_1024")
  {
    return VolumeDataLayoutDescriptor::BrickSize_1024;
  }
  else if(brickSizeString == "BrickSize_2048")
  {
    return VolumeDataLayoutDescriptor::BrickSize_2048;
  }
  else if(brickSizeString == "BrickSize_4096")
  {
    return VolumeDataLayoutDescriptor::BrickSize_4096;
  }

  throw Json::Exception("Illegal brick size");
}

static enum VolumeDataLayoutDescriptor::LODLevels lodLevelsFromJson(Json::Value const &jsonLODLevels)
{
  std::string lodLevelString = jsonLODLevels.asString();

  if(lodLevelString == "LODLevels_None")
  {
    return VolumeDataLayoutDescriptor::LODLevels_None;
  }
  else if(lodLevelString == "LODLevels_1")
  {
    return VolumeDataLayoutDescriptor::LODLevels_1;
  }
  else if(lodLevelString == "LODLevels_2")
  {
    return VolumeDataLayoutDescriptor::LODLevels_2;
  }
  else if(lodLevelString == "LODLevels_3")
  {
    return VolumeDataLayoutDescriptor::LODLevels_3;
  }
  else if(lodLevelString == "LODLevels_4")
  {
    return VolumeDataLayoutDescriptor::LODLevels_4;
  }
  else if(lodLevelString == "LODLevels_5")
  {
    return VolumeDataLayoutDescriptor::LODLevels_5;
  }
  else if(lodLevelString == "LODLevels_6")
  {
    return VolumeDataLayoutDescriptor::LODLevels_6;
  }
  else if(lodLevelString == "LODLevels_7")
  {
    return VolumeDataLayoutDescriptor::LODLevels_7;
  }
  else if(lodLevelString == "LODLevels_8")
  {
    return VolumeDataLayoutDescriptor::LODLevels_8;
  }
  else if(lodLevelString == "LODLevels_9")
  {
    return VolumeDataLayoutDescriptor::LODLevels_9;
  }
  else if(lodLevelString == "LODLevels_10")
  {
    return VolumeDataLayoutDescriptor::LODLevels_10;
  }
  else if(lodLevelString == "LODLevels_11")
  {
    return VolumeDataLayoutDescriptor::LODLevels_11;
  }
  else if(lodLevelString == "LODLevels_12")
  {
    return VolumeDataLayoutDescriptor::LODLevels_12;
  }
  throw Json::Exception("Illegal LOD levels");
}

const char *addDescriptorString(std::string const &descriptorString, VDSHandle &handle)
{
  char *data = new char[descriptorString.size() + 1];
  memcpy(data, descriptorString.data(), descriptorString.size());
  data[descriptorString.size()] = 0;
  handle.descriptorStrings.emplace_back(data);
  return data;
}

static VolumeDataChannelDescriptor::Format voxelFormatFromJson(Json::Value const &jsonVoxelFormat)
{
  std::string voxelFormatString = jsonVoxelFormat.asString();

  if(voxelFormatString == "Format_R32")
  {
    return VolumeDataChannelDescriptor::Format_R32;
  }
  else if(voxelFormatString == "Format_R64")
  {
    return VolumeDataChannelDescriptor::Format_R64;
  }
  else if(voxelFormatString == "Format_U8")
  {
    return VolumeDataChannelDescriptor::Format_U8;
  }
  else if(voxelFormatString == "Format_U16")
  {
    return VolumeDataChannelDescriptor::Format_U16;
  }
  else if(voxelFormatString == "Format_U32")
  {
    return VolumeDataChannelDescriptor::Format_U32;
  }
  else if(voxelFormatString == "Format_U64")
  {
    return VolumeDataChannelDescriptor::Format_U64;
  }
  else if(voxelFormatString == "Format_1Bit")
  {
    return VolumeDataChannelDescriptor::Format_1Bit;
  }
  throw Json::Exception("Illegal voxel format");
}

static VolumeDataChannelDescriptor::Components voxelComponentsFromJson(Json::Value const &jsonVoxelComponents)
{
  std::string voxelComponentsString = jsonVoxelComponents.asString();

  if(voxelComponentsString == "Components_1")
  {
    return VolumeDataChannelDescriptor::Components_1;
  }
  else if(voxelComponentsString == "Components_2")
  {
    return VolumeDataChannelDescriptor::Components_2;
  }
  else if(voxelComponentsString == "Components_4")
  {
    return VolumeDataChannelDescriptor::Components_4;
  }
  throw Json::Exception("Illegal voxel components");
}

static VolumeDataMapping channelMappingFromJson(Json::Value const &jsonChannelMapping)
{
  std::string channelMappingString = jsonChannelMapping.asString();

  if(channelMappingString == "Direct")
  {
    return VolumeDataMapping::Direct;
  }
  else if(channelMappingString == "PerTrace")
  {
    return VolumeDataMapping::PerTrace;
  }
  throw Json::Exception("Illegal channel mapping");
}

static bool parseJSONFromBuffer(const std::vector<uint8_t> &json, Json::Value &root, Error &error)
{
  try
  {
    Json::CharReaderBuilder rbuilder;
    rbuilder["collectComments"] = false;

    std::string errs;

    std::unique_ptr<Json::CharReader> reader(rbuilder.newCharReader());
    const char *json_begin = reinterpret_cast<const char *>(json.data());
    reader->parse(json_begin, json_begin + json.size(), &root, &error.string);

    return true;
  }
  catch(Json::Exception e)
  {
    error.code = -1;
    error.string = e.what() + std::string(" : ") + error.string;
  }

  return false;
}

bool parseVolumeDataLayout(const std::vector<uint8_t> &json, VDSHandle &handle, Error &error)
{
  Json::Value root;

  if (!parseJSONFromBuffer(json, root, error))
  {
    return false;
  }

  if (root.empty())
    return true;

  Json::Value
    layoutDescriptor = root["layoutDescriptor"];

  handle.layoutDescriptor = VolumeDataLayoutDescriptor(brickSizeFromJson(layoutDescriptor["brickSize"]),
                                                       layoutDescriptor["negativeMargin"].asInt(),
                                                       layoutDescriptor["positiveMargin"].asInt(),
                                                       layoutDescriptor["brickSize2DMultiplier"].asInt(),
                                                       lodLevelsFromJson(layoutDescriptor["lodLevels"]),
                                                       (layoutDescriptor["create2DLODs"].asBool() ? VolumeDataLayoutDescriptor::Options_Create2DLODs : VolumeDataLayoutDescriptor::Options_None) |
                                                       (layoutDescriptor["forceFullResolutionDimension"].asBool() ? VolumeDataLayoutDescriptor::Options_ForceFullResolutionDimension : VolumeDataLayoutDescriptor::Options_None),
                                                       layoutDescriptor["fullResolutionDimension"].asInt());

  for (const Json::Value &axisDescriptor : root["axisDescriptors"])
  {
    VolumeDataAxisDescriptor
      volumeDataAxisDescriptor(axisDescriptor["numSamples"].asInt(),
                               addDescriptorString(axisDescriptor["name"].asString(), handle),
                               addDescriptorString(axisDescriptor["unit"].asString(), handle),
                               axisDescriptor["coordinateMin"].asFloat(),
                               axisDescriptor["coordinateMax"].asFloat());

    handle.axisDescriptors.push_back(volumeDataAxisDescriptor);
  }

  for (const Json::Value &channelDescriptor : root["channelDescriptors"])
  {
    if (channelDescriptor["useNoValue"].asBool())
    {
      handle.channelDescriptors.push_back(VolumeDataChannelDescriptor(
        voxelFormatFromJson(channelDescriptor["format"]),
        voxelComponentsFromJson(channelDescriptor["components"]),
        addDescriptorString(channelDescriptor["name"].asString(), handle),
        addDescriptorString(channelDescriptor["unit"].asString(), handle),
        channelDescriptor["valueRange"][0].asFloat(),
        channelDescriptor["valueRange"][1].asFloat(),
        channelMappingFromJson(channelDescriptor["channelMapping"]),
        channelDescriptor["mappedValues"].asInt(),
        (channelDescriptor["discrete"].asBool() ? VolumeDataChannelDescriptor::DiscreteData : VolumeDataChannelDescriptor::Default) |
        (channelDescriptor["renderable"].asBool() ? VolumeDataChannelDescriptor::Default : VolumeDataChannelDescriptor::NotRenderable) |
        (channelDescriptor["allowLossyCompression"].asBool() ? VolumeDataChannelDescriptor::Default : VolumeDataChannelDescriptor::NoLossyCompression),
        channelDescriptor["noValue"].asFloat(),
        channelDescriptor["integerScale"].asFloat(),
        channelDescriptor["integerOffset"].asFloat()));
    }
    else
    {
      handle.channelDescriptors.push_back(VolumeDataChannelDescriptor(
        voxelFormatFromJson(channelDescriptor["format"]),
        voxelComponentsFromJson(channelDescriptor["components"]),
        addDescriptorString(channelDescriptor["name"].asString(), handle),
        addDescriptorString(channelDescriptor["unit"].asString(), handle),
        channelDescriptor["valueRange"][0].asFloat(),
        channelDescriptor["valueRange"][1].asFloat(),
        channelMappingFromJson(channelDescriptor["channelMapping"]),
        channelDescriptor["mappedValues"].asInt(),
        (channelDescriptor["discrete"].asBool() ? VolumeDataChannelDescriptor::DiscreteData : VolumeDataChannelDescriptor::Default) |
        (channelDescriptor["renderable"].asBool() ? VolumeDataChannelDescriptor::Default : VolumeDataChannelDescriptor::NotRenderable) |
        (channelDescriptor["allowLossyCompression"].asBool() ? VolumeDataChannelDescriptor::Default : VolumeDataChannelDescriptor::NoLossyCompression),
        channelDescriptor["integerScale"].asFloat(),
        channelDescriptor["integerOffset"].asFloat()));
    }
  }

  for (const Json::Value &metadata : root["metadata"])
  {
    MetadataKey key = { metadata["category"].asString(), metadata["name"].asString() };

    if (metadata["type"].asString() == "Int")
    {
      handle.metadataContainer.intData[key] = metadata["value"].asInt();
    }
    else if (metadata["type"].asString() == "IntVector2")
    {
      handle.metadataContainer.intVector2Data[key] = { metadata["value"][0].asInt(), metadata["value"][1].asInt()};
    }
    else if (metadata["type"].asString() == "IntVector3")
    {
      handle.metadataContainer.intVector3Data[key]  = { metadata["value"][0].asInt(), metadata["value"][1].asInt(), metadata["value"][2].asInt() };
    }
    else if (metadata["type"].asString() == "IntVector4")
    {
      handle.metadataContainer.intVector4Data[key] = { metadata["value"][0].asInt(), metadata["value"][1].asInt(), metadata["value"][2].asInt(), metadata["value"][3].asInt() };
    }
    else if (metadata["type"].asString() == "Float")
    {
      handle.metadataContainer.floatData[key] = metadata["value"].asFloat();
    }
    else if (metadata["type"].asString() == "FloatVector2")
    {
      handle.metadataContainer.floatVector2Data[key] = { metadata["value"][0].asFloat(), metadata["value"][1].asFloat() };
    }
    else if (metadata["type"].asString() == "FloatVector3")
    {
      handle.metadataContainer.floatVector3Data[key] = { metadata["value"][0].asFloat(), metadata["value"][1].asFloat(), metadata["value"][2].asFloat() };
    }
    else if (metadata["type"].asString() == "FloatVector4")
    {
      handle.metadataContainer.floatVector4Data[key] = { metadata["value"][0].asFloat(), metadata["value"][1].asFloat(), metadata["value"][2].asFloat(), metadata["value"][3].asFloat() };
    }
    else if (metadata["type"].asString() == "Double")
    {
      handle.metadataContainer.doubleData[key] = metadata["value"].asDouble();
    }
    else if (metadata["type"].asString() == "DoubleVector2")
    {
      handle.metadataContainer.doubleVector2Data[key] = { metadata["value"][0].asDouble(), metadata["value"][1].asDouble() };
    }
    else if (metadata["type"].asString() == "DoubleVector3")
    {
      handle.metadataContainer.doubleVector3Data[key] = { metadata["value"][0].asDouble(), metadata["value"][1].asDouble(), metadata["value"][2].asDouble() };
    }
    else if (metadata["type"].asString() == "DoubleVector4")
    {
      handle.metadataContainer.doubleVector4Data[key] = { metadata["value"][0].asDouble(), metadata["value"][1].asDouble(), metadata["value"][2].asDouble(), metadata["value"][3].asDouble() };
    }
    else if (metadata["type"].asString() == "String")
    {
      handle.metadataContainer.stringData[key] = metadata["value"].asString();
    }
    else if (metadata["type"].asString() == "BLOB")
    {
      const char* value = metadata["value"].asCString();

      int len = (int)strlen(value);

      std::vector<uint8_t> &data = handle.metadataContainer.blobData[key];
      data.clear();

      bool success = Base64Decode(value, len, data);

      if (!success)
      {
        data.clear();
        return false;
      }
    }
  }

  return true;
}

static VolumeDataLayer::ProduceStatus produceStatusFromJson(Json::Value const &jsonProduceStatus)
{
  std::string produceStatusString = jsonProduceStatus.asString();

  if (produceStatusString == "Normal")
  {
    return VolumeDataLayer::ProduceStatus_Normal;
  }
  else if (produceStatusString == "Remapped")
  {
    return VolumeDataLayer::ProduceStatus_Remapped;
  }
  else if (produceStatusString == "Unavailable")
  {
    return VolumeDataLayer::ProduceStatus_Unavailable;
  }

  throw Json::Exception("Illegal produce status");
}

static CompressionMethod compressionMethodFromJson(Json::Value const &jsonCompressionMethod)
{
  std::string compressionMethodString = jsonCompressionMethod.asString();

  if(compressionMethodString == "None")
  {
    return CompressionMethod::None;
  }
  else if(compressionMethodString == "Wavelet")
  {
    return CompressionMethod::Wavelet;
  }
  else if(compressionMethodString == "RLE")
  {
    return CompressionMethod::RLE;
  }
  else if(compressionMethodString == "Zip")
  {
    return CompressionMethod::Zip;
  }
  else if(compressionMethodString == "WaveletNormalizeBlockExperimental")
  {
    return CompressionMethod::WaveletNormalizeBlock;
  }
  else if(compressionMethodString == "WaveletLossless")
  {
    return CompressionMethod::WaveletLossless;
  }
  else if(compressionMethodString == "WaveletNormalizeBlockExperimentalLossless")
  {
    return CompressionMethod::WaveletNormalizeBlockLossless;
  }
  else
  {
    throw Json::Exception("Illegal compression method");
  }
}

static MetadataStatus MetadataStatusFromJSON(Json::Value const &jsonMetadataStatus)
{
  if(jsonMetadataStatus.empty())
  {
    return MetadataStatus();
  }

  MetadataStatus metadataStatus = MetadataStatus();

  metadataStatus.m_chunkIndexCount       = jsonMetadataStatus["chunkCount"].asInt();
  metadataStatus.m_chunkMetadataPageSize = jsonMetadataStatus["chunkMetadataPageSize"].asInt();
  metadataStatus.m_chunkMetadataByteSize = jsonMetadataStatus["chunkMetadataByteSize"].asInt();
  metadataStatus.m_compressionMethod     = compressionMethodFromJson(jsonMetadataStatus["compressionMethod"]);
  metadataStatus.m_compressionTolerance  = jsonMetadataStatus["compressionTolerance"].asFloat();

  metadataStatus.m_uncompressedSize = jsonMetadataStatus["uncompressedSize"].asInt64();

  Json::Value
    adaptiveLevelSizesJSON = jsonMetadataStatus["adaptiveLevelSizes"];

  if(!adaptiveLevelSizesJSON.empty())
  {
    for(int i = 0; i < MetadataStatus::WAVELET_ADAPTIVE_LEVELS; i++)
    {
      metadataStatus.m_adaptiveLevelSizes[i] = adaptiveLevelSizesJSON[i].asInt64();
    }
  }

  return metadataStatus;
}

bool parseLayerStatus(const std::vector<uint8_t> &json, VDSHandle &handle, Error &error)
{
  handle.produceStatuses.clear();
  handle.produceStatuses.resize(int(Dimensions_45) + 1, VolumeDataLayer::ProduceStatus_Unavailable);

  Json::Value root;
  if (!parseJSONFromBuffer(json, root, error))
  {
    return false;
  }

  if (root.empty())
      return true;

  try
  {
    for (const Json::Value &layerStatus : root)
    {
      bool
        hasChunkMetadataPages = layerStatus["hasChunkMetadataPages"].asBool();

      VolumeDataLayer::ProduceStatus
        produceStatus = produceStatusFromJson(layerStatus["produceStatus"]);

      DimensionsND
        dimensionsND = dimensionsNDFromJson(layerStatus["dimensionGroup"]);

      int
        lod = layerStatus["lod"].asInt();

      if (lod == 0 && handle.produceStatuses[dimensionsND] == VolumeDataLayer::ProduceStatus_Unavailable)
      {
        handle.produceStatuses[dimensionsND] = produceStatus;
      }

      MetadataStatus
        metadataStatus = MetadataStatus();

      metadataStatus.m_chunkIndexCount = layerStatus["chunkCount"].asInt();
      metadataStatus.m_chunkMetadataPageSize = layerStatus["chunkMetadataPageSize"].asInt();
      metadataStatus.m_chunkMetadataByteSize = layerStatus["chunkMetadataByteSize"].asInt();
      metadataStatus.m_compressionMethod = compressionMethodFromJson(layerStatus["compressionMethod"]);
      metadataStatus.m_compressionTolerance = layerStatus["compressionTolerance"].asFloat();
      metadataStatus.m_uncompressedSize = layerStatus["uncompressedSize"].asInt64();

      Json::Value
        adaptiveLevelSizesJSON = layerStatus["adaptiveLevelSizes"];

      if (!adaptiveLevelSizesJSON.empty())
      {
        for (int i = 0; i < MetadataStatus::WAVELET_ADAPTIVE_LEVELS; i++)
        {
          metadataStatus.m_adaptiveLevelSizes[i] = adaptiveLevelSizesJSON[i].asInt64();
        }
      }

      std::string
        layerName = layerStatus["layerName"].asString();

      if (hasChunkMetadataPages)
      {
        std::unique_lock<std::mutex> metadataManagersMutexLock(handle.layerMetadataContainer.mutex);
        auto &managers = handle.layerMetadataContainer.managers;

        if (managers.find(layerName) == handle.layerMetadataContainer.managers.end())
        {
          int pageLimit = handle.axisDescriptors.size() <= 3 ? 64 : 1024;

          handle.layerMetadataContainer.managers.insert(std::make_pair(layerName, std::unique_ptr<MetadataManager>(new MetadataManager(handle.ioManager.get(), layerName, metadataStatus, pageLimit))));
        }
      }
    }
  }
  catch(Json::Exception e)
  {
    error.string = e.what();
    error.code = -1;
    return false;
  }

  return true;
}

static int32_t getInternalCubeSizeLOD0(const VolumeDataLayoutDescriptor &desc)
{
  int32_t size = int32_t(1) << desc.getBrickSize();

  size -= desc.getNegativeMargin();
  size -= desc.getPositiveMargin();

  assert(size > 0);

  return size;
}

static int32_t getLODCount(const VolumeDataLayoutDescriptor &desc)
{
  return desc.getLODLevels() + 1;
}

static void createVolumeDataLayout(VDSHandle &handle)
{
  //handle.volumeDataLayout.reset(new VolumeDataLayout(handle.channelDescriptors)
  int32_t dimensionality = int32_t(handle.axisDescriptors.size());

  // Check if input layouts are valid so we can create a new layout
  if (dimensionality < 2)
  {
    handle.volumeDataLayout.reset();
    return;
  }

  handle.volumeDataLayout.reset(
    new VolumeDataLayout(
      handle,
      handle.layoutDescriptor,
      handle.axisDescriptors,
      handle.channelDescriptors,
      0, //MIA for now
      { 1, 0 }, //MIA for now
      VolumeDataHash::getUniqueHash(),
      CompressionMethod::None,
      0,
      false,
      0));

  for(int32_t iDimensionGroup = 0; iDimensionGroup < DimensionGroup_3D_Max; iDimensionGroup++)
  {
    DimensionGroup dimensionGroup = (DimensionGroup)iDimensionGroup;

    int32_t nChunkDimensionality = DimensionGroupUtil::getDimensionality(dimensionGroup);

        // Check if highest dimension in chunk is higher than the highest dimension in the dataset or 1D
    if(DimensionGroupUtil::getDimension(dimensionGroup, nChunkDimensionality - 1) >= dimensionality ||
       nChunkDimensionality == 1)
    {
      continue;
    }

    assert(nChunkDimensionality == 2 || nChunkDimensionality == 3);

    int32_t physicalLODLevels = (nChunkDimensionality == 3 || handle.layoutDescriptor.isCreate2DLODs()) ? getLODCount(handle.layoutDescriptor) : 1;
    int32_t brickSize = getInternalCubeSizeLOD0(handle.layoutDescriptor) * (nChunkDimensionality == 2 ? handle.layoutDescriptor.getBrickSizeMultiplier2D() : 1);

    handle.volumeDataLayout->createRenderLayers(dimensionGroup, brickSize, physicalLODLevels);
  }
}

class SyncTransferHandler : public TransferHandler
{
public:
    void handleData(std::vector<uint8_t> &&data) override
    {
      *(this->data) = data;
    }
    void handleError(Error &error) override
    {
      *(this->error) = error;
    }

  std::vector<uint8_t> *data;
  Error *error;
};

bool downloadAndParseVDSJson(VDSHandle& handle, Error& error)
{
  std::vector<uint8_t> volumedatalayout_json;
  std::shared_ptr<SyncTransferHandler> syncTransferHandler = std::make_shared<SyncTransferHandler>();
  syncTransferHandler->error = &error;
  syncTransferHandler->data = &volumedatalayout_json;
  auto req = handle.ioManager->requestObject("VolumeDataLayout", syncTransferHandler);
  req->waitForFinish();
  if (!req->isSuccess(error) || volumedatalayout_json.empty())
  {
    error.string = "S3 Error on downloading VolumeDataLayout object: " + error.string;
    return false;
  }
  std::vector<uint8_t> layerstatus_json;
  syncTransferHandler->data = &layerstatus_json;
  req = handle.ioManager->requestObject("LayerStatus", syncTransferHandler);
  req->waitForFinish();
  if (!req->isSuccess(error) || layerstatus_json.empty())
  {
    error.string = "S3 Error on downloading LayerStatus object: " + error.string;
    return false;
  }

  try
  {
    if (!parseVolumeDataLayout(volumedatalayout_json, handle, error))
      return false;
    if (!parseLayerStatus(layerstatus_json, handle, error))
      return false;
  }
  catch (Json::Exception& e)
  {
    error.string = e.what();
    error.code = -2;
    return false;
  }

  createVolumeDataLayout(handle);

  return true;
}

bool serializeAndUploadVDSJson(VDSHandle& handle, Error& error)
{
  return true;
}

}
