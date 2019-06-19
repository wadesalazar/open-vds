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

#include <gtest/gtest.h>

GTEST_TEST(DownloadJson, OpenVDS_integration)
{
  OpenVDS::Error error;
  OpenVDS::OpenOptions options;
  options.bucket = "bluware-vds-us-east-2";
  options.key = "47231464AA898322";
  options.region = "us-east-2";
  OpenVDS::VDSHandle *handle = OpenVDS::Open(options, error);
  EXPECT_TRUE(handle);
}