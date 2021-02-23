
/****************************************************************************
** Copyright 2021 The Open Group
** Copyright 2021 Bluware, Inc.
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

#include <gtest/gtest.h>

#include "../../tools/SEGYImport/SplitUrl.h"

GTEST_TEST(SplitUrlOn, splitUrlOnParameters)
{
  std::string basic = "s3://url/to/some/file.json";
  std::string withquery_parameter = "http://url/to/some/other/file.json?magicparams=here";
  std::string nofile = "az://url/to/some/dir/";
  std::string nofile_parameter = "gs://url/to/some/dir/?foobar=params";

  {
    std::string out_url;
    std::string out_params;
    splitUrlOnParameters(basic, out_url, out_params);
    ASSERT_EQ(basic, out_url);
    ASSERT_EQ(std::string(""), out_params);
  }
  {
    std::string out_url;
    std::string out_params;
    splitUrlOnParameters(withquery_parameter, out_url, out_params);
    ASSERT_EQ(std::string("http://url/to/some/other/file.json"), out_url);
    ASSERT_EQ(std::string("?magicparams=here"), out_params);
  }
  {
    std::string out_url;
    std::string out_params;
    splitUrlOnParameters(nofile, out_url, out_params);
    ASSERT_EQ(nofile, out_url);
    ASSERT_EQ(std::string(""), out_params);
  }
  {
    std::string out_url;
    std::string out_params;
    splitUrlOnParameters(nofile_parameter, out_url, out_params);
    ASSERT_EQ("gs://url/to/some/dir/", out_url);
    ASSERT_EQ("?foobar=params",out_params);
  }
}

GTEST_TEST(SplitUrlOn, splitUrlFile)
{
  std::string basic = "s3://url/to/some/file.json";
  std::string withquery_parameter = "http://url/to/some/other/file.json?magicparams=here";
  std::string nofile = "az://url/to/some/dir/";
  std::string nofile_parameter = "gs://url/to/some/dir/?foobar=params";

  {
    std::string dirname;
    std::string basename;
    std::string parameters;
    OpenVDS::Error error;
    splitUrl(basic, dirname, basename, parameters, error);
    ASSERT_TRUE(error.code == 0);
    ASSERT_EQ(parameters, "");
    ASSERT_EQ(basename, "file.json");
    ASSERT_EQ(dirname, "s3://url/to/some");
  }
  {
    std::string dirname;
    std::string basename;
    std::string parameters;
    OpenVDS::Error error;
    splitUrl(withquery_parameter, dirname, basename, parameters, error);
    ASSERT_TRUE(error.code == 0);
    ASSERT_EQ(parameters, "?magicparams=here");
    ASSERT_EQ(basename, "file.json");
    ASSERT_EQ(dirname, "http://url/to/some/other");
  }
  {
    std::string dirname;
    std::string basename;
    std::string parameters;
    OpenVDS::Error error;
    splitUrl(nofile, dirname, basename, parameters, error);
    ASSERT_TRUE(error.code != 0);
    error = OpenVDS::Error();
  }
  {
    std::string dirname;
    std::string basename;
    std::string parameters;
    OpenVDS::Error error;
    splitUrl(nofile_parameter, dirname, basename, parameters, error);
    ASSERT_TRUE(error.code != 0);
  }

}
