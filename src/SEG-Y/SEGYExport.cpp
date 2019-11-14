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

#include "SEGY.h"
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

int
main(int argc, char *argv[])
{
  cxxopts::Options options("SEGYExport", "SEGYExport - A tool to export a volume data store (VDS) to a SEG-Y file");
  options.positional_help("<output file>");

  std::string bucket;
  std::string region;
  std::string prefix;
  std::string persistentID;
  std::string fileName;

  options.add_option("", "", "bucket", "Bucket to export from.", cxxopts::value<std::string>(bucket), "<string>");
  options.add_option("", "", "region", "Region of bucket to export from.", cxxopts::value<std::string>(region), "<string>");
  options.add_option("", "", "prefix", "Top-level prefix to prepend to all object-keys.", cxxopts::value<std::string>(prefix), "<string>");
  options.add_option("", "", "persistentID", "persistentID", cxxopts::value<std::string>(persistentID), "<ID>");

  options.add_option("", "", "output", "", cxxopts::value<std::string>(fileName), "");
  options.parse_positional("output");

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
    fmt::print(stderr, "{}", e.what());
    return EXIT_FAILURE;
  }

  if(fileName.empty())
  {
    fmt::print(stderr, "No output SEG-Y file specified");
    return EXIT_FAILURE;
  }

  // Open the VDS
  OpenVDS::Error
    openError;

  std::string
    key = !prefix.empty() ? prefix + "/" + persistentID : persistentID;

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> vds(OpenVDS::Open(OpenVDS::AWSOpenOptions(bucket, key, region), openError), &OpenVDS::Close);

  if(openError.Code != 0)
  {
    fmt::print(stderr, "Could not open VDS: {}", openError.String);
    return EXIT_FAILURE;
  }

  OpenVDS::File
    file;

  OpenVDS::IOError
    error;

  file.Open(fileName.c_str(), true, true, true, error);

  if(error.code != 0)
  {
    fmt::print(stderr, "Could not open file: {}", fileName);
    return EXIT_FAILURE;
  }

  auto dataAccessManager = OpenVDS::GetDataAccessManager(vds.get());
  auto volumeDataLayout = dataAccessManager->GetVolumeDataLayout();

  if(!volumeDataLayout->IsChannelAvailable("Trace"))
  {
    fmt::print(stderr, "VDS has no \"Trace\" channel");
    return EXIT_FAILURE;
  }
  int traceFlagChannel = volumeDataLayout->GetChannelIndex("Trace");

  if(!volumeDataLayout->IsChannelAvailable("SEGYTraceHeader"))
  {
    fmt::print(stderr, "VDS has no \"SEGYTraceHeader\" channel");
    return EXIT_FAILURE;
  }
  int segyTraceHeaderChannel = volumeDataLayout->GetChannelIndex("SEGYTraceHeader");

  if(!volumeDataLayout->IsMetadataBLOBAvailable("SEGY", "TextHeader") || !volumeDataLayout->IsMetadataBLOBAvailable("SEGY", "BinaryHeader"))
  {
    fmt::print(stderr, "SEG-Y Text/Binary headers not found");
    return EXIT_FAILURE;
  }

  // Write headers
  std::vector<uint8_t> textHeader;
  std::vector<uint8_t> binaryHeader;

  volumeDataLayout->GetMetadataBLOB("SEGY", "TextHeader", textHeader);
  volumeDataLayout->GetMetadataBLOB("SEGY", "BinaryHeader", binaryHeader);

  if(textHeader.size() != SEGY::TextualFileHeaderSize || binaryHeader.size() != SEGY::BinaryFileHeaderSize)
  {
    fmt::print(stderr, "Invalid SEG-Y Text/Binary headers");
    return EXIT_FAILURE;
  }

  file.Write(textHeader.data(), 0, SEGY::TextualFileHeaderSize, error) && file.Write(binaryHeader.data(), SEGY::TextualFileHeaderSize, SEGY::BinaryFileHeaderSize, error);
  if(error.code != 0)
  {
    fmt::print(stderr, "Error writing SEG-Y headers to file: {}", fileName);
    return EXIT_FAILURE;
  }

  int dimensionality = dataAccessManager->GetVolumeDataLayout()->GetDimensionality();
  int outerDimension = std::max(2, dimensionality - 1);
  int lineCount = volumeDataLayout->GetDimensionNumSamples(outerDimension);

  int64_t traceCount = 1;
  for(int dimension = 1; dimension < outerDimension; dimension++)
  {
    traceCount *= volumeDataLayout->GetDimensionNumSamples(dimension);
  }

  const int sampleFormatSize = 4;
  const int sampleCount = volumeDataLayout->GetDimensionNumSamples(0);
  const int traceDataSize = sampleCount * sampleFormatSize;

  std::unique_ptr<char[]> data(new char[traceCount * traceDataSize]);
  std::unique_ptr<char[]> traceFlag(new char[traceCount]);
  std::unique_ptr<char[]> segyTraceHeader(new char[traceCount * SEGY::TraceHeaderSize]);

  int64_t
    offset = SEGY::TextualFileHeaderSize + SEGY::BinaryFileHeaderSize;

  for(int line = 0; line < lineCount; line++)
  {
    int min[OpenVDS::Dimensionality_Max] = {},
        max[OpenVDS::Dimensionality_Max] = {};

    for(int dimension = 0; dimension < outerDimension; dimension++)
    {
      max[dimension] = volumeDataLayout->GetDimensionNumSamples(dimension);
    }

    min[outerDimension] = line;
    max[outerDimension] = line + 1;

    int64_t dataRequestID = dataAccessManager->RequestVolumeSubset(data.get(), volumeDataLayout, OpenVDS::Dimensions_012, 0, 0, min, max, OpenVDS::VolumeDataChannelDescriptor::Format_R32);

    max[0] = 1;
    int64_t traceFlagRequestID = dataAccessManager->RequestVolumeSubset(traceFlag.get(), volumeDataLayout, OpenVDS::Dimensions_012, 0, traceFlagChannel, min, max, OpenVDS::VolumeDataChannelDescriptor::Format_U8);

    max[0] = 240;
    int64_t segyTraceHaderRequestID = dataAccessManager->RequestVolumeSubset(segyTraceHeader.get(), volumeDataLayout, OpenVDS::Dimensions_012, 0, segyTraceHeaderChannel, min, max, OpenVDS::VolumeDataChannelDescriptor::Format_U8);

    // Need to queue the writing on another thread to get max. performance
    dataAccessManager->WaitForCompletion(dataRequestID);
    dataAccessManager->WaitForCompletion(traceFlagRequestID);
    dataAccessManager->WaitForCompletion(segyTraceHaderRequestID);

    std::unique_ptr<char[]> writeBuffer(new char[traceCount * (traceDataSize + SEGY::TraceHeaderSize)]);
    int activeTraceCount = 0;

    SEGY::Endianness headerEndianness = SEGY::Endianness::BigEndian;

    auto dataSampleFormatCode = SEGY::BinaryHeader::DataSampleFormatCode(ReadFieldFromHeader(binaryHeader.data(), SEGY::BinaryHeader::DataSampleFormatCodeHeaderField, headerEndianness));

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
          memcpy(writeBuffer.get() + activeTraceCount * (traceDataSize + SEGY::TraceHeaderSize) + SEGY::TraceHeaderSize, data.get() + trace * traceDataSize, traceDataSize);
        }
        else
        {
          assert(0 && "other formats not implemented yet");
        }

        activeTraceCount++;
      }
    }

    file.Write(writeBuffer.get(), offset, activeTraceCount * (traceDataSize + SEGY::TraceHeaderSize), error);
    if(error.code != 0)
    {
      fmt::print(stderr, "Error writing SEG-Y traces to file: {}", fileName);
      return EXIT_FAILURE;
    }
    offset += activeTraceCount * (traceDataSize + SEGY::TraceHeaderSize);
  }

  return EXIT_SUCCESS;
}
