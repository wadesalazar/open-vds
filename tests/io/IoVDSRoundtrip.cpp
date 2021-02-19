#include <gtest/gtest.h>

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataAccess.h>

#include <VDS/VolumeDataLayoutImpl.h>
#include <IO/IOManagerInMemory.h>

#include "../utils/GenerateVDS.h"
#include "../utils/FacadeIOManager.h"

#include <chrono>
#include <fmt/format.h>
#include <stdio.h>

//#define IN_MEMORY_TEST 1

int handleUploadErrors(OpenVDS::VolumeDataAccessManager &accessManager)
{
  int32_t errorCount = accessManager.UploadErrorCount();
  for (int errorIndex = 0; errorIndex < errorCount; errorIndex++)
  {
    const char *object_id;
    int32_t error_code;
    const char *error_string;
    accessManager.GetCurrentUploadError(&object_id, &error_code, &error_string);
    fprintf(stderr, "\nFailed to upload object: %s. Error code %d: %s\n", object_id, error_code, error_string);
  }
  if (errorCount)
    accessManager.ClearUploadErrors();
  return errorCount;
}

TEST(IOTests, CreateSyntheticVDSAndVerifyUpload)
{
  GTEST_SKIP() << "This test has to be enabled manually";

  auto full_start = std::chrono::high_resolution_clock::now();
  OpenVDS::Error error;
  OpenVDS::IOManager *inMemory = OpenVDS::IOManagerInMemory::CreateIOManagerInMemory("", error);
#ifdef IN_MEMORY_TEST
  int createDim[] = {800,800,800};
#else
  int createDim[] = {400,400,400};
#endif

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> inMemoryVDS(generateSimpleInMemory3DVDS(createDim[0], createDim[1], createDim[2], OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataLayoutDescriptor::BrickSize_64, inMemory), OpenVDS::Close);
  fill3DVDSWithNoise(inMemoryVDS.get());
  OpenVDS::VolumeDataLayout *inMemoryLayout = OpenVDS::GetLayout(inMemoryVDS.get());

#ifndef IN_MEMORY_TEST
  std::string url = TEST_URL;
  std::string connectionString = TEST_CONNECTION;
  if(url.empty())
  {
    GTEST_SKIP() << "Test Environment for connecting to VDS is not set";
  }
#endif

  auto layoutDescriptor = inMemoryLayout->GetLayoutDescriptor();
  int dimensions = inMemoryLayout->GetDimensionality();
  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors;
  std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors;
  for (int i = 0; i < dimensions; i++)
  {
    axisDescriptors.push_back(inMemoryLayout->GetAxisDescriptor(i));
  }
  int channels = inMemoryLayout->GetChannelCount();
  for (int i = 0; i < channels; i++)
  {
    channelDescriptors.push_back(inMemoryLayout->GetChannelDescriptor(i));
  }

  OpenVDS::MetadataContainer metadata;
  for (auto &key : inMemoryLayout->GetMetadataKeys())
  {
    switch(key.GetType())
    {
    case OpenVDS::MetadataType::Int:        metadata.SetMetadataInt(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataInt(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::IntVector2: metadata.SetMetadataIntVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataIntVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::IntVector3: metadata.SetMetadataIntVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataIntVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::IntVector4: metadata.SetMetadataIntVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataIntVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::Float:        metadata.SetMetadataFloat(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataFloat(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::FloatVector2: metadata.SetMetadataFloatVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataFloatVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::FloatVector3: metadata.SetMetadataFloatVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataFloatVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::FloatVector4: metadata.SetMetadataFloatVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataFloatVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::Double:        metadata.SetMetadataDouble(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataDouble(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::DoubleVector2: metadata.SetMetadataDoubleVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataDoubleVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::DoubleVector3: metadata.SetMetadataDoubleVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataDoubleVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::DoubleVector4: metadata.SetMetadataDoubleVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataDoubleVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::String: metadata.SetMetadataString(key.GetCategory(), key.GetName(), std::string(inMemoryLayout->GetMetadataString(key.GetCategory(), key.GetName()))); break;

    case OpenVDS::MetadataType::BLOB:
    {
      std::vector<uint8_t> blob;
      inMemoryLayout->GetMetadataBLOB(key.GetCategory(), key.GetName(), blob);
      metadata.SetMetadataBLOB(key.GetCategory(), key.GetName(), blob);
      break;
    }
    }
  }

  auto create_start = std::chrono::high_resolution_clock::now();
#ifdef IN_MEMORY_TEST
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> networkVDS(OpenVDS::Create(new IOManagerFacadeLight(inMemory), layoutDescriptor, axisDescriptors, channelDescriptors, metadata, error), &OpenVDS::Close);
#else
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> networkVDS(OpenVDS::Create(url, connectionString, layoutDescriptor, axisDescriptors, channelDescriptors, metadata, error), &OpenVDS::Close);
#endif
  ASSERT_TRUE(networkVDS);


  OpenVDS::VolumeDataAccessManager inMemoryAccessManager = OpenVDS::GetAccessManager(inMemoryVDS.get());
  OpenVDS::VolumeDataPageAccessor *inMemoryPageAccessor = inMemoryAccessManager.CreateVolumeDataPageAccessor(OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly);
  int64_t chunkCount = inMemoryPageAccessor->GetChunkCount();

  {
    OpenVDS::VolumeDataAccessManager networkAccessManager = OpenVDS::GetAccessManager(networkVDS.get());
    OpenVDS::VolumeDataPageAccessor *networkPageAccessor = networkAccessManager.CreateVolumeDataPageAccessor(OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_Create);

    for (int64_t i = 0; i < chunkCount; i++)
    {
      int uploadErrors = handleUploadErrors(networkAccessManager);
      ASSERT_TRUE(uploadErrors == 0);
      int32_t min[OpenVDS::Dimensionality_Max];
      int32_t max[OpenVDS::Dimensionality_Max];
      int32_t size[OpenVDS::Dimensionality_Max];
      inMemoryPageAccessor->GetChunkMinMax(i, min, max);
      for (int dim = 0; dim < OpenVDS::Dimensionality_Max; dim++)
      {
        size[dim] = max[dim] - min[dim];
      }

      int32_t readPitch[OpenVDS::Dimensionality_Max];
      OpenVDS::VolumeDataPage *inMemoryPage = inMemoryPageAccessor->ReadPage(i);
      const void *readBuffer = inMemoryPage->GetBuffer(readPitch);

      int32_t writePitch[OpenVDS::Dimensionality_Max];
      OpenVDS::VolumeDataPage *networkPage = networkPageAccessor->CreatePage(i);
      void *writeBuffer = networkPage->GetWritableBuffer(writePitch);

      for (int dim = 0; dim < dimensions; dim++)
      {
        ASSERT_TRUE(readPitch[dim] == writePitch[dim]);
      }
      memcpy(writeBuffer, readBuffer, readPitch[2] * size[2] * 4);
      inMemoryPage->Release();
      networkPage->Release();
    }
    networkPageAccessor->Commit();
    int uploadErrors = handleUploadErrors(networkAccessManager);
    ASSERT_TRUE(uploadErrors == 0);
    networkAccessManager.DestroyVolumeDataPageAccessor(networkPageAccessor);
  }

  networkVDS.reset();

  auto create_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - create_start);

  auto read_start = std::chrono::high_resolution_clock::now();
#ifdef IN_MEMORY_TEST
  networkVDS.reset(OpenVDS::Open(new IOManagerFacadeLight(inMemory), error));
#else
  networkVDS.reset(OpenVDS::Open(url, connectionString, error));
#endif
  ASSERT_TRUE(networkVDS);

  OpenVDS::VolumeDataLayout *networkLayout = OpenVDS::GetLayout(networkVDS.get());
  OpenVDS::VolumeDataAccessManager networkAccessManager = OpenVDS::GetAccessManager(networkVDS.get());

  std::vector<uint8_t> networkData;

  int32_t x_samples = networkLayout->GetDimensionNumSamples(0);
  int32_t y_samples = networkLayout->GetDimensionNumSamples(1);
  int32_t z_samples = networkLayout->GetDimensionNumSamples(2);

  int minPos[OpenVDS::Dimensionality_Max] = {};
  int maxPos[OpenVDS::Dimensionality_Max] = {x_samples, y_samples, z_samples, 0, 0, 0};

  networkData.resize(networkAccessManager.GetVolumeSubsetBufferSize(minPos, maxPos, networkLayout->GetChannelFormat(0), 0));


  fmt::print(stderr, "About to start download request.\n");
  auto download_start_time = std::chrono::high_resolution_clock::now();
  auto networkRequest = networkAccessManager.RequestVolumeSubset((void *)networkData.data(), networkData.size(), OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, networkLayout->GetChannelFormat(0));

  ASSERT_TRUE(networkRequest->WaitForCompletion());

  auto download_end_time = std::chrono::high_resolution_clock::now();

  networkVDS.reset();

  auto end_time = std::chrono::high_resolution_clock::now();
  auto read_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - read_start);
  auto full_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - full_start);
  fmt::print(stderr, "Create time {} - Read time {} - Full time {}\n", create_time.count(), read_time.count(), full_time.count());
  
  double download_time = std::chrono::duration_cast<std::chrono::milliseconds>(download_end_time - download_start_time).count() / 1000.0;
  int64_t bytes = networkData.size();
  int64_t bits = bytes * 8;
  int mega = 1 << 20;
  double mbytes = bytes / double(mega);
  double mbitsSecDownload = bits / download_time / double(mega);
  fmt::print(stderr, "Completed Downloading {} MB in {} Seconds giving {} Mbit/s\n", mbytes, download_time, mbitsSecDownload);

  auto memcopy_start = std::chrono::high_resolution_clock::now();
  uint8_t *memcpy_data = new uint8_t[networkData.size()];
  memcpy(memcpy_data, networkData.data(), networkData.size());
  auto memcopy_end = std::chrono::high_resolution_clock::now();
  double memcopy_time = std::chrono::duration_cast<std::chrono::milliseconds>(memcopy_end - memcopy_start).count() / 1000.0;
  double mbitsSecMemcpy = bits / memcopy_time / double(mega);
  fmt::print(stderr, "Completed Alloc&Memcopying {} MB in {} Seconds giving {} Mbit/s\n", mbytes, memcopy_time, mbitsSecMemcpy);
  delete[] memcpy_data;

  std::vector<uint8_t> inMemoryData;
  inMemoryData.resize(inMemoryAccessManager.GetVolumeSubsetBufferSize(minPos, maxPos, inMemoryLayout->GetChannelFormat(0), 0));
  auto inMemoryRequest = inMemoryAccessManager.RequestVolumeSubset((void *)inMemoryData.data(), inMemoryData.size(), OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, inMemoryLayout->GetChannelFormat(0));
  ASSERT_TRUE(inMemoryRequest->WaitForCompletion());
  inMemoryAccessManager.DestroyVolumeDataPageAccessor(inMemoryPageAccessor);

  ASSERT_EQ(inMemoryData.size(), networkData.size());
  ASSERT_TRUE(memcmp(inMemoryData.data(), networkData.data(), networkData.size()) == 0);
}

TEST(IOTests, CreateSyntheticVDSAndVerifyCreateVDSFile)
{
  auto full_start = std::chrono::high_resolution_clock::now();
  OpenVDS::Error error;
  OpenVDS::IOManager *inMemory = OpenVDS::IOManagerInMemory::CreateIOManagerInMemory("", error);
  int createDim[] = {200,200,200};

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> inMemoryVDS(generateSimpleInMemory3DVDS(createDim[0], createDim[1], createDim[2], OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataLayoutDescriptor::BrickSize_64, inMemory), OpenVDS::Close);
  fill3DVDSWithNoise(inMemoryVDS.get());
  OpenVDS::VolumeDataLayout *inMemoryLayout = OpenVDS::GetLayout(inMemoryVDS.get());

  auto layoutDescriptor = inMemoryLayout->GetLayoutDescriptor();
  int dimensions = inMemoryLayout->GetDimensionality();
  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors;
  std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors;
  for (int i = 0; i < dimensions; i++)
  {
    axisDescriptors.push_back(inMemoryLayout->GetAxisDescriptor(i));
  }
  int channels = inMemoryLayout->GetChannelCount();
  for (int i = 0; i < channels; i++)
  {
    channelDescriptors.push_back(inMemoryLayout->GetChannelDescriptor(i));
  }

  OpenVDS::MetadataContainer metadata;
  for (auto &key : inMemoryLayout->GetMetadataKeys())
  {
    switch(key.GetType())
    {
    case OpenVDS::MetadataType::Int:        metadata.SetMetadataInt(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataInt(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::IntVector2: metadata.SetMetadataIntVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataIntVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::IntVector3: metadata.SetMetadataIntVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataIntVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::IntVector4: metadata.SetMetadataIntVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataIntVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::Float:        metadata.SetMetadataFloat(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataFloat(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::FloatVector2: metadata.SetMetadataFloatVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataFloatVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::FloatVector3: metadata.SetMetadataFloatVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataFloatVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::FloatVector4: metadata.SetMetadataFloatVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataFloatVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::Double:        metadata.SetMetadataDouble(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataDouble(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::DoubleVector2: metadata.SetMetadataDoubleVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataDoubleVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::DoubleVector3: metadata.SetMetadataDoubleVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataDoubleVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::DoubleVector4: metadata.SetMetadataDoubleVector2(key.GetCategory(), key.GetName(), inMemoryLayout->GetMetadataDoubleVector2(key.GetCategory(), key.GetName())); break;
    case OpenVDS::MetadataType::String: metadata.SetMetadataString(key.GetCategory(), key.GetName(), std::string(inMemoryLayout->GetMetadataString(key.GetCategory(), key.GetName()))); break;

    case OpenVDS::MetadataType::BLOB:
    {
      std::vector<uint8_t> blob;
      inMemoryLayout->GetMetadataBLOB(key.GetCategory(), key.GetName(), blob);
      metadata.SetMetadataBLOB(key.GetCategory(), key.GetName(), blob);
      break;
    }
    }
  }

  const OpenVDS::VDSFileOpenOptions openOptions("roundtrip.vds");
  remove(openOptions.fileName.c_str());

  auto create_start = std::chrono::high_resolution_clock::now();
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> fileVDS(OpenVDS::Create(openOptions, layoutDescriptor, axisDescriptors, channelDescriptors, metadata, error), &OpenVDS::Close);
  ASSERT_TRUE(fileVDS);

  OpenVDS::VolumeDataAccessManager inMemoryAccessManager = OpenVDS::GetAccessManager(inMemoryVDS.get());
  OpenVDS::VolumeDataPageAccessor *inMemoryPageAccessor = inMemoryAccessManager.CreateVolumeDataPageAccessor(OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly);
  int64_t chunkCount = inMemoryPageAccessor->GetChunkCount();

  {
    OpenVDS::VolumeDataAccessManager fileAccessManager = OpenVDS::GetAccessManager(fileVDS.get());
    OpenVDS::VolumeDataPageAccessor *filePageAccessor = fileAccessManager.CreateVolumeDataPageAccessor(OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_Create);

    for (int64_t i = 0; i < chunkCount; i++)
    {
      int uploadErrors = handleUploadErrors(fileAccessManager);
      ASSERT_TRUE(uploadErrors == 0);
      int32_t min[OpenVDS::Dimensionality_Max];
      int32_t max[OpenVDS::Dimensionality_Max];
      int32_t size[OpenVDS::Dimensionality_Max];
      inMemoryPageAccessor->GetChunkMinMax(i, min, max);
      for (int dim = 0; dim < OpenVDS::Dimensionality_Max; dim++)
      {
        size[dim] = max[dim] - min[dim];
      }

      int32_t readPitch[OpenVDS::Dimensionality_Max];
      OpenVDS::VolumeDataPage *inMemoryPage = inMemoryPageAccessor->ReadPage(i);
      const void *readBuffer = inMemoryPage->GetBuffer(readPitch);

      int32_t writePitch[OpenVDS::Dimensionality_Max];
      OpenVDS::VolumeDataPage *filePage = filePageAccessor->CreatePage(i);
      void *writeBuffer = filePage->GetWritableBuffer(writePitch);

      for (int dim = 0; dim < dimensions; dim++)
      {
        ASSERT_TRUE(readPitch[dim] == writePitch[dim]);
      }
      memcpy(writeBuffer, readBuffer, readPitch[2] * size[2] * 4);
      inMemoryPage->Release();
      filePage->Release();
    }
    filePageAccessor->Commit();
    int uploadErrors = handleUploadErrors(fileAccessManager);
    ASSERT_TRUE(uploadErrors == 0);
    fileAccessManager.DestroyVolumeDataPageAccessor(filePageAccessor);
  }

  fileVDS.reset();

  auto create_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - create_start);

  auto read_start = std::chrono::high_resolution_clock::now();
#ifdef IN_MEMORY_TEST
  fileVDS.reset(OpenVDS::Open(new IOManagerFacadeLight(inMemory), error));
#else
  fileVDS.reset(OpenVDS::Open(openOptions, error));
#endif
  ASSERT_TRUE(fileVDS);

  OpenVDS::VolumeDataLayout *fileLayout = OpenVDS::GetLayout(fileVDS.get());
  OpenVDS::VolumeDataAccessManager fileAccessManager = OpenVDS::GetAccessManager(fileVDS.get());

  std::vector<uint8_t> fileData;

  int32_t x_samples = fileLayout->GetDimensionNumSamples(0);
  int32_t y_samples = fileLayout->GetDimensionNumSamples(1);
  int32_t z_samples = fileLayout->GetDimensionNumSamples(2);

  int minPos[OpenVDS::Dimensionality_Max] = {};
  int maxPos[OpenVDS::Dimensionality_Max] = {x_samples, y_samples, z_samples, 0, 0, 0};

  fileData.resize(fileAccessManager.GetVolumeSubsetBufferSize(minPos, maxPos, fileLayout->GetChannelFormat(0), 0));


  fmt::print(stderr, "About to start download request.\n");
  auto download_start_time = std::chrono::high_resolution_clock::now();
  auto fileRequest = fileAccessManager.RequestVolumeSubset((void *)fileData.data(), fileData.size(), OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, fileLayout->GetChannelFormat(0));

  ASSERT_TRUE(fileRequest->WaitForCompletion());

  auto download_end_time = std::chrono::high_resolution_clock::now();

  fileVDS.reset();

  auto end_time = std::chrono::high_resolution_clock::now();
  auto read_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - read_start);
  auto full_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - full_start);
  fmt::print(stderr, "Create time {} - Read time {} - Full time {}\n", create_time.count(), read_time.count(), full_time.count());
  
  double download_time = std::chrono::duration_cast<std::chrono::milliseconds>(download_end_time - download_start_time).count() / 1000.0;
  int64_t bytes = fileData.size();
  int64_t bits = bytes * 8;
  int mega = 1 << 20;
  double mbytes = bytes / double(mega);
  double mbitsSecDownload = bits / download_time / double(mega);
  fmt::print(stderr, "Completed Downloading {} MB in {} Seconds giving {} Mbit/s\n", mbytes, download_time, mbitsSecDownload);

  auto memcopy_start = std::chrono::high_resolution_clock::now();
  uint8_t *memcpy_data = new uint8_t[fileData.size()];
  memcpy(memcpy_data, fileData.data(), fileData.size());
  auto memcopy_end = std::chrono::high_resolution_clock::now();
  double memcopy_time = std::chrono::duration_cast<std::chrono::milliseconds>(memcopy_end - memcopy_start).count() / 1000.0;
  double mbitsSecMemcpy = bits / memcopy_time / double(mega);
  fmt::print(stderr, "Completed Alloc&Memcopying {} MB in {} Seconds giving {} Mbit/s\n", mbytes, memcopy_time, mbitsSecMemcpy);
  delete[] memcpy_data;

  std::vector<uint8_t> inMemoryData;
  inMemoryData.resize(inMemoryAccessManager.GetVolumeSubsetBufferSize(minPos, maxPos, inMemoryLayout->GetChannelFormat(0), 0));
  auto inMemoryRequest = inMemoryAccessManager.RequestVolumeSubset((void *)inMemoryData.data(), inMemoryData.size(), OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, inMemoryLayout->GetChannelFormat(0));
  ASSERT_TRUE(inMemoryRequest->WaitForCompletion());
  inMemoryAccessManager.DestroyVolumeDataPageAccessor(inMemoryPageAccessor);

  ASSERT_EQ(inMemoryData.size(), fileData.size());
  ASSERT_TRUE(memcmp(inMemoryData.data(), fileData.data(), fileData.size()) == 0);
  remove(openOptions.fileName.c_str());
}
