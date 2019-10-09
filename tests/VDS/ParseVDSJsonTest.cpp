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

#include <OpenVDS/OpenVDS.h>
#include <OpenVDSHandle.h>
#include <IO/File.h>

#include <json/json.h>

#include <cstdlib>

#include <gtest/gtest.h>

namespace OpenVDS
{
  bool parseVolumeDataLayout(const std::vector<uint8_t> &json, VDSHandle &handle, Error &error);
  bool parseLayerStatus(const std::vector<uint8_t> &json, VDSHandle &handle, Error &error);
  bool parseJSONFromBuffer(const std::vector<uint8_t> &json, Json::Value &root, Error &error);

  Json::Value serializeVolumeDataLayout(VolumeDataLayout const &volumeDataLayout, MetadataContainer const &metadataContainer);
  Json::Value serializeLayerStatusArray(VolumeDataLayout const &volumeDataLayout, LayerMetadataContainer const &layerMetadataContainer);

  std::vector<uint8_t> writeJson(Json::Value root);
}

struct TestOpenOptions : OpenVDS::OpenOptions
{
  TestOpenOptions() : OpenVDS::OpenOptions(OpenVDS::OpenOptions::File) {}
};


GTEST_TEST(VDS_integration, ParseVolumeDataLayoutAndLayerStatus)
{
  std::vector<uint8_t>
    serializedVolumeDataLayout;

  // Read input VolumeDataLayout.json
  {
    OpenVDS::File volumeDataLayoutFile;

    OpenVDS::IOError error;

    volumeDataLayoutFile.open(TEST_DATA_PATH "/VolumeDataLayout.json", false, false, false, error);
    EXPECT_EQ(error.code, 0);

    int64_t
      fileSize = volumeDataLayoutFile.size(error);
    EXPECT_EQ(error.code, 0);

    serializedVolumeDataLayout.resize(fileSize);
    volumeDataLayoutFile.read(serializedVolumeDataLayout.data(), 0, (int32_t)fileSize, error);
    EXPECT_EQ(error.code, 0);
  }

  std::vector<uint8_t>
    serializedLayerStatus;

  // Read input LayerStatus.json
  {
    OpenVDS::File layerStatusFile;

    OpenVDS::IOError error;

    layerStatusFile.open(TEST_DATA_PATH "/LayerStatus.json", false, false, false, error);
    EXPECT_EQ(error.code, 0);

    int64_t
      fileSize = layerStatusFile.size(error);
    EXPECT_EQ(error.code, 0);

    serializedLayerStatus.resize(fileSize);
    layerStatusFile.read(serializedLayerStatus.data(), 0, (int32_t)fileSize, error);
    EXPECT_EQ(error.code, 0);
  }

  OpenVDS::Error
    error;

  OpenVDS::VDSHandle
    handle(TestOpenOptions(), error);

  // Clear error
  error = OpenVDS::Error();

  // Parse volume data layout
  OpenVDS::parseVolumeDataLayout(serializedVolumeDataLayout, handle, error);
  EXPECT_EQ(error.code, 0);

  // Parse layer status
  OpenVDS::parseLayerStatus(serializedLayerStatus, handle, error);
  EXPECT_EQ(error.code, 0);

  // Create volume data layout from descriptors
  createVolumeDataLayout(handle);

  // Serialize volume data layout
  Json::Value
    volumeDataLayoutJson = OpenVDS::serializeVolumeDataLayout(*handle.volumeDataLayout, handle.metadataContainer);

  // Compare volume data layout with original
  {
    std::vector<uint8_t>
      result = OpenVDS::writeJson(volumeDataLayoutJson);

    std::string
      originalString(serializedVolumeDataLayout.data(), serializedVolumeDataLayout.data() + serializedVolumeDataLayout.size()),
      resultString(result.data(), result.data() + result.size());

// This won't work as the formatting of double numbers differs between implementations and the reference data was generated using Python
//  EXPECT_EQ(resultString, originalString);

    Json::Value originalJson, resultJson;

    OpenVDS::parseJSONFromBuffer(serializedVolumeDataLayout, originalJson, error);
    EXPECT_EQ(error.code, 0);

    OpenVDS::parseJSONFromBuffer(result, resultJson, error);
    EXPECT_EQ(error.code, 0);

    EXPECT_TRUE(originalJson.compare(resultJson) == 0);
  }

  // Serialize layer status
  Json::Value
    layerStatusJson = OpenVDS::serializeLayerStatusArray(*handle.volumeDataLayout, handle.layerMetadataContainer);

  // Compare layer status with original
  {
    std::vector<uint8_t>
      result = OpenVDS::writeJson(layerStatusJson);

    std::string
      originalString(serializedLayerStatus.data(), serializedLayerStatus.data() + serializedLayerStatus.size()),
      resultString(result.data(), result.data() + result.size());

// This won't work as the formatting of double numbers differs between implementations and the reference data was generated using Python
//  EXPECT_EQ(resultString, originalString);

    Json::Value originalJson, resultJson;

    OpenVDS::parseJSONFromBuffer(serializedLayerStatus, originalJson, error);
    EXPECT_EQ(error.code, 0);

    OpenVDS::parseJSONFromBuffer(result, resultJson, error);
    EXPECT_EQ(error.code, 0);

    EXPECT_TRUE(originalJson.compare(resultJson) == 0);
  }
}
