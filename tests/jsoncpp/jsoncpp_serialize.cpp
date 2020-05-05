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

#include <gtest/gtest.h>

#include <json/json.h>

static const char simple_serialize_data[] = R"json({
    "a" : 44.5,
    "b" :
    {
        "a" : 89,
        "b" : "some text",
        "c" : false
    },
    "c" : "more text",
    "d" : [ 1, 2, 3, 4, 5, 6, 7 ],
    "enull" : null
})json";

GTEST_TEST(JsonCppSerialize, simple_serialize)
{
  Json::Value root;
  Json::CharReaderBuilder rbuilder;
  rbuilder["collectComments"] = false;

  std::string errs;

  std::unique_ptr<Json::CharReader> reader(rbuilder.newCharReader());
  EXPECT_TRUE(reader->parse(simple_serialize_data, simple_serialize_data + sizeof(simple_serialize_data), &root, &errs));
  
  std::stringstream stream;
  Json::StreamWriterBuilder builder;
  builder["commentStyle"] = "None";
  builder["indentation"] = "    ";  // or whatever you like
  std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
  writer->write(root, &stream);
  auto str = stream.str();
  EXPECT_TRUE(strncmp(str.c_str(), simple_serialize_data, sizeof(simple_serialize_data)));
}
