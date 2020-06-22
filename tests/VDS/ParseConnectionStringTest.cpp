/****************************************************************************
** Copyright 2020 The Open Group
** Copyright 2020 Bluware, Inc.
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

#include <VDS/ConnectionStringParser.h>
#include <gtest/gtest.h>

GTEST_TEST(VDS_integration, ParseConnectionString)
{
  OpenVDS::Error error;
  std::string empty;
  auto map = OpenVDS::ParseConnectionString(empty.data(), empty.size(), error);
  EXPECT_TRUE(map.empty());
  EXPECT_EQ(error.code, 0);

  std::string emptyValue = "Foo=hello;bar=";
  map = OpenVDS::ParseConnectionString(emptyValue.data(), emptyValue.size(), error);
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(error.code, 0);
  
  std::string emptyValue2 = "Foo=;bar=hello";
  map = OpenVDS::ParseConnectionString(emptyValue2.data(), emptyValue2.size(), error);
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(error.code, 0);

  std::string emptyKey = "Foo=bar;=hello";
  map = OpenVDS::ParseConnectionString(emptyKey.data(), emptyKey.size(), error);
  EXPECT_NE(error.code, 0);
 
  error = OpenVDS::Error();
  std::string doubleSemicolon= "Foo=bar;;hello=hello";
  map = OpenVDS::ParseConnectionString(doubleSemicolon.data(), doubleSemicolon.size(), error);
  EXPECT_EQ(error.code, 0);
  EXPECT_EQ(map.size(), 2);
  
  error = OpenVDS::Error();
  std::string manySemicolons= ";Foo=bar;;hello=hello;;";
  map = OpenVDS::ParseConnectionString(manySemicolons.data(), manySemicolons.size(), error);
  EXPECT_EQ(error.code, 0);
  EXPECT_EQ(map.size(), 2);
  
  error = OpenVDS::Error();
  std::string noEquals = "Foo;hello=hello";
  map = OpenVDS::ParseConnectionString(noEquals.data(), noEquals.size(), error);
  EXPECT_NE(error.code, 0);
  
  error = OpenVDS::Error();
  std::string noEquals2 = "Foo=bar;hello";
  map = OpenVDS::ParseConnectionString(noEquals2.data(), noEquals2.size(), error);
  EXPECT_NE(error.code, 0);
}
