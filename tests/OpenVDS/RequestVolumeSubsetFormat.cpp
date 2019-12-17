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
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/ValueConversion.h>

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "../utils/GenerateVDS.h"

void setupIntegrationTestHandle(std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> &handle)
{
    OpenVDS::Error error;
    OpenVDS::AWSOpenOptions options;

    options.region = TEST_AWS_REGION;
    options.bucket = TEST_AWS_BUCKET;
    options.key = TEST_AWS_OBJECTID;

    if(options.region.empty() || options.bucket.empty() || options.key.empty())
    {
      //GTEST_SKIP() << "Environment variables not set";
    }

    //ASSERT_TRUE(options.region.size() && options.bucket.size() && options.key.size());
    handle.reset(OpenVDS::Open(options, error));
    //ASSERT_TRUE(handle);
}

void setupNoiceTestHandle(std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> &handle)
{
  OpenVDS::Error error;
  handle.reset(generateSimpleInMemory3DVDS(60,60,60));

  fill3DVDSWithNoice(handle.get());
}

struct RequestSharedData
{
  RequestSharedData()
    : handle(nullptr, &OpenVDS::Close)
  {
    setupNoiceTestHandle(handle);
    layout = OpenVDS::GetLayout(handle.get());
    accessManager = OpenVDS::GetAccessManager(handle.get());

    minPos[0] = 10; minPos[1] = 10; minPos[2] = 10;
    maxPos[0] = 50; maxPos[1] = 50; maxPos[2] = 50;
    voxelCount = (maxPos[0] - minPos[0]) * (maxPos[1] - minPos[1]) * (maxPos[2] - minPos[2]);

    bufferFloat.resize(voxelCount);
    int64_t requestFloat = accessManager->RequestVolumeSubset(bufferFloat.data(), layout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, OpenVDS::VolumeDataChannelDescriptor::Format_R32);
    accessManager->WaitForCompletion(requestFloat);
  }
  ~RequestSharedData()
  {

  }
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle;
  OpenVDS::VolumeDataLayout *layout;
  OpenVDS::VolumeDataAccessManager *accessManager;
  int32_t minPos[OpenVDS::Dimensionality_Max];
  int32_t maxPos[OpenVDS::Dimensionality_Max];
  int32_t voxelCount;
  std::vector<float> bufferFloat;
};


class RequestVolumeSubsetFormat : public ::testing::Test
{
protected:
  static void SetupTestSuit()
  {

  }
  static void TearDownTestSuit()
  {

  }

  virtual void SetUp() override
  {
    shared_data.reset(new RequestSharedData());
  }

  virtual void TearDown() override
  {
    shared_data.reset();
  }

  static std::unique_ptr<RequestSharedData> shared_data;
};

std::unique_ptr<RequestSharedData> RequestVolumeSubsetFormat::shared_data;

TEST_F(RequestVolumeSubsetFormat, test1Byte)
{
  auto *shared_data = RequestVolumeSubsetFormat::shared_data.get();

  std::vector<uint8_t> buffer;
  buffer.resize(shared_data->voxelCount);
  int64_t request= shared_data->accessManager->RequestVolumeSubset(buffer.data(), shared_data->layout, OpenVDS::Dimensions_012, 0, 0, shared_data->minPos, shared_data->maxPos, OpenVDS::VolumeDataChannelDescriptor::Format_U8);

  shared_data->accessManager->WaitForCompletion(request);

  float minValue = shared_data->layout->GetChannelValueRangeMin(0);
  float maxValue = shared_data->layout->GetChannelValueRangeMax(0);
  float intScale = shared_data->layout->GetChannelIntegerScale(0);
  float intOffset = shared_data->layout->GetChannelIntegerOffset(0);

  OpenVDS::QuantizingValueConverterWithNoValue<uint8_t, float, false> valueConverter(minValue, maxValue, intScale, intOffset, 0.f, 0.f);

  for (size_t i = 0; i < buffer.size(); i++)
  {
    ASSERT_EQ(buffer[i], valueConverter.ConvertValue(shared_data->bufferFloat[i]));
  }
}

TEST_F(RequestVolumeSubsetFormat, test1Bit)
{
  auto *shared_data = RequestVolumeSubsetFormat::shared_data.get();

  std::vector<uint8_t> buffer;
  buffer.resize(shared_data->voxelCount / 8 + 1);
  int64_t request= shared_data->accessManager->RequestVolumeSubset(buffer.data(), shared_data->layout, OpenVDS::Dimensions_012, 0, 0, shared_data->minPos, shared_data->maxPos, OpenVDS::VolumeDataChannelDescriptor::Format_1Bit);
  shared_data->accessManager->WaitForCompletion(request);

  for (size_t i = 0; i < shared_data->voxelCount; i++)
  {
    uint8_t data = (buffer[i/8] & (1 << (i % 8))) >> (i % 8);
    uint8_t value = shared_data->bufferFloat[i] != shared_data->layout->GetChannelNoValue(0);
    ASSERT_TRUE(bool(data) == bool(value));
  }
}

TEST(ReqeustVolumeSubsetFormat, source1Bit)
{
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(generateSimpleInMemory3DVDS(60,60,60, OpenVDS::VolumeDataChannelDescriptor::Format_1Bit), OpenVDS::Close);
  fill3DVDSWithBitNoice(handle.get());
  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(handle.get());
  OpenVDS::VolumeDataAccessManager *accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max];
  int32_t maxPos[OpenVDS::Dimensionality_Max];
  int32_t voxelCount;
  minPos[0] = 10; minPos[1] = 10; minPos[2] = 10;
  maxPos[0] = 50; maxPos[1] = 50; maxPos[2] = 50;
  voxelCount = (maxPos[0] - minPos[0]) * (maxPos[1] - minPos[1]) * (maxPos[2] - minPos[2]);

  std::vector<float> bufferFloat;
  bufferFloat.resize(voxelCount);
  int64_t requestFloat = accessManager->RequestVolumeSubset(bufferFloat.data(), layout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, OpenVDS::VolumeDataChannelDescriptor::Format_R32);
  accessManager->WaitForCompletion(requestFloat);

  std::vector<uint8_t> bufferByte;
  bufferByte.resize(voxelCount / 8);
  int64_t requestByte = accessManager->RequestVolumeSubset(bufferByte.data(), layout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, OpenVDS::VolumeDataChannelDescriptor::Format_1Bit);
  accessManager->WaitForCompletion(requestByte);

  for (int i = 0; i < voxelCount; i++)
  {
    float floatVal = bufferFloat[i];
    bool floatBool = bool(floatVal);
    uint8_t byteBool = bufferByte[i / 8] & (uint8_t(1) << (i%8));
    bool boolBool = bool(byteBool);
    ASSERT_EQ(floatBool, boolBool);
  }

}
