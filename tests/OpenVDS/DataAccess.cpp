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

#include <array>

#include <gtest/gtest.h>

GTEST_TEST(OpenVDS_integration, SimpleVolumeDataPageRead)
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

  OpenVDS::VolumeDataAccessManager *accessManager = OpenVDS::GetAccessManager(handle.get());
  ASSERT_TRUE(accessManager);

  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(handle.get());

  OpenVDS::VolumeDataPageAccessor *pageAccessor = accessManager->CreateVolumeDataPageAccessor(layout, OpenVDS::Dimensions_012, 0, 0, 10, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly);
  ASSERT_TRUE(pageAccessor);

  int pos[OpenVDS::Dimensionality_Max] = {layout->GetDimensionNumSamples(0) / 2, layout->GetDimensionNumSamples(1) /2, layout->GetDimensionNumSamples(2) / 2};
  OpenVDS::VolumeDataPage *page = pageAccessor->ReadPageAtPosition(pos);
  ASSERT_TRUE(page);

  int pitch[OpenVDS::Dimensionality_Max] = {}; 
  const void *buffer = page->GetBuffer(pitch);
  ASSERT_TRUE(buffer);
  for (int i = 0; i < layout->GetDimensionality(); i++)
    ASSERT_NE(pitch[i], 0);

  int min[6];
  int max[6];
  page->GetMinMax(min, max);

  ASSERT_TRUE(min[0] <=  pos[0]);
  ASSERT_TRUE(min[1] <=  pos[1]);
  ASSERT_TRUE(min[2] <=  pos[2]);
  ASSERT_TRUE(pos[0] <  max[0]);
  ASSERT_TRUE(pos[1] <  max[1]);
  ASSERT_TRUE(pos[2] <  max[2]);
}

GTEST_TEST(OpenVDS_integration, SimpleVolumeDataPageReadVDSFile)
{
  OpenVDS::Error error;
  std::string fileName = TEST_DATA_PATH "/subset.vds";

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(OpenVDS::VDSFileOpenOptions(fileName), error), &OpenVDS::Close);
  ASSERT_TRUE(handle);

  OpenVDS::VolumeDataAccessManager *accessManager = OpenVDS::GetAccessManager(handle.get());
  ASSERT_TRUE(accessManager);

  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(handle.get());

  OpenVDS::VolumeDataPageAccessor *pageAccessor = accessManager->CreateVolumeDataPageAccessor(layout, OpenVDS::Dimensions_012, 0, 0, 10, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly);
  ASSERT_TRUE(pageAccessor);

  int pos[OpenVDS::Dimensionality_Max] = {layout->GetDimensionNumSamples(0) / 2, layout->GetDimensionNumSamples(1) /2, layout->GetDimensionNumSamples(2) / 2};
  OpenVDS::VolumeDataPage *page = pageAccessor->ReadPageAtPosition(pos);
  ASSERT_TRUE(page);

  int pitch[OpenVDS::Dimensionality_Max] = {}; 
  const void *buffer = page->GetBuffer(pitch);
  ASSERT_TRUE(buffer);
  for (int i = 0; i < layout->GetDimensionality(); i++)
    ASSERT_NE(pitch[i], 0);

  int min[6];
  int max[6];
  page->GetMinMax(min, max);

  ASSERT_TRUE(min[0] <=  pos[0]);
  ASSERT_TRUE(min[1] <=  pos[1]);
  ASSERT_TRUE(min[2] <=  pos[2]);
  ASSERT_TRUE(pos[0] <  max[0]);
  ASSERT_TRUE(pos[1] <  max[1]);
  ASSERT_TRUE(pos[2] <  max[2]);
}

template<typename T, size_t N> inline T (&PODArrayReference(std::array<T,N> &a))[N] { return *reinterpret_cast<T (*)[N]>(a.data()); }

GTEST_TEST(OpenVDS_integration, SimpleRequestVolumeSubset)
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

  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(handle.get());
  ASSERT_TRUE(layout);

  OpenVDS::VolumeDataAccessManager *accessManager = OpenVDS::GetAccessManager(handle.get());
  ASSERT_TRUE(accessManager);
 
  int loopDimension = 4;
  int groupSize = 100;

  int traceDimension = (loopDimension == 0) ? 1 : 0;
  int traceDimensionSize = layout->GetDimensionNumSamples(traceDimension);

  int groupDimension = (loopDimension == traceDimension + 1) ? traceDimension + 2 : traceDimension + 1;
  int groupDimensionSize = layout->GetDimensionNumSamples(groupDimension);

  if(groupSize == 0)
  {
    groupSize = groupDimensionSize;
  }

  std::array<int, OpenVDS::Dimensionality_Max> voxelMin = { 0, 0, 0, 0, 0, 0};
  std::array<int, OpenVDS::Dimensionality_Max> voxelMax = { 1, 1, 1, 1, 1, 1};

  voxelMin[traceDimension] = 0;
  voxelMax[traceDimension] = traceDimensionSize;
  voxelMin[loopDimension] = 2;
  voxelMax[loopDimension] = 2 + 1;

  std::vector<float> buffer((voxelMax[groupDimension] - voxelMin[groupDimension]) * traceDimensionSize);

  int64_t iRequestID = accessManager->RequestVolumeSubset(buffer.data(), layout, OpenVDS::Dimensions_012, 0, 0, PODArrayReference(voxelMin), PODArrayReference(voxelMax), OpenVDS::VolumeDataChannelDescriptor::Format_R32);
  ASSERT_GT(iRequestID, 0);
  bool returned = accessManager->WaitForCompletion(iRequestID);
  ASSERT_TRUE(returned);

}
