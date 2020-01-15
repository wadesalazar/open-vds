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

#include "SEGYFileInfo.h"
#include "IO/File.h"
#include "VDS/Hash.h"

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
ParseHeaderFormatFile(OpenVDS::File const& file, std::map<std::string, SEGY::HeaderField>& traceHeaderFields, SEGY::Endianness& headerEndianness)
{
  OpenVDS::IOError error;

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
    std::cerr << "Failed to parse JSON header format file: " << e.what();
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
findRepresentativeSegment(SEGYFileInfo const& fileInfo)
{
  float
    bestScore = 0.0f;

  int
    bestIndex = 0;

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
  }

  return fileInfo.m_segmentInfo[bestIndex];
}

bool
analyzeSegment(OpenVDS::File const& file, SEGYFileInfo const& fileInfo, SEGYSegmentInfo const& segmentInfo, float valueRangePercentile, OpenVDS::FloatRange& valueRange, OpenVDS::IOError& error)
{
  int traceByteSize = fileInfo.TraceByteSize();

  int64_t offset = SEGY::TextualFileHeaderSize + SEGY::BinaryFileHeaderSize + segmentInfo.m_traceStart * traceByteSize;

  int traceCount = int(segmentInfo.m_traceStop - segmentInfo.m_traceStart);

  std::unique_ptr<char[]> buffer(new char[(segmentInfo.m_traceStop - segmentInfo.m_traceStart) * traceByteSize]);

  file.Read(buffer.get(), offset, traceCount * traceByteSize, error);

  if (error.code != 0)
  {
    return false;
  }

  if (fileInfo.m_sampleCount == 0 || traceCount == 0)
  {
    valueRange = OpenVDS::FloatRange(0.0f, 0.0f);
  }
  else if (fileInfo.m_dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IBMFloat || fileInfo.m_dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IEEEFloat)
  {
    std::unique_ptr<float[]> sampleBuffer;

    float* samples = nullptr;

    if (fileInfo.m_dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IBMFloat)
    {
      sampleBuffer.reset(new float[fileInfo.m_sampleCount]);
      samples = sampleBuffer.get();
    }

    int heapSizeMax = (int)(((100.0f - valueRangePercentile) / 100.0f) * traceCount * fileInfo.m_sampleCount / 2);

    std::vector<float> minHeap, maxHeap;

    minHeap.reserve(heapSizeMax + 1);
    maxHeap.reserve(heapSizeMax + 1);

    for (int trace = 0; trace < traceCount; trace++)
    {
      if (fileInfo.m_dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IBMFloat)
      {
        SEGY::Ibm2ieee(sampleBuffer.get(), buffer.get() + traceByteSize * trace + SEGY::TraceHeaderSize, fileInfo.m_sampleCount);
      }
      else
      {
        assert(fileInfo.m_dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IEEEFloat);
        samples = reinterpret_cast<float*>(buffer.get() + traceByteSize * trace + SEGY::TraceHeaderSize);
      }

      if (trace == 0)
      {
        minHeap.push_back(samples[0]);
        maxHeap.push_back(samples[0]);
      }

      for (int sample = 0; sample < fileInfo.m_sampleCount; sample++)
      {
        if (samples[sample] < minHeap[0])
        {
          minHeap.push_back(samples[sample]);
          if (minHeap.size() <= heapSizeMax)
          {
            std::push_heap(minHeap.begin(), minHeap.end(), std::less<float>());
          }
          else
          {
            std::pop_heap(minHeap.begin(), minHeap.end(), std::less<float>());
            minHeap.pop_back();
          }
        }

        if (samples[sample] > maxHeap[0])
        {
          maxHeap.push_back(samples[sample]);

          if (maxHeap.size() <= heapSizeMax)
          {
            std::push_heap(maxHeap.begin(), maxHeap.end(), std::greater<float>());
          }
          else
          {
            std::pop_heap(maxHeap.begin(), maxHeap.end(), std::greater<float>());
            maxHeap.pop_back();
          }
        }
      }
    }

    if (minHeap[0] != maxHeap[0])
    {
      valueRange = OpenVDS::FloatRange(minHeap[0], maxHeap[0]);
    }
    else
    {
      valueRange = OpenVDS::FloatRange(minHeap[0], minHeap[0] + 1.0f);
    }
  }

  return true;
}

