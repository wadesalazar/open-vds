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

#include "IO/File.h"
#include "SEGYFileInfo.h"
#include "cxxopts.hpp"

#include <cstdlib>
#include <climits>
#include <json/json.h>
#include <assert.h>
#include <fmt/format.h>

Json::Value
serializeSEGYBinInfo(SEGYBinInfo const &binInfo)
{
  Json::Value
    jsonBinInfo;

  jsonBinInfo["inlineNumber"]    = binInfo.m_inlineNumber;
  jsonBinInfo["crosslineNumber"] = binInfo.m_crosslineNumber;

  jsonBinInfo["ensembleXCoordinate"]  = binInfo.m_ensembleXCoordinate;
  jsonBinInfo["ensembleYCoordinate"]  = binInfo.m_ensembleYCoordinate;

  return jsonBinInfo;
}

Json::Value
serializeSEGYSegmentInfo(SEGYSegmentInfo const &segmentInfo)
{
  Json::Value
    jsonSegmentInfo;

  jsonSegmentInfo["primaryKey"] = segmentInfo.m_primaryKey;
  jsonSegmentInfo["traceStart"] = segmentInfo.m_traceStart;
  jsonSegmentInfo["traceStop"]  = segmentInfo.m_traceStop;

  jsonSegmentInfo["binInfoStart"] = serializeSEGYBinInfo(segmentInfo.m_binInfoStart);
  jsonSegmentInfo["binInfoStop"]  = serializeSEGYBinInfo(segmentInfo.m_binInfoStop);

  return jsonSegmentInfo;
}

std::string
to_string(SEGY::Endianness endiannness)
{
  switch(endiannness)
  {
  case SEGY::Endianness::BigEndian:    return "BigEndian";
  case SEGY::Endianness::LittleEndian: return "LittleEndian";
  default:
    assert(0); return "";
  }
}

std::string
to_string(SEGY::FieldWidth fieldWidth)
{
  switch(fieldWidth)
  {
  case SEGY::FieldWidth::TwoByte:    return "TwoByte";
  case SEGY::FieldWidth::FourByte: return "FourByte";
  default:
    assert(0); return "";
  }
}

Json::Value
serializeSEGYHeaderField(SEGY::HeaderField const &headerField)
{
  Json::Value
    jsonHeaderField(Json::ValueType::arrayValue);

  jsonHeaderField.append(headerField.byteLocation);
  jsonHeaderField.append(to_string(headerField.fieldWidth));

  return jsonHeaderField;
}

Json::Value
serializeSEGYFileInfo(SEGYFileInfo const &fileInfo)
{
  Json::Value
    jsonFileInfo;

  jsonFileInfo["persistentID"]         = fmt::format("{:X}", fileInfo.m_persistentID);
  jsonFileInfo["headerEndianness"]     = to_string(fileInfo.m_headerEndianness);
  jsonFileInfo["dataSampleFormatCode"] = (int)fileInfo.m_dataSampleFormatCode;
  jsonFileInfo["sampleCount"]          = fileInfo.m_sampleCount;
  jsonFileInfo["sampleInterval"]       = fileInfo.m_sampleIntervalMilliseconds;
  jsonFileInfo["traceCount"]           = fileInfo.m_traceCount;
  jsonFileInfo["primaryKey"]           = serializeSEGYHeaderField(fileInfo.m_primaryKey);
  jsonFileInfo["secondaryKey"]         = serializeSEGYHeaderField(fileInfo.m_secondaryKey);

  Json::Value
    jsonSegmentInfoArray(Json::ValueType::arrayValue);

  for(auto const &segmentInfo : fileInfo.m_segmentInfo)
  {
    jsonSegmentInfoArray.append(serializeSEGYSegmentInfo(segmentInfo));
  }

  jsonFileInfo["segmentInfo"] = jsonSegmentInfoArray;

  return jsonFileInfo;
}

