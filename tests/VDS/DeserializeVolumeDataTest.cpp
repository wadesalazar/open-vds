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
#include <VDS/VolumeDataStore.h>
#include <IO/File.h>

#include <cstdlib>

#include <gtest/gtest.h>

namespace OpenVDS
{
  bool deserializeVolumeData(const std::vector<uint8_t> &serializedData, VolumeDataChannelDescriptor::Format format, CompressionMethod compressionMethod, bool isRenderable, const FloatRange &valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, int32_t adaptiveLevel, std::vector<uint8_t> &destination, Error &error);
}

GTEST_TEST(OpenVDS_integration, DISABLED_DeSerializeVolumeData)
{
  std::vector<uint8_t> serializedData;

  {
    OpenVDS::File chunkFile;

    OpenVDS::IOError error;

    chunkFile.open(TEST_DATA_PATH "/chunk.CompressionMethod_None", false, false, false, error);
    EXPECT_EQ(error.code, 0);

    int64_t
      fileSize = chunkFile.size(error);
    EXPECT_EQ(error.code, 0);

    serializedData.resize(fileSize);
    chunkFile.read(&serializedData[0], 0, (int32_t)fileSize, error);
    EXPECT_EQ(error.code, 0);
  }

  std::vector<uint8_t> data;

  OpenVDS::Error error;

  OpenVDS::deserializeVolumeData(serializedData, OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::CompressionMethod::None, true, OpenVDS::FloatRange(-0.07883811742067337f, 0.07883811742067337f), 1.0f, 0.0f, false, 0.0f, 0, data, error);
  EXPECT_EQ(error.code, 0);
}
