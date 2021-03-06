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
    inMemoryIOManager = OpenVDS::IOManagerInMemory::CreateIOManager(options, OpenVDS::IOManager::AccessPattern::ReadWrite, error);

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

  static OpenVDS::IOManager *inMemoryIOManager;
};

OpenVDS::IOManager *IOErrorHandlingFixture::inMemoryIOManager = NULL;

TEST_F(IOErrorHandlingFixture, ErrorHandlingCorruptMetadata)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);

  facadeIoManager->m_data["Dimensions_012LOD0/ChunkMetadata/0"].error.code = 0;

  auto accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max] = { 15, 15, 15 };
  int32_t maxPos[OpenVDS::Dimensionality_Max] = { 55, 55, 55 };
  auto request = accessManager.RequestVolumeSubset<float>(OpenVDS::Dimensions_012, 0, 0, minPos, maxPos);
  bool finished = request->WaitForCompletion();
  ASSERT_FALSE(finished);
  ASSERT_TRUE(request->IsCanceled());
}

TEST_F(IOErrorHandlingFixture, ErrorHandlingMetadataHttpError)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);

  facadeIoManager->m_data["Dimensions_012LOD0/ChunkMetadata/0"].error.code = 403;
  facadeIoManager->m_data["Dimensions_012LOD0/ChunkMetadata/0"].error.string = "Test test meta deleted test";

  auto accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max] = { 15, 15, 15 };
  int32_t maxPos[OpenVDS::Dimensionality_Max] = { 55, 55, 55 };
  auto request = accessManager.RequestVolumeSubset<float>(OpenVDS::Dimensions_012, 0, 0, minPos, maxPos);
  bool finished = request->WaitForCompletion();
  ASSERT_FALSE(finished);
  ASSERT_TRUE(request->IsCanceled());
}

TEST_F(IOErrorHandlingFixture, ErrorHandlingCorruptChunk)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);

  std::string str = "THIS IS INVALID CHUNK DATA";
  auto fiveData = facadeIoManager->m_data["Dimensions_012LOD0/5"].data;
  fiveData.insert(fiveData.end(), str.begin(), str.end());

  auto accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max] = { 15, 15, 15 };
  int32_t maxPos[OpenVDS::Dimensionality_Max] = { 55, 55, 55 };
  auto request = accessManager.RequestVolumeSubset<float>(OpenVDS::Dimensions_012, 0, 0, minPos, maxPos);
  bool finished = request->WaitForCompletion();
  ASSERT_FALSE(finished);
  ASSERT_TRUE(request->IsCanceled());
}

TEST_F(IOErrorHandlingFixture, ErrorHandlingChunkHttpError)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);

  facadeIoManager->m_data["Dimensions_012LOD0/4"].error.code = 402;
  facadeIoManager->m_data["Dimensions_012LOD0/4"].error.string = "Four four four";

  auto accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max] = { 15, 15, 15 };
  int32_t maxPos[OpenVDS::Dimensionality_Max] = { 55, 55, 55 };
  auto request = accessManager.RequestVolumeSubset<float>(OpenVDS::Dimensions_012, 0, 0, minPos, maxPos);
  bool finished = request->WaitForCompletion();
  ASSERT_FALSE(finished);
  ASSERT_TRUE(request->IsCanceled());
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


static bool strContains(const std::string& searchIn, const std::string& searchFor)
{
  return searchIn.find(searchFor) != std::string::npos;
}

class SyncTransfer : public OpenVDS::TransferDownloadHandler
{
public:
  void HandleObjectSize(int64_t size) override { }
  void HandleObjectLastWriteTime(const std::string& lastWriteTimeISO8601) override {}
  void HandleMetadata(const std::string& key, const std::string& header) override { headers.emplace_back(key, header); }
  void HandleData(std::vector<uint8_t>&& data) override { this->data = std::move(data); }
  void Completed(const OpenVDS::Request& request, const OpenVDS::Error& error) override {}

