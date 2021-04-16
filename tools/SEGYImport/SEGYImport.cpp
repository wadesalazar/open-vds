/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**   http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
****************************************************************************/

#define _CRT_SECURE_NO_WARNINGS 1
#include <SEGYUtils/SEGYFileInfo.h>
#include "IO/File.h"
#include "VDS/Hash.h"
#include <SEGYUtils/DataProvider.h>
#include <SEGYUtils/TraceDataManager.h>

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/MetadataContainer.h>
#include <OpenVDS/VolumeDataLayoutDescriptor.h>
#include <OpenVDS/VolumeDataAxisDescriptor.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/Range.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/KnownMetadata.h>
#include <OpenVDS/GlobalMetadataCommon.h>

#include "IO/IOManager.h"

#include <mutex>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <algorithm>

#include "cxxopts.hpp"
#include <json/json.h>
#include <fmt/format.h>

#include "SplitUrl.h"
#include <PrintHelpers.h>

#include <chrono>
#include <numeric>

#if defined(WIN32)
#undef WIN32_LEAN_AND_MEAN // avoid warnings if defined on command line
#define WIN32_LEAN_AND_MEAN 1
#define NOMINMAX 1
#include <io.h>
#include <windows.h>

int64_t GetTotalSystemMemory()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return int64_t(status.ullTotalPhys);
}

#else
#include <unistd.h>

int64_t GetTotalSystemMemory()
{
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return int64_t(pages) * int64_t(page_size);
}
#endif

inline char asciitolower(char in) {
  if (in <= 'Z' && in >= 'A')
    return in - ('Z' - 'z');
  return in;
}

DataProvider CreateDataProviderFromFile(const std::string &filename, OpenVDS::Error &error)
{
  std::unique_ptr<OpenVDS::File> file(new OpenVDS::File());
  if (!file->Open(filename, false, false, false, error))
    return DataProvider((OpenVDS::File *)nullptr);
  return DataProvider(file.release());
}

DataProvider CreateDataProviderFromOpenOptions(const std::string &url, const std::string &connectionString, const std::string &objectId, OpenVDS::Error &error)
{
  std::unique_ptr<OpenVDS::IOManager> ioManager(OpenVDS::IOManager::CreateIOManager(url, connectionString, OpenVDS::IOManager::AccessPattern::ReadOnly, error));
  if (error.code)
    return DataProvider((OpenVDS::IOManager *)nullptr, "", error);
  return DataProvider(ioManager.release(), objectId, error);
}

DataProvider CreateDataProvider(const std::string& name, const std::string& connection, OpenVDS::Error& error)
{
  if (OpenVDS::IsSupportedProtocol(name))
  {
    std::string dirname;
    std::string basename;
    std::string parameters;
    splitUrl(name, dirname, basename, parameters, error);
    if (error.code)
      return DataProvider(nullptr);

    std::string url = dirname + parameters;
    return CreateDataProviderFromOpenOptions(url, connection, basename, error);
  }
  else
  {
    return CreateDataProviderFromFile(name, error);
  }
  return DataProvider(nullptr);
}

static std::vector<DataProvider> CreateDataProviders(const std::vector<std::string> &fileNames, const std::string &connection, OpenVDS::Error &error, std::string &errorFileName)
{
  std::vector<DataProvider>
    dataProviders;

  for (const auto& fileName : fileNames)
  {
    dataProviders.push_back(CreateDataProvider(fileName, connection, error));

    if (error.code != 0)
    {
      errorFileName = fileName;
      dataProviders.clear();
      break;
    }
  }
  return dataProviders;
}

Json::Value
SerializeSEGYBinInfo(SEGYBinInfo const& binInfo)
{
  Json::Value
    jsonBinInfo;

  jsonBinInfo["inlineNumber"] = binInfo.m_inlineNumber;
  jsonBinInfo["crosslineNumber"] = binInfo.m_crosslineNumber;

  jsonBinInfo["ensembleXCoordinate"] = binInfo.m_ensembleXCoordinate;
  jsonBinInfo["ensembleYCoordinate"] = binInfo.m_ensembleYCoordinate;

  return jsonBinInfo;
}

Json::Value
SerializeSEGYSegmentInfo(SEGYSegmentInfo const& segmentInfo)
{
  Json::Value
    jsonSegmentInfo;

  jsonSegmentInfo["primaryKey"] = segmentInfo.m_primaryKey;
  jsonSegmentInfo["traceStart"] = segmentInfo.m_traceStart;
  jsonSegmentInfo["traceStop"] = segmentInfo.m_traceStop;

  jsonSegmentInfo["binInfoStart"] = SerializeSEGYBinInfo(segmentInfo.m_binInfoStart);
  jsonSegmentInfo["binInfoStop"] = SerializeSEGYBinInfo(segmentInfo.m_binInfoStop);

  return jsonSegmentInfo;
}

std::string
ToString(SEGY::Endianness endiannness)
{
  switch (endiannness)
  {
  case SEGY::Endianness::BigEndian:  return "BigEndian";
  case SEGY::Endianness::LittleEndian: return "LittleEndian";
  default:
    assert(0); return "";
  }
}

std::string
ToString(SEGY::FieldWidth fieldWidth)
{
  switch (fieldWidth)
  {
  case SEGY::FieldWidth::TwoByte:  return "TwoByte";
  case SEGY::FieldWidth::FourByte: return "FourByte";
  default:
    assert(0); return "";
  }
}

Json::Value
SerializeSEGYHeaderField(SEGY::HeaderField const& headerField)
{
  Json::Value
    jsonHeaderField(Json::ValueType::arrayValue);

  jsonHeaderField.append(headerField.byteLocation);
  jsonHeaderField.append(ToString(headerField.fieldWidth));

  return jsonHeaderField;
}

Json::Value
SerializeSEGYFileInfo(SEGYFileInfo const& fileInfo, const int fileIndex)
{
  Json::Value
    jsonFileInfo;

  jsonFileInfo["persistentID"] = fmt::format("{:X}", fileInfo.m_persistentID);
  jsonFileInfo["headerEndianness"] = ToString(fileInfo.m_headerEndianness);
  jsonFileInfo["dataSampleFormatCode"] = (int)fileInfo.m_dataSampleFormatCode;
  jsonFileInfo["sampleCount"] = fileInfo.m_sampleCount;
  jsonFileInfo["startTime"] = fileInfo.m_startTimeMilliseconds;
  jsonFileInfo["sampleInterval"] = fileInfo.m_sampleIntervalMilliseconds;
  jsonFileInfo["traceCount"] = fileInfo.m_traceCounts[fileIndex];
  jsonFileInfo["primaryKey"] = SerializeSEGYHeaderField(fileInfo.m_primaryKey);
  jsonFileInfo["secondaryKey"] = SerializeSEGYHeaderField(fileInfo.m_secondaryKey);

  Json::Value
    jsonSegmentInfoArray(Json::ValueType::arrayValue);

  for (auto const& segmentInfo : fileInfo.m_segmentInfoLists[fileIndex])
  {
    jsonSegmentInfoArray.append(SerializeSEGYSegmentInfo(segmentInfo));
  }

  jsonFileInfo["segmentInfo"] = jsonSegmentInfoArray;

  return jsonFileInfo;
}

std::map<std::string, SEGY::HeaderField>
g_traceHeaderFields =
{
 { "tracesequencenumber",           SEGY::TraceHeader::TraceSequenceNumberHeaderField },
 { "tracesequencenumberwithinfile", SEGY::TraceHeader::TraceSequenceNumberWithinFileHeaderField },
 { "energysourcepointnumber",       SEGY::TraceHeader::EnergySourcePointNumberHeaderField },
 { "ensemblenumber",                SEGY::TraceHeader::EnsembleNumberHeaderField },
 { "tracenumberwithinensemble",     SEGY::TraceHeader::TraceNumberWithinEnsembleHeaderField },
 { "traceidentificationcode",       SEGY::TraceHeader::TraceIdentificationCodeHeaderField },
 { "coordinatescale",               SEGY::TraceHeader::CoordinateScaleHeaderField },
 { "sourcexcoordinate",             SEGY::TraceHeader::SourceXCoordinateHeaderField },
 { "sourceycoordinate",             SEGY::TraceHeader::SourceYCoordinateHeaderField },
 { "groupxcoordinate",              SEGY::TraceHeader::GroupXCoordinateHeaderField },
 { "groupycoordinate",              SEGY::TraceHeader::GroupYCoordinateHeaderField },
 { "coordinateunits",               SEGY::TraceHeader::CoordinateUnitsHeaderField },
 { "starttime",                     SEGY::TraceHeader::StartTimeHeaderField },
 { "numsamples",                    SEGY::TraceHeader::NumSamplesHeaderField },
 { "sampleinterval",                SEGY::TraceHeader::SampleIntervalHeaderField },
 { "ensemblexcoordinate",           SEGY::TraceHeader::EnsembleXCoordinateHeaderField },
 { "ensembleycoordinate",           SEGY::TraceHeader::EnsembleYCoordinateHeaderField },
 { "inlinenumber",                  SEGY::TraceHeader::InlineNumberHeaderField },
 { "crosslinenumber",               SEGY::TraceHeader::CrosslineNumberHeaderField },
 { "receiver",                      SEGY::TraceHeader::ReceiverHeaderField },
 { "offset",                        SEGY::TraceHeader::OffsetHeaderField }
};

std::map<std::string, std::string>
g_aliases =
{
 { "inline",              "inlinenumber" },
 { "crossline",           "crosslinenumber" },
 { "shot",                "energysourcepointnumber" },
 { "sp",                  "energysourcepointnumber" },
 { "cdp",                 "ensemblenumber" },
 { "cmp",                 "ensemblenumber" },
 { "easting",             "ensemblexcoordinate" },
 { "northing",            "ensembleycoordinate" },
 { "cdpxcoordinate",      "ensemblexcoordinate" },
 { "cdpycoordinate",      "ensembleycoordinate" },
 { "cdp-x",               "ensemblexcoordinate" },
 { "cdp-y",               "ensembleycoordinate" },
 { "source-x",            "sourcexcoordinate" },
 { "source-y",            "sourceycoordinate" },
 { "group-x",             "groupxcoordinate" },
 { "group-y",             "groupycoordinate" },
 { "receiverxcoordinate", "groupxcoordinate" },
 { "receiverycoordinate", "groupycoordinate" },
 { "receiver-x",          "groupxcoordinate" },
 { "receiver-y",          "groupycoordinate" },
 { "scalar",              "coordinatescale" }
};

void
ResolveAlias(std::string& fieldName)
{

  std::transform(fieldName.begin(), fieldName.end(), fieldName.begin(), asciitolower);
  if (g_aliases.find(fieldName) != g_aliases.end())
  {
    fieldName = g_aliases[fieldName];
  }
}

SEGY::Endianness
EndiannessFromJson(Json::Value const& jsonEndianness)
{
  std::string
    endiannessString = jsonEndianness.asString();

  std::transform(endiannessString.begin(), endiannessString.end(), endiannessString.begin(), asciitolower);
  if (endiannessString == "bigendian")
  {
    return SEGY::Endianness::BigEndian;
  }
  else if (endiannessString == "littleendian")
  {
    return SEGY::Endianness::LittleEndian;
  }

  throw Json::Exception("Illegal endianness");
}

SEGY::FieldWidth
FieldWidthFromJson(Json::Value const& jsonFieldWidth)
{
  std::string
    fieldWidthString = jsonFieldWidth.asString();
  std::transform(fieldWidthString.begin(), fieldWidthString.end(), fieldWidthString.begin(), asciitolower);

  if (fieldWidthString == "twobyte")
  {
    return SEGY::FieldWidth::TwoByte;
  }
  else if (fieldWidthString == "fourbyte")
  {
    return SEGY::FieldWidth::FourByte;
  }

  throw Json::Exception("Illegal field width");
}

SEGY::HeaderField
HeaderFieldFromJson(Json::Value const& jsonHeaderField)
{
  int
    bytePosition = jsonHeaderField[0].asInt();

  SEGY::FieldWidth
    fieldWidth = FieldWidthFromJson(jsonHeaderField[1]);

  if (bytePosition < 1 || bytePosition > SEGY::TraceHeaderSize - ((fieldWidth == SEGY::FieldWidth::TwoByte) ? 2 : 4))
  {
    throw Json::Exception(std::string("Illegal field definition: ") + jsonHeaderField.toStyledString());
  }

  return SEGY::HeaderField(bytePosition, fieldWidth);
}

