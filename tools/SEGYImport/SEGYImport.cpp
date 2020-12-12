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

#include <mutex>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <algorithm>

#include "cxxopts.hpp"
#include <json/json.h>
#include <fmt/format.h>

#include <chrono>

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

static DataProvider CreateDataProviderFromFile(const std::string &filename, OpenVDS::Error &error)
{
  std::unique_ptr<OpenVDS::File> file(new OpenVDS::File());
  if (!file->Open(filename, false, false, false, error))
    return DataProvider((OpenVDS::File *)nullptr);
  return DataProvider(file.release());
}

static DataProvider CreateDataProviderFromOpenOptions(const std::string &url, const std::string &connectionString, const std::string &objectId, OpenVDS::Error &error)
{
  std::unique_ptr<OpenVDS::IOManager> ioManager(OpenVDS::IOManager::CreateIOManager(url, connectionString, error));
  if (error.code)
    return DataProvider((OpenVDS::IOManager *)nullptr, "", error);
  return DataProvider(ioManager.release(), objectId, error);
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
 { "TraceSequenceNumber"     , SEGY::TraceHeader::TraceSequenceNumberHeaderField },
 { "TraceSequenceNumberWithinFile", SEGY::TraceHeader::TraceSequenceNumberWithinFileHeaderField },
 { "EnergySourcePointNumber"   , SEGY::TraceHeader::EnergySourcePointNumberHeaderField },
 { "EnsembleNumber"        , SEGY::TraceHeader::EnsembleNumberHeaderField },
 { "TraceNumberWithinEnsemble"  , SEGY::TraceHeader::TraceNumberWithinEnsembleHeaderField },
 { "TraceIdentificationCode"   , SEGY::TraceHeader::TraceIdentificationCodeHeaderField },
 { "CoordinateScale"       , SEGY::TraceHeader::CoordinateScaleHeaderField },
 { "SourceXCoordinate"      , SEGY::TraceHeader::SourceXCoordinateHeaderField },
 { "SourceYCoordinate"      , SEGY::TraceHeader::SourceYCoordinateHeaderField },
 { "GroupXCoordinate"       , SEGY::TraceHeader::GroupXCoordinateHeaderField },
 { "GroupYCoordinate"       , SEGY::TraceHeader::GroupYCoordinateHeaderField },
 { "CoordinateUnits"       , SEGY::TraceHeader::CoordinateUnitsHeaderField },
 { "StartTime"          , SEGY::TraceHeader::StartTimeHeaderField },
 { "NumSamples"          , SEGY::TraceHeader::NumSamplesHeaderField },
 { "SampleInterval"        , SEGY::TraceHeader::SampleIntervalHeaderField },
 { "EnsembleXCoordinate"     , SEGY::TraceHeader::EnsembleXCoordinateHeaderField },
 { "EnsembleYCoordinate"     , SEGY::TraceHeader::EnsembleYCoordinateHeaderField },
 { "InlineNumber"         , SEGY::TraceHeader::InlineNumberHeaderField },
 { "CrosslineNumber"       , SEGY::TraceHeader::CrosslineNumberHeaderField }
};

std::map<std::string, std::string>
g_aliases =
{
 { "Inline",       "InlineNumber" },
 { "InLine",       "InlineNumber" },
 { "InLineNumber",    "InlineNumber" },
 { "Crossline",      "CrosslineNumber" },
 { "CrossLine",      "CrosslineNumber" },
 { "CrossLineNumber",   "CrosslineNumber" },
 { "Easting",       "EnsembleXCoordinate" },
 { "Northing",      "EnsembleYCoordinate" },
 { "CDPXCoordinate",   "EnsembleXCoordinate" },
 { "CDPYCoordinate",   "EnsembleYCoordinate" },
 { "CDP-X",        "EnsembleXCoordinate" },
 { "CDP-Y",        "EnsembleYCoordinate" },
 { "Source-X",      "SourceXCoordinate" },
 { "Source-Y",      "SourceYCoordinate" },
 { "Group-X",       "GroupXCoordinate" },
 { "Group-Y",       "GroupYCoordinate" },
 { "ReceiverXCoordinate", "GroupXCoordinate" },
 { "ReceiverYCoordinate", "GroupYCoordinate" },
 { "Receiver-X",     "GroupXCoordinate" },
 { "Receiver-Y",     "GroupYCoordinate" },
 { "Scalar",       "CoordinateScale" }
};