  std::vector<uint8_t> data;
  std::vector<std::pair<std::string, std::string>> headers;
};

TEST_F(IOErrorHandlingFixture, ErrorHandlingMissingMetadataTag)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);

  std::string objectName = "Dimensions_012LOD0/1";
  auto syncTransfer = std::make_shared<SyncTransfer>();
  auto req = IOErrorHandlingFixture::inMemoryIOManager->ReadObject(objectName, syncTransfer);
  ASSERT_TRUE(req->WaitForFinish(error));
  auto& object = facadeIoManager->m_data[objectName];
  object.data = std::move(syncTransfer->data);
  object.metaHeader = std::move(syncTransfer->headers);
  object.metaHeader.erase(
    std::remove_if(object.metaHeader.begin(), object.metaHeader.end(), [](const std::pair<std::string, std::string>& a)
     {
       return strContains(a.first, "vdschunkmetadata");
     })
    , object.metaHeader.end());

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);
  ASSERT_TRUE(handle);
  auto accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max] = { 15, 15, 15 };
  int32_t maxPos[OpenVDS::Dimensionality_Max] = { 55, 55, 55 };
  auto request = accessManager.RequestVolumeSubset<float>(OpenVDS::Dimensions_012, 0, 0, minPos, maxPos);
  bool finished = request->WaitForCompletion();
  ASSERT_TRUE(finished);
}

TEST_F(IOErrorHandlingFixture, ErrorHandlingChangedMetadataTag)
{
  OpenVDS::Error error;
  IOManagerFacade *facadeIoManager = new IOManagerFacade(IOErrorHandlingFixture::inMemoryIOManager);

  std::string objectName = "Dimensions_012LOD0/1";
  auto syncTransfer = std::make_shared<SyncTransfer>();
  auto req = IOErrorHandlingFixture::inMemoryIOManager->ReadObject(objectName, syncTransfer);
  ASSERT_TRUE(req->WaitForFinish(error));
  auto& object = facadeIoManager->m_data[objectName];
  object.data = std::move(syncTransfer->data);
  object.metaHeader = std::move(syncTransfer->headers);
  for (auto& metaHeader : object.metaHeader)
  {
    if (strContains(metaHeader.first, "vdschunkmetadata"))
    {
      if (metaHeader.second[4] == 'a')
        metaHeader.second[4] = 'b';
      else
        metaHeader.second[4] = 'a';
    }
  }
  
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(facadeIoManager, error), OpenVDS::Close);
  ASSERT_TRUE(handle);
  auto accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max] = { 15, 15, 15 };
  int32_t maxPos[OpenVDS::Dimensionality_Max] = { 55, 55, 55 };
  auto request = accessManager.RequestVolumeSubset<float>(OpenVDS::Dimensions_012, 0, 0, minPos, maxPos);
  bool finished = request->WaitForCompletion();
  ASSERT_FALSE(finished);
  int errorCode = 0;
  const char* errorString = nullptr;
  accessManager.GetCurrentDownloadError(&errorCode, &errorString);
  ASSERT_TRUE(strContains(errorString, "Inconsistent metadata"));
  ASSERT_TRUE(request->IsCanceled());
}