bool
ParseHeaderFormatFile(DataProvider &dataProvider, std::map<std::string, SEGY::HeaderField>& traceHeaderFields, SEGY::Endianness& headerEndianness, OpenVDS::Error &error)
{
  int64_t dataSize = dataProvider.Size(error);

  if (error.code != 0)
  {
    return false;
  }

  if (dataSize > INT_MAX)
  {
    return false;
  }

  std::unique_ptr<char[]>
    buffer(new char[dataSize]);

  dataProvider.Read(buffer.get(), 0, (int32_t)dataSize, error);

  if (error.code != 0)
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
      root;

    bool
      success = reader->parse(buffer.get(), buffer.get() + dataSize, &root, &errs);

    if (!success)
    {
      throw Json::Exception(errs);
    }

    for (std::string const& fieldName : root.getMemberNames())
    {
      std::string canonicalFieldName = fieldName;
      ResolveAlias(canonicalFieldName);

      if (canonicalFieldName == "endianness")
      {
        headerEndianness = EndiannessFromJson(root[fieldName]);
      }
      else
      {
        traceHeaderFields[canonicalFieldName] = HeaderFieldFromJson(root[fieldName]);
      }
    }
  }
  catch (Json::Exception &e)
  {
    error.code = -1;
    error.string = e.what();
    return false;
  }

  return true;
}

bool OnlyDigits(const std::string& str)
{
  for (auto a : str)
  {
    if (a < '0' || a > '9')
      return false;
  }
  return true;
}

bool
ParseHeaderFieldArgs(const std::vector<std::string> &header_fields_args, std::map<std::string, SEGY::HeaderField>& traceHeaderFields, SEGY::Endianness& headerEndianness, OpenVDS::Error& error)
{
  for (auto& header_field : header_fields_args)
  {
    if (header_field.empty())
    {
      error.code = -1;
      error.string = "Cannot parse empty header-field";
      return false;
    }
    auto it = std::find(header_field.begin(), header_field.end(), '=');
    if (it == header_field.end())
    {
      error.code = -1;
      error.string = fmt::format("Failed to parse header-field {}.", header_field);
      return false;
    }
    std::string header_name(header_field.begin(), it);
    if (it + 1 == header_field.end())
    {
      error.code = -1;
      error.string = fmt::format("Can not find value for header-field {}.", header_name);
      return false;
    }
    std::string header_value(it + 1, header_field.end());
    auto min_delimiter = std::find(header_value.begin(), header_value.end(), '-');
    int field_width = -1;
    int offset = -1;
    if (min_delimiter != header_value.end())
    {
      if (min_delimiter + 1 == header_value.end())
      {
        error.code = -1;
        error.string = fmt::format("unable to parse value for header-field {} with value {}.", header_name, header_value);
        return false;
      }
      std::string value_start(header_value.begin(), min_delimiter);
      std::string value_end(min_delimiter + 1, header_value.end());
      if (!OnlyDigits(value_start) || !OnlyDigits(value_end))
      {
        error.code = -1;
        error.string = fmt::format("unable to parse header-field {} value range {}.", header_name, header_value);
        return false;
      }
      int value_start_value = atoi(value_start.c_str());
      int value_end_value = atoi(value_end.c_str());
      offset = value_start_value;
      field_width = value_end_value - value_start_value;
    }
    else
    {
      auto colon_delimiter = std::find(header_value.begin(), header_value.end(), ':');
      std::string offset_str(header_value.begin(), colon_delimiter);
      if (!OnlyDigits(offset_str))
      {
        error.code = -1;
        error.string = fmt::format("unable to parse offset for header-field {}: {}.", header_name, header_value);
        return false;
      }
      offset = atoi(offset_str.c_str());
      if (colon_delimiter < header_value.end() && colon_delimiter + 1 < header_value.end())
      {
        std::string width_str(colon_delimiter + 1, header_value.end());
        if (!OnlyDigits(width_str))
        {
          error.code = -1;
          error.string = fmt::format("unable to parse width specifier for header-field {}: {}.", header_name, width_str);
          return false;
        }
        field_width = atoi(width_str.c_str());
      }
    }
    if (offset < 0)
    {
      error.code = -1;
      error.string = fmt::format("unable to find offset for header-field {}: {}.", header_name, header_value);
      return false;
    }
    ResolveAlias(header_name);
    auto& traceHeaderField = traceHeaderFields[header_name];
    traceHeaderField.byteLocation = offset;
    if (field_width != -1)
    {
      if (field_width == 2)
      {
        traceHeaderField.fieldWidth = SEGY::FieldWidth::TwoByte;
      }
      else if (field_width == 4)
      {
        traceHeaderField.fieldWidth = SEGY::FieldWidth::FourByte;
      }
      else
      {
        error.code = -1;
        error.string = fmt::format("header-field {} has illegal field width of {}. Only widths of 2 or 4 are accepted.", header_name, field_width);
        return false;
      }
    }
  }
  return true;
}

SEGYBinInfo
binInfoFromJson(Json::Value const& jsonBinInfo)
{
  int inlineNumber = jsonBinInfo["inlineNumber"].asInt();
  int crosslineNumber = jsonBinInfo["crosslineNumber"].asInt();
  double ensembleXCoordinate = jsonBinInfo["ensembleXCoordinate"].asDouble();
  double ensembleYCoordinate = jsonBinInfo["ensembleYCoordinate"].asDouble();

  return SEGYBinInfo(inlineNumber, crosslineNumber, ensembleXCoordinate, ensembleYCoordinate);
}

SEGYSegmentInfo
segmentInfoFromJson(Json::Value const& jsonSegmentInfo)
{
  int primaryKey = jsonSegmentInfo["primaryKey"].asInt();
  int traceStart = jsonSegmentInfo["traceStart"].asInt();
  int traceStop = jsonSegmentInfo["traceStop"].asInt();
  SEGYBinInfo binInfoStart = binInfoFromJson(jsonSegmentInfo["binInfoStart"]);
  SEGYBinInfo binInfoStop = binInfoFromJson(jsonSegmentInfo["binInfoStop"]);

  return SEGYSegmentInfo(primaryKey, traceStart, traceStop, binInfoStart, binInfoStop);
}

std::vector<int>
getOrderedSegmentListIndices(SEGYFileInfo const& fileInfo, size_t& globalTotalSegments)
{
  // Generate a list of indices that will traverse m_segyFileInfo.m_segmentInfoLists in primary key order, which
  // may be different from the order that the files were given.

  std::vector<int>
    orderedListIndices;
  size_t
    longestList = 0;
  globalTotalSegments = 0;
  for (size_t i = 0; i < fileInfo.m_segmentInfoLists.size(); ++i)
  {
    orderedListIndices.push_back(static_cast<int>(i));
    globalTotalSegments += fileInfo.m_segmentInfoLists[i].size();
    if (fileInfo.m_segmentInfoLists[i].size() > fileInfo.m_segmentInfoLists[longestList].size())
    {
      longestList = i;
    }
  }
  const bool
    isAscending = fileInfo.m_segmentInfoLists[longestList].front().m_binInfoStart.m_inlineNumber <= fileInfo.m_segmentInfoLists[longestList].back().m_binInfoStart.m_inlineNumber;
  auto
    comparator = [&](int i1, int i2)
  {
    const auto
      & v1 = fileInfo.m_segmentInfoLists[i1],
      & v2 = fileInfo.m_segmentInfoLists[i2];
    return isAscending ? v1.front().m_binInfoStart.m_inlineNumber < v2.front().m_binInfoStart.m_inlineNumber : v2.front().m_binInfoStart.m_inlineNumber < v1.front().m_binInfoStart.m_inlineNumber;
  };
  std::sort(orderedListIndices.begin(), orderedListIndices.end(), comparator);

  return orderedListIndices;
}

SEGYSegmentInfo const&
findRepresentativeSegment(SEGYFileInfo const& fileInfo, int& primaryStep, int& bestListIndex)
{
  // Since we give more weight to segments near the center of the data we need a sorted index of segment lists so that we can
  // traverse the lists in data order, instead of the arbitrary order given by the filename ordering.
  size_t
    globalTotalSegments = 0;
  auto
    orderedListIndices = getOrderedSegmentListIndices(fileInfo, globalTotalSegments);

  primaryStep = 0;
  bestListIndex = 0;

  float bestScore = 0.0f;
  size_t bestIndex = 0;

  int segmentPrimaryStep = 0;

  size_t
    globalOffset = 0;

  for (const auto listIndex : orderedListIndices)
  {
    const auto&
      segmentInfoList = fileInfo.m_segmentInfoLists[listIndex];

    for (size_t i = 0; i < segmentInfoList.size(); i++)
    {
      int64_t
        numTraces = (segmentInfoList[i].m_traceStop - segmentInfoList[i].m_traceStart + 1);

      // index of this segment within the entirety of segments from all input files
      const auto
        globalIndex = globalOffset + i;

      float
        multiplier = 1.5f - abs(globalIndex - (float)globalTotalSegments / 2) / (float)globalTotalSegments; // give 50% more importance to a segment in the middle of the dataset

      float
        score = float(numTraces) * multiplier;

      if (score > bestScore)
      {
        bestScore = score;
        bestListIndex = listIndex;
        bestIndex = i;
      }

      // Updating the primary step with the step for the previous segment intentionally ignores the step of the last segment since it can be anomalous
      if (segmentPrimaryStep && (!primaryStep || std::abs(segmentPrimaryStep) < std::abs(primaryStep)))
      {
        primaryStep = segmentPrimaryStep;
      }

      if (i > 0)
      {
        segmentPrimaryStep = segmentInfoList[i].m_primaryKey - segmentInfoList[i - 1].m_primaryKey;
      }
    }

    globalOffset += segmentInfoList.size();
  }

  // If the primary step couldn't be determined, set it to the last step or 1
  primaryStep = primaryStep ? primaryStep : std::max(segmentPrimaryStep, 1);

  return fileInfo.m_segmentInfoLists[bestListIndex][bestIndex];
}

void
copySamples(const void* data, SEGY::BinaryHeader::DataSampleFormatCode dataSampleFormatCode, SEGY::Endianness endianness, float* target, int sampleStart, int sampleCount)
{
  if (dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IBMFloat)
  {
    if(endianness == SEGY::Endianness::LittleEndian)
    {
      // Reverse endianness since Ibm2ieee expects big endian data
      const char * source = reinterpret_cast<const char*>((intptr_t)data + (size_t)sampleStart * 4);
      std::unique_ptr<char[]> temp(new char[sampleCount * 4]);
      for(int sample = 0; sample < sampleCount; sample++)
      {
        temp[sample * 4 + 0] = source[sample * 4 + 3];
        temp[sample * 4 + 1] = source[sample * 4 + 2];
        temp[sample * 4 + 2] = source[sample * 4 + 1];
        temp[sample * 4 + 3] = source[sample * 4 + 0];
      }
      SEGY::Ibm2ieee(target, temp.get(), sampleCount);
    }
    else
    {
      assert(endianness == SEGY::Endianness::BigEndian);
      SEGY::Ibm2ieee(target, reinterpret_cast<const uint32_t*>((intptr_t)data + (size_t)sampleStart * 4), sampleCount);
    }
  }
  else
  {
    assert(dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IEEEFloat);
    if(endianness == SEGY::Endianness::LittleEndian)
    {
      SEGY::ConvertFromEndianness<SEGY::Endianness::LittleEndian>(target, reinterpret_cast<const char*>((intptr_t)data + (size_t)sampleStart * 4), sampleCount);
    }
    else
    {
      assert(endianness == SEGY::Endianness::BigEndian);
      SEGY::ConvertFromEndianness<SEGY::Endianness::BigEndian>(target, reinterpret_cast<const char*>((intptr_t)data + (size_t)sampleStart * 4), sampleCount);
    }
  }
}

