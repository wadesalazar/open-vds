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

#include "SEGYFileInfo.h"
#include "IO/File.h"

#include "OpenVDS/KnownMetadata.h"
#include "Math/Range.h"
#include "VDS/VolumeDataLayoutDescriptor.h"
#include "VDS/VolumeDataAxisDescriptor.h"
#include "VDS/VolumeDataChannelDescriptor.h"
#include "VDS/VolumeDataLayout.h"
#include "cxxopts.hpp"

#include <cstdlib>
#include <json/json.h>

template<typename T>
void
heapInsert(std::vector<float> heap, int heapSizeMax, float element, T predicate)
{
  assert(heapSizeMax > 0);
  assert((int)heap.size() <= heapSizeMax);

  int
    heapSize = (int)heap.size();

  // Grow heap until we reach heapSize
  if(heapSize < heapSizeMax)
  {
    heap.push_back(element);

    for(int index = heapSize; index > 0; )
    {
      int parent = index / 2;

      if(predicate(heap[index], heap[parent]))
      {
        std::swap(heap[index], heap[parent]);
        index = parent;
      }
      else
      {
        break;
      }
    }
  }
  else if(predicate(element, heap[0]))
  {
    heap[0] = element;

    for(int index = 0; index < heapSize;)
    {
      int left = index * 2, right = left + 1;

      if(left < heapSize && predicate(heap[left], heap[index]))
      {
        std::swap(heap[left], heap[index]);
        index = left;
      }
      else if(right < heapSize && predicate(heap[right], heap[index]))
      {
        std::swap(heap[right], heap[index]);
        index = right;
      }
      else
      {
        break;
      }
    }
  }
}

SEGY::Endianness
endiannessFromJson(Json::Value const &jsonEndianness)
{
  std::string
    endiannessString = jsonEndianness.asString();

  if(endiannessString == "BigEndian")
  {
    return SEGY::Endianness::BigEndian;
  }
  else if(endiannessString == "LittleEndian")
  {
    return SEGY::Endianness::LittleEndian;
  }

  throw Json::Exception("Illegal endianness");
}

SEGYBinInfo
binInfoFromJson(Json::Value const &jsonBinInfo)
{
  int inlineNumber    = jsonBinInfo["inlineNumber"].asInt();
  int crosslineNumber = jsonBinInfo["crosslineNumber"].asInt();
  double ensembleXCoordinate = jsonBinInfo["ensembleXCoordinate"].asDouble();
  double ensembleYCoordinate = jsonBinInfo["ensembleYCoordinate"].asDouble();

  return SEGYBinInfo(inlineNumber, crosslineNumber, ensembleXCoordinate, ensembleYCoordinate);
}

SEGYSegmentInfo
segmentInfoFromJson(Json::Value const &jsonSegmentInfo)
{
  int primaryKey = jsonSegmentInfo["primaryKey"].asInt();
  int traceStart = jsonSegmentInfo["traceStart"].asInt();
  int traceStop  = jsonSegmentInfo["traceStop"].asInt();
  SEGYBinInfo binInfoStart = binInfoFromJson(jsonSegmentInfo["binInfoStart"]);
  SEGYBinInfo binInfoStop  = binInfoFromJson(jsonSegmentInfo["binInfoStop"]);

  return SEGYSegmentInfo(primaryKey, traceStart, traceStop, binInfoStart, binInfoStop);
}