TEST(IOErrorHandlingUpload, ErrorHandlingVolumeDataLayoutHttpError)
{
  OpenVDS::Error error;
  int negativeMargin = 4;
  int positiveMargin = 4;
  int brickSize2DMultiplier = 4;
  auto lodLevels = OpenVDS::VolumeDataLayoutDescriptor::LODLevels_None;
  auto layoutOptions = OpenVDS::VolumeDataLayoutDescriptor::Options_None;
  OpenVDS::VolumeDataLayoutDescriptor layoutDescriptor(OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32, negativeMargin, positiveMargin, brickSize2DMultiplier, lodLevels, layoutOptions);

  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors;
  axisDescriptors.emplace_back(100, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE, "ms", 0.0f, 4.f);
  axisDescriptors.emplace_back(100, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE, "", 1932.f, 2536.f);
  axisDescriptors.emplace_back(100, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE,    "", 9985.f, 10369.f);

  std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors;
  float rangeMin = -0.1234f;
  float rangeMax = 0.1234f;
  float intScale;
  float intOffset;
  getScaleOffsetForFormat(rangeMin, rangeMax, true, OpenVDS::VolumeDataChannelDescriptor::Format_U32, intScale, intOffset);
  channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_U32, OpenVDS::VolumeDataChannelDescriptor::Components_1, AMPLITUDE_ATTRIBUTE_NAME, "", rangeMin, rangeMax, OpenVDS::VolumeDataMapping::Direct, 1, OpenVDS::VolumeDataChannelDescriptor::Default, 0.f, intScale, intOffset);

  OpenVDS::MetadataContainer metadataContainer;

  std::unique_ptr<OpenVDS::IOManager> inMemoryIO(OpenVDS::IOManagerInMemory::CreateIOManagerInMemory("", error));
  IOManagerFacade *ioManager = new IOManagerFacade(inMemoryIO.get());
  auto &object = ioManager->m_data["VolumeDataLayout"];
  object.error.code = 456;
  object.error.string = "This tests refuses to accept your VolumeDataLayout";
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Create(ioManager, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, error), OpenVDS::Close);

  ASSERT_FALSE(handle);
  ASSERT_EQ(error.code, 456);
  ASSERT_TRUE(error.string.size());
}

TEST(IOErrorHandlingUpload, ErrorHandlingChunkHttpError)
{
  OpenVDS::Error error;
  int negativeMargin = 4;
  int positiveMargin = 4;
  int brickSize2DMultiplier = 4;
  auto lodLevels = OpenVDS::VolumeDataLayoutDescriptor::LODLevels_None;
  auto layoutOptions = OpenVDS::VolumeDataLayoutDescriptor::Options_None;
  OpenVDS::VolumeDataLayoutDescriptor layoutDescriptor(OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32, negativeMargin, positiveMargin, brickSize2DMultiplier, lodLevels, layoutOptions);

  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors;
  axisDescriptors.emplace_back(63, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE, "ms", 0.0f, 4.f);
  axisDescriptors.emplace_back(32, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE, "", 1932.f, 2536.f);
  axisDescriptors.emplace_back(32, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE,    "", 9985.f, 10369.f);

  std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors;
  float rangeMin = -0.1234f;
  float rangeMax = 0.1234f;
  float intScale;
  float intOffset;
  getScaleOffsetForFormat(rangeMin, rangeMax, true, OpenVDS::VolumeDataChannelDescriptor::Format_U32, intScale, intOffset);
  channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_U32, OpenVDS::VolumeDataChannelDescriptor::Components_1, AMPLITUDE_ATTRIBUTE_NAME, "", rangeMin, rangeMax, OpenVDS::VolumeDataMapping::Direct, 1, OpenVDS::VolumeDataChannelDescriptor::Default, 0.f, intScale, intOffset);

  OpenVDS::MetadataContainer metadataContainer;

  std::unique_ptr<OpenVDS::IOManager> inMemoryIO(OpenVDS::IOManagerInMemory::CreateIOManagerInMemory("", error));
  IOManagerFacade *ioManager = new IOManagerFacade(inMemoryIO.get());
  auto &chunk = ioManager->m_data["Dimensions_012LOD0/1"];
  chunk.error.code = 489;
  chunk.error.string = "We don't let chunk 1 through";
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Create(ioManager, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, error), OpenVDS::Close);
  ASSERT_TRUE(handle);

  OpenVDS::VolumeDataAccessManager accessManager = OpenVDS::GetAccessManager(handle.get());
  OpenVDS::VolumeDataPageAccessor *pageAccessor = accessManager.CreateVolumeDataPageAccessor(OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  ASSERT_TRUE(pageAccessor);

  int32_t chunkCount = int32_t(pageAccessor->GetChunkCount());

  for (int i = 0; i < chunkCount; i++)
  {
    OpenVDS::VolumeDataPage *page =  pageAccessor->CreatePage(i);
    int pitch[OpenVDS::Dimensionality_Max];
    void *buffer = page->GetWritableBuffer(pitch);
    (void)buffer;
    page->Release();
  }
  pageAccessor->Commit();
  pageAccessor->SetMaxPages(0);
  accessManager.DestroyVolumeDataPageAccessor(pageAccessor);

  ASSERT_EQ(accessManager.UploadErrorCount(), 1);
  const char *object;
  int errorCode;
  const char *errorString;
  accessManager.GetCurrentUploadError(&object, &errorCode, &errorString);
  ASSERT_EQ(errorCode, 489);
  ASSERT_EQ(std::string(object), std::string("Dimensions_012LOD0/1"));
}