bool
analyzeSegment(DataProvider &dataProvider, SEGYFileInfo const& fileInfo, SEGYSegmentInfo const& segmentInfo, float valueRangePercentile, OpenVDS::FloatRange& valueRange, int& fold, int& secondaryStep, const SEGY::SEGYType segyType, int& offsetStart, int& offsetEnd, int& offsetStep, bool jsonOutput, OpenVDS::Error& error)
{
  assert(segmentInfo.m_traceStop >= segmentInfo.m_traceStart && "A valid segment info should always have a stop trace greater or equal to the start trace");

  bool success = true;

  valueRange = OpenVDS::FloatRange(0.0f, 1.0f);
  secondaryStep = 0;
  fold = 1;
  offsetStart = 0;
  offsetEnd = 0;
  offsetStep = 0;

  const int traceByteSize = fileInfo.TraceByteSize();

  int64_t traceBufferStart = 0;
  int traceBufferSize = 0;
  std::unique_ptr<char[]> buffer;

  // Create min/max heaps for determining value range
  int heapSizeMax = int(((100.0f - valueRangePercentile) / 100.0f) * (segmentInfo.m_traceStop - segmentInfo.m_traceStart + 1) * fileInfo.m_sampleCount / 2) + 1;

  std::vector<float> minHeap, maxHeap;

  minHeap.reserve(heapSizeMax);
  maxHeap.reserve(heapSizeMax);

  // Allocate sample buffer for converting samples to float
  std::unique_ptr<float[]> sampleBuffer(new float[fileInfo.m_sampleCount]);
  float* samples = sampleBuffer.get();

  // Determine fold and secondary step
  int gatherSecondaryKey = 0, gatherFold = 0, gatherSecondaryStep = 0;

  bool
    hasPreviousGatherOffset = false;
  int
    previousGatherOffset = 0;

  for (int64_t trace = segmentInfo.m_traceStart; trace <= segmentInfo.m_traceStop; trace++)
  {
    if(trace - traceBufferStart >= traceBufferSize)
    {
      traceBufferStart = trace;
      traceBufferSize = (segmentInfo.m_traceStop - trace + 1) < 1000 ? int(segmentInfo.m_traceStop - trace + 1) : 1000;

      buffer.reset(new char[traceByteSize * traceBufferSize]);
      int64_t offset = SEGY::TextualFileHeaderSize + SEGY::BinaryFileHeaderSize + traceByteSize * traceBufferStart;
      success = dataProvider.Read(buffer.get(), offset, traceByteSize * traceBufferSize, error);

      if (!success)
      {
        break;
      }
    }

    const void *header = buffer.get() + traceByteSize * (trace - traceBufferStart);
    const void *data   = buffer.get() + traceByteSize * (trace - traceBufferStart) + SEGY::TraceHeaderSize;

    int tracePrimaryKey = SEGY::ReadFieldFromHeader(header, fileInfo.m_primaryKey, fileInfo.m_headerEndianness);
    int traceSecondaryKey = fileInfo.IsUnbinned() ? static_cast<int>(trace - segmentInfo.m_traceStart + 1) : SEGY::ReadFieldFromHeader(header, fileInfo.m_secondaryKey, fileInfo.m_headerEndianness);

    if(tracePrimaryKey != segmentInfo.m_primaryKey)
    {
      OpenVDS::printWarning(jsonOutput, "SEGY", fmt::format("trace {} has a primary key that doesn't match with the segment. This trace will be ignored.", segmentInfo.m_traceStart + trace));
      continue;
    }

    if(gatherFold > 0 && traceSecondaryKey == gatherSecondaryKey)
    {
      gatherFold++;
      fold = std::max(fold, gatherFold);
    }
    else
    {
      // Updating the secondary step with the step for the previous gather intentionally ignores the step of the last gather since it can be anomalous
      if(gatherSecondaryStep && (!secondaryStep || std::abs(gatherSecondaryStep) < std::abs(secondaryStep)))
      {
        secondaryStep = gatherSecondaryStep;
      }

      if(gatherFold > 0)
      {
        gatherSecondaryStep = traceSecondaryKey - gatherSecondaryKey;
      }
      gatherSecondaryKey = traceSecondaryKey;
      gatherFold = 1;
    }

    if (fileInfo.HasGatherOffset())
    {
      auto
        thisOffset = SEGY::ReadFieldFromHeader(header, g_traceHeaderFields["Offset"], fileInfo.m_headerEndianness);
      if (hasPreviousGatherOffset)
      {
        offsetStart = std::min(offsetStart, thisOffset);
        offsetEnd = std::max(offsetEnd, thisOffset);
        if (thisOffset != previousGatherOffset)
        {
          offsetStep = std::min(offsetStep, std::abs(thisOffset - previousGatherOffset));
        }
      }
      else
      {
        offsetStart = thisOffset;
        offsetEnd = thisOffset;
        offsetStep = INT32_MAX;
        hasPreviousGatherOffset = true;
      }
      previousGatherOffset = thisOffset;
    }

    // Update value range
    if (fileInfo.m_dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IBMFloat || fileInfo.m_dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IEEEFloat)
    {
      copySamples(data, fileInfo.m_dataSampleFormatCode, fileInfo.m_headerEndianness, samples, 0, fileInfo.m_sampleCount);

      for (int sample = 0; sample < fileInfo.m_sampleCount; sample++)
      {
        if (int(minHeap.size()) < heapSizeMax)
        {
          minHeap.push_back(samples[sample]);
          std::push_heap(minHeap.begin(), minHeap.end(), std::less<float>());
        }
        else if (samples[sample] < minHeap[0])
        {
          std::pop_heap(minHeap.begin(), minHeap.end(), std::less<float>());
          minHeap.back() = samples[sample];
          std::push_heap(minHeap.begin(), minHeap.end(), std::less<float>());
        }

        if (int(maxHeap.size()) < heapSizeMax)
        {
          maxHeap.push_back(samples[sample]);
          std::push_heap(maxHeap.begin(), maxHeap.end(), std::greater<float>());
        }
        else if (samples[sample] > maxHeap[0])
        {
          std::pop_heap(maxHeap.begin(), maxHeap.end(), std::greater<float>());
          maxHeap.back() = samples[sample];
          std::push_heap(maxHeap.begin(), maxHeap.end(), std::greater<float>());
        }
      }
    }
  }

  if (fileInfo.HasGatherOffset() && !fileInfo.IsUnbinned())
  {
    // check that offset start/end/step is consistent
    if (offsetStart + (fold - 1) * offsetStep != offsetEnd)
    {
      const auto
        msgFormat = "The detected gather offset start/end/step of '{0}/{1}/{2}' is not consistent with the detected fold of '{3}'. This usually indicates using the wrong header format for the input dataset.\n.";
      error.string = fmt::format(msgFormat, offsetStart, offsetEnd, offsetStep, fold);
      error.code = -1;
      return false;

    }
  }

  // If the secondary step couldn't be determined, set it to the last step or 1
  secondaryStep = secondaryStep ? secondaryStep : std::max(gatherSecondaryStep, 1);

  // Set value range
  if (!minHeap.empty())
  {
    assert(!maxHeap.empty());

    if (minHeap[0] != maxHeap[0])
    {
      valueRange = OpenVDS::FloatRange(minHeap[0], maxHeap[0]);
    }
    else
    {
      valueRange = OpenVDS::FloatRange(minHeap[0], minHeap[0] + 1.0f);
    }
  }

  return success;
}

bool
createSEGYMetadata(DataProvider &dataProvider, SEGYFileInfo const &fileInfo, OpenVDS::MetadataContainer& metadataContainer, SEGY::BinaryHeader::MeasurementSystem &measurementSystem, OpenVDS::Error& error)
{
  std::vector<uint8_t> textHeader(SEGY::TextualFileHeaderSize);
  std::vector<uint8_t> binaryHeader(SEGY::BinaryFileHeaderSize);

  // Read headers
  bool success = dataProvider.Read(textHeader.data(), 0, SEGY::TextualFileHeaderSize, error) &&
    dataProvider.Read(binaryHeader.data(), SEGY::TextualFileHeaderSize, SEGY::BinaryFileHeaderSize, error);

  if (!success) return false;

  // Create metadata
  metadataContainer.SetMetadataBLOB("SEGY", "TextHeader", textHeader);

  metadataContainer.SetMetadataBLOB("SEGY", "BinaryHeader", binaryHeader);

  metadataContainer.SetMetadataInt("SEGY", "Endianness", int(fileInfo.m_headerEndianness));
  metadataContainer.SetMetadataInt("SEGY", "DataSampleFormatCode", int(fileInfo.m_dataSampleFormatCode));

  measurementSystem = SEGY::BinaryHeader::MeasurementSystem(SEGY::ReadFieldFromHeader(binaryHeader.data(), SEGY::BinaryHeader::MeasurementSystemHeaderField, fileInfo.m_headerEndianness));
  return success;
}

void
createSurveyCoordinateSystemMetadata(SEGYFileInfo const& fileInfo, SEGY::BinaryHeader::MeasurementSystem measurementSystem, std::string const &crsWkt, OpenVDS::MetadataContainer& metadataContainer)
{
  if (fileInfo.m_segmentInfoLists.empty()) return;

  double inlineSpacing[2] = { 0, 0 };
  double crosslineSpacing[2] = { 0, 0 };

  size_t
    globalTotalSegments;
  auto
    orderedListIndices = getOrderedSegmentListIndices(fileInfo, globalTotalSegments);

  // Determine crossline spacing
  int countedCrosslineSpacings = 0;

  for (size_t listIndex : orderedListIndices)
  {
    const auto
      & segmentInfoList = fileInfo.m_segmentInfoLists[listIndex];

    for (auto const& segmentInfo : segmentInfoList)
    {
      int crosslineCount = segmentInfo.m_binInfoStop.m_crosslineNumber - segmentInfo.m_binInfoStart.m_crosslineNumber;

      if (crosslineCount == 0 || segmentInfo.m_binInfoStart.m_inlineNumber != segmentInfo.m_binInfoStop.m_inlineNumber) continue;

      double segmentCrosslineSpacing[3];

      segmentCrosslineSpacing[0] = (segmentInfo.m_binInfoStop.m_ensembleXCoordinate - segmentInfo.m_binInfoStart.m_ensembleXCoordinate) / crosslineCount;
      segmentCrosslineSpacing[1] = (segmentInfo.m_binInfoStop.m_ensembleYCoordinate - segmentInfo.m_binInfoStart.m_ensembleYCoordinate) / crosslineCount;

      crosslineSpacing[0] += segmentCrosslineSpacing[0];
      crosslineSpacing[1] += segmentCrosslineSpacing[1];

      countedCrosslineSpacings++;
    }
  }

  if (countedCrosslineSpacings > 0)
  {
    crosslineSpacing[0] /= countedCrosslineSpacings;
    crosslineSpacing[1] /= countedCrosslineSpacings;
  }
  else
  {
    crosslineSpacing[0] = 0;
    crosslineSpacing[1] = 1;
  }

  // Determine inline spacing
  SEGYSegmentInfo const& firstSegmentInfo = fileInfo.m_segmentInfoLists[orderedListIndices.front()].front();
  SEGYSegmentInfo const& lastSegmentInfo = fileInfo.m_segmentInfoLists[orderedListIndices.back()].back();

  if (firstSegmentInfo.m_binInfoStart.m_inlineNumber != lastSegmentInfo.m_binInfoStart.m_inlineNumber)
  {
    int inlineNunberDelta = lastSegmentInfo.m_binInfoStart.m_inlineNumber - firstSegmentInfo.m_binInfoStart.m_inlineNumber;
    int crosslineNunberDelta = lastSegmentInfo.m_binInfoStart.m_crosslineNumber - firstSegmentInfo.m_binInfoStart.m_crosslineNumber;

    double offset[2] = { crosslineSpacing[0] * crosslineNunberDelta,
               crosslineSpacing[1] * crosslineNunberDelta };

    inlineSpacing[0] = (lastSegmentInfo.m_binInfoStart.m_ensembleXCoordinate - firstSegmentInfo.m_binInfoStart.m_ensembleXCoordinate - offset[0]) / inlineNunberDelta;
    inlineSpacing[1] = (lastSegmentInfo.m_binInfoStart.m_ensembleYCoordinate - firstSegmentInfo.m_binInfoStart.m_ensembleYCoordinate - offset[1]) / inlineNunberDelta;
  }
  else
  {
    // make square voxels
    inlineSpacing[0] = crosslineSpacing[1];
    inlineSpacing[1] = -crosslineSpacing[0];
  }

  // Determine origin
  double origin[2];

  origin[0] = firstSegmentInfo.m_binInfoStart.m_ensembleXCoordinate;
  origin[1] = firstSegmentInfo.m_binInfoStart.m_ensembleYCoordinate;

  origin[0] -= inlineSpacing[0] * firstSegmentInfo.m_binInfoStart.m_inlineNumber;
  origin[1] -= inlineSpacing[1] * firstSegmentInfo.m_binInfoStart.m_inlineNumber;

  origin[0] -= crosslineSpacing[0] * firstSegmentInfo.m_binInfoStart.m_crosslineNumber;
  origin[1] -= crosslineSpacing[1] * firstSegmentInfo.m_binInfoStart.m_crosslineNumber;

  // Set coordinate system
  metadataContainer.SetMetadataDoubleVector2(LATTICE_CATEGORY, LATTICE_ORIGIN, OpenVDS::DoubleVector2(origin[0], origin[1]));
  metadataContainer.SetMetadataDoubleVector2(LATTICE_CATEGORY, LATTICE_INLINE_SPACING, OpenVDS::DoubleVector2(inlineSpacing[0], inlineSpacing[1]));
  metadataContainer.SetMetadataDoubleVector2(LATTICE_CATEGORY, LATTICE_CROSSLINE_SPACING, OpenVDS::DoubleVector2(crosslineSpacing[0], crosslineSpacing[1]));

  if(!crsWkt.empty())
  {
    metadataContainer.SetMetadataString(LATTICE_CATEGORY, CRS_WKT, crsWkt);
  }
  if(measurementSystem == SEGY::BinaryHeader::MeasurementSystem::Meters)
  {
    metadataContainer.SetMetadataString(LATTICE_CATEGORY, LATTICE_UNIT, KNOWNMETADATA_UNIT_METER);
  }
  else if(measurementSystem == SEGY::BinaryHeader::MeasurementSystem::Feet)
  {
    metadataContainer.SetMetadataString(LATTICE_CATEGORY, LATTICE_UNIT, KNOWNMETADATA_UNIT_FOOT);
  }
}

