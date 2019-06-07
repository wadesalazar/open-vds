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
#include "cxxopts.hpp"

#include <cstdlib>
#include <json/json.h>

Json::Value
serializeSEGYFileInfo(SEGYFileInfo const &segyFileInfo)
{
  Json::Value
    jsonFileInfo;

  Json::Value
    jsonSegmentInfoArray(Json::ValueType::arrayValue);

  for(auto const &segySegmentInfo : segyFileInfo.m_segmentInfo)
  {
    Json::Value
      jsonSegmentInfo;

    jsonSegmentInfo["primaryKey"] = segySegmentInfo.m_primaryKey;
    jsonSegmentInfo["traceStart"] = segySegmentInfo.m_traceStart;
    jsonSegmentInfo["traceStop"]  = segySegmentInfo.m_traceStop;

    jsonSegmentInfoArray.append(jsonSegmentInfo);
  }

  jsonFileInfo["segmentInfo"] = jsonSegmentInfoArray;

  return jsonFileInfo;
}


int
main(int argc, char *argv[])
{
  cxxopts::Options options("SEGYScan", "SEGYScan - A tool to scan a SEG-Y file and create an index");

  options.add_option("", "f", "file", "File name", cxxopts::value<std::string>(), "The input SEG-Y file");

  auto args = options.parse(argc, argv);

  if(args["file"].count() == 0)
  {
    std::cerr << std::string("No input SEG-Y file(s) specified");
    return EXIT_FAILURE;
  }

  OpenVDS::File
    file;

  OpenVDS::IOError
    error;

  file.open(args["file"].as<std::string>().c_str(), false, false, false, error);

  if(error.code != 0)
  {
    std::cerr << std::string("Could not open file: ") << args["file"].as<std::string>();
    return EXIT_FAILURE;
  }

  SEGYFileInfo
    fileInfo;

  bool success = fileInfo.scan(file, HeaderField(189, FieldWidth::FourByte));

  if(!success)
  {
    std::cerr << std::string("Failed to scan file: ") << args["file"].as<std::string>();
    return EXIT_FAILURE;
  }

  Json::Value jsonFileInfo = serializeSEGYFileInfo(fileInfo);

  Json::StreamWriterBuilder wbuilder;
  wbuilder["indentation"] = "  ";
  std::string document = Json::writeString(wbuilder, jsonFileInfo);
  
  std::cout << document;

  return EXIT_SUCCESS;
}