TEST(IOErrorHandlingUpload, ErrorHandlingLayerStatusHttpError)
{
  OpenVDS::Error error;
  int negativeMargin = 4;
  int positiveMargin = 4;
  int brickSize2DMultiplier = 4;
  auto lodLevels = OpenVDS::VolumeDataLayoutDescriptor::LODLevels_None;
  auto layoutOptions = OpenVDS::VolumeDataLayoutDescriptor::Options_None;
  OpenVDS::VolumeDataLayoutDescriptor layoutDescriptor(OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32, negativeMargin, positiveMargin, brickSize2DMultiplier, lodLevels, layoutOptions);

  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors;
  axisDescriptors.emplace_back(63, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE, "ms", 0.0f, 4.f);
  axisDescriptors.emplace_back(32, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE, "", 1932.f, 2536.f);
  axisDescriptors.emplace_back(32, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE,    "", 9985.f, 10369.f);

  std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors;
  float rangeMin = -0.1234f;
  float rangeMax = 0.1234f;
  float intScale;
  float intOffset;
  getScaleOffsetForFormat(rangeMin, rangeMax, true, OpenVDS::VolumeDataChannelDescriptor::Format_U32, intScale, intOffset);
  channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_U32, OpenVDS::VolumeDataChannelDescriptor::Components_1, AMPLITUDE_ATTRIBUTE_NAME, "", rangeMin, rangeMax, OpenVDS::VolumeDataMapping::Direct, 1, OpenVDS::VolumeDataChannelDescriptor::Default, 0.f, intScale, intOffset);

  OpenVDS::MetadataContainer metadataContainer;

  std::unique_ptr<OpenVDS::IOManager> inMemoryIO(OpenVDS::IOManagerInMemory::CreateIOManagerInMemory("", error));
  IOManagerFacade *ioManager = new IOManagerFacade(inMemoryIO.get());
  auto &chunk = ioManager->m_data["LayerStatus"];
  chunk.error.code = 466;
  chunk.error.string = "No Layerstatus";
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Create(ioManager, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, error), OpenVDS::Close);
  ASSERT_TRUE(handle);

  OpenVDS::VolumeDataAccessManager accessManager = OpenVDS::GetAccessManager(handle.get());
  OpenVDS::VolumeDataPageAccessor *pageAccessor = accessManager.CreateVolumeDataPageAccessor(OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  ASSERT_TRUE(pageAccessor);

  int32_t chunkCount = int32_t(pageAccessor->GetChunkCount());

  for (int i = 0; i < chunkCount; i++)
  {
    OpenVDS::VolumeDataPage *page =  pageAccessor->CreatePage(i);
    int pitch[OpenVDS::Dimensionality_Max];
    void *buffer = page->GetWritableBuffer(pitch);
    (void)buffer;
    page->Release();
  }
  pageAccessor->Commit();
  pageAccessor->SetMaxPages(0);
  accessManager.DestroyVolumeDataPageAccessor(pageAccessor);

  ASSERT_EQ(accessManager.UploadErrorCount(), 1);
  const char *object;
  int errorCode;
  const char *errorString;
  accessManager.GetCurrentUploadError(&object, &errorCode, &errorString);
  ASSERT_EQ(errorCode, 466);
  ASSERT_EQ(std::string(object), std::string("LayerStatus"));
}