/////////////////////////////////////////////////////////////////////////////
bool
createImportInformationMetadata(const std::vector<DataProvider> &dataProviders, OpenVDS::MetadataContainer& metadataContainer, OpenVDS::Error &error)
{
  auto now = std::chrono::system_clock::now();
  std::time_t tt = std::chrono::system_clock::to_time_t(now);
  std::tm tm = *std::gmtime(&tt);
  auto duration = now.time_since_epoch();
  int millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

  std::string importTimeStamp = fmt::format("{:04d}-{:02d}-{:02d}T{:02d}:{:02d}:{:02d}.{:03d}Z", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, millis);

  // Join all the input names into a single comma delimited string
  const char pathSeparators[] = { '/', '\\', ':' };
  std::stringstream
    allNames;
  size_t
    nameCount = 0;
  for (auto iter = dataProviders.begin(); iter != dataProviders.end(); ++iter, ++nameCount)
  {
    // Strip the path from the file/object name
    std::string
      currentName = iter->FileOrObjectName();
    for (auto pathSeparator : pathSeparators)
    {
      size_t pos = currentName.rfind(pathSeparator);
      if (pos != std::string::npos) currentName = currentName.substr(pos + 1);
    }

    allNames << currentName;
    if (nameCount < dataProviders.size() - 1)
    {
      allNames << ",";
    }
  }

  std::string inputFileName = allNames.str();

  // In lack of a better displayName we use the file name
  std::string displayName = inputFileName;

  // Use the timestamp from the first input
  std::string inputTimeStamp = dataProviders[0].LastWriteTime(error);
  if (error.code != 0)
  {
    return false;
  }

  // Sum the sizes of all the inputs
  int64_t
    inputFileSize = 0;
  for (const auto& provider : dataProviders)
  {
    inputFileSize += provider.Size(error);
    if (error.code != 0)
    {
      return false;
    }
  }

  // Set import information
  metadataContainer.SetMetadataString(KNOWNMETADATA_CATEGORY_IMPORTINFORMATION, KNOWNMETADATA_IMPORTINFORMATION_DISPLAYNAME, displayName);
  metadataContainer.SetMetadataString(KNOWNMETADATA_CATEGORY_IMPORTINFORMATION, KNOWNMETADATA_IMPORTINFORMATION_INPUTFILENAME, inputFileName);
  metadataContainer.SetMetadataDouble(KNOWNMETADATA_CATEGORY_IMPORTINFORMATION, KNOWNMETADATA_IMPORTINFORMATION_INPUTFILESIZE, (double)inputFileSize);
  metadataContainer.SetMetadataString(KNOWNMETADATA_CATEGORY_IMPORTINFORMATION, KNOWNMETADATA_IMPORTINFORMATION_INPUTTIMESTAMP, inputTimeStamp);
  metadataContainer.SetMetadataString(KNOWNMETADATA_CATEGORY_IMPORTINFORMATION, KNOWNMETADATA_IMPORTINFORMATION_IMPORTTIMESTAMP, importTimeStamp);
  return true;
}

bool
parseSEGYFileInfoFile(DataProvider &dataProvider, SEGYFileInfo& fileInfo, OpenVDS::Error &error)
{
  int64_t fileSize = dataProvider.Size(error);

  if (error.code != 0)
  {
    return false;
  }

  if (fileSize > INT_MAX)
  {
    return false;
  }

  std::unique_ptr<char[]>
    buffer(new char[fileSize]);

  dataProvider.Read(buffer.get(), 0, (int32_t)fileSize, error);

  if (error.code != 0)
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

    if (!success)
    {
      error.string = errs;
      error.code = -1;
      return false;
    }

    fileInfo.m_persistentID = strtoull(jsonFileInfo["persistentID"].asCString(), nullptr, 16);
    fileInfo.m_headerEndianness = EndiannessFromJson(jsonFileInfo["headerEndianness"]);
    fileInfo.m_dataSampleFormatCode = SEGY::BinaryHeader::DataSampleFormatCode(jsonFileInfo["dataSampleFormatCode"].asInt());
    fileInfo.m_sampleCount = jsonFileInfo["sampleCount"].asInt();
    fileInfo.m_startTimeMilliseconds = jsonFileInfo["startTime"].asDouble();
    fileInfo.m_sampleIntervalMilliseconds = jsonFileInfo["sampleInterval"].asDouble();
    fileInfo.m_traceCounts.clear();
    fileInfo.m_traceCounts.push_back(jsonFileInfo["traceCount"].asInt64());
    fileInfo.m_primaryKey = HeaderFieldFromJson(jsonFileInfo["primaryKey"]);
    fileInfo.m_secondaryKey = HeaderFieldFromJson(jsonFileInfo["secondaryKey"]);

    fileInfo.m_segmentInfoLists.clear();
    fileInfo.m_segmentInfoLists.emplace_back();
    auto&
      segmentInfo = fileInfo.m_segmentInfoLists.back();
    for (Json::Value jsonSegmentInfo : jsonFileInfo["segmentInfo"])
    {
      segmentInfo.push_back(segmentInfoFromJson(jsonSegmentInfo));
    }
  }
  catch (Json::Exception &e)
  {
    error.string = fmt::format("Failed to parse JSON SEG-Y file info file: {}", e.what());
    error.code = -1;
    return false;
  }

  return true;
}

std::vector<OpenVDS::VolumeDataAxisDescriptor>
createAxisDescriptors(SEGYFileInfo const& fileInfo, SEGY::SampleUnits sampleUnits, int fold, int inlineStep, int crosslineStep)
{
  std::vector<OpenVDS::VolumeDataAxisDescriptor>
    axisDescriptors;

  const char *sampleUnit = "";

  switch(sampleUnits)
  {
  case SEGY::SampleUnits::Milliseconds: sampleUnit = KNOWNMETADATA_UNIT_MILLISECOND; break;
  case SEGY::SampleUnits::Feet:         sampleUnit = KNOWNMETADATA_UNIT_FOOT;        break;
  case SEGY::SampleUnits::Meters:       sampleUnit = KNOWNMETADATA_UNIT_METER;       break;
  default:
    assert(0 && "Unknown sample unit");
  }

  axisDescriptors.emplace_back(fileInfo.m_sampleCount, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE, sampleUnit, (float)fileInfo.m_startTimeMilliseconds, (float)fileInfo.m_startTimeMilliseconds + (fileInfo.m_sampleCount - 1) * (float)fileInfo.m_sampleIntervalMilliseconds);

  if (fileInfo.IsUnbinned())
  {
    int
      maxTraceNumber = 0;
    size_t
      totalSegmentsCount = 0;

    for (const auto& segmentInfoList : fileInfo.m_segmentInfoLists)
    {
      totalSegmentsCount += segmentInfoList.size();

      for (const auto& segmentInfo : segmentInfoList)
      {
        maxTraceNumber = std::max(maxTraceNumber, segmentInfo.m_binInfoStop.m_crosslineNumber);
      }
    }

    axisDescriptors.emplace_back(maxTraceNumber, VDS_DIMENSION_TRACE_NAME(VDS_DIMENSION_TRACE_SORT_OFFSET), KNOWNMETADATA_UNIT_UNITLESS, 1.0f, (float)maxTraceNumber);

    const char
      * axisName;

    // figure out the unbinned primary axis name
    //switch (fileInfo.m_segyType)
    //{
    //case SEGY::SEGYType::ReceiverGathers:
    //  axisName = VDS_DIMENSION_RECEIVER_NAME;
    //  break;

    //case SEGY::SEGYType::ShotGathers:
    //  axisName = VDS_DIMENSION_SHOT_NAME;
    //  break;

    //case SEGY::SEGYType::CDPGathers:
    //default:
    //  axisName = VDS_DIMENSION_CDP_NAME;
    //  break;
    //}

    // Headwave uses Gather for all types?
    axisName = VDS_DIMENSION_GATHER_NAME;

    axisDescriptors.emplace_back(static_cast<int>(totalSegmentsCount), axisName, KNOWNMETADATA_UNIT_UNITLESS, 1.0f, static_cast<float>(totalSegmentsCount));
  }
  else
  {
    if (fold > 1)
    {
      axisDescriptors.emplace_back(fold, VDS_DIMENSION_TRACE_NAME(VDS_DIMENSION_TRACE_SORT_OFFSET), KNOWNMETADATA_UNIT_UNITLESS, 1.0f, static_cast<float>(fold));
    }

    int minInline = fileInfo.m_segmentInfoLists[0][0].m_binInfoStart.m_inlineNumber,
      minCrossline = fileInfo.m_segmentInfoLists[0][0].m_binInfoStart.m_crosslineNumber,
      maxInline = minInline,
      maxCrossline = minCrossline;

    for (const auto& segmentInfoList : fileInfo.m_segmentInfoLists)
    {
      for (const auto& segmentInfo : segmentInfoList)
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
    }

    // Ensure the max inline/crossline is a multiple of the step size from the min
    maxCrossline += (maxCrossline - minCrossline) % crosslineStep;
    maxInline += (maxInline - minInline) % inlineStep;

    const int
      inlineCount = 1 + (maxInline - minInline) / inlineStep,
      crosslineCount = 1 + (maxCrossline - minCrossline) / crosslineStep;

    axisDescriptors.emplace_back(crosslineCount, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE, KNOWNMETADATA_UNIT_UNITLESS, (float)minCrossline, (float)maxCrossline);
    axisDescriptors.emplace_back(inlineCount, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE, KNOWNMETADATA_UNIT_UNITLESS, (float)minInline, (float)maxInline);
  }

  return axisDescriptors;
}

struct OffsetChannelInfo
{
  int   offsetStart;
  int   offsetEnd;
  int   offsetStep;
  bool  hasOffset;

  OffsetChannelInfo(bool has, int start, int end, int step) : offsetStart(start), offsetEnd(end), offsetStep(step), hasOffset(has) {}
};

std::vector<OpenVDS::VolumeDataChannelDescriptor>
createChannelDescriptors(SEGYFileInfo const& fileInfo, OpenVDS::FloatRange const& valueRange, const OffsetChannelInfo& offsetInfo)
{
  std::vector<OpenVDS::VolumeDataChannelDescriptor>
    channelDescriptors;

  // Primary channel
  channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataChannelDescriptor::Components_1, AMPLITUDE_ATTRIBUTE_NAME, "", valueRange.Min, valueRange.Max);

  // Trace defined flag
  channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_U8, OpenVDS::VolumeDataChannelDescriptor::Components_1, "Trace", "", 0.0f, 1.0f, OpenVDS::VolumeDataMapping::PerTrace, OpenVDS::VolumeDataChannelDescriptor::DiscreteData);

  // SEG-Y trace headers
  channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_U8, OpenVDS::VolumeDataChannelDescriptor::Components_1, "SEGYTraceHeader", "", 0.0f, 255.0f, OpenVDS::VolumeDataMapping::PerTrace, SEGY::TraceHeaderSize, OpenVDS::VolumeDataChannelDescriptor::DiscreteData | OpenVDS::VolumeDataChannelDescriptor::NoLossyCompressionUseZip, 1.0f, 0.0f);

  if (offsetInfo.hasOffset)
  {
    // offset channel
    channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataChannelDescriptor::Components_1, "Offset", KNOWNMETADATA_UNIT_METER, static_cast<float>(offsetInfo.offsetStart), static_cast<float>(offsetInfo.offsetEnd),OpenVDS::VolumeDataMapping::PerTrace, OpenVDS::VolumeDataChannelDescriptor::NoLossyCompression);

    // TODO channels for other gather types - "Angle", "Vrms", "Frequency"
  }

  return channelDescriptors;
}

