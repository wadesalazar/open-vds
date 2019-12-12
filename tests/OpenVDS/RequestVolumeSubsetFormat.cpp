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

TEST(OpenVDS_integration, RequestVolumeSubsetFormat)
{
  OpenVDS::Error error;
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(generateSimpleInMemory3DVDS(60,60,60), &OpenVDS::Close);
  ASSERT_TRUE(handle);

  fill3DVDSWithNoice(handle.get());

  auto layout = OpenVDS::GetLayout(handle.get());
  auto accessManager = OpenVDS::GetDataAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max];
  int32_t maxPos[OpenVDS::Dimensionality_Max];

  minPos[0] = 10; minPos[1] = 10; minPos[2] = 10;
  maxPos[0] = 50; maxPos[1] = 50; maxPos[2] = 50;

  std::vector<uint8_t> bufferByte;
  bufferByte.resize(40 * 40 * 40);
  int64_t requestByte = accessManager->RequestVolumeSubset(bufferByte.data(), layout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, OpenVDS::VolumeDataChannelDescriptor::Format_U8);

  std::vector<float> bufferFloat;
  bufferFloat.resize(40 * 40 * 40);
  int64_t requestFloat = accessManager->RequestVolumeSubset(bufferFloat.data(), layout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, OpenVDS::VolumeDataChannelDescriptor::Format_R32);
  accessManager->WaitForCompletion(requestByte);
  accessManager->WaitForCompletion(requestFloat);

  float minValue = layout->GetChannelValueRangeMin(0);
  float maxValue = layout->GetChannelValueRangeMax(0);
  float intScale = layout->GetChannelIntegerScale(0);
  float intOffset = layout->GetChannelIntegerOffset(0);

  OpenVDS::QuantizingValueConverterWithNoValue<uint8_t, float, false> valueConverter(minValue, maxValue, intScale, intOffset, 0.f, 0.f);

  for (size_t i = 0; i < bufferByte.size(); i++)
  {
    ASSERT_EQ(bufferByte[i], valueConverter.ConvertValue(bufferFloat[i]));
  }
}
