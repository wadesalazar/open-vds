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

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataAccess.h>

#include <IO/IOManagerInMemory.h>
#include <VDS/ThreadPool.h>

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "../utils/GenerateVDS.h"
#include "../utils/FacadeIOManager.h"


struct IOErrorHandlingFixture : public ::testing::Test
{
   protected:
  static void SetUpTestSuite() {
    OpenVDS::Error error;
    OpenVDS::InMemoryOpenOptions options;
    inMemoryIOManager = new OpenVDS::IOManagerInMemory(options, error);

    IOManagerFacade* facadeIoManager = new IOManagerFacade(inMemoryIOManager);

    std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(generateSimpleInMemory3DVDS(60, 60, 60, OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32, facadeIoManager), &OpenVDS::Close);
    ASSERT_TRUE(handle);
    fill3DVDSWithNoise(handle.get());
    handle.reset();
  }

  static void TearDownTestSuite() {
    delete inMemoryIOManager; 
    inMemoryIOManager = NULL;
  }

  virtual void SetUp() { }
  virtual void TearDown() { }

  static OpenVDS::IOManagerInMemory *inMemoryIOManager;
};

OpenVDS::IOManagerInMemory *IOErrorHandlingFixture::inMemoryIOManager = NULL;

TEST_F(IOErrorHandlingFixture, ErrorHandlingCorruptMetadata)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);

  facadeIoManager->m_data["Dimensions_012LOD0/ChunkMetadata/0"].error.code = 0;

  auto layout = OpenVDS::GetLayout(handle.get());
  auto accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max] = { 15, 15, 15 };
  int32_t maxPos[OpenVDS::Dimensionality_Max] = { 55, 55, 55 };
  std::vector<float> data;
  data.resize((maxPos[0] - minPos[0]) * (maxPos[1] - minPos[1]) * (maxPos[2] - minPos[2]));
  int64_t request = accessManager->RequestVolumeSubset(data.data(), layout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, OpenVDS::VolumeDataChannelDescriptor::Format_R32);
  bool finished = accessManager->WaitForCompletion(request);
  ASSERT_FALSE(finished);
  ASSERT_TRUE(accessManager->IsCanceled(request));
}

TEST_F(IOErrorHandlingFixture, ErrorHandlingMetadataHttpError)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);

  facadeIoManager->m_data["Dimensions_012LOD0/ChunkMetadata/0"].error.code = 403;
  facadeIoManager->m_data["Dimensions_012LOD0/ChunkMetadata/0"].error.string = "Test test meta deleted test";

  auto layout = OpenVDS::GetLayout(handle.get());
  auto accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max] = { 15, 15, 15 };
  int32_t maxPos[OpenVDS::Dimensionality_Max] = { 55, 55, 55 };
  std::vector<float> data;
  data.resize((maxPos[0] - minPos[0]) * (maxPos[1] - minPos[1]) * (maxPos[2] - minPos[2]));
  int64_t request = accessManager->RequestVolumeSubset(data.data(), layout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, OpenVDS::VolumeDataChannelDescriptor::Format_R32);
  bool finished = accessManager->WaitForCompletion(request);
  ASSERT_FALSE(finished);
  ASSERT_TRUE(accessManager->IsCanceled(request));
}

TEST_F(IOErrorHandlingFixture, ErrorHandlingCorruptChunk)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);

  std::string str = "THIS IS INVALID CHUNK DATA";
  auto fiveData = facadeIoManager->m_data["Dimensions_012LOD0/5"].data;
  fiveData.insert(fiveData.end(), str.begin(), str.end());

  auto layout = OpenVDS::GetLayout(handle.get());
  auto accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max] = { 15, 15, 15 };
  int32_t maxPos[OpenVDS::Dimensionality_Max] = { 55, 55, 55 };
  std::vector<float> data;
  data.resize((maxPos[0] - minPos[0]) * (maxPos[1] - minPos[1]) * (maxPos[2] - minPos[2]));
  int64_t request = accessManager->RequestVolumeSubset(data.data(), layout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, OpenVDS::VolumeDataChannelDescriptor::Format_R32);
  bool finished = accessManager->WaitForCompletion(request);
  ASSERT_FALSE(finished);
  ASSERT_TRUE(accessManager->IsCanceled(request));
}

TEST_F(IOErrorHandlingFixture, ErrorHandlingChunkHttpError)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);

  facadeIoManager->m_data["Dimensions_012LOD0/4"].error.code = 402;
  facadeIoManager->m_data["Dimensions_012LOD0/4"].error.string = "Four four four";

  auto layout = OpenVDS::GetLayout(handle.get());
  auto accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max] = { 15, 15, 15 };
  int32_t maxPos[OpenVDS::Dimensionality_Max] = { 55, 55, 55 };
  std::vector<float> data;
  data.resize((maxPos[0] - minPos[0]) * (maxPos[1] - minPos[1]) * (maxPos[2] - minPos[2]));
  int64_t request = accessManager->RequestVolumeSubset(data.data(), layout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, OpenVDS::VolumeDataChannelDescriptor::Format_R32);
  bool finished = accessManager->WaitForCompletion(request);
  ASSERT_FALSE(finished);
  ASSERT_TRUE(accessManager->IsCanceled(request));
}

TEST_F(IOErrorHandlingFixture, ErrorHandlingVDSJsonHttpError)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);

  facadeIoManager->m_data["VolumeDataLayout"].error.code = 402;
  facadeIoManager->m_data["VolumeDataLayout"].error.string = "VolumeDataLayout is missing";

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);
  ASSERT_FALSE(handle);
}

TEST_F(IOErrorHandlingFixture, ErrorHandlingVDSInvalidJson)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);

  const char data[] = R"json({ "hello": "world" } )json";
  auto &volumedatalayout = facadeIoManager->m_data["VolumeDataLayout"].data;
  volumedatalayout.insert(volumedatalayout.end(), data, data + sizeof(data));

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);
  ASSERT_FALSE(handle);
}

TEST_F(IOErrorHandlingFixture, ErrorHandlingLayerStatusJsonHttpError)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);

  facadeIoManager->m_data["LayerStatus"].error.code = 402;
  facadeIoManager->m_data["LayerStatus"].error.string = "LayerStatus is missing";

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);
  ASSERT_FALSE(handle);
}

TEST_F(IOErrorHandlingFixture, ErrorHandlingLayerStatusInvalidJson)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);

  const char data[] = R"json({ "hello": "world" } )json";
  auto &volumedatalayout = facadeIoManager->m_data["LayerStatus"].data;
  volumedatalayout.insert(volumedatalayout.end(), data, data + sizeof(data));

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);
  ASSERT_FALSE(handle);
}

