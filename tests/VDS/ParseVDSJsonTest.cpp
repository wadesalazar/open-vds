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
#include <VDS/VDS.h>
#include <IO/File.h>

#include <json/json.h>

#include <cstdlib>

#include <gtest/gtest.h>

namespace OpenVDS
{
  bool ParseVolumeDataLayout(const std::vector<uint8_t> &json, VDS &handle, Error &error);
  bool ParseLayerStatus(const std::vector<uint8_t> &json, VDS &handle, Error &error);
  bool ParseJSONFromBuffer(const std::vector<uint8_t> &json, Json::Value &root, Error &error);

  Json::Value SerializeVolumeDataLayout(VolumeDataLayoutImpl const &volumeDataLayout, MetadataContainer const &metadataContainer);
  Json::Value SerializeLayerStatusArray(VolumeDataLayoutImpl const &volumeDataLayout, LayerMetadataContainer const &layerMetadataContainer);

  std::vector<uint8_t> WriteJson(Json::Value root);
}

GTEST_TEST(VDS_integration, ParseVolumeDataLayoutAndLayerStatus)
{
  std::vector<uint8_t>
    serializedVolumeDataLayout;

  // Read input VolumeDataLayout.json
  {
    OpenVDS::File volumeDataLayoutFile;

    OpenVDS::IOError error;

    volumeDataLayoutFile.Open(TEST_DATA_PATH "/VolumeDataLayout.json", false, false, false, error);
    EXPECT_EQ(error.code, 0);

    int64_t
      fileSize = volumeDataLayoutFile.Size(error);
    EXPECT_EQ(error.code, 0);

    serializedVolumeDataLayout.resize(fileSize);
    volumeDataLayoutFile.Read(serializedVolumeDataLayout.data(), 0, (int32_t)fileSize, error);
    EXPECT_EQ(error.code, 0);
  }

  std::vector<uint8_t>
    serializedLayerStatus;

  // Read input LayerStatus.json
  {
    OpenVDS::File layerStatusFile;

    OpenVDS::IOError error;

    layerStatusFile.Open(TEST_DATA_PATH "/LayerStatus.json", false, false, false, error);
    EXPECT_EQ(error.code, 0);

    int64_t
      fileSize = layerStatusFile.Size(error);
    EXPECT_EQ(error.code, 0);

    serializedLayerStatus.resize(fileSize);
    layerStatusFile.Read(serializedLayerStatus.data(), 0, (int32_t)fileSize, error);
    EXPECT_EQ(error.code, 0);
  }

  OpenVDS::Error
    error;

  OpenVDS::VDS
    handle(nullptr);

  // Clear error
  error = OpenVDS::Error();

  // Parse volume data layout
  OpenVDS::ParseVolumeDataLayout(serializedVolumeDataLayout, handle, error);
  EXPECT_EQ(error.code, 0);

  // Parse layer status
  OpenVDS::ParseLayerStatus(serializedLayerStatus, handle, error);
  EXPECT_EQ(error.code, 0);

  // Create volume data layout from descriptors
  CreateVolumeDataLayout(handle);

  // Serialize volume data layout
  Json::Value
    volumeDataLayoutJson = OpenVDS::SerializeVolumeDataLayout(*handle.volumeDataLayout, handle.metadataContainer);

  // Compare volume data layout with original
  {
    std::vector<uint8_t>
      result = OpenVDS::WriteJson(volumeDataLayoutJson);

    std::string
      originalString(serializedVolumeDataLayout.data(), serializedVolumeDataLayout.data() + serializedVolumeDataLayout.size()),
      resultString(result.data(), result.data() + result.size());

// This won't work as the formatting of double numbers differs between implementations and the reference data was generated using Python
//  EXPECT_EQ(resultString, originalString);

    Json::Value originalJson, resultJson;

    OpenVDS::ParseJSONFromBuffer(serializedVolumeDataLayout, originalJson, error);
    EXPECT_EQ(error.code, 0);

    OpenVDS::ParseJSONFromBuffer(result, resultJson, error);
    EXPECT_EQ(error.code, 0);

    EXPECT_TRUE(originalJson.compare(resultJson) == 0);
  }

  // Serialize layer status
  Json::Value
    layerStatusJson = OpenVDS::SerializeLayerStatusArray(*handle.volumeDataLayout, handle.layerMetadataContainer);

  // Compare layer status with original
  {
    std::vector<uint8_t>
      result = OpenVDS::WriteJson(layerStatusJson);

    std::string
      originalString(serializedLayerStatus.data(), serializedLayerStatus.data() + serializedLayerStatus.size()),
      resultString(result.data(), result.data() + result.size());

// This won't work as the formatting of double numbers differs between implementations and the reference data was generated using Python
//  EXPECT_EQ(resultString, originalString);

    Json::Value originalJson, resultJson;

    OpenVDS::ParseJSONFromBuffer(serializedLayerStatus, originalJson, error);
    EXPECT_EQ(error.code, 0);

    OpenVDS::ParseJSONFromBuffer(result, resultJson, error);
    EXPECT_EQ(error.code, 0);

    EXPECT_TRUE(originalJson.compare(resultJson) == 0);
  }
}
