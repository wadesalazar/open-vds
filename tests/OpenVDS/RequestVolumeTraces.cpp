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
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeData.h>

#include <cstdlib>

#include <random>
#include <vector>

#include <gtest/gtest.h>
#include <fmt/format.h>

GTEST_TEST(OpenVDS_integration, SimpleRequestVolumeTraces)
{
  OpenVDS::Error error;
  OpenVDS::AWSOpenOptions options;

  options.Region = TEST_AWS_REGION;
  options.Bucket = TEST_AWS_BUCKET;
  options.Key = TEST_AWS_OBJECTID;

  if(options.Region.empty() || options.Bucket.empty() || options.Key.empty())
  {
    GTEST_SKIP() << "Environment variables not set";
  }

  ASSERT_TRUE(options.Region.size() && options.Bucket.size() && options.Key.size());
  std::unique_ptr<OpenVDS::VDSHandle, decltype(&OpenVDS::Destroy)> handle(OpenVDS::Open(options, error), &OpenVDS::Destroy);
  ASSERT_TRUE(handle);

  OpenVDS::VolumeDataAccessManager *dataAccessManager = OpenVDS::GetDataAccessManager(handle.get());
  ASSERT_TRUE(dataAccessManager);

  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(handle.get());
 
  int sampleCount0 = layout->GetDimensionNumSamples(0);
  std::vector<float> buffer(10 * sampleCount0);

  int sampleCount1 = layout->GetDimensionNumSamples(1);
  int sampleCount2 = layout->GetDimensionNumSamples(2);

  float tracePos[10][6];

  for (int trace = 0; trace < 10; trace++)
  {
    tracePos[trace][0] = 0;
    tracePos[trace][1] = (float)(trace * (sampleCount1 / 10));
    tracePos[trace][2] = (float)(trace * (sampleCount2 / 10));
    tracePos[trace][3] = 0;
    tracePos[trace][4] = 0;
    tracePos[trace][5] = 0;
  }

  int64_t requestId = dataAccessManager->RequestVolumeTraces(buffer.data(), layout, OpenVDS::Dimensions_012, 0, 0, tracePos, 10, OpenVDS::InterpolationMethod::Nearest, 0);
  dataAccessManager->WaitForCompletion(requestId);

  auto pageAccessor = dataAccessManager->CreateVolumeDataPageAccessor(layout, OpenVDS::Dimensions_012, 0, 0, 1000, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly);
  auto valueReader = dataAccessManager->Create3DInterpolatingVolumeDataAccessorR32(pageAccessor, 0.0f, OpenVDS::InterpolationMethod::Nearest);

  std::vector<float> verify(10 * sampleCount0);
  for (int trace = 0; trace < 10; trace++)
  {
    for (int i = 0; i < sampleCount0; i++)
    {
      verify[trace * sampleCount0 + i] = valueReader->GetValue(OpenVDS::FloatVector3(tracePos[trace][2], tracePos[trace][1], float(i)));
    }
  }
  ASSERT_EQ(buffer, verify);
}