int64_t
findFirstTrace(TraceDataManager& traceDataManager, int primaryKey, int secondaryKey, SEGYFileInfo const& fileInfo, int64_t traceStart, int64_t traceStop, int secondaryStart, int secondaryStop, OpenVDS::Error& error)
{
  const bool isSecondaryIncreasing = (secondaryStop >= secondaryStart);

  // Check if trace is at the start of the range or completely outside the range (this also handles cases where secondaryStart == secondaryStop which would fail to make a guess below)
  if (isSecondaryIncreasing ? (secondaryKey <= secondaryStart) :
    (secondaryKey >= secondaryStart))
  {
    return traceStart;
  }
  else if (isSecondaryIncreasing ? (secondaryKey > secondaryStop) :
    (secondaryKey < secondaryStop))
  {
    return traceStop + 1;
  }

  // Make an initial guess at which trace we start on based on linear interpolation
  int64_t trace = traceStart + (secondaryKey - secondaryStart) * (traceStop - traceStart) / (secondaryStop - secondaryStart);

  while (traceStart < traceStop - 1)
  {
    const char* header = traceDataManager.getTraceData(trace, error);
    if (error.code != 0)
    {
      return traceStart;
    }

    int primaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_primaryKey, fileInfo.m_headerEndianness),
      secondaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_secondaryKey, fileInfo.m_headerEndianness);
    int64_t traceDelta;

    if (primaryTest == primaryKey)
    {
      if ((secondaryTest >= secondaryKey) == isSecondaryIncreasing)
      {
        traceStop = trace;
        secondaryStop = secondaryTest;
      }
      else
      {
        traceStart = trace;
        secondaryStart = secondaryTest;
      }

      traceDelta = (secondaryKey - secondaryTest) * (traceStop - traceStart) / (secondaryStop - secondaryStart);
    }
    else
    {
      // We need to handle corrupted traces without hanging, so we scan backwards until we find a valid trace and then we update the interval to not include the corrupted traces
      for (int64_t scan = trace - 1; scan > traceStart; scan--)
      {
        header = traceDataManager.getTraceData(scan, error);
        if (error.code != 0)
        {
          return traceStart;
        }

        primaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_primaryKey, fileInfo.m_headerEndianness);
        if (primaryTest == primaryKey)
        {
          secondaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_secondaryKey, fileInfo.m_headerEndianness);

          if ((secondaryTest >= secondaryKey) == isSecondaryIncreasing)
          {
            traceStop = scan;
            secondaryStop = secondaryTest;
            break;
          }
          else
          {
            // Start with the invalid trace and pretend it has the same secondary key as the previous valid trace
            traceStart = trace;
            secondaryStart = secondaryTest;
            break;
          }
        }
      }

      // If no valid trace was found before, start with the invalid trace and pretend it has the same secondary key as the previous valid trace
      if (primaryTest != primaryKey)
      {
        traceStart = trace;
      }

      traceDelta = 0;
    }

    // If the guessed trace is outside the range we already established, we do binary search instead -- this should ensure the range is always shrinking so we are guaranteed to terminate
    if (traceDelta == 0 || trace + traceDelta <= traceStart || trace + traceDelta >= traceStop)
    {
      trace = (traceStart + traceStop) / 2;
    }
    else
    {
      trace += traceDelta;
    }
  }

  return traceStop;
}

int64_t
findFirstTrace(TraceDataManager& traceDataManager, const SEGYSegmentInfo& segment, int secondaryKey, SEGYFileInfo const& fileInfo, OpenVDS::Error error)
{
  // TODO do the info start and stop reflect the start and stop for reverse-sorted inlines, or are they simply min and max?
  const int secondaryStart = segment.m_binInfoStart.m_crosslineNumber;
  const int secondaryStop = segment.m_binInfoStop.m_crosslineNumber;

  return findFirstTrace(traceDataManager, segment.m_primaryKey, secondaryKey, fileInfo, segment.m_traceStart, segment.m_traceStop, secondaryStart, secondaryStop, error);
}

int
SecondaryKeyDimension(const SEGYFileInfo& fileInfo)
{
  if (fileInfo.Is4D())
  {
    return 2;
  }
  return 1;
}

