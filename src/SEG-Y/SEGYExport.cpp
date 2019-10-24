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
#include "cxxopts.hpp"
#include <fmt/format.h>

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <cstdlib>
#include <climits>
#include <json/json.h>
#include <assert.h>

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

  auto vds = OpenVDS::open(OpenVDS::AWSOpenOptions(bucket, key, region), openError);

  if(openError.code != 0)
  {
    fmt::print(stderr, "Could not open VDS: {}", openError.string);
    return EXIT_FAILURE;
  }

  OpenVDS::File
    file;

  OpenVDS::IOError
    error;

  file.open(fileName.c_str(), true, true, true, error);

  if(error.code != 0)
  {
    fmt::print(stderr, "Could not open file: {}", fileName);
    return EXIT_FAILURE;
  }

  auto dataAccessManager = OpenVDS::getDataAccessManager(vds);
  auto volumeDataLayout = dataAccessManager->getVolumeDataLayout();

  int dimensionality = dataAccessManager->getVolumeDataLayout()->getDimensionality();
  int outerDimension = std::max(2, dimensionality - 1);
  int lineCount = volumeDataLayout->getDimensionNumSamples(outerDimension);

  int64_t traceCount = 1;
  for(int dimension = 1; dimension < outerDimension; dimension++)
  {
    traceCount *= volumeDataLayout->getDimensionNumSamples(dimension);
  }

  const int elementSize = 4;
  std::unique_ptr<char[]> buffer(new char[traceCount * volumeDataLayout->getDimensionNumSamples(0) *elementSize]);
  std::unique_ptr<char[]> traceFlag(new char[traceCount]);
  std::unique_ptr<char[]> segyTraceHeader(new char[traceCount * 240]);

  if(!volumeDataLayout->isChannelAvailable("Trace"))
  {
    fmt::print(stderr, "VDS has no \"Trace\" channel");
    return EXIT_FAILURE;
  }
  int traceFlagChannel = volumeDataLayout->getChannelIndex("Trace");

  if(!volumeDataLayout->isChannelAvailable("SEGYTraceHeader"))
  {
    fmt::print(stderr, "VDS has no \"SEGYTraceHeader\" channel");
    return EXIT_FAILURE;
  }
  int segyTraceHeaderChannel = volumeDataLayout->getChannelIndex("SEGYTraceHeader");

  for(int line = 0; line < lineCount; line++)
  {
    int min[OpenVDS::Dimensionality_Max] = {},
        max[OpenVDS::Dimensionality_Max] = {};

    for(int dimension = 0; dimension < outerDimension; dimension++)
    {
      max[dimension] = volumeDataLayout->getDimensionNumSamples(dimension);
    }

    min[outerDimension] = line;
    max[outerDimension] = line + 1;

    int64_t dataRequestID = dataAccessManager->requestVolumeSubset(buffer.get(), volumeDataLayout, OpenVDS::Dimensions_012, 0, 0, min, max, OpenVDS::VolumeDataChannelDescriptor::Format_R32);

    max[0] = 1;
    int64_t traceFlagRequestID = dataAccessManager->requestVolumeSubset(buffer.get(), volumeDataLayout, OpenVDS::Dimensions_012, 0, traceFlagChannel, min, max, OpenVDS::VolumeDataChannelDescriptor::Format_U8);

    max[0] = 240;
    int64_t segyTraceHaderRequestID = dataAccessManager->requestVolumeSubset(buffer.get(), volumeDataLayout, OpenVDS::Dimensions_012, 0, segyTraceHeaderChannel, min, max, OpenVDS::VolumeDataChannelDescriptor::Format_U8);
  }

  return EXIT_SUCCESS;
}