bool
parseSEGYFileInfoFile(OpenVDS::File const &file, SEGYFileInfo &fileInfo)
{
  OpenVDS::IOError error;

  int64_t fileSize = file.size(error);

  if(error.code != 0)
  {
    return false;
  }

  if(fileSize > INT_MAX)
  {
    return false;
  }

  std::unique_ptr<char[]>
    buffer(new char[fileSize]);

  file.read(buffer.get(), 0, (int32_t)fileSize, error);

  if(error.code != 0)
  {
    return false;
  }

  try
  {
    Json::CharReaderBuilder
      rbuilder;

    rbuilder["collectComments"] = false;

    std::string
      errs;

    std::unique_ptr<Json::CharReader>
      reader(rbuilder.newCharReader());

    Json::Value
      jsonFileInfo;

    bool
      success = reader->parse(buffer.get(), buffer.get() + fileSize, &jsonFileInfo, &errs);

    if(!success)
    {
      throw Json::Exception(errs);
    }

    fileInfo.m_headerEndianness = endiannessFromJson(jsonFileInfo["headerEndianness"]);
    fileInfo.m_dataSampleFormatCode = SEGY::BinaryHeader::DataSampleFormatCode(jsonFileInfo["dataSampleFormatCode"].asInt());
    fileInfo.m_sampleCount = jsonFileInfo["sampleCount"].asInt();
    fileInfo.m_sampleIntervalMilliseconds = jsonFileInfo["sampleInterval"].asDouble();
    fileInfo.m_traceCount = jsonFileInfo["traceCount"].asInt64();


    for (Json::Value jsonSegmentInfo : jsonFileInfo["segmentInfo"])
    {
      fileInfo.m_segmentInfo.push_back(segmentInfoFromJson(jsonSegmentInfo));
    }
  }
  catch(Json::Exception e)
  {
    std::cerr << "Failed to parse JSON SEG-Y file info file: " << e.what();
    return false;
  }

  return true;
}

std::vector<OpenVDS::VolumeDataAxisDescriptor>
createAxisDescriptors(SEGYFileInfo const &fileInfo)
{
  std::vector<OpenVDS::VolumeDataAxisDescriptor>
    axisDescriptors;

  axisDescriptors.push_back(OpenVDS::VolumeDataAxisDescriptor(fileInfo.m_sampleCount, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE, "ms", 0.0f, (fileInfo.m_sampleCount - 1) * (float)fileInfo.m_sampleIntervalMilliseconds));

  int
    inlineStep = 0,
    crosslineStep = 0;

  int
    minInline    = fileInfo.m_segmentInfo[0].m_binInfoStart.m_inlineNumber,    maxInline    = minInline,
    minCrossline = fileInfo.m_segmentInfo[0].m_binInfoStart.m_crosslineNumber, maxCrossline = minCrossline;

  for(auto const &segmentInfo : fileInfo.m_segmentInfo)
  {
    minInline = std::min(minInline, segmentInfo.m_binInfoStart.m_inlineNumber);
    minInline = std::min(minInline, segmentInfo.m_binInfoStop.m_inlineNumber);
    maxInline = std::max(maxInline, segmentInfo.m_binInfoStart.m_inlineNumber);
    maxInline = std::max(maxInline, segmentInfo.m_binInfoStop.m_inlineNumber);

    minCrossline = std::min(minCrossline, segmentInfo.m_binInfoStart.m_crosslineNumber);
    minCrossline = std::min(minCrossline, segmentInfo.m_binInfoStop.m_crosslineNumber);
    maxCrossline = std::max(maxCrossline, segmentInfo.m_binInfoStart.m_crosslineNumber);
    maxCrossline = std::max(maxCrossline, segmentInfo.m_binInfoStop.m_crosslineNumber);
  }

  axisDescriptors.push_back(OpenVDS::VolumeDataAxisDescriptor(fileInfo.m_sampleCount, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE,   "ms", 0.0f, (fileInfo.m_sampleCount - 1) * (float)fileInfo.m_sampleIntervalMilliseconds));
  axisDescriptors.push_back(OpenVDS::VolumeDataAxisDescriptor(fileInfo.m_sampleCount, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE,  "", (float)minCrossline, (float)maxCrossline));
  axisDescriptors.push_back(OpenVDS::VolumeDataAxisDescriptor(fileInfo.m_sampleCount, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE,     "", (float)minInline,    (float)maxInline));

  return axisDescriptors;
}

std::vector<OpenVDS::VolumeDataChannelDescriptor>
createChannelDescriptors(SEGYFileInfo const &fileInfo, OpenVDS::FloatRange const &valueRange)
{
  std::vector<OpenVDS::VolumeDataChannelDescriptor>
    channelDescriptors;

  return channelDescriptors;
}