void
ResolveAlias(std::string& fieldName)
{
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

  if (endiannessString == "BigEndian")
  {
    return SEGY::Endianness::BigEndian;
  }
  else if (endiannessString == "LittleEndian")
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

  if (fieldWidthString == "TwoByte")
  {
    return SEGY::FieldWidth::TwoByte;
  }
  else if (fieldWidthString == "FourByte")
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
ParseHeaderFormatFile(OpenVDS::File const& file, std::map<std::string, SEGY::HeaderField>& traceHeaderFields, SEGY::Endianness& headerEndianness, OpenVDS::Error &error)
{
  int64_t fileSize = file.Size(error);

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

  file.Read(buffer.get(), 0, (int32_t)fileSize, error);

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
      success = reader->parse(buffer.get(), buffer.get() + fileSize, &root, &errs);

    if (!success)
    {
      throw Json::Exception(errs);
    }

    for (std::string const& fieldName : root.getMemberNames())
    {
      std::string canonicalFieldName = fieldName;
      ResolveAlias(canonicalFieldName);

      if (fieldName == "Endianness")
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

SEGYSegmentInfo const&
findRepresentativeSegment(SEGYFileInfo const& fileInfo, int& primaryStep)
{
  primaryStep = 0;

  float bestScore = 0.0f;
  int bestIndex = 0;

  int segmentPrimaryStep = 0;

  for (int i = 0; i < int(fileInfo.m_segmentInfo.size()); i++)
  {
    int64_t
      numTraces = (fileInfo.m_segmentInfo[i].m_traceStop - fileInfo.m_segmentInfo[i].m_traceStart + 1);

    float
      multiplier = 1.5f - std::abs(i - (float)fileInfo.m_segmentInfo.size() / 2) / (float)fileInfo.m_segmentInfo.size(); // give 50% more importance to a segment in the middle of the dataset

    float
      score = float(numTraces) * multiplier;

    if (score > bestScore)
    {
      bestScore = score;
      bestIndex = i;
    }

    // Updating the primary step with the step for the previous segment intentionally ignores the step of the last segment since it can be anomalous
    if(segmentPrimaryStep && (!primaryStep || std::abs(segmentPrimaryStep) < std::abs(primaryStep)))
    {
      primaryStep = segmentPrimaryStep;
    }

    if(i > 0)
    {
      segmentPrimaryStep = fileInfo.m_segmentInfo[i].m_primaryKey - fileInfo.m_segmentInfo[i - 1].m_primaryKey;
    }
  }

  // If the primary step couldn't be determined, set it to the last step or 1
  primaryStep = primaryStep ? primaryStep : std::max(segmentPrimaryStep, 1);

  return fileInfo.m_segmentInfo[bestIndex];
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
analyzeSegment(DataProvider &dataProvider, SEGYFileInfo const& fileInfo, SEGYSegmentInfo const& segmentInfo, float valueRangePercentile, OpenVDS::FloatRange& valueRange, int& fold, int& secondaryStep, OpenVDS::Error& error)
{
  assert(segmentInfo.m_traceStop >= segmentInfo.m_traceStart && "A valid segment info should always have a stop trace greater or equal to the start trace");

  bool success = true;

  valueRange = OpenVDS::FloatRange(0.0f, 1.0f);
  secondaryStep = 0;
  fold = 1;

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
    int traceSecondaryKey = SEGY::ReadFieldFromHeader(header, fileInfo.m_secondaryKey, fileInfo.m_headerEndianness);

    if(tracePrimaryKey != segmentInfo.m_primaryKey)
    {
      fmt::print(stderr, "Warning: trace {} has a primary key that doesn't match with the segment. This trace will be ignored.\n", segmentInfo.m_traceStart + trace);
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
  if (fileInfo.m_segmentInfo.empty()) return;

  double inlineSpacing[2] = { 0, 0 };
  double crosslineSpacing[2] = { 0, 0 };

  // Determine crossline spacing
  int countedCrosslineSpacings = 0;

  for (auto const& segmentInfo : fileInfo.m_segmentInfo)
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
  SEGYSegmentInfo const& firstSegmentInfo = fileInfo.m_segmentInfo.front();
  SEGYSegmentInfo const& lastSegmentInfo = fileInfo.m_segmentInfo.back();

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
createImportInformationMetadata(DataProvider &dataProvider, OpenVDS::MetadataContainer& metadataContainer, OpenVDS::Error &error)
{
  auto now = std::chrono::system_clock::now();
  std::time_t tt = std::chrono::system_clock::to_time_t(now);
  std::tm tm = *std::gmtime(&tt);
  auto duration = now.time_since_epoch();
  int millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

  std::string importTimeStamp = fmt::format("{:04d}-{:02d}-{:02d}T{:02d}:{:02d}:{:02d}.{:03d}Z", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, millis);

  std::string inputFileName = dataProvider.FileOrObjectName();

  // Strip the path from the file/object name
  const char pathSeparators[] = { '/', '\\', ':' };
  for(auto pathSeparator : pathSeparators)
  {
    size_t pos = inputFileName.rfind(pathSeparator);
    if(pos != std::string::npos) inputFileName = inputFileName.substr(pos + 1);
  }

  // In lack of a better displayName we use the file name
  std::string displayName = inputFileName;

  std::string inputTimeStamp = dataProvider.LastWriteTime(error);
  if (error.code != 0)
  {
    return false;
  }

  int64_t inputFileSize = dataProvider.Size(error);
  if (error.code != 0)
  {
    return false;
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
parseSEGYFileInfoFile(OpenVDS::File const& file, SEGYFileInfo& fileInfo)
{
  OpenVDS::Error error;

  int64_t fileSize = file.Size(error);

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

  file.Read(buffer.get(), 0, (int32_t)fileSize, error);

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
      throw Json::Exception(errs);
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
    std::cerr << "Failed to parse JSON SEG-Y file info file: " << e.what();
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
    //case SEGY::SEGYType::UnbinnedReceiver:
    //  axisName = VDS_DIMENSION_RECEIVER_NAME;
    //  break;

    //case SEGY::SEGYType::UnbinnedShot:
    //  axisName = VDS_DIMENSION_SHOT_NAME;
    //  break;

    //case SEGY::SEGYType::UnbinnedCDP:
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

  OffsetChannelInfo(bool has, int start, int end, int step) : hasOffset(has), offsetStart(start), offsetEnd(end), offsetStep(step) {}
};

std::vector<OpenVDS::VolumeDataChannelDescriptor>
createChannelDescriptors(SEGYFileInfo const& fileInfo, OpenVDS::FloatRange const& valueRange, const OffsetChannelInfo& offsetInfo)
{
  std::vector<OpenVDS::VolumeDataChannelDescriptor>
    channelDescriptors;

  // Primary channel
  channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataChannelDescriptor::Components_1, AMPLITUDE_ATTRIBUTE_NAME, "", valueRange.Min, valueRange.Max);

  // Trace defined flag
  channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_U8, OpenVDS::VolumeDataChannelDescriptor::Components_1, "Trace", "", 0, 1, OpenVDS::VolumeDataMapping::PerTrace, OpenVDS::VolumeDataChannelDescriptor::DiscreteData);

  // SEG-Y trace headers
  channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_U8, OpenVDS::VolumeDataChannelDescriptor::Components_1, "SEGYTraceHeader", "", 0, 255, OpenVDS::VolumeDataMapping::PerTrace, SEGY::TraceHeaderSize, OpenVDS::VolumeDataChannelDescriptor::DiscreteData, 1.0f, 0.0f);

  if (offsetInfo.hasOffset)
  {
    // offset channel
    channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataChannelDescriptor::Components_1, "Offset", KNOWNMETADATA_UNIT_UNITLESS, static_cast<float>(offsetInfo.offsetStart), static_cast<float>(offsetInfo.offsetEnd),OpenVDS::VolumeDataMapping::PerTrace, OpenVDS::VolumeDataChannelDescriptor::NoLossyCompression);

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

static DataProvider CreateDataProvider(const std::string &fileName,
                                       const std::string &url, const std::string &connection,
                                       OpenVDS::Error &error)
{
  if (url.empty())
  {
    error = OpenVDS::Error();
    return CreateDataProviderFromFile(fileName, error);
  }
  return CreateDataProviderFromOpenOptions(url, connection, fileName, error);
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
  std::string primaryKey = "InlineNumber";
  std::string secondaryKey = "CrosslineNumber";
  std::string sampleUnit;
  std::string crsWkt;
  double scale = 0;
  bool overrideSampleStart = false;
  double sampleStart = 0;
  bool littleEndian = false;
  bool scan = false;
  std::string fileInfoFileName;
  int brickSize;
  bool force = false;
  bool ignoreWarnings = false;
  std::string url;
  std::string connection;
  std::string vdsFileName;
  std::string sourceUrl;
  std::string sourceConnection;
  std::string persistentID;
  bool uniqueID = false;
  bool disablePersistentID = false;
  bool help = false;

  std::vector<std::string> fileNames;

  options.add_option("", "", "header-format", "A JSON file defining the header format for the input SEG-Y file. The expected format is a dictonary of strings (field names) to pairs (byte position, field width) where field width can be \"TwoByte\" or \"FourByte\". Additionally, an \"Endianness\" key can be specified as \"BigEndian\" or \"LittleEndian\".", cxxopts::value<std::string>(headerFormatFileName), "<file>");
  options.add_option("", "p", "primary-key", "The name of the trace header field to use as the primary key.", cxxopts::value<std::string>(primaryKey)->default_value("Inline"), "<field>");
  options.add_option("", "s", "secondary-key", "The name of the trace header field to use as the secondary key.", cxxopts::value<std::string>(secondaryKey)->default_value("Crossline"), "<field>");
  options.add_option("", "", "scale", "If a scale override (floating point) is given, it is used to scale the coordinates in the header instead of determining the scale factor from the coordinate scale trace header field.", cxxopts::value<double>(scale), "<value>");
  options.add_option("", "", "sample-unit", "A sample unit of 'ms' is used for datasets in the time domain (default), while a sample unit of 'm' or 'ft' is used for datasets in the depth domain", cxxopts::value<std::string>(sampleUnit), "<string>");
  options.add_option("", "", "sample-start", "The start time/depth/frequency (depending on the domain) of the sampling", cxxopts::value<double>(sampleStart), "<value>");
  options.add_option("", "", "crs-wkt", "A coordinate reference system in well-known text format can optionally be provided", cxxopts::value<std::string>(crsWkt), "<string>");
  options.add_option("", "l", "little-endian", "Force little-endian trace headers.", cxxopts::value<bool>(littleEndian), "");
  options.add_option("", "", "scan", "Generate a JSON file containing information about the input SEG-Y file.", cxxopts::value<bool>(scan), "");
  options.add_option("", "i", "file-info", "A JSON file (generated by the --scan option) containing information about the input SEG-Y file.", cxxopts::value<std::string>(fileInfoFileName), "<file>");
  options.add_option("", "b", "brick-size", "The brick size for the volume data store.", cxxopts::value<int>(brickSize)->default_value("64"), "<value>");
  options.add_option("", "f", "force", "Continue on upload error.", cxxopts::value<bool>(force), "");
  options.add_option("", "", "ignore-warnings", "Ignore warnings about import parameters.", cxxopts::value<bool>(ignoreWarnings), "");
  options.add_option("", "", "url", "Url with cloud vendor scheme used for target location.", cxxopts::value<std::string>(url), "<string>");
  options.add_option("", "", "connection", "Connection string used for additional parameters to the target connection", cxxopts::value<std::string>(connection), "<string>");
  options.add_option("", "", "vdsfile", "File name of output VDS file.", cxxopts::value<std::string>(vdsFileName), "<string>");
  options.add_option("", "", "source-url", "Url with cloud vendor scheme used for source location.", cxxopts::value<std::string>(sourceUrl), "<string>");
  options.add_option("", "", "source-connection", "Connection string used for additional parameters to the source connection", cxxopts::value<std::string>(sourceConnection), "<string>");
  options.add_option("", "", "persistentID", "A globally unique ID for the VDS, usually an 8-digit hexadecimal number.", cxxopts::value<std::string>(persistentID), "<ID>");
  options.add_option("", "", "uniqueID", "Generate a new globally unique ID when scanning the input SEG-Y file.", cxxopts::value<bool>(uniqueID), "");
  options.add_option("", "", "disable-persistentID", "Disable the persistentID usage, placing the VDS directly into the url location.", cxxopts::value<bool>(disablePersistentID), "");

  options.add_option("", "h", "help", "Print this help information", cxxopts::value<bool>(help), "");

  options.add_option("", "", "input", "", cxxopts::value<std::vector<std::string>>(fileNames), "");
  options.parse_positional("input");

  if (argc == 1)
  {
    std::cout << options.help();
    return EXIT_SUCCESS;
  }

  try
  {
    auto result = options.parse(argc, argv);
    overrideSampleStart = result.count("sample-start") != 0;
  }
  catch (cxxopts::OptionParseException &e)
  {
    std::cerr << e.what();
    return EXIT_FAILURE;
  }

  if (help)
  {
    std::cout << options.help();
    return EXIT_SUCCESS;
  }

  if (fileNames.empty())
  {
    std::cerr << std::string("No input SEG-Y file specified");
    return EXIT_FAILURE;
  }

  if (fileNames.size() > 1)
  {
    std::cerr << std::string("Only one input SEG-Y file may be specified");
    return EXIT_FAILURE;
  }

  if(uniqueID && !persistentID.empty())
  {
    std::cerr << std::string("--uniqueID does not make sense when the persistentID is specified");
    return EXIT_FAILURE;
  }
  
  if(disablePersistentID && !persistentID.empty())
  {
    std::cerr << std::string("--disable-PersistentID does not make sense when the persistentID is specified");
    return EXIT_FAILURE;
  }

  SEGY::Endianness headerEndianness = (littleEndian ? SEGY::Endianness::LittleEndian : SEGY::Endianness::BigEndian);

  if (!headerFormatFileName.empty())
  {
    OpenVDS::File
      headerFormatFile;

    OpenVDS::Error
      error;

    headerFormatFile.Open(headerFormatFileName.c_str(), false, false, false, error);

    if (error.code != 0)
    {
      std::cerr << std::string("Could not open file: ") << headerFormatFileName;
      return EXIT_FAILURE;
    }

    ParseHeaderFormatFile(headerFormatFile, g_traceHeaderFields, headerEndianness, error);

    if (error.code != 0)
    {
      fmt::print(stderr, "Could not read header format file {}: {}\n", headerFormatFileName, error.string);
      return EXIT_FAILURE;
    }
  }

  // get the canonical field name for the primary and secondary key
  ResolveAlias(primaryKey);
  ResolveAlias(secondaryKey);

  SEGY::HeaderField
    primaryKeyHeaderField,
    secondaryKeyHeaderField;

  if (g_traceHeaderFields.find(primaryKey) != g_traceHeaderFields.end())
  {
    primaryKeyHeaderField = g_traceHeaderFields[primaryKey];
  }
  else
  {
    std::cerr << std::string("Unrecognized header field given for primary key: ") << primaryKey;
    return EXIT_FAILURE;
  }

  if (g_traceHeaderFields.find(secondaryKey) != g_traceHeaderFields.end())
  {
    secondaryKeyHeaderField = g_traceHeaderFields[secondaryKey];
  }

  SEGY::HeaderField
    startTimeHeaderField = g_traceHeaderFields["StartTime"];

  SEGYBinInfoHeaderFields
    binInfoHeaderFields(g_traceHeaderFields["InlineNumber"], g_traceHeaderFields["CrosslineNumber"], g_traceHeaderFields["CoordinateScale"], g_traceHeaderFields["EnsembleXCoordinate"], g_traceHeaderFields["EnsembleYCoordinate"], scale);

  OpenVDS::Error
    error;

  DataProvider dataProvider = CreateDataProvider(fileNames[0], sourceUrl, sourceConnection, error);
  if (error.code != 0)
  {
    fmt::print(stderr, "Could not open: {} - {}\n", fileNames[0], error.string);
    return EXIT_FAILURE;
  }

  SEGYFileInfo
    fileInfo(headerEndianness);

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

    bool success = fileInfo.Scan(dataProvider, primaryKeyHeaderField, secondaryKeyHeaderField, startTimeHeaderField, binInfoHeaderFields);

    if (!success)
    {
      std::cerr << std::string("Failed to scan file: ") << fileNames[0];
      return EXIT_FAILURE;
    }

    if(overrideSampleStart)
    {
      fileInfo.m_startTimeMilliseconds = sampleStart;
    }

    // If we are in scan mode we serialize the result of the file scan either to a fileInfo file (if specified) or to stdout and exit
    if(scan)
    {
      Json::Value jsonFileInfo = SerializeSEGYFileInfo(fileInfo);

      Json::StreamWriterBuilder wbuilder;
      wbuilder["indentation"] = "  ";
      std::string document = Json::writeString(wbuilder, jsonFileInfo);

      if (fileInfoFileName.empty())
      {
        std::cout << document;
      }
      else
      {
        OpenVDS::File
          fileInfoFile;

        OpenVDS::Error
          error;

        fileInfoFile.Open(fileInfoFileName.c_str(), true, false, true, error);

        if (error.code != 0)
        {
          std::cerr << std::string("Could not create file: ") << fileInfoFileName;
          return EXIT_FAILURE;
        }

        fileInfoFile.Write(document.data(), 0, (int32_t)document.size(), error);

        if (error.code != 0)
        {
          std::cerr << std::string("Could not write to file: ") << fileInfoFileName;
          return EXIT_FAILURE;
        }
      }
      return EXIT_SUCCESS;
    }
  }
  else if (!fileInfoFileName.empty())
  {
    OpenVDS::File
      fileInfoFile;

    OpenVDS::Error
      error;

    fileInfoFile.Open(fileInfoFileName.c_str(), false, false, false, error);

    if (error.code != 0)
    {
      std::cerr << std::string("Could not open file: ") << fileInfoFileName;
      return EXIT_FAILURE;
    }

    bool success = parseSEGYFileInfoFile(fileInfoFile, fileInfo);

    if (!success)
    {
      return EXIT_FAILURE;
    }

    if(overrideSampleStart)
    {
      fileInfo.m_startTimeMilliseconds = sampleStart;
    }
  }
  else
  {
    std::cerr << std::string("No SEG-Y file info file specified");
    return EXIT_FAILURE;
  }

  if (persistentID.empty() && !disablePersistentID)
  {
    persistentID = fmt::format("{:X}", fileInfo.m_persistentID);
  }

  // Check for only a single segment

  if(fileInfo.m_segmentInfo.size() == 1)
  {
    fmt::print(stderr, "Warning: There is only one segment, either this is (as of now unsupported) 2D data or this usually indicates using the wrong header format for the input dataset.\n");
    if(!ignoreWarnings)
    {
      fmt::print(stderr, "Use --ignore-warnings to force the import to go ahead.\n");
      return EXIT_FAILURE;
    }
  }

  // Determine value range, fold and primary/secondary step

  OpenVDS::FloatRange valueRange;
  int fold = 1, primaryStep = 1, secondaryStep = 1;

  analyzeSegment(dataProvider, fileInfo, findRepresentativeSegment(fileInfo, primaryStep), 99.9f, valueRange, fold, secondaryStep, error);

  if (error.code != 0)
  {
    std::cerr << error.string;
    return EXIT_FAILURE;
  }

  if(fold > 1)
  {
    fmt::print(stderr, "Error: Detected a fold of {}, either this is (as of now unsupported) prestack data or this usually indicates using the wrong header format for the input dataset.\n", fold);
    return EXIT_FAILURE;
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
    std::cerr << std::string("Illegal brick size (must be 32, 64, 128 or 256)");
    return EXIT_FAILURE;
  }

  OpenVDS::VolumeDataLayoutDescriptor::Options layoutOptions = OpenVDS::VolumeDataLayoutDescriptor::Options_None;
  OpenVDS::VolumeDataLayoutDescriptor::LODLevels lodLevels = OpenVDS::VolumeDataLayoutDescriptor::LODLevels_None;

  int negativeMargin = 0;
  int positiveMargin = 0;
  int brickSize2DMultiplier = 4;

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
    fmt::print(stderr, "Unknown sample unit: {}, legal units are 'ms', 'm' or 'ft'\n", sampleUnit);
    return EXIT_FAILURE;
  }

  // Create axis descriptors
  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors = createAxisDescriptors(fileInfo, sampleUnits, primaryStep, secondaryStep);

  // Check for excess of empty traces

  int64_t
    traceCountInVDS = 1;

  for(int axis = 1; axis < (int)axisDescriptors.size(); axis++)
  {
    traceCountInVDS *= axisDescriptors[axis].GetNumSamples();
  }

  if(traceCountInVDS >= fileInfo.m_traceCount * 2)
  {
    fmt::print(stderr, "Warning: There is more than {:.1f}% empty traces in the VDS, this usually indicates using the wrong header format for the input dataset.\n", double(traceCountInVDS - fileInfo.m_traceCount) * 100.0 / double(traceCountInVDS));
    if(!ignoreWarnings)
    {
      fmt::print(stderr, "Use --ignore-warnings to force the import to go ahead.\n");
      return EXIT_FAILURE;
    }
  }

  // Create channel descriptors
  std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors = createChannelDescriptors(fileInfo, valueRange);

  // Create metadata
  OpenVDS::MetadataContainer
    metadataContainer;

  createImportInformationMetadata(dataProvider, metadataContainer, error);

  if (error.code != 0)
  {
    std::cerr << error.string;
    return EXIT_FAILURE;
  }

  SEGY::BinaryHeader::MeasurementSystem
    measurementSystem;

  createSEGYMetadata(dataProvider, fileInfo, metadataContainer, measurementSystem, error);

  if (error.code != 0)
  {
    std::cerr << error.string;
    return EXIT_FAILURE;
  }

  createSurveyCoordinateSystemMetadata(fileInfo, measurementSystem, crsWkt, metadataContainer);

  OpenVDS::Error createError;

  if (!persistentID.empty())
  {
    if (!url.empty() && url.back() != '/')
    {
      url.push_back('/');
    }
    url.insert(url.end(), persistentID.begin(), persistentID.end());
  }

  OpenVDS::VDSHandle handle;

  if(vdsFileName.empty())
  {
    handle = OpenVDS::Create(url, connection, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, createError);
  }
  else
  {
    handle = OpenVDS::Create(OpenVDS::VDSFileOpenOptions(vdsFileName), layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, createError);
  }

  if (createError.code != 0)
  {
    fmt::print(stderr, "Could not create VDS: {}\n", createError.string);
    return EXIT_FAILURE;
  }

  // auto-close vds handle when it goes out of scope
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> vdsGuard(handle, &OpenVDS::Close);

  auto accessManager = OpenVDS::GetAccessManager(handle);
  auto layout = accessManager->GetVolumeDataLayout();

  auto amplitudeAccessor = accessManager->CreateVolumeDataPageAccessor(accessManager->GetVolumeDataLayout(), OpenVDS::DimensionsND::Dimensions_012, 0, 0, 8, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  auto traceFlagAccessor = accessManager->CreateVolumeDataPageAccessor(accessManager->GetVolumeDataLayout(), OpenVDS::DimensionsND::Dimensions_012, 0, 1, 8, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  auto segyTraceHeaderAccessor = accessManager->CreateVolumeDataPageAccessor(accessManager->GetVolumeDataLayout(), OpenVDS::DimensionsND::Dimensions_012, 0, 2, 8, OpenVDS::VolumeDataAccessManager::AccessMode_Create);

  int64_t traceByteSize = fileInfo.TraceByteSize();

  std::shared_ptr<DataView> dataView;

  int percentage = -1;
  fmt::print("\nImporting into: {}\n\n", url);

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
    size_t lowerSegmentIndex;
    size_t upperSegmentIndex;
    //int64_t traceStart;
    //int64_t traceStop;
    //DataRequestInfo dataRequestInfo;
  };

  // limit DataViewManager's memory use to 1.5 sets of brick inlines
  const int64_t dvmMemoryLimit = 3LL * brickSize * axisDescriptors[1].GetNumSamples() * fileInfo.TraceByteSize() / 2LL;

  DataViewManager dataViewManager(dataProvider, dvmMemoryLimit);

  TraceDataManager traceDataManager(dataViewManager, 128, traceByteSize, fileInfo.m_traceCount);

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

    chunkInfo.secondaryKeyStart = (int)floorf(layout->GetAxisDescriptor(1).SampleIndexToCoordinate(chunkInfo.min[1]) + 0.5f);
    chunkInfo.secondaryKeyStop = (int)floorf(layout->GetAxisDescriptor(1).SampleIndexToCoordinate(chunkInfo.max[1] - 1) + 0.5f);

    chunkInfo.primaryKeyStart = (int)floorf(layout->GetAxisDescriptor(2).SampleIndexToCoordinate(chunkInfo.min[2]) + 0.5f);
    chunkInfo.primaryKeyStop = (int)floorf(layout->GetAxisDescriptor(2).SampleIndexToCoordinate(chunkInfo.max[2] - 1) + 0.5f);

    auto lower = std::lower_bound(fileInfo.m_segmentInfo.begin(), fileInfo.m_segmentInfo.end(), chunkInfo.primaryKeyStart, [](SEGYSegmentInfo const& segmentInfo, int primaryKey)->bool { return segmentInfo.m_primaryKey < primaryKey; });
    chunkInfo.lowerSegmentIndex = std::distance(fileInfo.m_segmentInfo.begin(), lower);
    auto upper = std::upper_bound(fileInfo.m_segmentInfo.begin(), fileInfo.m_segmentInfo.end(), chunkInfo.primaryKeyStop, [](int primaryKey, SEGYSegmentInfo const& segmentInfo)->bool { return primaryKey < segmentInfo.m_primaryKey; });
    chunkInfo.upperSegmentIndex = std::distance(fileInfo.m_segmentInfo.begin(), upper);

    //chunkInfo.traceStart = lower->m_traceStart;
    //chunkInfo.traceStop = std::prev(upper)->m_traceStop;

    //assert(chunkInfo.traceStop > chunkInfo.traceStart);

    //chunkInfo.dataRequestInfo.offset = SEGY::TextualFileHeaderSize + SEGY::BinaryFileHeaderSize + chunkInfo.traceStart * traceByteSize;
    //chunkInfo.dataRequestInfo.size = (chunkInfo.traceStop - chunkInfo.traceStart + 1) * traceByteSize;
    //dataRequests.push_back(chunkInfo.dataRequestInfo);

    traceDataManager.addDataRequests(chunkInfo.secondaryKeyStart, chunkInfo.secondaryKeyStop, lower, upper);
  }

  for (int64_t chunk = 0; chunk < amplitudeAccessor->GetChunkCount() && error.code == 0; chunk++)
  {
    int new_percentage = int(double(chunk) / amplitudeAccessor->GetChunkCount() * 100);
    if (is_tty && percentage != new_percentage)
    {
      percentage = new_percentage;
      fmt::print(stdout, "\r {:3}% Done. ", percentage);
      fflush(stdout);
    }
    int32_t errorCount = accessManager->UploadErrorCount();
    for (int i = 0; i < errorCount; i++)
    {
      const char* object_id;
      int32_t error_code;
      const char* error_string;
      accessManager->GetCurrentUploadError(&object_id, &error_code, &error_string);
      fprintf(stderr, "\nFailed to upload object: %s. Error code %d: %s\n", object_id, error_code, error_string);
    }
    if (errorCount && !force)
    {
      return EXIT_FAILURE;
    }

    auto &chunkInfo = chunkInfos[chunk];

    // if we've crossed to a new inline then trim the trace page cache
    if (chunk > 0 && chunkInfo.lowerSegmentIndex > chunkInfos[chunk - 1].lowerSegmentIndex)
    {
      // we've progressed to a new set of inlines; remove earlier pages from the cache
      traceDataManager.retirePagesBefore(fileInfo.m_segmentInfo[chunkInfo.lowerSegmentIndex].m_traceStart);
    }

    OpenVDS::VolumeDataPage* amplitudePage = amplitudeAccessor->CreatePage(chunk);
    OpenVDS::VolumeDataPage* traceFlagPage = nullptr;
    OpenVDS::VolumeDataPage* segyTraceHeaderPage = nullptr;

    if (chunkInfo.min[0] == 0)
    {
      traceFlagPage = traceFlagAccessor->CreatePage(traceFlagAccessor->GetChunkIndex(chunkInfo.min));
      segyTraceHeaderPage = segyTraceHeaderAccessor->CreatePage(segyTraceHeaderAccessor->GetChunkIndex(chunkInfo.min));
    }

    int amplitudePitch[OpenVDS::Dimensionality_Max];
    int traceFlagPitch[OpenVDS::Dimensionality_Max];
    int segyTraceHeaderPitch[OpenVDS::Dimensionality_Max];

    void* amplitudeBuffer = amplitudePage->GetWritableBuffer(amplitudePitch);
    void* traceFlagBuffer = traceFlagPage ? traceFlagPage->GetWritableBuffer(traceFlagPitch) : nullptr;
    void* segyTraceHeaderBuffer = segyTraceHeaderPage ? segyTraceHeaderPage->GetWritableBuffer(segyTraceHeaderPitch) : nullptr;

    assert(amplitudePitch[0] == 1);
    assert(!traceFlagBuffer || traceFlagPitch[1] == 1);
    assert(!segyTraceHeaderBuffer || segyTraceHeaderPitch[1] == SEGY::TraceHeaderSize);

    // We loop through the segments that have primary keys inside this block and copy the traces that have secondary keys inside this block
    auto lower = fileInfo.m_segmentInfo.begin() + chunkInfo.lowerSegmentIndex;
    auto upper = fileInfo.m_segmentInfo.begin() + chunkInfo.upperSegmentIndex;

    for (auto segment = lower; segment != upper; ++segment)
    {
      int64_t firstTrace = findFirstTrace(traceDataManager, *segment, chunkInfo.secondaryKeyStart, fileInfo, error);
      if (error.code)
      {
        fmt::print(stderr, "Failed when reading data: {} - {}", error.code, error.string);
        break;
      }

      for (int64_t trace = firstTrace; trace <= segment->m_traceStop; trace++)
      {
        const char* header = traceDataManager.getTraceData(trace, error);
        if (error.code)
        {
          fmt::print(stderr, "Failed when reading data: {} - {}\n", error.code, error.string);
          break;
        }

        const void* data = header + SEGY::TraceHeaderSize;

        int primaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_primaryKey, fileInfo.m_headerEndianness),
          secondaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_secondaryKey, fileInfo.m_headerEndianness);

        // Check if the trace is outside the secondary range and go to the next segment if it is
        if (primaryTest == segment->m_primaryKey && secondaryTest > chunkInfo.secondaryKeyStop)
        {
          break;
        }

        int primaryIndex = layout->GetAxisDescriptor(2).CoordinateToSampleIndex((float)segment->m_primaryKey);
        int secondaryIndex = layout->GetAxisDescriptor(1).CoordinateToSampleIndex((float)secondaryTest);

        assert(primaryIndex >= chunkInfo.min[2] && primaryIndex < chunkInfo.max[2]);
        assert(secondaryIndex >= chunkInfo.min[1] && secondaryIndex < chunkInfo.max[1]);

        {
          int targetOffset = (primaryIndex - chunkInfo.min[2]) * amplitudePitch[2] + (secondaryIndex - chunkInfo.min[1]) * amplitudePitch[1];

          copySamples(data, fileInfo.m_dataSampleFormatCode, fileInfo.m_headerEndianness, &reinterpret_cast<float*>(amplitudeBuffer)[targetOffset], chunkInfo.sampleStart, chunkInfo.sampleCount);
        }

        if (traceFlagBuffer)
        {
          int targetOffset = (primaryIndex - chunkInfo.min[2]) * traceFlagPitch[2] + (secondaryIndex - chunkInfo.min[1]) * traceFlagPitch[1];

          reinterpret_cast<uint8_t*>(traceFlagBuffer)[targetOffset] = true;
        }

        if (segyTraceHeaderBuffer)
        {
          int targetOffset = (primaryIndex - chunkInfo.min[2]) * segyTraceHeaderPitch[2] + (secondaryIndex - chunkInfo.min[1]) * segyTraceHeaderPitch[1];

          memcpy(&reinterpret_cast<uint8_t*>(segyTraceHeaderBuffer)[targetOffset], header, SEGY::TraceHeaderSize);
        }
      }
    }

    amplitudePage->Release();
    if (traceFlagPage) traceFlagPage->Release();
    if (segyTraceHeaderPage) segyTraceHeaderPage->Release();
  }

  amplitudeAccessor->Commit();
  traceFlagAccessor->Commit();
  segyTraceHeaderAccessor->Commit();

  dataView.reset();

  if (error.code != 0)
  {
    return EXIT_FAILURE;
  }
  fmt::print("\r100% done processing {}.\n", url);
  //double elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start_time).count();
  //fmt::print("Elapsed time is {}.\n", elapsed / 1000);

  return EXIT_SUCCESS;
}
