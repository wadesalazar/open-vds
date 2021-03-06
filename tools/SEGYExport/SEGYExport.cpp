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

#include <SEGYUtils/SEGY.h>
#include "IO/File.h"

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/VolumeDataLayout.h>

#include "cxxopts.hpp"

#include <cstdlib>
#include <climits>
#include <json/json.h>
#include <assert.h>
#include <fmt/format.h>
#include <chrono>

#include <PrintHelpers.h>

int
main(int argc, char *argv[])
{
  //auto start_time = std::chrono::high_resolution_clock::now();

  cxxopts::Options options("SEGYExport", "SEGYExport - A tool to export a volume data store (VDS) to a SEG-Y file\n\nSee online documentation for connection paramters:\nhttp://osdu.pages.community.opengroup.org/platform/domain-data-mgmt-services/seismic/open-vds/connection.html\n");
  options.positional_help("<output file>");

  std::string url;
  std::string connection;
  std::string persistentID;
  std::string fileName;
  bool jsonOutput = false;
  bool help = false;
  bool version = false;

  options.add_option("", "", "url", "Url with vendor specific protocol or VDS file name.", cxxopts::value<std::string>(url), "<string>");
  options.add_option("", "", "connection", "Vendor specific connection string.", cxxopts::value<std::string>(connection), "<string>");
  options.add_option("", "", "vdsfile", "Input VDS file name.", cxxopts::value<std::string>(url), "<string>");
  options.add_option("", "", "persistentID", "A globally unique ID for the VDS, usually an 8-digit hexadecimal number.", cxxopts::value<std::string>(persistentID), "<ID>");

  options.add_option("", "", "json-output", "Enable json output.", cxxopts::value<bool>(jsonOutput), "");
  options.add_option("", "h", "help", "Print this help information", cxxopts::value<bool>(help), "");
  options.add_option("", "", "version", "Print version information.", cxxopts::value<bool>(version), "");

  options.add_option("", "", "output", "", cxxopts::value<std::string>(fileName), "");

  options.parse_positional("output");

  if(argc == 1)
  {
    OpenVDS::printInfo(jsonOutput, "Args", options.help());
    return EXIT_SUCCESS;
  }

  try
  {
    options.parse(argc, argv);
  }
  catch(cxxopts::OptionParseException &e)
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
    OpenVDS::printVersion(jsonOutput, "SEGYExport");
    return EXIT_SUCCESS;
  }

  if(fileName.empty())
  {
    OpenVDS::printError(jsonOutput, "Args", "No output SEG-Y file specified");
    return EXIT_FAILURE;
  }

  // Open the VDS
  if (!persistentID.empty())
  {
    if (!url.empty() && url.back() != '/')
    {
      url.push_back('/');
    }
    url.insert(url.end(), persistentID.begin(), persistentID.end());
  }

  OpenVDS::Error openError;

  OpenVDS::VDSHandle handle;
  
  if(OpenVDS::IsSupportedProtocol(url))
  {
    handle = OpenVDS::Open(url, connection, openError);
  }
  else
  {
    handle = OpenVDS::Open(OpenVDS::VDSFileOpenOptions(url), openError);
  }

  if(openError.code != 0)
  {
    OpenVDS::printError(jsonOutput, "VDS", "Could not open VDS", openError.string);
    return EXIT_FAILURE;
  }

  // auto-close vds handle when it goes out of scope
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> vdsGuard(handle, &OpenVDS::Close);

  auto accessManager = OpenVDS::GetAccessManager(handle);
  auto volumeDataLayout = accessManager.GetVolumeDataLayout();

  int dimensionality = accessManager.GetVolumeDataLayout()->GetDimensionality();
  int outerDimension = std::max(2, dimensionality - 1);

  // Find a dimension group containing (at least) the inner dimensions and can be produced
  OpenVDS::DimensionsND dimensionGroup = OpenVDS::Dimensions_01;

  if(outerDimension == 1)
  {
    dimensionGroup = OpenVDS::Dimensions_02;
  }

  if(accessManager.GetVDSProduceStatus(dimensionGroup, 0, 0) != OpenVDS::VDSProduceStatus::Normal)
  {
    if(dimensionality == 4 && outerDimension == 2)
    {
      if(accessManager.GetVDSProduceStatus(OpenVDS::Dimensions_013, 0, 0) == OpenVDS::VDSProduceStatus::Normal || 
        (accessManager.GetVDSProduceStatus(OpenVDS::Dimensions_013, 0, 0) == OpenVDS::VDSProduceStatus::Remapped && accessManager.GetVDSProduceStatus(dimensionGroup, 0, 0) == OpenVDS::VDSProduceStatus::Unavailable))
      {
        dimensionGroup = OpenVDS::Dimensions_013;
      }
    }
    else
    {
      if(accessManager.GetVDSProduceStatus(OpenVDS::Dimensions_012, 0, 0) == OpenVDS::VDSProduceStatus::Normal || 
        (accessManager.GetVDSProduceStatus(OpenVDS::Dimensions_012, 0, 0) == OpenVDS::VDSProduceStatus::Remapped && accessManager.GetVDSProduceStatus(dimensionGroup, 0, 0) == OpenVDS::VDSProduceStatus::Unavailable))
      {
        dimensionGroup = OpenVDS::Dimensions_012;
      }
    }
  }

  if(accessManager.GetVDSProduceStatus(dimensionGroup, 0, 0) == OpenVDS::VDSProduceStatus::Unavailable)
  {
    OpenVDS::printError(jsonOutput, "VDS", "VDS cannot produce data");
    return EXIT_FAILURE;
  }

  if(!volumeDataLayout->IsChannelAvailable("Trace"))
  {
    OpenVDS::printError(jsonOutput, "VDS", "VDS has no \"Trace\" channel");
    return EXIT_FAILURE;
  }
  int traceFlagChannel = volumeDataLayout->GetChannelIndex("Trace");

  if(!volumeDataLayout->IsChannelAvailable("SEGYTraceHeader"))
  {
    OpenVDS::printError(jsonOutput, "VDS", "VDS has no \"SEGYTraceHeader\" channel");
    return EXIT_FAILURE;
  }
  int segyTraceHeaderChannel = volumeDataLayout->GetChannelIndex("SEGYTraceHeader");

  if((!volumeDataLayout->IsMetadataBLOBAvailable("SEGY", "TextHeader")   && !volumeDataLayout->IsMetadataBLOBAvailable("", "SEGYTextHeader")) ||
     (!volumeDataLayout->IsMetadataBLOBAvailable("SEGY", "BinaryHeader") && !volumeDataLayout->IsMetadataBLOBAvailable("", "SEGYBinaryHeader")))
  {
    OpenVDS::printError(jsonOutput, "VDS", "SEG-Y Text/Binary headers not found");
    return EXIT_FAILURE;
  }

  // Write headers
  std::vector<uint8_t> textHeader(SEGY::TextualFileHeaderSize);
  std::vector<uint8_t> binaryHeader(SEGY::BinaryFileHeaderSize);

  if(volumeDataLayout->IsMetadataBLOBAvailable("SEGY", "TextHeader"))
  {
    volumeDataLayout->GetMetadataBLOB("SEGY", "TextHeader", textHeader);
  }
  else if(volumeDataLayout->IsMetadataBLOBAvailable("", "SEGYTextHeader")) // This non-standard metadata is written by Bluware software that pre-dates the standardization of VDS
  {
    volumeDataLayout->GetMetadataBLOB("", "SEGYTextHeader", textHeader);

    int a2e[] = {  0,  1,  2,  3, 55, 45, 46, 47, 22,  5, 37, 11, 12, 13, 14, 15,
                  16, 17, 18, 19, 60, 61, 50, 38, 24, 25, 63, 39, 28, 29, 30, 31,
                  64, 79,127,123, 91,108, 80,125, 77, 93, 92, 78,107, 96, 75, 97,
                  240,241,242,243,244,245,246,247,248,249,122, 94, 76,126,110,111,
                  124,193,194,195,196,197,198,199,200,201,209,210,211,212,213,214,
                  215,216,217,226,227,228,229,230,231,232,233, 74,224, 90, 95,109,
                  121,129,130,131,132,133,134,135,136,137,145,146,147,148,149,150,
                  151,152,153,162,163,164,165,166,167,168,169,192,106,208,161,  7,
                   32, 33, 34, 35, 36, 21,  6, 23, 40, 41, 42, 43, 44,  9, 10, 27,
                   48, 49, 26, 51, 52, 53, 54,  8, 56, 57, 58, 59,  4, 20, 62,225,
                   65, 66, 67, 68, 69, 70, 71, 72, 73, 81, 82, 83, 84, 85, 86, 87,
                   88, 89, 98, 99,100,101,102,103,104,105,112,113,114,115,116,117,
                  118,119,120,128,138,139,140,141,142,143,144,154,155,156,157,158,
                  159,160,170,171,172,173,174,175,176,177,178,179,180,181,182,183,
                  184,185,186,187,188,189,190,191,202,203,204,205,206,207,218,219,
                  220,221,222,223,234,235,236,237,238,239,250,251,252,253,254,255 };

    // Convert to EBCDIC
    for(int i = 0; i < int(textHeader.size()); i++) textHeader[i] = a2e[textHeader[i]];
  }

  SEGY::Endianness headerEndianness = SEGY::Endianness::BigEndian;

  if(volumeDataLayout->IsMetadataIntAvailable("SEGY", "Endianness"))
  {
    switch(volumeDataLayout->GetMetadataInt("SEGY", "Endianness"))
    {
    case 0: headerEndianness = SEGY::Endianness::BigEndian;    break;
    case 1: headerEndianness = SEGY::Endianness::LittleEndian; break;
    }
  }

  SEGY::Endianness dataEndianness = headerEndianness;

  if(volumeDataLayout->IsMetadataIntAvailable("SEGY", "DataEndianness"))
  {
    switch(volumeDataLayout->GetMetadataInt("SEGY", "DataEndianness"))
    {
    case 0: dataEndianness = SEGY::Endianness::BigEndian;    break;
    case 1: dataEndianness = SEGY::Endianness::LittleEndian; break;
    }
  }

  if(volumeDataLayout->IsMetadataBLOBAvailable("SEGY", "BinaryHeader"))
  {
    volumeDataLayout->GetMetadataBLOB("SEGY", "BinaryHeader", binaryHeader);
  }
  else if(volumeDataLayout->IsMetadataBLOBAvailable("", "SEGYBinaryHeader")) // This non-standard metadata is written by Bluware software that pre-dates the standardization of VDS
  {
    std::vector<uint8_t> littleEndianBinaryHeader(SEGY::BinaryFileHeaderSize);

    volumeDataLayout->GetMetadataBLOB("", "SEGYBinaryHeader", littleEndianBinaryHeader);

    binaryHeader.resize(littleEndianBinaryHeader.size());

    // Convert to big-endian
    if(headerEndianness == SEGY::Endianness::BigEndian)
    {
      for(int i = 0; i < int(littleEndianBinaryHeader.size()); i++) binaryHeader[i] = littleEndianBinaryHeader[(i < 3 * 4) ? (i ^ 3) : (i ^ 1)];
    }
  }

  if(textHeader.size() != SEGY::TextualFileHeaderSize || binaryHeader.size() != SEGY::BinaryFileHeaderSize)
  {
    OpenVDS::printError(jsonOutput, "SEGY", "Invalid SEG-Y Text/Binary headers");
    return EXIT_FAILURE;
  }

  SEGY::BinaryHeader::DataSampleFormatCode dataSampleFormatCode;

  if(volumeDataLayout->IsMetadataIntAvailable("SEGY", "DataSampleFormatCode"))
  {
    dataSampleFormatCode = SEGY::BinaryHeader::DataSampleFormatCode(volumeDataLayout->GetMetadataInt("SEGY", "DataSampleFormatCode"));
  }
  else
  {
    dataSampleFormatCode = SEGY::BinaryHeader::DataSampleFormatCode(ReadFieldFromHeader(binaryHeader.data(), SEGY::BinaryHeader::DataSampleFormatCodeHeaderField, headerEndianness));
  }

  if(dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IBMFloat && dataEndianness == SEGY::Endianness::LittleEndian)
  {
    OpenVDS::printError(jsonOutput, "SEGY", "Little-endian IBM float is not supported");
    return EXIT_FAILURE;
  }

  if(dataSampleFormatCode != SEGY::BinaryHeader::DataSampleFormatCode::IBMFloat &&
     dataSampleFormatCode != SEGY::BinaryHeader::DataSampleFormatCode::IEEEFloat)
  {
    OpenVDS::printError(jsonOutput, "SEGY", "Unsupported data sample format", fmt::format("{}", dataSampleFormatCode));
    return EXIT_FAILURE;
  }

  OpenVDS::File
    file;

  OpenVDS::Error
    error;

  file.Open(fileName.c_str(), true, true, true, error);

  if(error.code != 0)
  {
    OpenVDS::printError(jsonOutput, "VDS", "Could not open file", fileName);
    return EXIT_FAILURE;
  }

  file.Write(textHeader.data(), 0, SEGY::TextualFileHeaderSize, error) && file.Write(binaryHeader.data(), SEGY::TextualFileHeaderSize, SEGY::BinaryFileHeaderSize, error);
  if(error.code != 0)
  {
    OpenVDS::printError(jsonOutput, "VDS", "Error writing SEG-Y headers to file", fileName);
    return EXIT_FAILURE;
  }

  int lineCount = volumeDataLayout->GetDimensionNumSamples(outerDimension);

  // Find which dimension to loop over in case the data has been transposed on import (i.e. crossline-sorted binned data)
  const char *primaryKey = volumeDataLayout->GetMetadataString("SEGY", "PrimaryKey");
  for(int dimension = 1; dimension < dimensionality; dimension++)
  {
    if(strcmp(primaryKey, volumeDataLayout->GetDimensionName(dimension)) == 0)
    {
      outerDimension = dimension;
    }
  }

  // Count the total number of traces for each request
  int64_t traceCount = 1;
  for(int dimension = 1; dimension < dimensionality; dimension++)
  {
    if(dimension != outerDimension)
    {
      traceCount *= volumeDataLayout->GetDimensionNumSamples(dimension);
    }
  }

  const int sampleFormatSize = 4;
  const int sampleCount = volumeDataLayout->GetDimensionNumSamples(0);
  const int traceDataSize = sampleCount * sampleFormatSize;

  std::unique_ptr<char[]> data(new char[traceCount * traceDataSize]);
  std::unique_ptr<char[]> traceFlag(new char[traceCount]);
  std::unique_ptr<char[]> segyTraceHeader(new char[traceCount * SEGY::TraceHeaderSize]);

  int64_t
    offset = SEGY::TextualFileHeaderSize + SEGY::BinaryFileHeaderSize;

  int percentage = -1;
  for(int line = 0; line < lineCount; line++)
  {
    int new_percentage = int(line / double(lineCount) * 100);
    if (!jsonOutput && percentage != new_percentage)
    {
      percentage = new_percentage;
      fmt::print(stdout, "\33[2K\r {:3}% Done. ", percentage);
      fflush(stdout);
    }
    int min[OpenVDS::Dimensionality_Max] = {},
        max[OpenVDS::Dimensionality_Max] = {};

    for(int dimension = 0; dimension < outerDimension; dimension++)
    {
      max[dimension] = volumeDataLayout->GetDimensionNumSamples(dimension);
    }

    min[outerDimension] = line;
    max[outerDimension] = line + 1;

    auto dataRequest = accessManager.RequestVolumeSubset((void *)data.get(), traceCount * traceDataSize, dimensionGroup, 0, 0, min, max, OpenVDS::VolumeDataChannelDescriptor::Format_R32);

    max[0] = 1;
    auto traceFlagRequest = accessManager.RequestVolumeSubset((void *)traceFlag.get(), traceCount, dimensionGroup, 0, traceFlagChannel, min, max, OpenVDS::VolumeDataChannelDescriptor::Format_U8);

    max[0] = 240;
    auto segyTraceHaderRequest = accessManager.RequestVolumeSubset((void *)segyTraceHeader.get(), traceCount * SEGY::TraceHeaderSize, dimensionGroup, 0, segyTraceHeaderChannel, min, max, OpenVDS::VolumeDataChannelDescriptor::Format_U8);

    // Need to queue the writing on another thread to get max. performance
    if (!dataRequest->WaitForCompletion())
    {
      int errorCode;
      const char* errorString;
      accessManager.GetCurrentDownloadError(&errorCode, &errorString);
      OpenVDS::printError(jsonOutput, "Error in data request", errorString, fmt::format("{}", errorCode));
      assert(dataRequest->IsCanceled());
      exit(1);
    }
    if (!traceFlagRequest->WaitForCompletion())
    {
      int errorCode;
      const char* errorString;
      accessManager.GetCurrentDownloadError(&errorCode, &errorString);
      OpenVDS::printError(jsonOutput, "Error in traceFlag request", errorString, fmt::format("{}", errorCode));
      assert(traceFlagRequest->IsCanceled());
      exit(1);
    }
    if (!segyTraceHaderRequest->WaitForCompletion())
    {
      int errorCode;
      const char* errorString;
      accessManager.GetCurrentDownloadError(&errorCode, &errorString);
      OpenVDS::printError(jsonOutput, "Error in segyTraceHeader request", errorString, fmt::format("{}", errorCode));
      assert(segyTraceHaderRequest->IsCanceled());
      exit(1);
    }

    std::unique_ptr<char[]> writeBuffer(new char[traceCount * (traceDataSize + SEGY::TraceHeaderSize)]);
    int activeTraceCount = 0;

    for(int trace = 0; trace < traceCount; trace++)
    {
      if(traceFlag[trace])
      {
        // Copy trace header
        memcpy(writeBuffer.get() + activeTraceCount * (traceDataSize + SEGY::TraceHeaderSize), segyTraceHeader.get() + trace * SEGY::TraceHeaderSize, SEGY::TraceHeaderSize);

        // Convert trace data
        if(dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IBMFloat)
        {
          SEGY::Ieee2ibm(writeBuffer.get() + activeTraceCount * (traceDataSize + SEGY::TraceHeaderSize) + SEGY::TraceHeaderSize, data.get() + trace * traceDataSize, sampleCount);
        }
        else if(dataSampleFormatCode == SEGY::BinaryHeader::DataSampleFormatCode::IEEEFloat)
        {
          if(dataEndianness == SEGY::Endianness::BigEndian)
          {
            SEGY::ConvertToEndianness<SEGY::Endianness::BigEndian>(writeBuffer.get() + activeTraceCount * (traceDataSize + SEGY::TraceHeaderSize) + SEGY::TraceHeaderSize, reinterpret_cast<float *>(data.get() + trace * traceDataSize), sampleCount);
          }
          else
          {
            SEGY::ConvertToEndianness<SEGY::Endianness::LittleEndian>(writeBuffer.get() + activeTraceCount * (traceDataSize + SEGY::TraceHeaderSize) + SEGY::TraceHeaderSize, reinterpret_cast<float *>(data.get() + trace * traceDataSize), sampleCount);
          }
        }

        activeTraceCount++;
      }
    }

    file.Write(writeBuffer.get(), offset, activeTraceCount * (traceDataSize + SEGY::TraceHeaderSize), error);
    if(error.code != 0)
    {
      OpenVDS::printError(jsonOutput, "Error writing SEG-Y traces to file", fileName);
      return EXIT_FAILURE;
    }
    offset += activeTraceCount * (traceDataSize + SEGY::TraceHeaderSize);
  }
  if (!jsonOutput)
    fmt::print(stdout, "\33[2K\r 100% Done.\n", percentage);

  //double elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start_time).count();
  //fmt::print("Elapsed time is {}.\n", elapsed / 1000);

  return EXIT_SUCCESS;
}
