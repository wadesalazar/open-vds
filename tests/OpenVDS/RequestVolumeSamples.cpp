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
#include <array>

#include <gtest/gtest.h>

static inline void GenerateRandomCoordinate(std::mt19937 &generator, std::vector<std::uniform_real_distribution<float>> &dimensionDistribution, int dimensionality, float (&coordinate)[OpenVDS::Dimensionality_Max])
{
  for (int dimension = 0; dimension < OpenVDS::Dimensionality_Max; dimension++)
  {
    coordinate[dimension] = (dimension < dimensionality) ? dimensionDistribution[dimension](generator) : 0.0f;
  }
}

static inline void CoordinateToSamplePosition(OpenVDS::VolumeDataLayout *layout, int dimensionality, float (&coordinate)[OpenVDS::Dimensionality_Max])
{
  for (int dimension = 0; dimension < dimensionality; dimension++)
  {
    coordinate[dimension] = layout->GetAxisDescriptor(dimension).CoordinateToSamplePosition(coordinate[dimension]);
  }
}

GTEST_TEST(OpenVDS_integration, SimpleRequestVolumeSamples)
{
  OpenVDS::Error error;
  
  std::string url = TEST_URL;
  std::string connectionString = TEST_CONNECTION;
  if(url.empty())
  {
    GTEST_SKIP() << "Test Environment for connecting to VDS is not set";
  }

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(url, connectionString, error), &OpenVDS::Close);
  ASSERT_TRUE(handle);

  OpenVDS::VolumeDataAccessManager accessManager = OpenVDS::GetAccessManager(handle.get());
  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(handle.get());

  int32_t dimensionality = layout->GetDimensionality();

  std::mt19937 gen(5746);
  std::vector<std::uniform_real_distribution<float>> dimensionDistribution;
  dimensionDistribution.reserve(OpenVDS::Dimensionality_Max);
  for (int i = 0; i < dimensionality; i++)
  {
    float min = layout->GetDimensionMin(i) + 1; 
    float max = layout->GetDimensionMax(i);
    dimensionDistribution.emplace_back(min, max);
  }
  
  float positions[100][OpenVDS::Dimensionality_Max];

  for(int i = 0; i < 100; i++)
  {
    GenerateRandomCoordinate(gen, dimensionDistribution, dimensionality, positions[i]);
    CoordinateToSamplePosition(layout, dimensionality, positions[i]);
  }

  float buffer[100];
  auto request = accessManager.RequestVolumeSamples(buffer, sizeof(buffer) * sizeof(float), OpenVDS::Dimensions_012, 0, 0, positions, 100, OpenVDS::InterpolationMethod::Linear);
  bool success = request->WaitForCompletion(10000);
  if(!success)
  {
    if(request->IsCanceled())
    {
      int errorCode = 0;
      const char *errorMessage = nullptr;
      accessManager.GetCurrentDownloadError(&errorCode, &errorMessage);
      ASSERT_NE(errorCode, 0);
      ASSERT_NE(errorMessage, nullptr);
      FAIL() << std::string(errorMessage);
    }
    else
    {
      // We need to cancel and then wait until we can safely free the buffer
      request->CancelAndWaitForCompletion();
      FAIL() << std::string("Timeout waiting for RequestVolumeSamples");
    }
  }

  auto readAccessor = accessManager.CreateVolumeData3DInterpolatingAccessorR32(OpenVDS::Dimensions_012, 0, 0, OpenVDS::InterpolationMethod::Linear, 100, 0.0f);

  float verifyBuffer[100];
  for (int i = 0; i < 100; i++)
  {
    auto &p = positions[i];
    OpenVDS::FloatVector3 pos(p[2], p[1], p[0]);
    verifyBuffer[i] = readAccessor.GetValue(pos);
  }

  for (int i = 0; i < 100; i++)
  {
    float diff = std::abs(buffer[i] - verifyBuffer[i]);
    ASSERT_EQ(diff, 0.0f);
  }
}