Json::Value
serializeVolumeDataLayout(OpenVDS::VolumeDataLayout const &layout)
{
  Json::Value
    jsonLayout;

  Json::Value
    vcl;

  return jsonLayout;
}

int
main(int argc, char *argv[])
{
  cxxopts::Options options("SEGYUpload", "SEGYUpload - A tool to upload a SEG-Y file to a volume data store (VDS)");
  options.positional_help("<input file>");

  std::vector<std::string> fileNames;
  std::string fileInfoFileName;
  int brickSize;

  options.add_option("", "i", "file-info", "A JSON file (generated by the SEGYScan tool) containing information about the input SEG-Y file.", cxxopts::value<std::string>(fileInfoFileName), "<file>");
  options.add_option("", "b", "brick-size", "The brick size for the volume data store.", cxxopts::value<int>(brickSize)->default_value("64"), "<value>");

  options.add_option("", "", "input", "", cxxopts::value<std::vector<std::string>>(fileNames), "");
  options.parse_positional("input");

  if(argc == 1)
  {
    std::cout << options.help();
    return EXIT_SUCCESS;
  }

  try
  {
    options.parse(argc, argv);
  }
  catch(cxxopts::OptionParseException e)
  {
    std::cerr << e.what();
    return EXIT_FAILURE;
  }

  if(fileNames.empty())
  {
    std::cerr << std::string("No input SEG-Y file specified");
    return EXIT_FAILURE;
  }

  if(fileNames.size() > 1)
  {
    std::cerr << std::string("Only one input SEG-Y file may be specified");
    return EXIT_FAILURE;
  }

  SEGYFileInfo
    fileInfo;

  if(!fileInfoFileName.empty())
  {
    OpenVDS::File
      fileInfoFile;

    OpenVDS::IOError
      error;

    fileInfoFile.open(fileInfoFileName.c_str(), false, false, false, error);

    if(error.code != 0)
    {
      std::cerr << std::string("Could not open file: ") << fileInfoFileName;
      return EXIT_FAILURE;
    }

    bool success = parseSEGYFileInfoFile(fileInfoFile, fileInfo);

    if(!success)
    {
      return EXIT_FAILURE;
    }
  }
  else
  {
    std::cerr << std::string("No SEG-Y file info file specified");
    return EXIT_FAILURE;
  }

  OpenVDS::File
    file;

  OpenVDS::IOError
    error;

  file.open(fileNames[0].c_str(), false, false, false, error);

  if(error.code != 0)
  {
    std::cerr << std::string("Could not open file: ") << fileNames[0];
    return EXIT_FAILURE;
  }

  // Determine value range
  OpenVDS::FloatRange
    valueRange;

  // Create layout descriptor

  enum OpenVDS::VolumeDataLayoutDescriptor::BrickSize
    brickSizeEnum;

  switch(brickSize)
  {
  case  32: brickSizeEnum = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32;  break;
  case  64: brickSizeEnum = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_64;  break;
  case 128: brickSizeEnum = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_128; break;
  case 256: brickSizeEnum = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_256; break;
  default:
    std::cerr << std::string("Illegal brick size (must be 32, 64, 128 or 256)");
    return EXIT_FAILURE;
  }

  OpenVDS::VolumeDataLayoutDescriptor::Options layoutOptions = OpenVDS::VolumeDataLayoutDescriptor::Options_None;
  OpenVDS::VolumeDataLayoutDescriptor::LodLevels lodLevels = OpenVDS::VolumeDataLayoutDescriptor::LodLevelNone;

  int negativeMargin = 0;
  int positiveMargin = 0;
  int brickSizeMultiplier2D = 4;

  OpenVDS::VolumeDataLayoutDescriptor layoutDescriptor(brickSizeEnum, negativeMargin, positiveMargin, brickSizeMultiplier2D, lodLevels, layoutOptions);

  // Create axis descriptors

  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors = createAxisDescriptors(fileInfo);

  // Create channel descriptors

  std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors = createChannelDescriptors(fileInfo, valueRange);

  return EXIT_SUCCESS;
}