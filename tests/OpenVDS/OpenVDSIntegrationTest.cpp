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

GTEST_TEST(OpenVDS_integration, OpenClose)
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
}

GTEST_TEST(OpenVDS_integration, OpenCloseVDSFile)
{
  OpenVDS::Error error;
  std::string fileName = TEST_DATA_PATH "/subset.vds";

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(OpenVDS::VDSFileOpenOptions(fileName), error), &OpenVDS::Close);
  ASSERT_TRUE(handle);
}
