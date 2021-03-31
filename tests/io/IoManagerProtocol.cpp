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

#include <OpenVDS/OpenVDS.h>
#include <IO/IOManagerInMemory.h>

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <chrono>

TEST(IOManagerProtocolTests, thread)
{
  std::string url = TEST_URL;
  std::string connectionString = TEST_CONNECTION;
  if(url.empty())
  {
    GTEST_SKIP() << "Test Environment for connecting to VDS is not set";
  }
  OpenVDS::Error error;
  std::unique_ptr<OpenVDS::IOManager> iomanager(OpenVDS::IOManager::CreateIOManager(url, connectionString, OpenVDS::IOManager::ReadWrite, error));

  auto data = std::make_shared<std::vector<uint8_t>>();
  const char hello_world[] = "hello world";
  data->insert(data->end(), hello_world, hello_world + sizeof(hello_world) - 1);

  auto calling_thread_id = std::this_thread::get_id();
  bool callback_called = false;

  auto request = iomanager->WriteObject("IoManagerProtocolTest/thread_test", "thread_test", "application/text", {}, data, [&calling_thread_id, &callback_called](const OpenVDS::Request& request, const OpenVDS::Error& error)
  {
    ASSERT_NE(calling_thread_id, std::this_thread::get_id());
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    callback_called = true;
  });
  ASSERT_FALSE(callback_called);
  ASSERT_TRUE(request->WaitForFinish(error));
  ASSERT_TRUE(callback_called);

}
