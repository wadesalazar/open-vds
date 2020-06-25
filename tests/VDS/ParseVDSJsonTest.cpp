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
#include <VDS/ParseVDSJson.h>
#include <IO/File.h>

#include <json/json.h>

#include <cstdlib>

#include <gtest/gtest.h>

// Copied from ParseVDSJson.cpp
bool ParseJSONFromBuffer(const std::vector<uint8_t> &json, Json::Value &root, OpenVDS::Error &error)
{
  try
  {
    Json::CharReaderBuilder rbuilder;
    rbuilder["collectComments"] = false;

    std::unique_ptr<Json::CharReader> reader(rbuilder.newCharReader());
    const char *json_begin = reinterpret_cast<const char *>(json.data());
    reader->parse(json_begin, json_begin + json.size(), &root, &error.string);

    return true;
  }
  catch(Json::Exception &e)
  {
    error.code = -1;
    error.string = e.what() + std::string(" : ") + error.string;
  }

  return false;
}

class TestLayerMetadataContainer : public OpenVDS::LayerMetadataContainer
{
  std::map<std::string, OpenVDS::MetadataStatus>
    m_metadataStatusMap;
public:
  bool GetMetadataStatus(std::string const &layerName, OpenVDS::MetadataStatus &metadataStatus) const override
  {
    auto it = m_metadataStatusMap.find(layerName);
    if(it != m_metadataStatusMap.end())
    {
      metadataStatus = it->second;
      return true;
    }
    else
    {
      metadataStatus = OpenVDS::MetadataStatus();
      return false;
    }
  }
  void SetMetadataStatus(std::string const &layerName, OpenVDS::MetadataStatus &metadataStatus, int /*pageLimit*/) override
  {
    m_metadataStatusMap[layerName] = metadataStatus;
  }
};

GTEST_TEST(VDS_integration, ParseVolumeDataLayoutAndLayerStatus)
{
  std::vector<uint8_t>
    serializedVolumeDataLayoutReference;

  // Read input VolumeDataLayout.json
  {
    OpenVDS::File volumeDataLayoutFile;

    OpenVDS::Error error;

    volumeDataLayoutFile.Open(TEST_DATA_PATH "/VolumeDataLayout.json", false, false, false, error);
    EXPECT_EQ(error.code, 0);

    int64_t
      fileSize = volumeDataLayoutFile.Size(error);
    EXPECT_EQ(error.code, 0);

    serializedVolumeDataLayoutReference.resize(fileSize);
    volumeDataLayoutFile.Read(serializedVolumeDataLayoutReference.data(), 0, (int32_t)fileSize, error);
    EXPECT_EQ(error.code, 0);
  }

  std::vector<uint8_t>
    serializedLayerStatusReference;

  // Read input LayerStatus.json
  {
    OpenVDS::File layerStatusFile;

    OpenVDS::Error error;

    layerStatusFile.Open(TEST_DATA_PATH "/LayerStatus.json", false, false, false, error);
    EXPECT_EQ(error.code, 0);

    int64_t
      fileSize = layerStatusFile.Size(error);
    EXPECT_EQ(error.code, 0);

    serializedLayerStatusReference.resize(fileSize);
    layerStatusFile.Read(serializedLayerStatusReference.data(), 0, (int32_t)fileSize, error);
    EXPECT_EQ(error.code, 0);
  }

  OpenVDS::Error
    error;

  OpenVDS::VDS
    handle;

  TestLayerMetadataContainer
    layerMetadataContainer;

  // Clear error
  error = OpenVDS::Error();

  // Parse volume data layout
  OpenVDS::ParseVolumeDataLayout(serializedVolumeDataLayoutReference, handle.layoutDescriptor, handle.axisDescriptors, handle.channelDescriptors, handle.descriptorStrings, handle.metadataContainer, error);
  EXPECT_EQ(error.code, 0);

  // Parse layer status
  OpenVDS::ParseLayerStatus(serializedLayerStatusReference, handle, layerMetadataContainer, error);
  EXPECT_EQ(error.code, 0);

  // Create volume data layout from descriptors
  CreateVolumeDataLayout(handle);

  // Serialize volume data layout
  std::vector<uint8_t>
    serializedVolumeDataLayoutResult = OpenVDS::SerializeVolumeDataLayout(handle);

  // Compare volume data layout with original
  {
    std::string
      originalString(serializedVolumeDataLayoutReference.data(), serializedVolumeDataLayoutReference.data() + serializedVolumeDataLayoutReference.size()),
      resultString(serializedVolumeDataLayoutResult.data(), serializedVolumeDataLayoutResult.data() + serializedVolumeDataLayoutResult.size());

// This won't work as the formatting of double numbers differs between implementations and the reference data was generated using Python
//  EXPECT_EQ(resultString, originalString);

    Json::Value originalJson, resultJson;

    ParseJSONFromBuffer(serializedVolumeDataLayoutReference, originalJson, error);
    EXPECT_EQ(error.code, 0);

    ParseJSONFromBuffer(serializedVolumeDataLayoutResult, resultJson, error);
    EXPECT_EQ(error.code, 0);

    EXPECT_TRUE(originalJson.compare(resultJson) == 0);
  }

  // Serialize layer status
  std::vector<uint8_t>
    serializedLayerStatusResult = OpenVDS::SerializeLayerStatus(handle, layerMetadataContainer);

  // Compare layer status with original
  {
    std::string
      originalString(serializedLayerStatusReference.data(), serializedLayerStatusReference.data() + serializedLayerStatusReference.size()),
      resultString(serializedLayerStatusResult.data(), serializedLayerStatusResult.data() + serializedLayerStatusResult.size());

// This won't work as the formatting of double numbers differs between implementations and the reference data was generated using Python
//  EXPECT_EQ(resultString, originalString);

    Json::Value originalJson, resultJson;

    ParseJSONFromBuffer(serializedLayerStatusReference, originalJson, error);
    EXPECT_EQ(error.code, 0);

    ParseJSONFromBuffer(serializedLayerStatusResult, resultJson, error);
    EXPECT_EQ(error.code, 0);

    EXPECT_TRUE(originalJson.compare(resultJson) == 0);
  }
}