std::map<std::string, SEGY::HeaderField>
g_traceHeaderFields = 
{
  { "TraceSequenceNumber"          , SEGY::TraceHeader::TraceSequenceNumberHeaderField },
  { "TraceSequenceNumberWithinFile", SEGY::TraceHeader::TraceSequenceNumberWithinFileHeaderField },
  { "EnergySourcePointNumber"      , SEGY::TraceHeader::EnergySourcePointNumberHeaderField },
  { "EnsembleNumber"               , SEGY::TraceHeader::EnsembleNumberHeaderField },
  { "TraceNumberWithinEnsemble"    , SEGY::TraceHeader::TraceNumberWithinEnsembleHeaderField },
  { "TraceIdentificationCode"      , SEGY::TraceHeader::TraceIdentificationCodeHeaderField },
  { "CoordinateScale"              , SEGY::TraceHeader::CoordinateScaleHeaderField },
  { "SourceXCoordinate"            , SEGY::TraceHeader::SourceXCoordinateHeaderField },
  { "SourceYCoordinate"            , SEGY::TraceHeader::SourceYCoordinateHeaderField },
  { "GroupXCoordinate"             , SEGY::TraceHeader::GroupXCoordinateHeaderField },
  { "GroupYCoordinate"             , SEGY::TraceHeader::GroupYCoordinateHeaderField },
  { "CoordinateUnits"              , SEGY::TraceHeader::CoordinateUnitsHeaderField },
  { "StartTime"                    , SEGY::TraceHeader::StartTimeHeaderField },
  { "NumSamples"                   , SEGY::TraceHeader::NumSamplesHeaderField },
  { "SampleInterval"               , SEGY::TraceHeader::SampleIntervalHeaderField },
  { "EnsembleXCoordinate"          , SEGY::TraceHeader::EnsembleXCoordinateHeaderField },
  { "EnsembleYCoordinate"          , SEGY::TraceHeader::EnsembleYCoordinateHeaderField },
  { "InlineNumber"                 , SEGY::TraceHeader::InlineNumberHeaderField },
  { "CrosslineNumber"              , SEGY::TraceHeader::CrosslineNumberHeaderField }
};

std::map<std::string, std::string>
g_aliases = 
{
  { "Inline",              "InlineNumber" },
  { "InLine",              "InlineNumber" },
  { "InLineNumber",        "InlineNumber" },
  { "Crossline",           "CrosslineNumber" },
  { "CrossLine",           "CrosslineNumber" },
  { "CrossLineNumber",     "CrosslineNumber" },
  { "Easting",             "EnsembleXCoordinate" },
  { "Northing",            "EnsembleYCoordinate" },
  { "CDPXCoordinate",      "EnsembleXCoordinate" },
  { "CDPYCoordinate",      "EnsembleYCoordinate" },
  { "CDP-X",               "EnsembleXCoordinate" },
  { "CDP-Y",               "EnsembleYCoordinate" },
  { "Source-X",            "SourceXCoordinate" },
  { "Source-Y",            "SourceYCoordinate" },
  { "Group-X",             "GroupXCoordinate" },
  { "Group-Y",             "GroupYCoordinate" },
  { "ReceiverXCoordinate", "GroupXCoordinate" },
  { "ReceiverYCoordinate", "GroupYCoordinate" },
  { "Receiver-X",          "GroupXCoordinate" },
  { "Receiver-Y",          "GroupYCoordinate" },
  { "Scalar",              "CoordinateScale" }
};

void
resolveAlias(std::string &fieldName)
{
  if(g_aliases.find(fieldName) != g_aliases.end())
  {
    fieldName = g_aliases[fieldName];
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

SEGY::FieldWidth
fieldWidthFromJson(Json::Value const &jsonFieldWidth)
{
  std::string
    fieldWidthString = jsonFieldWidth.asString();

  if(fieldWidthString == "TwoByte")
  {
    return SEGY::FieldWidth::TwoByte;
  }
  else if(fieldWidthString == "FourByte")
  {
    return SEGY::FieldWidth::FourByte;
  }

  throw Json::Exception("Illegal field width");
}

SEGY::HeaderField
headerFieldFromJson(Json::Value const &jsonHeaderField)
{
  int
    bytePosition = jsonHeaderField[0].asInt();

  SEGY::FieldWidth
    fieldWidth = fieldWidthFromJson(jsonHeaderField[1]);

  if(bytePosition < 1 || bytePosition > SEGY::TraceHeaderSize - ((fieldWidth == SEGY::FieldWidth::TwoByte) ? 2 : 4))
  {
    throw Json::Exception(std::string("Illegal field definition: ") + jsonHeaderField.toStyledString());
  }

  return SEGY::HeaderField(bytePosition, fieldWidth);
}

bool
parseHeaderFormatFile(OpenVDS::File const &file, std::map<std::string, SEGY::HeaderField> &traceHeaderFields, SEGY::Endianness &headerEndianness)
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
      root;

    bool
      success = reader->parse(buffer.get(), buffer.get() + fileSize, &root, &errs);

    if(!success)
    {
      throw Json::Exception(errs);
    }

    for (std::string const & fieldName : root.getMemberNames())
    {
      std::string canonicalFieldName = fieldName;
      resolveAlias(canonicalFieldName);

      if(fieldName == "Endianness")
      {
        headerEndianness = endiannessFromJson(root[fieldName]);
      }
      else
      {
        traceHeaderFields[canonicalFieldName] = headerFieldFromJson(root[fieldName]);
      }
    }
  }
  catch(Json::Exception e)
  {
    std::cerr << "Failed to parse JSON header format file: " << e.what();
    return false;
  }

  return true;
}

