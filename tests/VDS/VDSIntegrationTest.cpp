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

#include <cstdlib>

#include <gtest/gtest.h>

GTEST_TEST(OpenVDS_integration, DownloadJson)
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
}
