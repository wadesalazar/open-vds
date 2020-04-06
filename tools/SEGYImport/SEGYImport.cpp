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

#include <SEGYUtils/SEGYFileInfo.h>
#include "IO/File.h"
#include "VDS/Hash.h"
#include <SEGYUtils/DataProvider.h>

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/KnownMetadata.h>
#include <OpenVDS/VolumeDataLayoutDescriptor.h>
#include <OpenVDS/VolumeDataAxisDescriptor.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/Range.h>
#include <OpenVDS/VolumeDataLayout.h>
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

static DataProvider CreateDataProviderFromOpenOptions(const OpenVDS::OpenOptions &openoptions, const std::string &objectId, OpenVDS::Error &error)
{
  std::unique_ptr<OpenVDS::IOManager> ioManager(OpenVDS::IOManager::CreateIOManager(openoptions, error));
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
SerializeSEGYFileInfo(SEGYFileInfo const& fileInfo)
{
  Json::Value
    jsonFileInfo;

  jsonFileInfo["persistentID"] = fmt::format("{:X}", fileInfo.m_persistentID);
  jsonFileInfo["headerEndianness"] = ToString(fileInfo.m_headerEndianness);
  jsonFileInfo["dataSampleFormatCode"] = (int)fileInfo.m_dataSampleFormatCode;
  jsonFileInfo["sampleCount"] = fileInfo.m_sampleCount;
  jsonFileInfo["sampleInterval"] = fileInfo.m_sampleIntervalMilliseconds;
  jsonFileInfo["traceCount"] = fileInfo.m_traceCount;
  jsonFileInfo["primaryKey"] = SerializeSEGYHeaderField(fileInfo.m_primaryKey);
  jsonFileInfo["secondaryKey"] = SerializeSEGYHeaderField(fileInfo.m_secondaryKey);

  Json::Value
    jsonSegmentInfoArray(Json::ValueType::arrayValue);

  for (auto const& segmentInfo : fileInfo.m_segmentInfo)
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
  catch (Json::Exception e)
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

  for (int i = 0; i < fileInfo.m_segmentInfo.size(); i++)
  {
    int64_t
      numTraces = (fileInfo.m_segmentInfo[i].m_traceStop - fileInfo.m_segmentInfo[i].m_traceStart);

    float
      multiplier = 1.5f - abs(i - (float)fileInfo.m_segmentInfo.size() / 2) / (float)fileInfo.m_segmentInfo.size(); // give 50% more importance to a segment in the middle of the dataset

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
  assert(segmentInfo.m_traceStop > segmentInfo.m_traceStart && "A valid segment info should always have a greater stop trace than the start trace");

  bool success = true;

  valueRange = OpenVDS::FloatRange(0.0f, 1.0f);
  secondaryStep = 0;
  fold = 1;

  const int traceByteSize = fileInfo.TraceByteSize();

  int64_t traceBufferStart = 0;
  int traceBufferSize = 0;
  std::unique_ptr<char[]> buffer;

  // Create min/max heaps for determining value range
  int heapSizeMax = int(((100.0f - valueRangePercentile) / 100.0f) * (segmentInfo.m_traceStop - segmentInfo.m_traceStart) * fileInfo.m_sampleCount / 2) + 1;

  std::vector<float> minHeap, maxHeap;

  minHeap.reserve(heapSizeMax);
  maxHeap.reserve(heapSizeMax);

  // Allocate sample buffer for converting samples to float
  std::unique_ptr<float[]> sampleBuffer(new float[fileInfo.m_sampleCount]);
  float* samples = sampleBuffer.get();

  // Determine fold and secondary step
  int gatherSecondaryKey = 0, gatherFold = 0, gatherSecondaryStep = 0;

  for (int64_t trace = segmentInfo.m_traceStart; trace <  segmentInfo.m_traceStop; trace++)
  {
    if(trace - traceBufferStart >= traceBufferSize)
    {
      traceBufferStart = trace;
      traceBufferSize = (segmentInfo.m_traceStop - trace) < 1000 ? int(segmentInfo.m_traceStop - trace) : 1000;

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
      fmt::print(stderr, "Warning: trace {} has a primary key that doesn't match with the segment. This trace will be ignored.", segmentInfo.m_traceStart + trace);
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
        if (minHeap.size() < heapSizeMax)
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

        if (maxHeap.size() < heapSizeMax)
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
createSEGYHeadersMetadata(DataProvider &dataProvider, OpenVDS::MetadataContainer& metadataContainer, OpenVDS::Error& error)
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

  return success;
}

void
createSurveyCoordinateSystemMetadata(SEGYFileInfo const& fileInfo, OpenVDS::MetadataContainer& metadataContainer)
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
    fileInfo.m_sampleIntervalMilliseconds = jsonFileInfo["sampleInterval"].asDouble();
    fileInfo.m_traceCount = jsonFileInfo["traceCount"].asInt64();
    fileInfo.m_primaryKey = HeaderFieldFromJson(jsonFileInfo["primaryKey"]);
    fileInfo.m_secondaryKey = HeaderFieldFromJson(jsonFileInfo["secondaryKey"]);

    for (Json::Value jsonSegmentInfo : jsonFileInfo["segmentInfo"])
    {
      fileInfo.m_segmentInfo.push_back(segmentInfoFromJson(jsonSegmentInfo));
    }
  }
  catch (Json::Exception e)
  {
    std::cerr << "Failed to parse JSON SEG-Y file info file: " << e.what();
    return false;
  }

  return true;
}

std::vector<OpenVDS::VolumeDataAxisDescriptor>
createAxisDescriptors(SEGYFileInfo const& fileInfo, int inlineStep, int crosslineStep)
{
  std::vector<OpenVDS::VolumeDataAxisDescriptor>
    axisDescriptors;

  axisDescriptors.push_back(OpenVDS::VolumeDataAxisDescriptor(fileInfo.m_sampleCount, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE, "ms", 0.0f, (fileInfo.m_sampleCount - 1) * (float)fileInfo.m_sampleIntervalMilliseconds));

  int minInline    = fileInfo.m_segmentInfo[0].m_binInfoStart.m_inlineNumber,
      minCrossline = fileInfo.m_segmentInfo[0].m_binInfoStart.m_crosslineNumber,
      maxInline    = minInline,
      maxCrossline = minCrossline;

  for (int segment = 0; segment < (int)fileInfo.m_segmentInfo.size(); segment++)
  {
    auto const &segmentInfo = fileInfo.m_segmentInfo[segment];

    minInline = std::min(minInline, segmentInfo.m_binInfoStart.m_inlineNumber);
    minInline = std::min(minInline, segmentInfo.m_binInfoStop.m_inlineNumber);
    maxInline = std::max(maxInline, segmentInfo.m_binInfoStart.m_inlineNumber);
    maxInline = std::max(maxInline, segmentInfo.m_binInfoStop.m_inlineNumber);

    minCrossline = std::min(minCrossline, segmentInfo.m_binInfoStart.m_crosslineNumber);
    minCrossline = std::min(minCrossline, segmentInfo.m_binInfoStop.m_crosslineNumber);
    maxCrossline = std::max(maxCrossline, segmentInfo.m_binInfoStart.m_crosslineNumber);
    maxCrossline = std::max(maxCrossline, segmentInfo.m_binInfoStop.m_crosslineNumber);
  }

  // Ensure the max inline/crossline is a multiple of the step size from the min
  maxCrossline += (maxCrossline - minCrossline) % crosslineStep;
  maxInline    += (maxInline    - minInline)    % inlineStep;

  int inlineCount    = 1 + (maxInline    - minInline   ) / inlineStep,
      crosslineCount = 1 + (maxCrossline - minCrossline) / crosslineStep;

  axisDescriptors.push_back(OpenVDS::VolumeDataAxisDescriptor(crosslineCount, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE, "", (float)minCrossline, (float)maxCrossline));
  axisDescriptors.push_back(OpenVDS::VolumeDataAxisDescriptor(inlineCount,    KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE,    "", (float)minInline,    (float)maxInline));

  return axisDescriptors;
}

std::vector<OpenVDS::VolumeDataChannelDescriptor>
createChannelDescriptors(SEGYFileInfo const& fileInfo, OpenVDS::FloatRange const& valueRange)
{
  std::vector<OpenVDS::VolumeDataChannelDescriptor>
    channelDescriptors;

  // Primary channel
  channelDescriptors.push_back(OpenVDS::VolumeDataChannelDescriptor(OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataChannelDescriptor::Components_1, AMPLITUDE_ATTRIBUTE_NAME, "", valueRange.Min, valueRange.Max));

  // Trace defined flag
  channelDescriptors.push_back(OpenVDS::VolumeDataChannelDescriptor(OpenVDS::VolumeDataChannelDescriptor::Format_U8, OpenVDS::VolumeDataChannelDescriptor::Components_1, "Trace", "", 0, 1, OpenVDS::VolumeDataMapping::PerTrace, OpenVDS::VolumeDataChannelDescriptor::DiscreteData));

  // SEG-Y trace headers
  channelDescriptors.push_back(OpenVDS::VolumeDataChannelDescriptor(OpenVDS::VolumeDataChannelDescriptor::Format_U8, OpenVDS::VolumeDataChannelDescriptor::Components_1, "SEGYTraceHeader", "", 0, 255, OpenVDS::VolumeDataMapping::PerTrace, SEGY::TraceHeaderSize, OpenVDS::VolumeDataChannelDescriptor::DiscreteData, 1.0f, 0.0f));

  return channelDescriptors;
}

int
findFirstTrace(int primaryKey, int secondaryKey, SEGYFileInfo const& fileInfo, const void* traceData, int traceCount, int secondaryStart, int secondaryStop)
{
  int traceStart = 0,
    traceStop = traceCount - 1;

  bool isSecondaryIncreasing = (secondaryStop >= secondaryStart);

  // Check if trace is at the start of the range or completely outside the range (this also handles cases where secondaryStart == secondaryStop which would fail to make a guess below)
  if (isSecondaryIncreasing ? (secondaryKey <= secondaryStart) :
    (secondaryKey >= secondaryStart))
  {
    return 0;
  }
  else if (isSecondaryIncreasing ? (secondaryKey > secondaryStop) :
    (secondaryKey < secondaryStop))
  {
    return traceCount;
  }

  // Make an initial guess at which trace we start on based on linear interpolation
  int trace = (int)((long long)(secondaryKey - secondaryStart) * (traceStop - traceStart) / (secondaryStop - secondaryStart));

  assert(trace >= 0 && trace < traceCount && "The guessed trace must be in [traceStart = 0, traceStop = traceCount -1]");

  while (traceStart < traceStop - 1)
  {
    const char* header = reinterpret_cast<const char*>(intptr_t(traceData) + ptrdiff_t(fileInfo.TraceByteSize()) * trace);

    int primaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_primaryKey, fileInfo.m_headerEndianness),
      secondaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_secondaryKey, fileInfo.m_headerEndianness),
      traceDelta;

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
      for (int scan = trace - 1; scan > traceStart; scan--)
      {
        header = reinterpret_cast<const char*>(intptr_t(traceData) + ptrdiff_t(fileInfo.TraceByteSize()) * scan);
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

int
findFirstTrace(int primaryKey, int secondaryKey, SEGYFileInfo const& fileInfo, const void* traceData, int traceCount)
{
  int traceStart = 0,
    traceStop = traceCount - 1;

  const char* headerStart = reinterpret_cast<const char*>(intptr_t(traceData) + ptrdiff_t(fileInfo.TraceByteSize()) * traceStart);
  const char* headerStop = reinterpret_cast<const char*>(intptr_t(traceData) + ptrdiff_t(fileInfo.TraceByteSize()) * traceStop);

  int secondaryStart = SEGY::ReadFieldFromHeader(headerStart, fileInfo.m_secondaryKey, fileInfo.m_headerEndianness);
  int secondaryStop = SEGY::ReadFieldFromHeader(headerStop, fileInfo.m_secondaryKey, fileInfo.m_headerEndianness);

  return findFirstTrace(primaryKey, secondaryKey, fileInfo, traceData, traceCount, secondaryStart, secondaryStop);
}

std::unique_ptr<OpenVDS::OpenOptions> createOpenOptions(const std::string &prefix, const std::string &persistentID,
                                                          const std::string &bucket, const std::string &region,
                                                          const std::string &container, const std::string &connectionString, int azureParallelismFactor,
                                                          OpenVDS::Error &error)
{

  // Create the VDS
  std::string key = !prefix.empty() ? prefix + "/" + persistentID : persistentID;

  std::unique_ptr<OpenVDS::OpenOptions> openOptions;

  if(!bucket.empty())
  {
    openOptions.reset(new OpenVDS::AWSOpenOptions(bucket, key, region));
  }
  else if(!container.empty())
  {
    openOptions.reset(new OpenVDS::AzureOpenOptions(connectionString, container, key));
  }

  if(azureParallelismFactor)
  {
    if(openOptions->connectionType == OpenVDS::OpenOptions::Azure)
    {
      auto &azureOpenOptions = *static_cast<OpenVDS::AzureOpenOptions *>(openOptions.get());

      azureOpenOptions.parallelism_factor = azureParallelismFactor;
    }
    else
    {
      error.code = EXIT_FAILURE;
      error.string = "Cannot specify parallelism-factor with other backends than Azure";
      openOptions.reset(nullptr);
    }
  }
  return openOptions;
}

int
main(int argc, char* argv[])
{
#if defined(WIN32)
  bool is_tty = _isatty(_fileno(stdout));
#else
  bool is_tty = isatty(fileno(stdout));
#endif
  auto start_time = std::chrono::high_resolution_clock::now();
  cxxopts::Options options("SEGYImport", "SEGYImport - A tool to scan and import a SEG-Y file to a volume data store (VDS)");
  options.positional_help("<input file>");

  std::string headerFormatFileName;
  std::string primaryKey = "InlineNumber";
  std::string secondaryKey = "CrosslineNumber";
  double scale = 0;
  bool littleEndian = false;
  bool scan = false;
  std::string fileInfoFileName;
  int brickSize;
  bool force = false;
  bool ignoreWarnings = false;
  std::string bucket;
  std::string sourceBucket;
  std::string region;
  std::string connectionString;
  std::string container;
  int azureParallelismFactor = 0;
  std::string prefix;
  std::string sourcePrefix;
  std::string persistentID;
  bool uniqueID;

  std::vector<std::string> fileNames;


  options.add_option("", "h", "header-format", "A JSON file defining the header format for the input SEG-Y file. The expected format is a dictonary of strings (field names) to pairs (byte position, field width) where field width can be \"TwoByte\" or \"FourByte\". Additionally, an \"Endianness\" key can be specified as \"BigEndian\" or \"LittleEndian\".", cxxopts::value<std::string>(headerFormatFileName), "<file>");
  options.add_option("", "p", "primary-key", "The name of the trace header field to use as the primary key.", cxxopts::value<std::string>(primaryKey)->default_value("Inline"), "<field>");
  options.add_option("", "s", "secondary-key", "The name of the trace header field to use as the secondary key.", cxxopts::value<std::string>(secondaryKey)->default_value("Crossline"), "<field>");
  options.add_option("", "", "scale", "If a scale override (floating point) is given, it is used to scale the coordinates in the header instead of determining the scale factor from the coordinate scale trace header field.", cxxopts::value<double>(scale), "<value>");
  options.add_option("", "l", "little-endian", "Force (non-standard) little-endian trace headers.", cxxopts::value<bool>(littleEndian), "");
  options.add_option("", "", "scan", "Generate a JSON file containing information about the input SEG-Y file.", cxxopts::value<bool>(scan), "");
  options.add_option("", "i", "file-info", "A JSON file (generated by the --scan option) containing information about the input SEG-Y file.", cxxopts::value<std::string>(fileInfoFileName), "<file>");
  options.add_option("", "b", "brick-size", "The brick size for the volume data store.", cxxopts::value<int>(brickSize)->default_value("64"), "<value>");
  options.add_option("", "f", "force", "Continue on upload error.", cxxopts::value<bool>(force), "");
  options.add_option("", "", "ignore-warnings", "Ignore warnings about import parameters.", cxxopts::value<bool>(ignoreWarnings), "");
  options.add_option("", "", "bucket", "AWS S3 bucket to upload to.", cxxopts::value<std::string>(bucket), "<string>");
  options.add_option("", "", "source-bucket", "AWS S3 bucket to download from.", cxxopts::value<std::string>(sourceBucket), "<string>");
  options.add_option("", "", "region", "AWS region of bucket to upload to.", cxxopts::value<std::string>(region), "<string>");
  options.add_option("", "", "connection-string", "Azure Blob Storage connection string.", cxxopts::value<std::string>(connectionString), "<string>");
  options.add_option("", "", "container", "Azure Blob Storage container to upload to.", cxxopts::value<std::string>(container), "<string>");
  options.add_option("", "", "parallelism-factor", "Azure parallelism factor.", cxxopts::value<int>(azureParallelismFactor), "<value>");
  options.add_option("", "", "prefix", "Top-level prefix to prepend to all object-keys.", cxxopts::value<std::string>(prefix), "<string>");
  options.add_option("", "", "source-prefix", "Top-level prefix to prepend to all source object-keys.", cxxopts::value<std::string>(sourcePrefix), "<string>");
  options.add_option("", "", "persistentID", "persistentID", cxxopts::value<std::string>(persistentID), "<ID>");
  options.add_option("", "", "uniqueID", "uniqueID", cxxopts::value<bool>(uniqueID), "<ID>");

  options.add_option("", "", "input", "", cxxopts::value<std::vector<std::string>>(fileNames), "");
  options.parse_positional("input");

  if (argc == 1)
  {
    std::cout << options.help();
    return EXIT_SUCCESS;
  }

  try
  {
    options.parse(argc, argv);
  }
  catch (cxxopts::OptionParseException e)
  {
    std::cerr << e.what();
    return EXIT_FAILURE;
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

  if (!scan)
  {
    if(container.empty() && bucket.empty())
    {
      std::cerr << std::string("Either an Azure Blob Storage container name or an AWS S3 bucket name must be specified");
      return EXIT_FAILURE;
    }

    if(!container.empty() && !bucket.empty())
    {
      std::cerr << std::string("Only one of Azure Blob Storage container name and AWS S3 bucket name may be specified");
      return EXIT_FAILURE;
    }
  }

  if(uniqueID && !persistentID.empty())
  {
    std::cerr << std::string("--uniqueID does not make sense when the persistentID is specified");
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

  SEGYBinInfoHeaderFields
    binInfoHeaderFields(g_traceHeaderFields["InlineNumber"], g_traceHeaderFields["CrosslineNumber"], g_traceHeaderFields["CoordinateScale"], g_traceHeaderFields["EnsembleXCoordinate"], g_traceHeaderFields["EnsembleYCoordinate"], scale);

  OpenVDS::Error
    error;

  auto sourceBucketOpenOptions = createOpenOptions(sourcePrefix, persistentID, sourceBucket, region, container, connectionString, azureParallelismFactor, error);

  DataProvider dataProvider = sourceBucket.empty() ? CreateDataProviderFromFile(fileNames[0], error) : CreateDataProviderFromOpenOptions(*sourceBucketOpenOptions, fileNames[0], error);

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

    bool success = fileInfo.Scan(dataProvider, primaryKeyHeaderField, secondaryKeyHeaderField, binInfoHeaderFields);

    if (!success)
    {
      std::cerr << std::string("Failed to scan file: ") << fileNames[0];
      return EXIT_FAILURE;
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
  }
  else
  {
    std::cerr << std::string("No SEG-Y file info file specified");
    return EXIT_FAILURE;
  }

  if (persistentID.empty())
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

  // Create axis descriptors
  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors = createAxisDescriptors(fileInfo, primaryStep, secondaryStep);

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

  createSEGYHeadersMetadata(dataProvider, metadataContainer, error);

  if (error.code != 0)
  {
    std::cerr << error.string;
    return EXIT_FAILURE;
  }

  createSurveyCoordinateSystemMetadata(fileInfo, metadataContainer);

  OpenVDS::Error
    createError;

  auto openOptions = createOpenOptions(prefix, persistentID, bucket, region, container, connectionString, azureParallelismFactor, createError);

  if (createError.code)
    fmt::print(stderr, "{}\n", createError.code);

  OpenVDS::VDSHandle
    handle = OpenVDS::Create(*openOptions.get(), layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, createError);

  if (createError.code != 0)
  {
    std::cerr << std::string("Create error: " + createError.string);
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
  fmt::print("\nImporting into PersistentID: {}\n\n", persistentID);

  struct ChunkIndex
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
    int64_t traceStart;
    int64_t traceStop;
    DataRequestInfo dataRequestInfo;
  };

  std::vector<ChunkIndex> chunkIndices;
  chunkIndices.resize(amplitudeAccessor->GetChunkCount());
  std::vector<DataRequestInfo> dataRequests;
  dataRequests.reserve(chunkIndices.capacity());
  for (int64_t chunk = 0; chunk < amplitudeAccessor->GetChunkCount(); chunk++)
  {
    auto &chunkIndex = chunkIndices[chunk];
    amplitudeAccessor->GetChunkMinMax(chunk, chunkIndex.min, chunkIndex.max);

    chunkIndex.sampleStart = chunkIndex.min[0];
    chunkIndex.sampleCount = chunkIndex.max[0] - chunkIndex.min[0];

    chunkIndex.secondaryKeyStart = (int)floorf(layout->GetAxisDescriptor(1).SampleIndexToCoordinate(chunkIndex.min[1]) + 0.5f);
    chunkIndex.secondaryKeyStop = (int)floorf(layout->GetAxisDescriptor(1).SampleIndexToCoordinate(chunkIndex.max[1] - 1) + 0.5f);

    chunkIndex.primaryKeyStart = (int)floorf(layout->GetAxisDescriptor(2).SampleIndexToCoordinate(chunkIndex.min[2]) + 0.5f);
    chunkIndex.primaryKeyStop = (int)floorf(layout->GetAxisDescriptor(2).SampleIndexToCoordinate(chunkIndex.max[2] - 1) + 0.5f);

    auto lower = std::lower_bound(fileInfo.m_segmentInfo.begin(), fileInfo.m_segmentInfo.end(), chunkIndex.primaryKeyStart, [](SEGYSegmentInfo const& segmentInfo, int primaryKey)->bool { return segmentInfo.m_primaryKey < primaryKey; });
    chunkIndex.lowerSegmentIndex = std::distance(fileInfo.m_segmentInfo.begin(), lower);
    auto upper = std::upper_bound(fileInfo.m_segmentInfo.begin(), fileInfo.m_segmentInfo.end(), chunkIndex.primaryKeyStop, [](int primaryKey, SEGYSegmentInfo const& segmentInfo)->bool { return primaryKey < segmentInfo.m_primaryKey; });
    chunkIndex.upperSegmentIndex = std::distance(fileInfo.m_segmentInfo.begin(), upper);

    chunkIndex.traceStart = lower->m_traceStart;
    chunkIndex.traceStop = std::prev(upper)->m_traceStop;

    assert(chunkIndex.traceStop > chunkIndex.traceStart);

    chunkIndex.dataRequestInfo.offset = SEGY::TextualFileHeaderSize + SEGY::BinaryFileHeaderSize + chunkIndex.traceStart * traceByteSize;
    chunkIndex.dataRequestInfo.size = (chunkIndex.traceStop - chunkIndex.traceStart + 1) * traceByteSize;
    dataRequests.push_back(chunkIndex.dataRequestInfo);
  }

  int64_t memoryAvailable = GetTotalSystemMemory();
  DataViewManager dataViewManager(dataProvider, memoryAvailable / 3, std::move(dataRequests));

  for (int64_t chunk = 0; chunk < amplitudeAccessor->GetChunkCount(); chunk++)
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

    auto &chunkIndex = chunkIndices[chunk];

    // This acquires the new file view before releasing the previous so we usually end up re-using the same file view
    dataView = dataViewManager.acquireDataView(chunkIndex.dataRequestInfo, false, error);

    if (error.code == 0)
    {
      OpenVDS::VolumeDataPage* amplitudePage = amplitudeAccessor->CreatePage(chunk);
      OpenVDS::VolumeDataPage* traceFlagPage = nullptr;
      OpenVDS::VolumeDataPage* segyTraceHeaderPage = nullptr;

      if (chunkIndex.min[0] == 0)
      {
        traceFlagPage = traceFlagAccessor->CreatePage(traceFlagAccessor->GetChunkIndex(chunkIndex.min));
        segyTraceHeaderPage = segyTraceHeaderAccessor->CreatePage(segyTraceHeaderAccessor->GetChunkIndex(chunkIndex.min));
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
      auto lower = fileInfo.m_segmentInfo.begin() + chunkIndex.lowerSegmentIndex;
      auto upper = fileInfo.m_segmentInfo.begin() + chunkIndex.upperSegmentIndex;

      for (auto segment = lower; segment != upper; ++segment)
      {
        const void* traceData = reinterpret_cast<const void*>(intptr_t(dataView->Pointer(error)) + (segment->m_traceStart - chunkIndex.traceStart) * traceByteSize);
        if (error.code)
        {
          fmt::print(stderr, "Failed when reading data");
          break;
        }
        int traceCount = int(segment->m_traceStop - segment->m_traceStart + 1);

        int firstTrace = findFirstTrace(segment->m_primaryKey, chunkIndex.secondaryKeyStart, fileInfo, traceData, traceCount);

        for (int trace = firstTrace; trace < traceCount; trace++)
        {
          const char* header = reinterpret_cast<const char*>(intptr_t(traceData) + traceByteSize * trace);
          const void* data = header + SEGY::TraceHeaderSize;

          int primaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_primaryKey, fileInfo.m_headerEndianness),
            secondaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_secondaryKey, fileInfo.m_headerEndianness);

          // Check if the trace is outside the secondary range and go to the next segment if it is
          if (primaryTest == segment->m_primaryKey && secondaryTest > chunkIndex.secondaryKeyStop)
          {
            break;
          }

          int primaryIndex = layout->GetAxisDescriptor(2).CoordinateToSampleIndex((float)segment->m_primaryKey);
          int secondaryIndex = layout->GetAxisDescriptor(1).CoordinateToSampleIndex((float)secondaryTest);

          assert(primaryIndex >= chunkIndex.min[2] && primaryIndex < chunkIndex.max[2]);
          assert(secondaryIndex >= chunkIndex.min[1] && secondaryIndex < chunkIndex.max[1]);

          {
            int targetOffset = (primaryIndex - chunkIndex.min[2]) * amplitudePitch[2] + (secondaryIndex - chunkIndex.min[1]) * amplitudePitch[1];

            copySamples(data, fileInfo.m_dataSampleFormatCode, fileInfo.m_headerEndianness, &reinterpret_cast<float*>(amplitudeBuffer)[targetOffset], chunkIndex.sampleStart, chunkIndex.sampleCount);
          }

          if (traceFlagBuffer)
          {
            int targetOffset = (primaryIndex - chunkIndex.min[2]) * traceFlagPitch[2] + (secondaryIndex - chunkIndex.min[1]) * traceFlagPitch[1];

            reinterpret_cast<uint8_t*>(traceFlagBuffer)[targetOffset] = true;
          }

          if (segyTraceHeaderBuffer)
          {
            int targetOffset = (primaryIndex - chunkIndex.min[2]) * segyTraceHeaderPitch[2] + (secondaryIndex - chunkIndex.min[1]) * segyTraceHeaderPitch[1];

            memcpy(&reinterpret_cast<uint8_t*>(segyTraceHeaderBuffer)[targetOffset], header, SEGY::TraceHeaderSize);
          }
        }
      }

      amplitudePage->Release();
      if (traceFlagPage) traceFlagPage->Release();
      if (segyTraceHeaderPage) segyTraceHeaderPage->Release();
    }
  }

  amplitudeAccessor->Commit();
  traceFlagAccessor->Commit();
  segyTraceHeaderAccessor->Commit();

  fmt::print("\r100% done processing {}.\n", persistentID);

  dataView.reset();

  double elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start_time).count();
  //fmt::print("Elapsed time is {}.\n", elapsed / 1000);

  return EXIT_SUCCESS;
}