int
main(int argc, char *argv[])
{
  cxxopts::Options options("SEGYScan", "SEGYScan - A tool to scan a SEG-Y file and create an index");
  options.positional_help("<input file>");

  std::vector<std::string> fileNames;
  std::string headerFormatFileName;
  std::string primaryKey = "InlineNumber";
  std::string secondaryKey = "CrosslineNumber";
  double scale = 0;
  bool littleEndian = false;

  options.add_option("", "h", "header-format", "A JSON file defining the header format for the input SEG-Y file. The expected format is a dictonary of strings (field names) to pairs (byte position, field width) where field width can be \"TwoByte\" or \"FourByte\". Additionally, an \"Endianness\" key can be specified as \"BigEndian\" or \"LittleEndian\".", cxxopts::value<std::string>(headerFormatFileName), "<file>");
  options.add_option("", "p", "primary-key", "The name of the trace header field to use as the primary key.", cxxopts::value<std::string>(primaryKey)->default_value("Inline"), "<field>");
  options.add_option("", "s", "secondary-key", "The name of the trace header field to use as the secondary key.", cxxopts::value<std::string>(secondaryKey)->default_value("Crossline"), "<field>");
  options.add_option("", "", "scale", "If a scale override (floating point) is given, it is used to scale the coordinates in the header instead of determining the scale factor from the coordinate scale trace header field.", cxxopts::value<double>(scale), "<value>");
  options.add_option("", "l", "little-endian", "Force (non-standard) little-endian trace headers.", cxxopts::value<bool>(littleEndian), "");

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

  SEGY::Endianness headerEndianness = (littleEndian ? SEGY::Endianness::LittleEndian : SEGY::Endianness::BigEndian);

  if(!headerFormatFileName.empty())
  {
    OpenVDS::File
      headerFormatFile;

    OpenVDS::IOError
      error;

    headerFormatFile.open(headerFormatFileName.c_str(), false, false, false, error);

    if(error.code != 0)
    {
      std::cerr << std::string("Could not open file: ") << headerFormatFileName;
      return EXIT_FAILURE;
    }

    parseHeaderFormatFile(headerFormatFile, g_traceHeaderFields, headerEndianness);
  }

  // get the canonical field name for the primary and secondary key
  resolveAlias(primaryKey);
  resolveAlias(secondaryKey);

  SEGY::HeaderField
    primaryKeyHeaderField,
    secondaryKeyHeaderField;

  if(g_traceHeaderFields.find(primaryKey) != g_traceHeaderFields.end())
  {
    primaryKeyHeaderField = g_traceHeaderFields[primaryKey];
  }
  else
  {
    std::cerr << std::string("Unrecognized header field given for primary key: ") << primaryKey;
    return EXIT_FAILURE;
  }

  if(g_traceHeaderFields.find(secondaryKey) != g_traceHeaderFields.end())
  {
    secondaryKeyHeaderField = g_traceHeaderFields[secondaryKey];
  }

  SEGYBinInfoHeaderFields
    binInfoHeaderFields(g_traceHeaderFields["InlineNumber"], g_traceHeaderFields["CrosslineNumber"], g_traceHeaderFields["CoordinateScale"], g_traceHeaderFields["EnsembleXCoordinate"], g_traceHeaderFields["EnsembleYCoordinate"], scale);

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

  SEGYFileInfo
    fileInfo(headerEndianness);

  bool success = fileInfo.scan(file, primaryKeyHeaderField, secondaryKeyHeaderField, binInfoHeaderFields);

  if(!success)
  {
    std::cerr << std::string("Failed to scan file: ") << fileNames[0];
    return EXIT_FAILURE;
  }

  Json::Value jsonFileInfo = serializeSEGYFileInfo(fileInfo);

  Json::StreamWriterBuilder wbuilder;
  wbuilder["indentation"] = "  ";
  std::string document = Json::writeString(wbuilder, jsonFileInfo);
  
  std::cout << document;

  return EXIT_SUCCESS;
}
