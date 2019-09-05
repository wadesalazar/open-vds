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

std::string getEnvironmentVariable(std::string variableName)
{
  const char *value = getenv(variableName.c_str());
  return value ? value : "";
}

GTEST_TEST(OpenVDS_integration, DownloadJson)
{
  OpenVDS::Error error;
  OpenVDS::AWSOpenOptions options;

  options.region = getEnvironmentVariable("OPENVDS_TEST_AWS_REGION");
  options.bucket = getEnvironmentVariable("OPENVDS_TEST_AWS_BUCKET");
  options.key = getEnvironmentVariable("OPENVDS_TEST_AWS_OBJECTID");

  if(options.region.empty() || options.bucket.empty() || options.key.empty())
  {
#ifdef GTEST_SKIP
    GTEST_SKIP();
#else
    SUCCEED() << "Skipped DownloadJson test -- Environment variables not set";
    return;
#endif
  }

  ASSERT_TRUE(options.region.size() && options.bucket.size() && options.key.size());
  OpenVDS::VDSHandle *handle = OpenVDS::open(options, error);
  ASSERT_TRUE(handle);
  OpenVDS::destroy(handle);
}
