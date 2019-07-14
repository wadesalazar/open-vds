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
#include <IO/S3_Downloader.h>

#include <json/json.h>

#include "Bitmask.h"

namespace OpenVDS
{

class Base64Table
{
  static const char alphabet[65];
  static unsigned char table[256];

  static Base64Table    instance;

  Base64Table()
  {
    memset(table, -1, sizeof(table));

    for(int i = 0; i < sizeof(alphabet) - 1; i++)
    {
      table[alphabet[i]] = i;
    }
  }

public:
  static int  decode(char a) { return table[(unsigned char)a]; }
  static char encode(unsigned char u) { return alphabet[u]; }
};

Base64Table
Base64Table::instance;

const char
Base64Table::alphabet[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

unsigned char
Base64Table::table[256];

bool
Base64Decode(const char *data, int len, std::vector<unsigned char> &result)
{
  bool error = false;

  int
    decode = 0;

  // skip leading whitespace
  while(len && isspace(*data)) len--, data++;
  if(len == 0) { error = true; }

  result.reserve(result.size() + len / 4 * 3);

  while(len != 0 && !isspace(*data))
  {
    int a = Base64Table::decode(*data++); len--; if(a == -1) { error = true; break; }

    if(len == 0 || isspace(*data)) { error = true; break; }
    int b = Base64Table::decode(*data++); len--; if(b == -1) { error = true; break; }
    result.push_back((a << 2) | (b >> 4));

    if(len == 0 || isspace(*data)) break; if(*data == '=') { data++; len--; if(len==0 || *data++ != '=') { error = true; break; } len--; break; }
    int c = Base64Table::decode(*data++); len--; if(c == -1) { error = true; break; }
    result.push_back(((b & 0xf) << 4) | (c >> 2));

    if(len == 0 || isspace(*data)) break; if(*data == '=') { data++; len--; break; }
    int d = Base64Table::decode(*data++); len--; if(d == -1) { error = true; break; }
    result.push_back(((c & 0x3) << 6) | d);
  }

  // skip trailing whitespace
  while(len && isspace(*data)) len--, data++;
  if(len != 0) { error = true; }

  return !error;
}

static enum VolumeDataLayoutDescriptor::BrickSize convertToBrickSize(Json::Value const &jsonBrickSize)
{
  std::string brickSizeString = jsonBrickSize.asString();

  if(brickSizeString == "HUEVCSIZE_32")
  {
    return VolumeDataLayoutDescriptor::BrickSize_32;
  }
  else if(brickSizeString == "HUEVCSIZE_64")
  {
    return VolumeDataLayoutDescriptor::BrickSize_64;
  }
  else if(brickSizeString == "HUEVCSIZE_128")
  {
    return VolumeDataLayoutDescriptor::BrickSize_128;
  }
  else if(brickSizeString == "HUEVCSIZE_256")
  {
    return VolumeDataLayoutDescriptor::BrickSize_256;
  }
  else if(brickSizeString == "HUEVCSIZE_512")
  {
    return VolumeDataLayoutDescriptor::BrickSize_512;
  }
  else if(brickSizeString == "HUEVCSIZE_1024")
  {
    return VolumeDataLayoutDescriptor::BrickSize_1024;
  }
  else if(brickSizeString == "HUEVCSIZE_2048")
  {
    return VolumeDataLayoutDescriptor::BrickSize_2048;
  }
  else if(brickSizeString == "HUEVCSIZE_4096")
  {
    return VolumeDataLayoutDescriptor::BrickSize_4096;
  }

  throw Json::Exception("Illegal brick size");
}

static enum VolumeDataLayoutDescriptor::LodLevels convertToLodLevel(Json::Value const &jsonLodLevels)
{
  std::string lodLevelString = jsonLodLevels.asString();

  if(lodLevelString == "LODLevelNone")
  {
    return VolumeDataLayoutDescriptor::LodLevelNone;
  }
  else if(lodLevelString == "LODLevel1")
  {
    return VolumeDataLayoutDescriptor::LodLevel1;
  }
  else if(lodLevelString == "LODLevel2")
  {
    return VolumeDataLayoutDescriptor::LodLevel2;
  }
  else if(lodLevelString == "LODLevel3")
  {
    return VolumeDataLayoutDescriptor::LodLevel3;
  }
  else if(lodLevelString == "LODLevel4")
  {
    return VolumeDataLayoutDescriptor::LodLevel4;
  }
  else if(lodLevelString == "LODLevel5")
  {
    return VolumeDataLayoutDescriptor::LodLevel5;
  }
  else if(lodLevelString == "LODLevel6")
  {
    return VolumeDataLayoutDescriptor::LodLevel6;
  }
  else if(lodLevelString == "LODLevel7")
  {
    return VolumeDataLayoutDescriptor::LodLevel7;
  }
  else if(lodLevelString == "LODLevel8")
  {
    return VolumeDataLayoutDescriptor::LodLevel8;
  }
  else if(lodLevelString == "LODLevel9")
  {
    return VolumeDataLayoutDescriptor::LodLevel9;
  }
  else if(lodLevelString == "LODLevel10")
  {
    return VolumeDataLayoutDescriptor::LodLevel10;
  }
  else if(lodLevelString == "LODLevel11")
  {
    return VolumeDataLayoutDescriptor::LodLevel11;
  }
  else if(lodLevelString == "LODLevel12")
  {
    return VolumeDataLayoutDescriptor::LodLevel12;
  }
  throw Json::Exception("Illegal lod levels");
}

static int convertToDimensionality(Json::Value const &jsonDimensionlaity)
{
  std::string dimensionalityString = jsonDimensionlaity.asString();
  if(dimensionalityString == "HUEDIMENSION_1")
  {
    return 1;
  }
  else if(dimensionalityString == "HUEDIMENSION_2")
  {
    return 2;
  }
  else if(dimensionalityString == "HUEDIMENSION_3")
  {
    return 3;
  }
  else if(dimensionalityString == "HUEDIMENSION_4")
  {
    return 4;
  }
  else if(dimensionalityString == "HUEDIMENSION_5")
  {
    return 5;
  }
  else if(dimensionalityString == "HUEDIMENSION_6")
  {
    return 6;
  }
  throw Json::Exception("Illegal dimensionality");
}

const char *addDescriptorString(std::string const &descriptorString, VDSHandle &handle)
{
  char *data = new char[descriptorString.size() + 1];
  memcpy(data, descriptorString.data(), descriptorString.size());
  data[descriptorString.size()] = 0;
  handle.descriptorStrings.emplace_back(data);
  return nullptr;
}

static VolumeDataChannelDescriptor::Format convertToVoxelFormat(Json::Value const &jsonVoxelFormat)
{
  std::string voxelFormatString = jsonVoxelFormat.asString();

  if(voxelFormatString == "HUEVOXELFORMAT_R32")
  {
    return VolumeDataChannelDescriptor::FormatR32;
  }
  else if(voxelFormatString == "HUEVOXELFORMAT_R64")
  {
    return VolumeDataChannelDescriptor::FormatR64;
  }
  else if(voxelFormatString == "HUEVOXELFORMAT_U8")
  {
    return VolumeDataChannelDescriptor::FormatU8;
  }
  else if(voxelFormatString == "HUEVOXELFORMAT_U16")
  {
    return VolumeDataChannelDescriptor::FormatU16;
  }
  else if(voxelFormatString == "HUEVOXELFORMAT_U32")
  {
    return VolumeDataChannelDescriptor::FormatU32;
  }
  else if(voxelFormatString == "HUEVOXELFORMAT_U64")
  {
    return VolumeDataChannelDescriptor::FormatU64;
  }
  else if(voxelFormatString == "HUEVOXELFORMAT_1BIT")
  {
    return VolumeDataChannelDescriptor::Format1Bit;
  }
  throw Json::Exception("Illegal voxel format");
}

static VolumeDataChannelDescriptor::Components convertToVoxelComponents(Json::Value const &jsonVoxelComponents)
{
  std::string voxelComponentsString = jsonVoxelComponents.asString();

  if(voxelComponentsString == "HUEVOXELCOMPONENTS_1")
  {
    return VolumeDataChannelDescriptor::Components_1;
  }
  else if(voxelComponentsString == "HUEVOXELCOMPONENTS_2")
  {
    return VolumeDataChannelDescriptor::Components_2;
  }
  else if(voxelComponentsString == "HUEVOXELCOMPONENTS_4")
  {
    return VolumeDataChannelDescriptor::Components_4;
  }
  throw Json::Exception("Illegal voxel components");
}

static VolumeDataMapping convertToChannelMapping(Json::Value const &jsonChannelMapping)
{
  if(VolumeDataMapping(jsonChannelMapping.asUInt64()) == VolumeDataMapping::PerTrace)
  {
    return VolumeDataMapping::PerTrace;
  }
  else if(VolumeDataMapping(jsonChannelMapping.asUInt64()) == VolumeDataMapping::Direct)
  {
    return VolumeDataMapping::Direct;
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

static bool parseVDSObject(const std::vector<uint8_t> &json, VDSHandle &handle, Error &error)
{
  Json::Value root;
  if (!parseJSONFromBuffer(json, root, error))
  {
    return false;
  }

  if (root.empty())
    return true;
  

  enum VolumeDataLayoutDescriptor::BrickSize brickSize = convertToBrickSize(root["FullVCSize"]);
  enum VolumeDataLayoutDescriptor::LodLevels lodLevel = convertToLodLevel(root["LODLevels"]);
  Internal::BitMask<enum VolumeDataLayoutDescriptor::Options> options(root["Create2DLODs"].asBool() ? VolumeDataLayoutDescriptor::Options_Create2DLods : VolumeDataLayoutDescriptor::Options_None);
  options |= root["ForceFullResolutionDimension"].asBool() ? VolumeDataLayoutDescriptor::Options_ForceFullResolutionDimension : VolumeDataLayoutDescriptor::Options_None;
  int brickSizeMultiplier2D = root.isMember("2DBrickSizeMultiplier") ? root["2DBrickSizeMultiplier"].asInt() : 4;
  handle.layoutDescriptor = VolumeDataLayoutDescriptor(brickSize,
                                                  root["NegativeMargin"].asInt(),
                                                  root["PositiveMargin"].asInt(),
                                                  brickSizeMultiplier2D,
                                                  lodLevel,
                                                  options.to_enum(),
                                                  root["FullResolutionDimension"].asInt());

  int dimensionality = convertToDimensionality(root["Dimensions"]);

  for (int dimension = 0; dimension < dimensionality; dimension++)
  {
    std::string
      prefix = std::string("Dimension") + std::to_string(dimension);

    handle.axisDescriptors.emplace_back(root[prefix + "Size"].asInt(),
                                       addDescriptorString(root[prefix + "Name"].asString(), handle),
                                       addDescriptorString(root[prefix + "Unit"].asString(), handle),
                                       root[prefix + "Coordinate"][0].asFloat(),
                                       root[prefix + "Coordinate"][1].asFloat());

  }

  VolumeDataChannelDescriptor primaryChannel;

  VolumeDataChannelDescriptor::Format format = convertToVoxelFormat(root["VoxelFormat"]);
  VolumeDataChannelDescriptor::Components components = convertToVoxelComponents(root["VoxelComponents"]);

  if (root["UseNoValue"].asBool())
  {
    primaryChannel = VolumeDataChannelDescriptor(
      format,
      components,
      addDescriptorString(root["ValueName"].asString(), handle),
      addDescriptorString(root["ValueUnit"].asString(), handle),
      root["ValueRange"][0].asFloat(),
      root["ValueRange"][1].asFloat(),
      VolumeDataMapping::Direct, 1,
      root["DiscreteData"].asBool() ? VolumeDataChannelDescriptor::DiscreteData : VolumeDataChannelDescriptor::Default,
      root["NoValue"].asFloat(),
      root["IntegerScale"].asFloat(),
      root["IntegerOffset"].asFloat());
  }
  else
  {
    primaryChannel = VolumeDataChannelDescriptor(
      format,
      components,
      addDescriptorString(root["ValueName"].asCString(), handle),
      addDescriptorString(root["ValueUnit"].asCString(), handle),
      root["ValueRange"][0].asFloat(),
      root["ValueRange"][1].asFloat(),
      VolumeDataMapping::Direct, 1,
      root["DiscreteData"].asBool() ? VolumeDataChannelDescriptor::DiscreteData : VolumeDataChannelDescriptor::Default,
      root["IntegerScale"].asFloat(),
      root["IntegerOffset"].asFloat());
  }

  handle.channelDescriptors.push_back(primaryChannel);

  for (const Json::Value &channelDescriptor : root["VolumeDataChannelDescriptor"])
  {
    VolumeDataChannelDescriptor additionalChannel;

    format = convertToVoxelFormat(channelDescriptor["Format"]);
    components = convertToVoxelComponents(channelDescriptor["Components"]);
    VolumeDataMapping  mapping = convertToChannelMapping(channelDescriptor["ChannelMapping"]);
    Internal::BitMask<VolumeDataChannelDescriptor::Flags> flags(channelDescriptor["DiscreteData"].asBool() ? VolumeDataChannelDescriptor::DiscreteData : VolumeDataChannelDescriptor::Default);
    flags |= (channelDescriptor["Renderable"].asBool() ? VolumeDataChannelDescriptor::Default : VolumeDataChannelDescriptor::NotRenderable);
    flags |= (channelDescriptor["AllowLossyCompression"].asBool() ? VolumeDataChannelDescriptor::Default : VolumeDataChannelDescriptor::NoLossyCompression);

    if (channelDescriptor["UseNoValue"].asBool())
    {
      additionalChannel = VolumeDataChannelDescriptor(
        format,
        components,
        addDescriptorString(channelDescriptor["Name"].asString(), handle),
        addDescriptorString(channelDescriptor["Unit"].asString(), handle),
        channelDescriptor["ValueRange"][0].asFloat(),
        channelDescriptor["ValueRange"][1].asFloat(),
        mapping,
        channelDescriptor["MappedValues"].asInt(),
        flags.to_enum(),
        channelDescriptor["NoValue"].asFloat(),
        channelDescriptor["IntegerScale"].asFloat(),
        channelDescriptor["IntegerOffset"].asFloat());
    }
    else
    {
      additionalChannel = VolumeDataChannelDescriptor(
        format,
        components,
        addDescriptorString(channelDescriptor["Name"].asString(), handle),
        addDescriptorString(channelDescriptor["Unit"].asString(), handle),
        channelDescriptor["ValueRange"][0].asFloat(),
        channelDescriptor["ValueRange"][1].asFloat(),
        mapping,
        channelDescriptor["MappedValues"].asInt(),
        flags.to_enum(),
        channelDescriptor["IntegerScale"].asFloat(),
        channelDescriptor["IntegerOffset"].asFloat());
    }

    handle.channelDescriptors.push_back(additionalChannel);
  }

  for (const Json::Value &metadata : root["MetadataList"])
  {
    MetadataKey key = { metadata["Category"].asString(), metadata["Name"].asString() };

    if (metadata["Type"].asString() == "Int")
    {
      handle.metaDataContainer.intData[key] = metadata["Value"].asInt();
    }
    else if (metadata["Type"].asString() == "IntVector2")
    {
      handle.metaDataContainer.intVector2Data[key] = { metadata["Value"][0].asInt(), metadata["Value"][1].asInt()};
    }
    else if (metadata["Type"].asString() == "IntVector3")
    {
      handle.metaDataContainer.intVector3Data[key]  = { metadata["Value"][0].asInt(), metadata["Value"][1].asInt(), metadata["Value"][2].asInt() };
    }
    else if (metadata["Type"].asString() == "IntVector4")
    {
      handle.metaDataContainer.intVector4Data[key] = { metadata["Value"][0].asInt(), metadata["Value"][1].asInt(), metadata["Value"][2].asInt(), metadata["Value"][3].asInt() };
    }
    else if (metadata["Type"].asString() == "Float")
    {
      handle.metaDataContainer.floatData[key] = metadata["Value"].asFloat();
    }
    else if (metadata["Type"].asString() == "FloatVector2")
    {
      handle.metaDataContainer.floatVector2Data[key] = { metadata["Value"][0].asFloat(), metadata["Value"][1].asFloat() };
    }
    else if (metadata["Type"].asString() == "FloatVector3")
    {
      handle.metaDataContainer.floatVector3Data[key] = { metadata["Value"][0].asFloat(), metadata["Value"][1].asFloat(), metadata["Value"][2].asFloat() };
    }
    else if (metadata["Type"].asString() == "FloatVector4")
    {
      handle.metaDataContainer.floatVector4Data[key] = { metadata["Value"][0].asFloat(), metadata["Value"][1].asFloat(), metadata["Value"][2].asFloat(), metadata["Value"][3].asFloat() };
    }
    else if (metadata["Type"].asString() == "Double")
    {
      handle.metaDataContainer.doubleData[key] = metadata["Value"].asDouble();
    }
    else if (metadata["Type"].asString() == "DoubleVector2")
    {
      handle.metaDataContainer.doubleVector2Data[key] = { metadata["Value"][0].asDouble(), metadata["Value"][1].asDouble() };
    }
    else if (metadata["Type"].asString() == "DoubleVector3")
    {
      handle.metaDataContainer.doubleVector3Data[key] = { metadata["Value"][0].asDouble(), metadata["Value"][1].asDouble(), metadata["Value"][2].asDouble() };
    }
    else if (metadata["Type"].asString() == "DoubleVector4")
    {
      handle.metaDataContainer.doubleVector4Data[key] = { metadata["Value"][0].asDouble(), metadata["Value"][1].asDouble(), metadata["Value"][2].asDouble(), metadata["Value"][3].asDouble() };
    }
    else if (metadata["Type"].asString() == "String")
    {
      handle.metaDataContainer.stringData[key] = metadata["Value"].asString();
    }
    else if (metadata["Type"].asString() == "BLOB")
    {
      const char* value = metadata["Value"].asCString();

      int len = (int)strlen(value);

      std::vector<uint8_t> &data = handle.metaDataContainer.blobData[key];
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

static VolumeDataLayer::ProduceStatus produceStatusFromJSON(Json::Value const &jsonProduceStatus)
{
  std::string produceStatusString = jsonProduceStatus.asString();

  if (produceStatusString == "Normal")
  {
    return VolumeDataLayer::ProduceStatusNormal;
  }
  else if (produceStatusString == "Remapped")
  {
    return VolumeDataLayer::ProduceStatusRemapped;
  }
  else if (produceStatusString == "Unavailable")
  {
    return VolumeDataLayer::ProduceStatusUnavailable;
  }

  throw Json::Exception("Illegal produce status");
}

static bool parseProduceStatus(const std::vector<uint8_t> &json, VDSHandle &handle, Error &error)
{
  handle.produceStatuses.clear();
  handle.produceStatuses.resize(int(DimensionsND::Group45) + 1, VolumeDataLayer::ProduceStatusUnavailable);
  Json::Value root;
  if (!parseJSONFromBuffer(json, root, error))
  {
    return false;
  }

  if (root.empty())
    return true;

  handle.produceStatuses[int32_t(DimensionsND::Group012)] = produceStatusFromJSON(root["ProduceStatusDimensions_012"]);
  handle.produceStatuses[int32_t(DimensionsND::Group013)] = produceStatusFromJSON(root["ProduceStatusDimensions_013"]);
  handle.produceStatuses[int32_t(DimensionsND::Group014)] = produceStatusFromJSON(root["ProduceStatusDimensions_014"]);
  handle.produceStatuses[int32_t(DimensionsND::Group015)] = produceStatusFromJSON(root["ProduceStatusDimensions_015"]);
  handle.produceStatuses[int32_t(DimensionsND::Group023)] = produceStatusFromJSON(root["ProduceStatusDimensions_023"]);
  handle.produceStatuses[int32_t(DimensionsND::Group024)] = produceStatusFromJSON(root["ProduceStatusDimensions_024"]);
  handle.produceStatuses[int32_t(DimensionsND::Group025)] = produceStatusFromJSON(root["ProduceStatusDimensions_025"]);
  handle.produceStatuses[int32_t(DimensionsND::Group034)] = produceStatusFromJSON(root["ProduceStatusDimensions_034"]);
  handle.produceStatuses[int32_t(DimensionsND::Group035)] = produceStatusFromJSON(root["ProduceStatusDimensions_035"]);
  handle.produceStatuses[int32_t(DimensionsND::Group045)] = produceStatusFromJSON(root["ProduceStatusDimensions_045"]);
  handle.produceStatuses[int32_t(DimensionsND::Group123)] = produceStatusFromJSON(root["ProduceStatusDimensions_123"]);
  handle.produceStatuses[int32_t(DimensionsND::Group124)] = produceStatusFromJSON(root["ProduceStatusDimensions_124"]);
  handle.produceStatuses[int32_t(DimensionsND::Group125)] = produceStatusFromJSON(root["ProduceStatusDimensions_125"]);
  handle.produceStatuses[int32_t(DimensionsND::Group134)] = produceStatusFromJSON(root["ProduceStatusDimensions_134"]);
  handle.produceStatuses[int32_t(DimensionsND::Group135)] = produceStatusFromJSON(root["ProduceStatusDimensions_135"]);
  handle.produceStatuses[int32_t(DimensionsND::Group145)] = produceStatusFromJSON(root["ProduceStatusDimensions_145"]);
  handle.produceStatuses[int32_t(DimensionsND::Group234)] = produceStatusFromJSON(root["ProduceStatusDimensions_234"]);
  handle.produceStatuses[int32_t(DimensionsND::Group235)] = produceStatusFromJSON(root["ProduceStatusDimensions_235"]);
  handle.produceStatuses[int32_t(DimensionsND::Group245)] = produceStatusFromJSON(root["ProduceStatusDimensions_245"]);
  handle.produceStatuses[int32_t(DimensionsND::Group345)] = produceStatusFromJSON(root["ProduceStatusDimensions_345"]);
  handle.produceStatuses[int32_t(DimensionsND::Group01)] = produceStatusFromJSON(root["ProduceStatusDimensions_01"]);
  handle.produceStatuses[int32_t(DimensionsND::Group02)] = produceStatusFromJSON(root["ProduceStatusDimensions_02"]);
  handle.produceStatuses[int32_t(DimensionsND::Group03)] = produceStatusFromJSON(root["ProduceStatusDimensions_03"]);
  handle.produceStatuses[int32_t(DimensionsND::Group04)] = produceStatusFromJSON(root["ProduceStatusDimensions_04"]);
  handle.produceStatuses[int32_t(DimensionsND::Group05)] = produceStatusFromJSON(root["ProduceStatusDimensions_05"]);
  handle.produceStatuses[int32_t(DimensionsND::Group12)] = produceStatusFromJSON(root["ProduceStatusDimensions_12"]);
  handle.produceStatuses[int32_t(DimensionsND::Group13)] = produceStatusFromJSON(root["ProduceStatusDimensions_13"]);
  handle.produceStatuses[int32_t(DimensionsND::Group14)] = produceStatusFromJSON(root["ProduceStatusDimensions_14"]);
  handle.produceStatuses[int32_t(DimensionsND::Group15)] = produceStatusFromJSON(root["ProduceStatusDimensions_15"]);
  handle.produceStatuses[int32_t(DimensionsND::Group23)] = produceStatusFromJSON(root["ProduceStatusDimensions_23"]);
  handle.produceStatuses[int32_t(DimensionsND::Group24)] = produceStatusFromJSON(root["ProduceStatusDimensions_24"]);
  handle.produceStatuses[int32_t(DimensionsND::Group25)] = produceStatusFromJSON(root["ProduceStatusDimensions_25"]);
  handle.produceStatuses[int32_t(DimensionsND::Group34)] = produceStatusFromJSON(root["ProduceStatusDimensions_34"]);
  handle.produceStatuses[int32_t(DimensionsND::Group35)] = produceStatusFromJSON(root["ProduceStatusDimensions_35"]);
  handle.produceStatuses[int32_t(DimensionsND::Group45)] = produceStatusFromJSON(root["ProduceStatusDimensions_45"]);

  return true;
}

static bool parseMetaDataStatus(const std::vector<uint8_t> &json, VDSHandle &handle, Error &error)
{
  return true;
}

static int32_t getInternalCubeSizeLod0(const VolumeDataLayoutDescriptor &desc)
{
  int32_t size = int32_t(1) << desc.getBrickSize();

  size -= desc.getNegativeMargin();
  size -= desc.getPositiveMargin();

  assert(size > 0);

  return size;
}

static int32_t getLodCount(const VolumeDataLayoutDescriptor &desc)
{
  return desc.getLodLevels() + 1;
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

    int32_t physicalLODLevels = (nChunkDimensionality == 3 || handle.layoutDescriptor.isCreate2DLods()) ? getLodCount(handle.layoutDescriptor) : 1;
    int32_t brickSize = getInternalCubeSizeLod0(handle.layoutDescriptor) * (nChunkDimensionality == 2 ? handle.layoutDescriptor.getBrickSizeMultiplier2D() : 1);

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
  std::vector<uint8_t> vdsobject_json;
  std::shared_ptr<SyncTransferHandler> syncTransferHandler = std::make_shared<SyncTransferHandler>();
  syncTransferHandler->error = &error;
  syncTransferHandler->data = &vdsobject_json;
  auto req = handle.ioManager->requestObject("", syncTransferHandler);
  req->waitForFinish();
  if (!req->isSuccess(error) || vdsobject_json.empty())
  {
    error.string = "S3 Error on downloading root object: " + error.string;
    return false;
  }
  std::vector<uint8_t> producestatus_json;
  syncTransferHandler->data = &producestatus_json;
  req = handle.ioManager->requestObject("ProduceStatus", syncTransferHandler);
  req->waitForFinish();
  if (!req->isSuccess(error) || producestatus_json.empty())
  {
    error.string = "S3 Error on downloading file ProduceStatus: " + error.string;
    return false;
  }
  std::vector<uint8_t> metadatastatus_json;
  syncTransferHandler->data = &metadatastatus_json;
  req = handle.ioManager->requestObject("MetadataStatus", syncTransferHandler);
  req->waitForFinish();
  if (!req->isSuccess(error) || metadatastatus_json.empty())
  {
    error.string = "S3 Error on downloading file MetadataStatus: " + error.string;
    return false;
  }

  try
  {
    if (!parseVDSObject(vdsobject_json, handle, error))
      return false;
    if (!parseProduceStatus(producestatus_json, handle, error))
      return false;
    if (!parseMetaDataStatus(metadatastatus_json, handle, error))
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

