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

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "../utils/GenerateVDS.h"

template<typename T>
size_t byteSize(const std::vector<T> vec)
{
  return sizeof(T) * vec.size();
}

TEST(IOTests, InMemory)
{
  OpenVDS::Error error;
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(generateSimpleInMemory3DVDS(60,60,60), &OpenVDS::Close);
  ASSERT_TRUE(handle);

  fill3DVDSWithNoise(handle.get());

  auto layout = OpenVDS::GetLayout(handle.get());
  auto accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max] = {15, 15, 15};
  int32_t maxPos[OpenVDS::Dimensionality_Max] = {55, 55, 55};
  auto request =  accessManager.RequestVolumeSubset<float>(OpenVDS::Dimensions_012, 0, 0,minPos, maxPos);
  bool finished =  request->WaitForCompletion();
  if (!finished)
  {
    fmt::print(stderr, "Failed to download reuqest. Failing\n");
    ASSERT_TRUE(false);
  }
  std::vector<float> data = std::move(request->Data());

  float minValue = layout->GetChannelValueRangeMin(0);
  float maxValue = layout->GetChannelValueRangeMax(0);

  float range = maxValue - minValue;

  std::vector<uint32_t> histogram;
  histogram.resize(1000);

  float bucketSize = range / 1000.f;

  for (auto f : data)
  {
    ASSERT_GE(f, minValue);
    ASSERT_LE(f, maxValue);
    histogram[int((f - minValue) / bucketSize)]++;
  }
  size_t limit = data.size() / 100;
  for (auto &h : histogram)
  {
    ASSERT_LT(h, limit);
  }

}
