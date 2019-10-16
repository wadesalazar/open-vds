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
#include <cstdlib>

#include <gtest/gtest.h>

GTEST_TEST(OpenVDS_integration, SimpleVolumeDataPageRead)
{
  OpenVDS::Error error;
  OpenVDS::AWSOpenOptions options;

  options.region = TEST_AWS_REGION;
  options.bucket = TEST_AWS_BUCKET;
  options.key = TEST_AWS_OBJECTID;

  if(options.region.empty() || options.bucket.empty() || options.key.empty())
  {
    GTEST_SKIP() << "Environment variables not set";
  }

  ASSERT_TRUE(options.region.size() && options.bucket.size() && options.key.size());
  OpenVDS::VDSHandle *handle = OpenVDS::open(options, error);
  ASSERT_TRUE(handle);

  OpenVDS::VolumeDataAccessManager *dataAccessManager = OpenVDS::getDataAccessManager(handle);
  ASSERT_TRUE(dataAccessManager);

  OpenVDS::VolumeDataLayout *layout = OpenVDS::getLayout(handle);

  OpenVDS::VolumeDataPageAccessor *pageAccessor = dataAccessManager->createVolumeDataPageAccessor(layout, OpenVDS::Dimensions_012, 0, 0, 10, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly);
  ASSERT_TRUE(pageAccessor);

  int pos[OpenVDS::Dimensionality_Max] = {layout->getDimensionNumSamples(0) / 2, layout->getDimensionNumSamples(1) /2, layout->getDimensionNumSamples(2) / 2};
  OpenVDS::VolumeDataPage *page = pageAccessor->readPageAtPosition(pos);
  ASSERT_TRUE(page);

  int pitch[OpenVDS::Dimensionality_Max] = {}; 
  const void *buffer = page->getBuffer(pitch);
  ASSERT_TRUE(buffer);
  for (auto p : pitch) ASSERT_NE(p, 0);

  int min[6];
  int max[6];
  page->getMinMax(min, max);

  ASSERT_TRUE(min[0] < pos[0]);
  ASSERT_TRUE(min[1] < pos[1]);
  ASSERT_TRUE(min[2] < pos[2]);
  ASSERT_TRUE(pos[0] < max[0]);
  ASSERT_TRUE(pos[1] < max[1]);
  ASSERT_TRUE(pos[2] < max[2]);

  OpenVDS::destroy(handle);
}
