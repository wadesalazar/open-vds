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

#include <cstdlib>

#include <gtest/gtest.h>

namespace OpenVDS
{
  bool parseVolumeDataLayout(const std::vector<uint8_t> &json, VDSHandle &handle, Error &error);
  bool parseLayerStatus(const std::vector<uint8_t> &json, VDSHandle &handle, Error &error);
}

struct TestOpenOptions : OpenVDS::OpenOptions
{
  TestOpenOptions() : OpenVDS::OpenOptions(OpenVDS::OpenOptions::File) {}
};


GTEST_TEST(OpenVDS_integration, ParseVolumeDataLayoutJson)
{
  std::vector<uint8_t> 
    jsonVolumeDataLayout;

  // Read input VolumeDataLayout.json
  {
    OpenVDS::File volumeDataLayoutFile;

    OpenVDS::IOError error;

    volumeDataLayoutFile.open(TEST_DATA_PATH "/VolumeDataLayout.json", false, false, false, error);
    EXPECT_EQ(error.code, 0);

    int64_t
      fileSize = volumeDataLayoutFile.size(error);
    EXPECT_EQ(error.code, 0);

    jsonVolumeDataLayout.resize(fileSize + 1); // remember null-termination
    volumeDataLayoutFile.read(&jsonVolumeDataLayout[0], 0, (int32_t)fileSize, error);
    EXPECT_EQ(error.code, 0);
  }

  OpenVDS::Error
    error;

  OpenVDS::VDSHandle
    handle(TestOpenOptions(), error);

  // Clear error
  error = OpenVDS::Error();

  OpenVDS::parseVolumeDataLayout(jsonVolumeDataLayout, handle, error);
  EXPECT_EQ(error.code, 0);
}