TEST(IOErrorHandlingUpload, ErrorHandlingChunkMetadataHttpError)
{
  OpenVDS::Error error;
  int negativeMargin = 4;
  int positiveMargin = 4;
  int brickSize2DMultiplier = 4;
  auto lodLevels = OpenVDS::VolumeDataLayoutDescriptor::LODLevels_None;
  auto layoutOptions = OpenVDS::VolumeDataLayoutDescriptor::Options_None;
  OpenVDS::VolumeDataLayoutDescriptor layoutDescriptor(OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32, negativeMargin, positiveMargin, brickSize2DMultiplier, lodLevels, layoutOptions);

  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors;
  axisDescriptors.emplace_back(63, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE, "ms", 0.0f, 4.f);
  axisDescriptors.emplace_back(32, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE, "", 1932.f, 2536.f);
  axisDescriptors.emplace_back(32, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE,    "", 9985.f, 10369.f);

  std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors;
  float rangeMin = -0.1234f;
  float rangeMax = 0.1234f;
  float intScale;
  float intOffset;
  getScaleOffsetForFormat(rangeMin, rangeMax, true, OpenVDS::VolumeDataChannelDescriptor::Format_U32, intScale, intOffset);
  channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_U32, OpenVDS::VolumeDataChannelDescriptor::Components_1, AMPLITUDE_ATTRIBUTE_NAME, "", rangeMin, rangeMax, OpenVDS::VolumeDataMapping::Direct, 1, OpenVDS::VolumeDataChannelDescriptor::Default, 0.f, intScale, intOffset);

  OpenVDS::MetadataContainer metadataContainer;

  std::unique_ptr<OpenVDS::IOManager> inMemoryIO(OpenVDS::IOManagerInMemory::CreateIOManagerInMemory("", error));
  IOManagerFacade *ioManager = new IOManagerFacade(inMemoryIO.get());
  auto &chunk = ioManager->m_data["Dimensions_012LOD0/ChunkMetadata/0"];
  chunk.error.code = 433;
  chunk.error.string = "No metadata";
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Create(ioManager, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, error), OpenVDS::Close);
  ASSERT_TRUE(handle);

  OpenVDS::VolumeDataAccessManager accessManager = OpenVDS::GetAccessManager(handle.get());
  OpenVDS::VolumeDataPageAccessor *pageAccessor = accessManager.CreateVolumeDataPageAccessor(OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  ASSERT_TRUE(pageAccessor);

  int32_t chunkCount = int32_t(pageAccessor->GetChunkCount());

  for (int i = 0; i < chunkCount; i++)
  {
    OpenVDS::VolumeDataPage *page =  pageAccessor->CreatePage(i);
    int pitch[OpenVDS::Dimensionality_Max];
    void *buffer = page->GetWritableBuffer(pitch);
    (void)buffer;
    page->Release();
  }
  pageAccessor->Commit();
  pageAccessor->SetMaxPages(0);
  accessManager.DestroyVolumeDataPageAccessor(pageAccessor);

  ASSERT_EQ(accessManager.UploadErrorCount(), 1);
  const char *object;
  int errorCode;
  const char *errorString;
  accessManager.GetCurrentUploadError(&object, &errorCode, &errorString);
  ASSERT_EQ(errorCode, 433);
  ASSERT_EQ(std::string(object), std::string("Dimensions_012LOD0/ChunkMetadata/0"));
}