int
PrimaryKeyDimension(const SEGYFileInfo& fileInfo)
{
  if (fileInfo.Is4D())
  {
    return 3;
  }
  return 2;
}
int
main(int argc, char* argv[])
{
#if defined(WIN32)
  bool is_tty = _isatty(_fileno(stdout)) != 0;
#else
  bool is_tty = isatty(fileno(stdout));
#endif
  //auto start_time = std::chrono::high_resolution_clock::now();
  cxxopts::Options options("SEGYImport", "SEGYImport - A tool to scan and import a SEG-Y file to a volume data store (VDS)\n\nSee online documentation for connection paramters:\nhttp://osdu.pages.community.opengroup.org/platform/domain-data-mgmt-services/seismic/open-vds/connection.html\n");
  options.positional_help("<input file>");

  std::string headerFormatFileName;
  std::vector<std::string> headerFields;
  std::string primaryKey = "InlineNumber";
  std::string secondaryKey = "CrosslineNumber";
  std::string sampleUnit;
  std::string crsWkt;
  double scale = 0;
  bool overrideSampleStart = false;
  bool overrideBrickSize = false;
  bool overrideMargin = false;
  double sampleStart = 0;
  bool littleEndian = false;
  bool scan = false;
  std::string fileInfoFileName;
  int brickSize = 64;
  int margin = 0;
  bool force = false;
  bool ignoreWarnings = false;
  std::string compressionMethodString;
  float compressionTolerance = 0;
  std::string url;
  std::string urlConnection;
  std::string inputConnection;
  std::string persistentID;
  bool uniqueID = false;
  bool disablePersistentID = false;
  bool prestack = false;
  bool traceOrderByOffset = true;
  bool jsonOutput = false;
  bool help = false;
  bool version = false;

  std::string supportedCompressionMethods = "None";
  if(OpenVDS::IsCompressionMethodSupported(OpenVDS::CompressionMethod::Wavelet)) supportedCompressionMethods += ", Wavelet";
  if(OpenVDS::IsCompressionMethodSupported(OpenVDS::CompressionMethod::RLE)) supportedCompressionMethods += ", RLE";
  if(OpenVDS::IsCompressionMethodSupported(OpenVDS::CompressionMethod::Zip)) supportedCompressionMethods += ", Zip";
  if(OpenVDS::IsCompressionMethodSupported(OpenVDS::CompressionMethod::WaveletNormalizeBlock)) supportedCompressionMethods += ", WaveletNormalizeBlock";
  if(OpenVDS::IsCompressionMethodSupported(OpenVDS::CompressionMethod::WaveletLossless)) supportedCompressionMethods += ", WaveletLossless";
  if(OpenVDS::IsCompressionMethodSupported(OpenVDS::CompressionMethod::WaveletNormalizeBlockLossless)) supportedCompressionMethods += ", WaveletNormalizeBlockLossless";

  std::vector<std::string> fileNames;

  options.add_option("", "", "header-format", "A JSON file defining the header format for the input SEG-Y file. The expected format is a dictonary of strings (field names) to pairs (byte position, field width) where field width can be \"TwoByte\" or \"FourByte\". Additionally, an \"Endianness\" key can be specified as \"BigEndian\" or \"LittleEndian\".", cxxopts::value<std::string>(headerFormatFileName), "<file>");
  options.add_option("", "", "header-field", "A single definition of a header field. The expected format is a \"fieldname=offset:width\" where the \":width\" is optional. Its also possible to specify range: \"fieldname=begin-end\". Multiple header-fields is specified by providing multiple --header-field arguments.", cxxopts::value<std::vector<std::string>>(headerFields), "header_name=offset:width");
  options.add_option("", "p", "primary-key", "The name of the trace header field to use as the primary key.", cxxopts::value<std::string>(primaryKey)->default_value("Inline"), "<field>");
  options.add_option("", "s", "secondary-key", "The name of the trace header field to use as the secondary key.", cxxopts::value<std::string>(secondaryKey)->default_value("Crossline"), "<field>");
  options.add_option("", "", "prestack", "Import binned prestack data (PSTM/PSDM gathers).", cxxopts::value<bool>(prestack), "");
  options.add_option("", "", "scale", "If a scale override (floating point) is given, it is used to scale the coordinates in the header instead of determining the scale factor from the coordinate scale trace header field.", cxxopts::value<double>(scale), "<value>");
  options.add_option("", "", "sample-unit", "A sample unit of 'ms' is used for datasets in the time domain (default), while a sample unit of 'm' or 'ft' is used for datasets in the depth domain", cxxopts::value<std::string>(sampleUnit), "<string>");
  options.add_option("", "", "sample-start", "The start time/depth/frequency (depending on the domain) of the sampling", cxxopts::value<double>(sampleStart), "<value>");
  options.add_option("", "", "crs-wkt", "A coordinate reference system in well-known text format can optionally be provided", cxxopts::value<std::string>(crsWkt), "<string>");
  options.add_option("", "l", "little-endian", "Force little-endian trace headers.", cxxopts::value<bool>(littleEndian), "");
  options.add_option("", "", "scan", "Generate a JSON file containing information about the input SEG-Y file.", cxxopts::value<bool>(scan), "");
  options.add_option("", "i", "file-info", "A JSON file (generated by the --scan option) containing information about the input SEG-Y file.", cxxopts::value<std::string>(fileInfoFileName), "<file>");
  options.add_option("", "b", "brick-size", "The brick size for the volume data store.", cxxopts::value<int>(brickSize), "<value>");
  options.add_option("", "",  "margin", "The margin size (overlap) of the bricks.", cxxopts::value<int>(margin), "<value>");
  options.add_option("", "f", "force", "Continue on upload error.", cxxopts::value<bool>(force), "");
  options.add_option("", "", "ignore-warnings", "Ignore warnings about import parameters.", cxxopts::value<bool>(ignoreWarnings), "");
  options.add_option("", "", "compression-method", std::string("Compression method. Supported compression methods are: ") + supportedCompressionMethods + ".", cxxopts::value<std::string>(compressionMethodString), "<string>");
  options.add_option("", "", "tolerance", "This parameter specifies the compression tolerance when using the wavelet compression method. This value is the maximum deviation from the original data value when the data is converted to 8-bit using the value range. A value of 1 means the maximum allowable loss is the same as quantizing to 8-bit (but the average loss will be much much lower than quantizing to 8-bit). It is not a good idea to directly relate the tolerance to the quality of the compressed data, as the average loss will in general be an order of magnitude lower than the allowable loss.", cxxopts::value<float>(compressionTolerance), "<value>");
  options.add_option("", "", "url", "Url with cloud vendor scheme used for target location or file name of output VDS file.", cxxopts::value<std::string>(url), "<string>");
  options.add_option("", "", "url-connection", "Connection string used for additional parameters to the url connection", cxxopts::value<std::string>(urlConnection), "<string>");
  options.add_option("", "", "vdsfile", "File name of output VDS file.", cxxopts::value<std::string>(url), "<string>");
  options.add_option("", "", "input-connection", "Connection string used for additional parameters to the input connection", cxxopts::value<std::string>(inputConnection), "<string>");
  options.add_option("", "", "persistentID", "A globally unique ID for the VDS, usually an 8-digit hexadecimal number.", cxxopts::value<std::string>(persistentID), "<ID>");
  options.add_option("", "", "uniqueID", "Generate a new globally unique ID when scanning the input SEG-Y file.", cxxopts::value<bool>(uniqueID), "");
  options.add_option("", "", "disable-persistentID", "Disable the persistentID usage, placing the VDS directly into the url location.", cxxopts::value<bool>(disablePersistentID), "");
  options.add_option("", "", "json-output", "Enable json output.", cxxopts::value<bool>(jsonOutput), "");
  // TODO add option for turning off traceOrderByOffset

  options.add_option("", "h", "help", "Print this help information", cxxopts::value<bool>(help), "");
  options.add_option("", "", "version", "Print version information.", cxxopts::value<bool>(version), "");

  options.add_option("", "", "input", "", cxxopts::value<std::vector<std::string>>(fileNames), "");
  options.parse_positional("input");

  if (argc == 1)
  {
    OpenVDS::printInfo(jsonOutput, "Args", options.help());
    return EXIT_SUCCESS;
  }

  try
  {
    auto result = options.parse(argc, argv);
    overrideSampleStart = result.count("sample-start") != 0;
    overrideBrickSize = result.count("brick-size") != 0;
    overrideMargin = result.count("margin") != 0;
  }
  catch (cxxopts::OptionParseException &e)
  {
    OpenVDS::printError(jsonOutput, "Args", e.what());
    return EXIT_FAILURE;
  }

  if (help)
  {
    OpenVDS::printInfo(jsonOutput, "Args", options.help());
    return EXIT_SUCCESS;
  }

  if (version)
  {
    OpenVDS::printVersion(jsonOutput, "SEGYImport");
    return EXIT_SUCCESS;
  }

  // set up the compression method
  OpenVDS::CompressionMethod compressionMethod = OpenVDS::CompressionMethod::None;

  std::transform(compressionMethodString.begin(), compressionMethodString.end(), compressionMethodString.begin(), asciitolower);

  if(compressionMethodString.empty()) compressionMethod = OpenVDS::CompressionMethod::None;
  else if(compressionMethodString == "none")                          compressionMethod = OpenVDS::CompressionMethod::None;
  else if(compressionMethodString == "wavelet")                       compressionMethod = OpenVDS::CompressionMethod::Wavelet;
  else if(compressionMethodString == "rle")                           compressionMethod = OpenVDS::CompressionMethod::RLE;
  else if(compressionMethodString == "zip")                           compressionMethod = OpenVDS::CompressionMethod::Zip;
  else if(compressionMethodString == "waveletnormalizeblock")         compressionMethod = OpenVDS::CompressionMethod::WaveletNormalizeBlock;
  else if(compressionMethodString == "waveletlossless")               compressionMethod = OpenVDS::CompressionMethod::WaveletLossless;
  else if(compressionMethodString == "waveletnormalizeblocklossless") compressionMethod = OpenVDS::CompressionMethod::WaveletNormalizeBlockLossless;
  else
  {
    OpenVDS::printError(jsonOutput, "CompressionMethod", "Unknown compression method", compressionMethodString);
    return EXIT_FAILURE;
  }

  if(!OpenVDS::IsCompressionMethodSupported(compressionMethod))
  {
    OpenVDS::printError(jsonOutput, "CompressionMethod", "Unsupported compression method", compressionMethodString);
    return EXIT_FAILURE;
  }

  if(compressionMethod == OpenVDS::CompressionMethod::Wavelet || compressionMethod == OpenVDS::CompressionMethod::WaveletNormalizeBlock || compressionMethod == OpenVDS::CompressionMethod::WaveletLossless || compressionMethod == OpenVDS::CompressionMethod::WaveletNormalizeBlockLossless)
  {
    if(!overrideBrickSize)
    {
      brickSize = 128;
    }
    if(!overrideMargin)
    {
      margin = 4;
    }
  }

  // get the canonical field name for the primary and secondary key
  ResolveAlias(primaryKey);
  ResolveAlias(secondaryKey);

  SEGY::SEGYType segyType = SEGY::SEGYType::Poststack;

  if (primaryKey == "inlinenumber" || primaryKey == "crosslinenumber" )
  {
    if(prestack)
    {
      segyType = SEGY::SEGYType::Prestack;
    }
    else
    {
      segyType = SEGY::SEGYType::Poststack;
    }
  }
  else if (primaryKey == "receiver")
  {
    segyType = SEGY::SEGYType::ReceiverGathers;
  }
  else if (primaryKey == "energysourcepointnumber")
  {
    segyType = SEGY::SEGYType::ShotGathers;
  }
  else
  {
    OpenVDS::printError(jsonOutput, "SEGY", "Primary key does not match a known SEG-Y type");
    return EXIT_FAILURE;
  }

  if (fileNames.empty())
  {
    OpenVDS::printError(jsonOutput, "SEGY", "No input SEG-Y file specified");
    return EXIT_FAILURE;
  }

  if (fileNames.size() > 1 && segyType != SEGY::SEGYType::Prestack)
  {
    OpenVDS::printError(jsonOutput, "SEGY", "Only one input SEG-Y file may be specified");
    return EXIT_FAILURE;
  }

  if(uniqueID && !persistentID.empty())
  {
    OpenVDS::printError(jsonOutput, "Args", "--uniqueID does not make sense when the persistentID is specified");
    return EXIT_FAILURE;
  }
  
  if(disablePersistentID && !persistentID.empty())
  {
    OpenVDS::printError(jsonOutput, "Args", "--disable-PersistentID does not make sense when the persistentID is specified");
    return EXIT_FAILURE;
  }

  SEGY::Endianness headerEndianness = (littleEndian ? SEGY::Endianness::LittleEndian : SEGY::Endianness::BigEndian);

  if (!headerFormatFileName.empty())
  {
    OpenVDS::Error
      error;
    DataProvider headerFormatDataProvider = CreateDataProvider(headerFormatFileName, inputConnection, error);

    if (error.code != 0)
    {
      OpenVDS::printError(jsonOutput, "File", "Could not open header format file", headerFormatFileName);
      return EXIT_FAILURE;
    }

    ParseHeaderFormatFile(headerFormatDataProvider, g_traceHeaderFields, headerEndianness, error);

    if (error.code != 0)
    {
      OpenVDS::printError(jsonOutput, "File", "Could not read header format file", headerFormatFileName, error.string);
      return EXIT_FAILURE;
    }
  }
  if (headerFields.size())
  {
    OpenVDS::Error error;
    if (!ParseHeaderFieldArgs(headerFields, g_traceHeaderFields, headerEndianness, error))
    {
      OpenVDS::printError(jsonOutput, "HeaderFields", "Could not parse header-fields", error.string);
      return EXIT_FAILURE;
    }
  }

  SEGY::HeaderField
    primaryKeyHeaderField,
    secondaryKeyHeaderField;

  if (g_traceHeaderFields.find(primaryKey) != g_traceHeaderFields.end())
  {
    primaryKeyHeaderField = g_traceHeaderFields[primaryKey];
  }
  else
  {
    OpenVDS::printError(jsonOutput, "HeaderFields", "Unrecognized header field given for primary key", primaryKey);
    return EXIT_FAILURE;
  }

  if (g_traceHeaderFields.find(secondaryKey) != g_traceHeaderFields.end())
  {
    secondaryKeyHeaderField = g_traceHeaderFields[secondaryKey];
  }

  SEGY::HeaderField
    startTimeHeaderField = g_traceHeaderFields["StartTime"];

  SEGYBinInfoHeaderFields
    binInfoHeaderFields(g_traceHeaderFields[primaryKey], g_traceHeaderFields[secondaryKey], g_traceHeaderFields["CoordinateScale"], g_traceHeaderFields["EnsembleXCoordinate"], g_traceHeaderFields["EnsembleYCoordinate"], scale);

  OpenVDS::Error
    error;
  std::string errorFileName;
  auto dataProviders = CreateDataProviders(fileNames, inputConnection, error, errorFileName);
  if (error.code != 0)
  {
    // TODO need to name which file failed to open
    OpenVDS::printError(jsonOutput, "IO", "Could not open input file", errorFileName, error.string);
    return EXIT_FAILURE;
  }

  SEGYFileInfo
    fileInfo(headerEndianness);
  fileInfo.m_segyType = segyType;

  // Scan the file if '--scan' was passed or we're uploading but no fileInfo file was specified
  if(scan || fileInfoFileName.empty())
  {
    if(!uniqueID)
    {
      OpenVDS::HashCombiner
        hash;

      for(std::string const &fileName : fileNames)
      {
        hash.Add(fileName);
      }

      fileInfo.m_persistentID = OpenVDS::HashCombiner(hash);
    }

    bool success = fileInfo.Scan(dataProviders, error, primaryKeyHeaderField, secondaryKeyHeaderField, startTimeHeaderField, binInfoHeaderFields);

    if (!success)
    {
      OpenVDS::printError(jsonOutput, "File", "Failed to scan file", fileNames[0], error.string);
      return EXIT_FAILURE;
    }

    if(overrideSampleStart)
    {
      fileInfo.m_startTimeMilliseconds = sampleStart;
    }

    // If we are in scan mode we serialize the result of the file scan either to a fileInfo file (if specified) or to stdout and exit
    if(scan)
    {
      // TODO if we have multiple input files we need to serialize multiple scan files
      Json::Value jsonFileInfo = SerializeSEGYFileInfo(fileInfo, 0);

      Json::StreamWriterBuilder wbuilder;
      wbuilder["indentation"] = "  ";
      std::string document = Json::writeString(wbuilder, jsonFileInfo);

      if (fileInfoFileName.empty())
      {
        fmt::print(stdout, "{}", document);
      }
      else
      {
        OpenVDS::Error
          error;

        if (OpenVDS::IsSupportedProtocol(fileInfoFileName))
        {
          std::string dirname;
          std::string basename;
          std::string parameters;
          splitUrl(fileInfoFileName, dirname, basename, parameters, error);
          if (error.code)
          {
            OpenVDS::printError(jsonOutput, "IO", "Failed to creating IOManager for", fileInfoFileName, error.string);
            return EXIT_FAILURE;
          }
          std::string scanUrl = dirname + parameters;
          std::unique_ptr<OpenVDS::IOManager> ioManager(OpenVDS::IOManager::CreateIOManager(scanUrl, urlConnection, OpenVDS::IOManager::ReadWrite, error));
          if (error.code)
          {
            OpenVDS::printError(jsonOutput, "IO", "Failed to creating IOManager for", fileInfoFileName, error.string);
            return EXIT_FAILURE;
          }
          auto shared_data = std::make_shared<std::vector<uint8_t>>();
          shared_data->insert(shared_data->end(), document.begin(), document.end());
          auto req = ioManager->WriteObject(basename, "", "text/plain", {}, shared_data, {});
          req->WaitForFinish();
          req->IsSuccess(error);
          if (error.code)
          {
            OpenVDS::printError(jsonOutput, "IO", "Failed to write", fileInfoFileName, error.string);
            return EXIT_FAILURE;
          }
        }
        else
        {
          OpenVDS::File
            fileInfoFile;

          fileInfoFile.Open(fileInfoFileName.c_str(), true, false, true, error);

          if (error.code != 0)
          {
            OpenVDS::printError(jsonOutput, "IO", "Could not create file info file", fileInfoFileName);
            return EXIT_FAILURE;
          }

          fileInfoFile.Write(document.data(), 0, (int32_t)document.size(), error);

          if (error.code != 0)
          {
            OpenVDS::printError(jsonOutput, "IO", "Could not write file info to file", fileInfoFileName);
            return EXIT_FAILURE;
          }
        }

      }
      return EXIT_SUCCESS;
    }
  }
  else if (!fileInfoFileName.empty())
  {
    OpenVDS::Error
      error;

    DataProvider fileInfoDataProvider = CreateDataProvider(fileInfoFileName, inputConnection, error);

    if (error.code != 0)
    {
      OpenVDS::printError(jsonOutput, "IO", "Could not create data provider for", fileInfoFileName, error.string);
      return EXIT_FAILURE;
    }

    bool success = parseSEGYFileInfoFile(fileInfoDataProvider, fileInfo, error);

    if (!success)
    {
      OpenVDS::printError(jsonOutput, "FileInfo", "Parse SEGYFileInfo", fileInfoFileName, error.string);
      return EXIT_FAILURE;
    }

    if(overrideSampleStart)
    {
      fileInfo.m_startTimeMilliseconds = sampleStart;
    }
  }
  else
  {
    OpenVDS::printError(jsonOutput, "IO", "No SEG-Y file info file specified");
    return EXIT_FAILURE;
  }

  if (persistentID.empty() && !disablePersistentID)
  {
    persistentID = fmt::format("{:X}", fileInfo.m_persistentID);
  }

  // Check for only a single segment

  if(fileInfo.m_segmentInfoLists.size() == 1 && fileInfo.m_segmentInfoLists[0].size() == 1)
  {
    OpenVDS::printWarning_with_condition_fatal(jsonOutput, !ignoreWarnings, "SegmentInfoList", "Warning: There is only one segment, either this is (as of now unsupported) 2D data or this usually indicates using the wrong header format for the input dataset.", "Use --ignore-warnings to force the import to go ahead.");
  }

  // Determine value range, fold and primary/secondary step

  OpenVDS::FloatRange valueRange;
  int fold = 1, primaryStep = 1, secondaryStep = 1;
  int offsetStart, offsetEnd, offsetStep;

  const float valueRangePercentile = 99.5f; // 99.5f is the same default as Petrel uses.

  int fileIndex;
  auto representativeSegment = findRepresentativeSegment(fileInfo, primaryStep, fileIndex);
  analyzeSegment(dataProviders[fileIndex], fileInfo, representativeSegment, valueRangePercentile, valueRange, fold, secondaryStep, segyType, offsetStart, offsetEnd, offsetStep, jsonOutput, error);

  if (error.code != 0)
  {
    OpenVDS::printError(jsonOutput, "SEGY", error.string);
    return EXIT_FAILURE;
  }

  if (IsSEGYTypeUnbinned(segyType))
  {
    // For unbinned data the segments are the gathers, so the fold is the number of traces in the longest segment
    assert(fold == 1 && "analyzeSegment should report a fold of 1 for unbinned types");
    // If we ever want to print the fold for diagnostic purposes we should loop through the segments and determine the actual fold here
  }
  else if (segyType == SEGY::SEGYType::Poststack || segyType == SEGY::SEGYType::Poststack2D)
  {
    if(fold > 1)
    {
      OpenVDS::printError(jsonOutput, "SEGY", fmt::format("Detected a fold of '{0}', this usually indicates using the wrong header format or primary key for the input dataset or that the input data is binned prestack data (PSTM/PSDM gathers) in which case the --prestack option should be used.", fold));
      return EXIT_FAILURE;
    }
  }
  else
  {
    if (fold <= 1)
    {
      OpenVDS::printError(jsonOutput, "SEGY", fmt::format("Detected a fold of '{0}', this usually indicates using the wrong header format or primary key for the input dataset or that the input data is poststack in which case the --prestack option should not been used.", fold));
      return EXIT_FAILURE;
    }
  }

  // Create layout descriptor
  enum OpenVDS::VolumeDataLayoutDescriptor::BrickSize
    brickSizeEnum;

  switch (brickSize)
  {
  case 32: brickSizeEnum = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32; break;
  case 64: brickSizeEnum = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_64; break;
  case 128: brickSizeEnum = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_128; break;
  case 256: brickSizeEnum = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_256; break;
  default:
    OpenVDS::printError(jsonOutput, "Args", "Illegal brick size (must be 32, 64, 128 or 256)");
    return EXIT_FAILURE;
  }

  OpenVDS::VolumeDataLayoutDescriptor::Options layoutOptions = OpenVDS::VolumeDataLayoutDescriptor::Options_None;
  OpenVDS::VolumeDataLayoutDescriptor::LODLevels lodLevels = OpenVDS::VolumeDataLayoutDescriptor::LODLevels_None;

  const int negativeMargin = margin;
  const int positiveMargin = margin;
  const int brickSize2DMultiplier = 4;

  OpenVDS::VolumeDataLayoutDescriptor layoutDescriptor(brickSizeEnum, negativeMargin, positiveMargin, brickSize2DMultiplier, lodLevels, layoutOptions);

  SEGY::SampleUnits
    sampleUnits;

  if(sampleUnit.empty() || sampleUnit == "ms" || sampleUnit == "millisecond"  || sampleUnit == "milliseconds")
  {
    sampleUnits = SEGY::SampleUnits::Milliseconds;
  }
  else if(sampleUnit == "m" || sampleUnit == "meter" || sampleUnit == "meters")
  {
    sampleUnits = SEGY::SampleUnits::Meters;
  }
  else if(sampleUnit == "ft" || sampleUnit == "foot" || sampleUnit == "feet")
  {
    sampleUnits = SEGY::SampleUnits::Feet;
  }
  else
  {
    OpenVDS::printError(jsonOutput, "Args", "Unknown sample unit: {}, legal units are 'ms', 'm' or 'ft'\n", sampleUnit);
    return EXIT_FAILURE;
  }

  // Create axis descriptors
  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors = createAxisDescriptors(fileInfo, sampleUnits, fold, primaryStep, secondaryStep);

  // Check for excess of empty traces

  int64_t
    traceCountInVDS = 1;

  for(int axis = 1; axis < (int)axisDescriptors.size(); axis++)
  {
    traceCountInVDS *= axisDescriptors[axis].GetNumSamples();
  }

  const auto
    totalTraceCount = std::accumulate(fileInfo.m_traceCounts.begin(), fileInfo.m_traceCounts.end(), static_cast<int64_t>(0));

  if(traceCountInVDS >= totalTraceCount * 2)
  {
    std::string msg = fmt::format("There is more than {:.1f}% empty traces in the VDS, this usually indicates using the wrong header format or primary key for the input dataset.\n", double(traceCountInVDS - totalTraceCount) * 100.0 / double(traceCountInVDS));
    std::string fatal_msg = "Use --ignore-warnings to force the import to go ahead.";
    OpenVDS::printWarning_with_condition_fatal(jsonOutput, !ignoreWarnings, "SEGY", msg, fatal_msg);
  }

  // Create channel descriptors
  std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors = createChannelDescriptors(fileInfo, valueRange, OffsetChannelInfo(fileInfo.HasGatherOffset(), offsetStart, offsetEnd, offsetStep));

  // Create metadata
  OpenVDS::MetadataContainer
    metadataContainer;

  createImportInformationMetadata(dataProviders, metadataContainer, error);

  if (error.code != 0)
  {
    OpenVDS::printError(jsonOutput, "Metadata", error.string);
    return EXIT_FAILURE;
  }

  SEGY::BinaryHeader::MeasurementSystem
    measurementSystem;

  // get SEGY metadata from first file
  createSEGYMetadata(dataProviders[0], fileInfo, metadataContainer, measurementSystem, error);

  if (error.code != 0)
  {
    OpenVDS::printError(jsonOutput, "Metadata", error.string);
    return EXIT_FAILURE;
  }

  if (primaryKey == "InlineNumber" || primaryKey == "CrosslineNumber")
  {
    // only create the lattice metadata if the primary key is Inline or Crossline, otherwise we may not be able to determine the bin grid
    createSurveyCoordinateSystemMetadata(fileInfo, measurementSystem, crsWkt, metadataContainer);
  }

  OpenVDS::Error createError;

  OpenVDS::VDSHandle handle;

  if(OpenVDS::IsSupportedProtocol(url))
  {
    if (!persistentID.empty())
    {
      std::string baseUrl;
      std::string parameters;
      splitUrlOnParameters(url, baseUrl, parameters);
      if (baseUrl.back() != '/')
      {
        baseUrl.push_back('/');
      }
      baseUrl.insert(baseUrl.end(), persistentID.begin(), persistentID.end());
      url = baseUrl + parameters;
    }
    handle = OpenVDS::Create(url, urlConnection, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, compressionMethod, compressionTolerance, createError);
  }
  else
  {
    handle = OpenVDS::Create(OpenVDS::VDSFileOpenOptions(url), layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, compressionMethod, compressionTolerance, createError);
  }

  if (createError.code != 0)
  {
    OpenVDS::printError(jsonOutput, "VDS", "Could not create VDS", createError.string);
    return EXIT_FAILURE;
  }

  // auto-close vds handle when it goes out of scope
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> vdsGuard(handle, &OpenVDS::Close);

  auto accessManager = OpenVDS::GetAccessManager(handle);
  auto layout = accessManager.GetVolumeDataLayout();


  OpenVDS::DimensionsND writeDimensionGroup = OpenVDS::DimensionsND::Dimensions_012;

  if (IsSEGYTypeUnbinned(segyType))
  {
    writeDimensionGroup = OpenVDS::DimensionsND::Dimensions_01;
  }
  else if(segyType == SEGY::SEGYType::Prestack && primaryKey == "CrosslineNumber")
  {
    writeDimensionGroup = OpenVDS::DimensionsND::Dimensions_013;
  }

  auto amplitudeAccessor = accessManager.CreateVolumeDataPageAccessor(writeDimensionGroup, 0, 0, 8, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  auto traceFlagAccessor = accessManager.CreateVolumeDataPageAccessor(writeDimensionGroup, 0, 1, 8, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  auto segyTraceHeaderAccessor = accessManager.CreateVolumeDataPageAccessor(writeDimensionGroup, 0, 2, 8, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  auto offsetAccessor = fileInfo.HasGatherOffset() ? accessManager.CreateVolumeDataPageAccessor(writeDimensionGroup, 0, 3, 8, OpenVDS::VolumeDataAccessManager::AccessMode_Create) : nullptr;

  int64_t traceByteSize = fileInfo.TraceByteSize();

  std::shared_ptr<DataView> dataView;

  int percentage = -1;
  OpenVDS::printInfo(jsonOutput, "ImportLocation", "Importing into", url);

  struct ChunkInfo
  {
    int min[OpenVDS::Dimensionality_Max];
    int max[OpenVDS::Dimensionality_Max];
    int64_t chunk;
    int sampleStart;
    int sampleCount;
    int secondaryKeyStart;
    int secondaryKeyStop;
    int primaryKeyStart;
    int primaryKeyStop;
    std::map<size_t, std::pair<size_t, size_t>>
      lowerUpperSegmentIndices;
  };

  // limit DataViewManager's memory use to 1.5 sets of brick inlines
  const int64_t dvmMemoryLimit = 3LL * (writeDimensionGroup == OpenVDS::DimensionsND::Dimensions_01 ? 1 : brickSize) * axisDescriptors[1].GetNumSamples() * fileInfo.TraceByteSize() / 2LL;

  // create DataViewManagers and TraceDataManagers for each input file
  std::vector<std::shared_ptr<DataViewManager>>
    dataViewManagers;
  std::vector<TraceDataManager>
    traceDataManagers;
  const auto
    perFileMemoryLimit = dvmMemoryLimit / dataProviders.size();

  for (size_t fileIndex = 0; fileIndex < fileInfo.m_segmentInfoLists.size(); ++fileIndex)
  {
    dataViewManagers.emplace_back(std::make_shared<DataViewManager>(dataProviders[fileIndex], perFileMemoryLimit));
    traceDataManagers.emplace_back(dataViewManagers.back(), 128, traceByteSize, fileInfo.m_traceCounts[fileIndex]);
  }

  std::vector<ChunkInfo> chunkInfos;
  chunkInfos.resize(amplitudeAccessor->GetChunkCount());
  std::vector<DataRequestInfo> dataRequests;
  dataRequests.reserve(chunkInfos.capacity());
  for (int64_t chunk = 0; chunk < amplitudeAccessor->GetChunkCount(); chunk++)
  {
    auto &chunkInfo = chunkInfos[chunk];
    amplitudeAccessor->GetChunkMinMax(chunk, chunkInfo.min, chunkInfo.max);

    chunkInfo.sampleStart = chunkInfo.min[0];
    chunkInfo.sampleCount = chunkInfo.max[0] - chunkInfo.min[0];

    chunkInfo.secondaryKeyStart = (int)floorf(layout->GetAxisDescriptor(SecondaryKeyDimension(fileInfo)).SampleIndexToCoordinate(chunkInfo.min[SecondaryKeyDimension(fileInfo)]) + 0.5f);
    chunkInfo.secondaryKeyStop = (int)floorf(layout->GetAxisDescriptor(SecondaryKeyDimension(fileInfo)).SampleIndexToCoordinate(chunkInfo.max[SecondaryKeyDimension(fileInfo)] - 1) + 0.5f);

    chunkInfo.primaryKeyStart = (int)floorf(layout->GetAxisDescriptor(PrimaryKeyDimension(fileInfo)).SampleIndexToCoordinate(chunkInfo.min[PrimaryKeyDimension(fileInfo)]) + 0.5f);
    chunkInfo.primaryKeyStop = (int)floorf(layout->GetAxisDescriptor(PrimaryKeyDimension(fileInfo)).SampleIndexToCoordinate(chunkInfo.max[PrimaryKeyDimension(fileInfo)] - 1) + 0.5f);

    // For each input file, find the lower/upper segments and then add data requests to that file's traceDataManager

    for (size_t fileIndex = 0; fileIndex < fileInfo.m_segmentInfoLists.size(); ++fileIndex)
    {
      auto&
        segmentInfoList = fileInfo.m_segmentInfoLists[fileIndex];

      // does this file have any segments in the primary key range?
      bool hasSegments;
      if (fileInfo.IsUnbinned())
      {
        // Unbinned primary keys are 1-based segment indices
        hasSegments = 1 <= chunkInfo.primaryKeyStop && static_cast<int>(segmentInfoList.size()) >= chunkInfo.primaryKeyStart;
      }
      else
      {
        hasSegments = segmentInfoList.front().m_primaryKey <= chunkInfo.primaryKeyStop && segmentInfoList.back().m_primaryKey >= chunkInfo.primaryKeyStart;
      }
      if (hasSegments)
      {
        std::vector<SEGYSegmentInfo>::iterator
          lower,
          upper;

        if (fileInfo.IsUnbinned())
        {
          // For unbinned data we set lower and upper based on the 1-based indices instead of searching on the segment primary keys.
          // When we implement raw gathers mode we'll need to modify this.
          lower = segmentInfoList.begin();
          std::advance(lower, chunkInfo.primaryKeyStart - 1);
          upper = segmentInfoList.begin();
          // no "- 1" because we want upper to be the stop iterator
          std::advance(upper, chunkInfo.primaryKeyStop);
        }
        else
        {
          lower = std::lower_bound(segmentInfoList.begin(), segmentInfoList.end(), chunkInfo.primaryKeyStart, [](SEGYSegmentInfo const& segmentInfo, int primaryKey)->bool { return segmentInfo.m_primaryKey < primaryKey; });
          upper = std::upper_bound(segmentInfoList.begin(), segmentInfoList.end(), chunkInfo.primaryKeyStop, [](int primaryKey, SEGYSegmentInfo const& segmentInfo)->bool { return primaryKey < segmentInfo.m_primaryKey; });
        }

        const size_t lowerSegmentIndex = std::distance(segmentInfoList.begin(), lower);
        const size_t upperSegmentIndex = std::distance(segmentInfoList.begin(), upper);
        chunkInfo.lowerUpperSegmentIndices[fileIndex] = std::make_pair(lowerSegmentIndex, upperSegmentIndex);

        traceDataManagers[fileIndex].addDataRequests(chunkInfo.secondaryKeyStart, chunkInfo.secondaryKeyStop, lower, upper);
      }
    }
  }

  for (int64_t chunk = 0; chunk < amplitudeAccessor->GetChunkCount() && error.code == 0; chunk++)
  {
    int new_percentage = int(double(chunk) / amplitudeAccessor->GetChunkCount() * 100);
    if (!jsonOutput && is_tty && percentage != new_percentage)
    {
      percentage = new_percentage;
      fmt::print(stdout, "\r {:3}% Done. ", percentage);
      fflush(stdout);
    }
    int32_t errorCount = accessManager.UploadErrorCount();
    if (errorCount)
    {
      OpenVDS::PrintWarningContext warningContext(jsonOutput, "VDS", !force, "Use -f/--force to continue uploading after upload errors");
      for (int i = 0; i < errorCount; i++)
      {
        const char* object_id;
        int32_t error_code;
        const char* error_string;
        accessManager.GetCurrentUploadError(&object_id, &error_code, &error_string);
        warningContext.addWarning("Failed to upload object", fmt::format("{}", object_id), fmt::format("Error code {}: {}", object_id, error_code, error_string));
      }
    }

    auto &chunkInfo = chunkInfos[chunk];

    // if we've crossed to a new inline then trim the trace page cache
    if (chunk > 0)
    {
      const auto& previousChunkInfo = chunkInfos[chunk - 1];

      for (size_t chunkFileIndex = 0; chunkFileIndex < dataProviders.size(); ++chunkFileIndex)
      {
        auto prevIndexIter = previousChunkInfo.lowerUpperSegmentIndices.find(chunkFileIndex);
        if (prevIndexIter != previousChunkInfo.lowerUpperSegmentIndices.end())
        {
          auto currentIndexIter = chunkInfo.lowerUpperSegmentIndices.find(chunkFileIndex);
          if (currentIndexIter != chunkInfo.lowerUpperSegmentIndices.end())
          {
            // This file is active in both the current and previous chunks. Check to see if we've progressed to a new set of inlines.
            auto previousLowerSegmentIndex = std::get<0>(prevIndexIter->second);
            auto currentLowerSegmentIndex = std::get<0>(currentIndexIter->second);
            if (currentLowerSegmentIndex > previousLowerSegmentIndex)
            {
              // we've progressed to a new set of inlines; remove earlier pages from the cache
              traceDataManagers[chunkFileIndex].retirePagesBefore(fileInfo.m_segmentInfoLists[chunkFileIndex][currentLowerSegmentIndex].m_traceStart);
            }
          }
          else
          {
            // This file was active in the previous chunk but not in the current chunk, which implies that we don't
            // need any more data from this file.
            traceDataManagers[chunkFileIndex].retireAllPages();
          }
        }
        // else This file isn't used in either the previous or current chunks. We don't need to do anything.
      }
    }

    OpenVDS::VolumeDataPage* amplitudePage = amplitudeAccessor->CreatePage(chunk);
    OpenVDS::VolumeDataPage* traceFlagPage = nullptr;
    OpenVDS::VolumeDataPage* segyTraceHeaderPage = nullptr;
    OpenVDS::VolumeDataPage* offsetPage = nullptr;

    if (chunkInfo.min[0] == 0)
    {
      traceFlagPage = traceFlagAccessor->CreatePage(traceFlagAccessor->GetMappedChunkIndex(chunk));
      segyTraceHeaderPage = segyTraceHeaderAccessor->CreatePage(segyTraceHeaderAccessor->GetMappedChunkIndex(chunk));
      if (offsetAccessor != nullptr)
      {
        offsetPage = offsetAccessor->CreatePage(offsetAccessor->GetMappedChunkIndex(chunk));
      }
    }

    int amplitudePitch[OpenVDS::Dimensionality_Max];
    int traceFlagPitch[OpenVDS::Dimensionality_Max];
    int segyTraceHeaderPitch[OpenVDS::Dimensionality_Max];
    int offsetPitch[OpenVDS::Dimensionality_Max];

    void* amplitudeBuffer = amplitudePage->GetWritableBuffer(amplitudePitch);
    void* traceFlagBuffer = traceFlagPage ? traceFlagPage->GetWritableBuffer(traceFlagPitch) : nullptr;
    void* segyTraceHeaderBuffer = segyTraceHeaderPage ? segyTraceHeaderPage->GetWritableBuffer(segyTraceHeaderPitch) : nullptr;
    void* offsetBuffer = offsetPage ? offsetPage->GetWritableBuffer(offsetPitch) : nullptr;

    assert(amplitudePitch[0] == 1);
    assert(!traceFlagBuffer || traceFlagPitch[1] == 1);
    assert(!segyTraceHeaderBuffer || segyTraceHeaderPitch[1] == SEGY::TraceHeaderSize);
    assert(!offsetBuffer || offsetPitch[1] == 1);
    
    for (size_t fileIndex = 0; fileIndex < fileInfo.m_segmentInfoLists.size(); ++fileIndex)
    {
      auto result = chunkInfo.lowerUpperSegmentIndices.find(fileIndex);
      if (result == chunkInfo.lowerUpperSegmentIndices.end())
      {
        continue;
      }

      const auto lowerSegmentIndex = std::get<0>(result->second);
      const auto upperSegmentIndex = std::get<1>(result->second);

      const auto&
        segmentInfo = fileInfo.m_segmentInfoLists[fileIndex];
      auto&
        traceDataManager = traceDataManagers[fileIndex];

      // We loop through the segments that have primary keys inside this block and copy the traces that have secondary keys inside this block
      auto lower = segmentInfo.begin() + lowerSegmentIndex;
      auto upper = segmentInfo.begin() + upperSegmentIndex;

      for (auto segment = lower; segment != upper; ++segment)
      {
        int64_t firstTrace;
        if (fileInfo.IsUnbinned())
        {
          // For unbinned gathers the secondary key is the 1-based index of the trace, so to get the
          // first trace we convert the index to 0-based and add that to the segment's start trace.
          firstTrace = segment->m_traceStart + (chunkInfo.secondaryKeyStart - 1);
        }
        else
        {
          firstTrace = findFirstTrace(traceDataManager, *segment, chunkInfo.secondaryKeyStart, fileInfo, error);
          if (error.code)
          {
            OpenVDS::printWarning(jsonOutput, "IO", "Failed when reading data", fmt::format("{}", error.code), error.string);
            break;
          }
        }

        int
          tertiaryIndex = 0,
          currentSecondaryKey = chunkInfo.secondaryKeyStart;

        for (int64_t trace = firstTrace; trace <= segment->m_traceStop; trace++, tertiaryIndex++)
        {
          const char* header = traceDataManager.getTraceData(trace, error);
          if (error.code)
          {
            OpenVDS::printWarning(jsonOutput, "IO", "Failed when reading data", fmt::format("{}", error.code), error.string);
            break;
          }

          const void* data = header + SEGY::TraceHeaderSize;

          int primaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_primaryKey, fileInfo.m_headerEndianness),
            secondaryTest = fileInfo.IsUnbinned() ? static_cast<int>(trace - segment->m_traceStart + 1) : SEGY::ReadFieldFromHeader(header, fileInfo.m_secondaryKey, fileInfo.m_headerEndianness);

          // Check if the trace is outside the secondary range and go to the next segment if it is
          if (primaryTest == segment->m_primaryKey && secondaryTest > chunkInfo.secondaryKeyStop)
          {
            break;
          }

          if (secondaryTest != currentSecondaryKey)
          {
            // we've progressed to a new secondary key, so reset the tertiary (gather) index
            currentSecondaryKey = secondaryTest;
            tertiaryIndex = 0;
          }

          int
            primaryIndex,
            secondaryIndex;
          if (fileInfo.IsUnbinned())
          {
            primaryIndex = static_cast<int>(segment - segmentInfo.begin());
            secondaryIndex = secondaryTest - 1;
          }
          else
          {
            primaryIndex = layout->GetAxisDescriptor(PrimaryKeyDimension(fileInfo)).CoordinateToSampleIndex((float)segment->m_primaryKey);
            secondaryIndex = layout->GetAxisDescriptor(SecondaryKeyDimension(fileInfo)).CoordinateToSampleIndex((float)secondaryTest);
          }

          assert(primaryIndex >= chunkInfo.min[PrimaryKeyDimension(fileInfo)] && primaryIndex < chunkInfo.max[PrimaryKeyDimension(fileInfo)]);
          assert(secondaryIndex >= chunkInfo.min[SecondaryKeyDimension(fileInfo)] && secondaryIndex < chunkInfo.max[SecondaryKeyDimension(fileInfo)]);

          if (fileInfo.Is4D() && traceOrderByOffset)
          {
            // recalculate tertiaryIndex from header offset value
            const auto
              thisOffset = SEGY::ReadFieldFromHeader(header, g_traceHeaderFields["Offset"], fileInfo.m_headerEndianness);
            tertiaryIndex = (thisOffset - offsetStart) / offsetStep;

            // sanity check the new index
            if (tertiaryIndex < 0 || tertiaryIndex >= fold)
            {
              continue;
            }
          }

          if (fileInfo.Is4D() && (tertiaryIndex < chunkInfo.min[1] || tertiaryIndex >= chunkInfo.max[1]))
          {
            // the current gather trace number is not within the request bounds
            continue;
          }

          {
            int targetOffset;
            if (fileInfo.Is4D())
            {
              targetOffset = (primaryIndex - chunkInfo.min[3]) * amplitudePitch[3] + (secondaryIndex - chunkInfo.min[2]) * amplitudePitch[2] + (tertiaryIndex - chunkInfo.min[1]) * amplitudePitch[1];
            }
            else
            {
              targetOffset = (primaryIndex - chunkInfo.min[2]) * amplitudePitch[2] + (secondaryIndex - chunkInfo.min[1]) * amplitudePitch[1];
            }

            copySamples(data, fileInfo.m_dataSampleFormatCode, fileInfo.m_headerEndianness, &reinterpret_cast<float*>(amplitudeBuffer)[targetOffset], chunkInfo.sampleStart, chunkInfo.sampleCount);
          }

          if (traceFlagBuffer)
          {
            int targetOffset;
            if (fileInfo.Is4D())
            {
              targetOffset = (primaryIndex - chunkInfo.min[3]) * traceFlagPitch[3] + (secondaryIndex - chunkInfo.min[2]) * traceFlagPitch[2] + (tertiaryIndex - chunkInfo.min[1]) * traceFlagPitch[1];
            }
            else
            {
              targetOffset = (primaryIndex - chunkInfo.min[2]) * traceFlagPitch[2] + (secondaryIndex - chunkInfo.min[1]) * traceFlagPitch[1];
            }

            reinterpret_cast<uint8_t*>(traceFlagBuffer)[targetOffset] = true;
          }

          if (segyTraceHeaderBuffer)
          {
            int targetOffset;
            if (fileInfo.Is4D())
            {
              targetOffset = (primaryIndex - chunkInfo.min[3]) * segyTraceHeaderPitch[3] + (secondaryIndex - chunkInfo.min[2]) * segyTraceHeaderPitch[2] + (tertiaryIndex - chunkInfo.min[1]) * segyTraceHeaderPitch[1];
            }
            else
            {
              targetOffset = (primaryIndex - chunkInfo.min[2]) * segyTraceHeaderPitch[2] + (secondaryIndex - chunkInfo.min[1]) * segyTraceHeaderPitch[1];
            }

            memcpy(&reinterpret_cast<uint8_t*>(segyTraceHeaderBuffer)[targetOffset], header, SEGY::TraceHeaderSize);
          }

          if (offsetBuffer)
          {
            // offset is only applicable to 4D?
            int targetOffset;
            if (fileInfo.Is4D())
            {
              targetOffset = (primaryIndex - chunkInfo.min[3]) * offsetPitch[3] + (secondaryIndex - chunkInfo.min[2]) * offsetPitch[2] + (tertiaryIndex - chunkInfo.min[1]) * offsetPitch[1];
            }
            else
            {
              targetOffset = (primaryIndex - chunkInfo.min[2]) * offsetPitch[2] + (secondaryIndex - chunkInfo.min[1]) * offsetPitch[1];
            }

            const int
              traceOffset = SEGY::ReadFieldFromHeader(header, g_traceHeaderFields["Offset"], fileInfo.m_headerEndianness);
            reinterpret_cast<float*>(offsetBuffer)[targetOffset] = static_cast<float>(traceOffset);
          }
        }
      }
    }

    amplitudePage->Release();
    if (traceFlagPage) traceFlagPage->Release();
    if (segyTraceHeaderPage) segyTraceHeaderPage->Release();
    if (offsetPage) offsetPage->Release();
  }

  amplitudeAccessor->Commit();
  traceFlagAccessor->Commit();
  segyTraceHeaderAccessor->Commit();
  if (offsetAccessor) offsetAccessor->Commit();

  dataView.reset();
  traceDataManagers.clear();
  dataViewManagers.clear();

  if (error.code != 0)
  {
    return EXIT_FAILURE;
  }
  if (!jsonOutput)
  {
    fmt::print("\r100% done processing {}.\n", url);
  }
  //double elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start_time).count();
  //fmt::print("Elapsed time is {}.\n", elapsed / 1000);

  return EXIT_SUCCESS;
}