bool
createSEGYHeadersMetadata(OpenVDS::File const& file, OpenVDS::MetadataContainer& metadataContainer, OpenVDS::IOError& error)
{
  std::vector<uint8_t> textHeader(SEGY::TextualFileHeaderSize);
  std::vector<uint8_t> binaryHeader(SEGY::BinaryFileHeaderSize);

  // Read headers
  bool success = file.Read(textHeader.data(), 0, SEGY::TextualFileHeaderSize, error) &&
    file.Read(binaryHeader.data(), SEGY::TextualFileHeaderSize, SEGY::BinaryFileHeaderSize, error);

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

bool
parseSEGYFileInfoFile(OpenVDS::File const& file, SEGYFileInfo& fileInfo)
{
  OpenVDS::IOError error;

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
createAxisDescriptors(SEGYFileInfo const& fileInfo)
{
  std::vector<OpenVDS::VolumeDataAxisDescriptor>
    axisDescriptors;

  axisDescriptors.push_back(OpenVDS::VolumeDataAxisDescriptor(fileInfo.m_sampleCount, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE, "ms", 0.0f, (fileInfo.m_sampleCount - 1) * (float)fileInfo.m_sampleIntervalMilliseconds));

  int inlineStep = 1,
    crosslineStep = 1;

  int minInline = fileInfo.m_segmentInfo[0].m_binInfoStart.m_inlineNumber, maxInline = minInline,
    minCrossline = fileInfo.m_segmentInfo[0].m_binInfoStart.m_crosslineNumber, maxCrossline = minCrossline;

  for (auto const& segmentInfo : fileInfo.m_segmentInfo)
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

  int inlineCount = 1 + (maxInline - minInline) / inlineStep,
    crosslineCount = 1 + (maxCrossline - minCrossline) / crosslineStep;

  axisDescriptors.push_back(OpenVDS::VolumeDataAxisDescriptor(crosslineCount, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE, "", (float)minCrossline, (float)maxCrossline));
  axisDescriptors.push_back(OpenVDS::VolumeDataAxisDescriptor(inlineCount, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE, "", (float)minInline, (float)maxInline));

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

class FileViewManager
{
  typedef std::map<std::pair<int64_t, int64_t>, OpenVDS::FileView*> FileViewMap;

  std::mutex m_mutex;
  FileViewMap m_fileViewMap;
  OpenVDS::File& m_file;

  void releaseFileView(OpenVDS::FileView* fileView)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    auto it = m_fileViewMap.find(FileViewMap::key_type(fileView->Pos(), fileView->Size()));
    assert(it != m_fileViewMap.end());

    if (OpenVDS::FileView::RemoveReference(fileView))
    {
      m_fileViewMap.erase(it);
    }
  }

public:
  FileViewManager(OpenVDS::File& file) : m_file(file) {}

  std::shared_ptr<OpenVDS::FileView> acquireFileView(int64_t pos, int64_t size, bool isPopulate, OpenVDS::IOError& error)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    OpenVDS::FileView* fileView = nullptr;

    auto it = m_fileViewMap.find(FileViewMap::key_type(pos, size));
    if (it != m_fileViewMap.end())
    {
      fileView = it->second;
      OpenVDS::FileView::AddReference(fileView);
    }
    else
    {
      fileView = m_file.CreateFileView(pos, size, isPopulate, error);
      if (fileView)
      {
        m_fileViewMap.insert(FileViewMap::value_type(FileViewMap::key_type(pos, size), fileView));
      }
    }

    return std::shared_ptr<OpenVDS::FileView>(fileView, [this](OpenVDS::FileView* fileView) { if (fileView) this->releaseFileView(fileView); });
  }
};

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

void
copySamples(const void* data, SEGY::BinaryHeader::DataSampleFormatCode dataSampleFormatCode, float* target, int sampleStart, int sampleCount)
{
  if (dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IBMFloat)
  {
    SEGY::Ibm2ieee(target, reinterpret_cast<const void*>((intptr_t)data + sampleStart * 4), sampleCount);
  }
  else
  {
    assert(dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IEEEFloat);
    memcpy(target, reinterpret_cast<const void*>((intptr_t)data + sampleStart * 4), sampleCount * 4);
  }
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

int
main(int argc, char* argv[])
{
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
  std::string bucket;
  std::string region;
  std::string prefix;
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
  options.add_option("", "", "bucket", "Bucket to upload to.", cxxopts::value<std::string>(bucket), "<string>");
  options.add_option("", "", "region", "Region of bucket to upload to.", cxxopts::value<std::string>(region), "<string>");
  options.add_option("", "", "prefix", "Top-level prefix to prepend to all object-keys.", cxxopts::value<std::string>(prefix), "<string>");
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
    if (bucket.empty())
    {
      std::cerr << std::string("No bucket specified");
      return EXIT_FAILURE;
    }

    if (region.empty())
    {
      std::cerr << std::string("No bucket specified");
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

    OpenVDS::IOError
      error;

    headerFormatFile.Open(headerFormatFileName.c_str(), false, false, false, error);

    if (error.code != 0)
    {
      std::cerr << std::string("Could not open file: ") << headerFormatFileName;
      return EXIT_FAILURE;
    }

    ParseHeaderFormatFile(headerFormatFile, g_traceHeaderFields, headerEndianness);
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

  OpenVDS::File
    file;

  OpenVDS::IOError
    error;

  file.Open(fileNames[0].c_str(), false, false, false, error);

  if (error.code != 0)
  {
    std::cerr << std::string("Could not open file: ") << fileNames[0];
    return EXIT_FAILURE;
  }

  SEGYFileInfo
    fileInfo(headerEndianness);

  if(scan)
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

    bool success = fileInfo.Scan(file, primaryKeyHeaderField, secondaryKeyHeaderField, binInfoHeaderFields);

    if (!success)
    {
      std::cerr << std::string("Failed to scan file: ") << fileNames[0];
      return EXIT_FAILURE;
    }

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

      OpenVDS::IOError
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
  else if (!fileInfoFileName.empty())
  {
    OpenVDS::File
      fileInfoFile;

    OpenVDS::IOError
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

  // Determine value range
  OpenVDS::FloatRange
    valueRange;

  analyzeSegment(file, fileInfo, findRepresentativeSegment(fileInfo), 99.9f, valueRange, error);

  if (error.code != 0)
  {
    std::cerr << error.string;
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

  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors = createAxisDescriptors(fileInfo);

  // Create channel descriptors

  std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors = createChannelDescriptors(fileInfo, valueRange);

  // Create metadata
  OpenVDS::MetadataContainer
    metadataContainer;

  createSEGYHeadersMetadata(file, metadataContainer, error);

  if (error.code != 0)
  {
    std::cerr << error.string;
    return EXIT_FAILURE;
  }

  createSurveyCoordinateSystemMetadata(fileInfo, metadataContainer);

  // Create the VDS
  OpenVDS::Error
    createError;

  std::string
    key = !prefix.empty() ? prefix + "/" + persistentID : persistentID;

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> vds(OpenVDS::Create(OpenVDS::AWSOpenOptions(bucket, key, region), layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, createError), &OpenVDS::Close);

  if (createError.code != 0)
  {
    std::cerr << std::string("Create error: " + createError.string);
    return EXIT_FAILURE;
  }

  FileViewManager fileViewManager(file);

  auto accessManager = OpenVDS::GetAccessManager(vds.get());
  auto layout = accessManager->GetVolumeDataLayout();

  auto amplitudeAccessor = accessManager->CreateVolumeDataPageAccessor(accessManager->GetVolumeDataLayout(), OpenVDS::DimensionsND::Dimensions_012, 0, 0, 8, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  auto traceFlagAccessor = accessManager->CreateVolumeDataPageAccessor(accessManager->GetVolumeDataLayout(), OpenVDS::DimensionsND::Dimensions_012, 0, 1, 8, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  auto segyTraceHeaderAccessor = accessManager->CreateVolumeDataPageAccessor(accessManager->GetVolumeDataLayout(), OpenVDS::DimensionsND::Dimensions_012, 0, 2, 8, OpenVDS::VolumeDataAccessManager::AccessMode_Create);

  int64_t traceByteSize = fileInfo.TraceByteSize();

  std::shared_ptr<OpenVDS::FileView> fileView;

  for (int64_t chunk = 0; chunk < amplitudeAccessor->GetChunkCount(); chunk++)
  {
    int done = int(double(chunk) / amplitudeAccessor->GetChunkCount() * 100);
    fmt::print("\r{:3d}% done.", done);
    fflush(stdout);
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

    int min[OpenVDS::Dimensionality_Max], max[OpenVDS::Dimensionality_Max];

    amplitudeAccessor->GetChunkMinMax(chunk, min, max);

    int sampleStart = min[0];
    int sampleCount = max[0] - min[0];

    int secondaryKeyStart = (int)floorf(layout->GetAxisDescriptor(1).SampleIndexToCoordinate(min[1]) + 0.5f);
    int secondaryKeyStop = (int)floorf(layout->GetAxisDescriptor(1).SampleIndexToCoordinate(max[1] - 1) + 0.5f);

    int primaryKeyStart = (int)floorf(layout->GetAxisDescriptor(2).SampleIndexToCoordinate(min[2]) + 0.5f);
    int primaryKeyStop = (int)floorf(layout->GetAxisDescriptor(2).SampleIndexToCoordinate(max[2] - 1) + 0.5f);

    auto lower = std::lower_bound(fileInfo.m_segmentInfo.begin(), fileInfo.m_segmentInfo.end(), primaryKeyStart, [](SEGYSegmentInfo const& segmentInfo, int primaryKey)->bool { return segmentInfo.m_primaryKey < primaryKey; });
    auto upper = std::upper_bound(fileInfo.m_segmentInfo.begin(), fileInfo.m_segmentInfo.end(), primaryKeyStop, [](int primaryKey, SEGYSegmentInfo const& segmentInfo)->bool { return primaryKey < segmentInfo.m_primaryKey; });

    int64_t traceStart = lower->m_traceStart;
    int64_t traceStop = std::prev(upper)->m_traceStop;

    assert(traceStop > traceStart);

    int64_t offset = SEGY::TextualFileHeaderSize + SEGY::BinaryFileHeaderSize + traceStart * traceByteSize;
    int64_t size = (traceStop - traceStart + 1) * traceByteSize;

    // This acquires the new file view before releasing the previous so we usually end up re-using the same file view
    fileView = fileViewManager.acquireFileView(offset, size, false, error);

    if (error.code == 0)
    {
      OpenVDS::VolumeDataPage* amplitudePage = amplitudeAccessor->CreatePage(chunk);
      OpenVDS::VolumeDataPage* traceFlagPage = nullptr;
      OpenVDS::VolumeDataPage* segyTraceHeaderPage = nullptr;

      if (min[0] == 0)
      {
        traceFlagPage = traceFlagAccessor->CreatePage(traceFlagAccessor->GetChunkIndex(min));
        segyTraceHeaderPage = segyTraceHeaderAccessor->CreatePage(segyTraceHeaderAccessor->GetChunkIndex(min));
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
      for (auto segment = lower; segment != upper; ++segment)
      {
        const void* traceData = reinterpret_cast<const void*>(intptr_t(fileView->Pointer()) + (segment->m_traceStart - traceStart) * traceByteSize);
        int traceCount = int(segment->m_traceStop - segment->m_traceStart + 1);

        int firstTrace = findFirstTrace(segment->m_primaryKey, secondaryKeyStart, fileInfo, traceData, traceCount);

        for (int trace = firstTrace; trace < traceCount; trace++)
        {
          const char* header = reinterpret_cast<const char*>(intptr_t(traceData) + traceByteSize * trace);
          const void* data = header + SEGY::TraceHeaderSize;

          int primaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_primaryKey, fileInfo.m_headerEndianness),
            secondaryTest = SEGY::ReadFieldFromHeader(header, fileInfo.m_secondaryKey, fileInfo.m_headerEndianness);

          // Check if the trace is outside the secondary range and go to the next segment if it is
          if (primaryTest == segment->m_primaryKey && secondaryTest > secondaryKeyStop)
          {
            break;
          }

          int primaryIndex = layout->GetAxisDescriptor(2).CoordinateToSampleIndex((float)segment->m_primaryKey);
          int secondaryIndex = layout->GetAxisDescriptor(1).CoordinateToSampleIndex((float)secondaryTest);

          assert(primaryIndex >= min[2] && primaryIndex < max[2]);
          assert(secondaryIndex >= min[1] && secondaryIndex < max[1]);

          {
            int targetOffset = (primaryIndex - min[2]) * amplitudePitch[2] + (secondaryIndex - min[1]) * amplitudePitch[1];

            copySamples(data, fileInfo.m_dataSampleFormatCode, &reinterpret_cast<float*>(amplitudeBuffer)[targetOffset], sampleStart, sampleCount);
          }

          if (traceFlagBuffer)
          {
            int targetOffset = (primaryIndex - min[2]) * traceFlagPitch[2] + (secondaryIndex - min[1]) * traceFlagPitch[1];

            reinterpret_cast<uint8_t*>(traceFlagBuffer)[targetOffset] = true;
          }

          if (segyTraceHeaderBuffer)
          {
            int targetOffset = (primaryIndex - min[2]) * segyTraceHeaderPitch[2] + (secondaryIndex - min[1]) * segyTraceHeaderPitch[1];

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

  fmt::print("\r100% done.\n");

  fileView.reset();
  return EXIT_SUCCESS;
}