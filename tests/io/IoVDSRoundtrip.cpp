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

//#define IN_MEMORY_TEST 1

int handleUploadErrors(OpenVDS::VolumeDataAccessManager *accessManager)
{
  int32_t errorCount = accessManager->UploadErrorCount();
  for (int errorIndex = 0; errorIndex < errorCount; errorIndex++)
  {
    const char *object_id;
    int32_t error_code;
    const char *error_string;
    accessManager->GetCurrentUploadError(&object_id, &error_code, &error_string);
    fprintf(stderr, "\nFailed to upload object: %s. Error code %d: %s\n", object_id, error_code, error_string);
  }
  if (errorCount)
    accessManager->ClearUploadErrors();
  return errorCount;
}

TEST(IOTests, CreateSyntheticVDSAndVerifyUpload)
{
  GTEST_SKIP() << "This test has to be enabled manually";

  auto full_start = std::chrono::high_resolution_clock::now();
  OpenVDS::Error error;
  OpenVDS::IOManagerInMemory *inMemory = new OpenVDS::IOManagerInMemory(OpenVDS::InMemoryOpenOptions(), error);
#ifdef IN_MEMORY_TEST
  int createDim[] = {800,800,800};
#else
  int createDim[] = {400,400,400};
#endif

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> inMemoryVDS(generateSimpleInMemory3DVDS(createDim[0], createDim[1], createDim[2], OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataLayoutDescriptor::BrickSize_64, inMemory), OpenVDS::Close);
  fill3DVDSWithNoise(inMemoryVDS.get());
  OpenVDS::VolumeDataLayout *inMemoryLayout = OpenVDS::GetLayout(inMemoryVDS.get());

#ifndef IN_MEMORY_TEST
  //OpenVDS::AzureOpenOptions options;
  //options.connectionString = TEST_AZURE_CONNECTION;
  //options.container = "SIMPLE_NOISE_VDS";
  //options.parallelism_factor = 8;

  //if (options.connectionString.empty())
  //{
  //  GTEST_SKIP() << "Environment variables not set";
  //}

  OpenVDS::AWSOpenOptions options;
  options.region = TEST_AWS_REGION;
  options.bucket = TEST_AWS_BUCKET;
  options.endpointOverride = TEST_AWS_ENDPOINT_OVERRIDE;
  options.key = TEST_AWS_OBJECTID;
  if (options.bucket.empty())
  {
    GTEST_SKIP() << "Environment variables not set";
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
    switch(key.Type())
    {
    case OpenVDS::MetadataType::Int:        metadata.SetMetadataInt(key.Category(), key.Name(), inMemoryLayout->GetMetadataInt(key.Category(), key.Name())); break;
    case OpenVDS::MetadataType::IntVector2: metadata.SetMetadataIntVector2(key.Category(), key.Name(), inMemoryLayout->GetMetadataIntVector2(key.Category(), key.Name())); break;
    case OpenVDS::MetadataType::IntVector3: metadata.SetMetadataIntVector2(key.Category(), key.Name(), inMemoryLayout->GetMetadataIntVector2(key.Category(), key.Name())); break;
    case OpenVDS::MetadataType::IntVector4: metadata.SetMetadataIntVector2(key.Category(), key.Name(), inMemoryLayout->GetMetadataIntVector2(key.Category(), key.Name())); break;
    case OpenVDS::MetadataType::Float:        metadata.SetMetadataFloat(key.Category(), key.Name(), inMemoryLayout->GetMetadataFloat(key.Category(), key.Name())); break;
    case OpenVDS::MetadataType::FloatVector2: metadata.SetMetadataFloatVector2(key.Category(), key.Name(), inMemoryLayout->GetMetadataFloatVector2(key.Category(), key.Name())); break;
    case OpenVDS::MetadataType::FloatVector3: metadata.SetMetadataFloatVector2(key.Category(), key.Name(), inMemoryLayout->GetMetadataFloatVector2(key.Category(), key.Name())); break;
    case OpenVDS::MetadataType::FloatVector4: metadata.SetMetadataFloatVector2(key.Category(), key.Name(), inMemoryLayout->GetMetadataFloatVector2(key.Category(), key.Name())); break;
    case OpenVDS::MetadataType::Double:        metadata.SetMetadataDouble(key.Category(), key.Name(), inMemoryLayout->GetMetadataDouble(key.Category(), key.Name())); break;
    case OpenVDS::MetadataType::DoubleVector2: metadata.SetMetadataDoubleVector2(key.Category(), key.Name(), inMemoryLayout->GetMetadataDoubleVector2(key.Category(), key.Name())); break;
    case OpenVDS::MetadataType::DoubleVector3: metadata.SetMetadataDoubleVector2(key.Category(), key.Name(), inMemoryLayout->GetMetadataDoubleVector2(key.Category(), key.Name())); break;
    case OpenVDS::MetadataType::DoubleVector4: metadata.SetMetadataDoubleVector2(key.Category(), key.Name(), inMemoryLayout->GetMetadataDoubleVector2(key.Category(), key.Name())); break;
    case OpenVDS::MetadataType::String: metadata.SetMetadataString(key.Category(), key.Name(), std::string(inMemoryLayout->GetMetadataString(key.Category(), key.Name()))); break;

    case OpenVDS::MetadataType::BLOB:
    {
      std::vector<uint8_t> blob;
      inMemoryLayout->GetMetadataBLOB(key.Category(), key.Name(), blob);
      metadata.SetMetadataBLOB(key.Category(), key.Name(), blob);
      break;
    }
    }
  }

  auto create_start = std::chrono::high_resolution_clock::now();
#ifdef IN_MEMORY_TEST
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> networkVDS(OpenVDS::Create(new IOManagerFacadeLight(inMemory), layoutDescriptor, axisDescriptors, channelDescriptors, metadata, error), &OpenVDS::Close);
#else
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> networkVDS(OpenVDS::Create(options, layoutDescriptor, axisDescriptors, channelDescriptors, metadata, error), &OpenVDS::Close);
#endif
  ASSERT_TRUE(networkVDS);


  OpenVDS::VolumeDataAccessManager *inMemoryAccessManager = OpenVDS::GetAccessManager(inMemoryVDS.get());
  OpenVDS::VolumeDataPageAccessor *inMemoryPageAccessor = inMemoryAccessManager->CreateVolumeDataPageAccessor(inMemoryLayout, OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly);
  int64_t chunkCount = inMemoryPageAccessor->GetChunkCount();

  {
    OpenVDS::VolumeDataLayout *networkLayout = OpenVDS::GetLayout(networkVDS.get());
    OpenVDS::VolumeDataAccessManager *networkAccessManager = OpenVDS::GetAccessManager(networkVDS.get());
    OpenVDS::VolumeDataPageAccessor *networkPageAccessor = networkAccessManager->CreateVolumeDataPageAccessor(networkLayout, OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_Create);

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
    networkAccessManager->DestroyVolumeDataPageAccessor(networkPageAccessor);
  }

  networkVDS.reset();

  auto create_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - create_start);

  auto read_start = std::chrono::high_resolution_clock::now();
#ifdef IN_MEMORY_TEST
  networkVDS.reset(OpenVDS::Open(new IOManagerFacadeLight(inMemory), error));
#else
  networkVDS.reset(OpenVDS::Open(options, error));
#endif
  ASSERT_TRUE(networkVDS);

  OpenVDS::VolumeDataLayout *networkLayout = OpenVDS::GetLayout(networkVDS.get());
  OpenVDS::VolumeDataAccessManager *networkAccessManager = OpenVDS::GetAccessManager(networkVDS.get());

  std::vector<uint8_t> networkData;

  int32_t x_samples = networkLayout->GetDimensionNumSamples(0);
  int32_t y_samples = networkLayout->GetDimensionNumSamples(1);
  int32_t z_samples = networkLayout->GetDimensionNumSamples(2);

  int minPos[OpenVDS::Dimensionality_Max] = {};
  int maxPos[OpenVDS::Dimensionality_Max] = {x_samples, y_samples, z_samples, 0, 0, 0};

  networkData.resize(networkAccessManager->GetVolumeSubsetBufferSize(networkLayout, minPos, maxPos, networkLayout->GetChannelFormat(0), 0));


  fmt::print(stderr, "About to start download request.\n");
  auto download_start_time = std::chrono::high_resolution_clock::now();
  int64_t networkRequest = networkAccessManager->RequestVolumeSubset(networkData.data(), networkLayout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, networkLayout->GetChannelFormat(0));

  ASSERT_TRUE(networkAccessManager->WaitForCompletion(networkRequest));

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
  inMemoryData.resize(inMemoryAccessManager->GetVolumeSubsetBufferSize(inMemoryLayout, minPos, maxPos, inMemoryLayout->GetChannelFormat(0), 0));
  int64_t inMemoryRequest = inMemoryAccessManager->RequestVolumeSubset(inMemoryData.data(), inMemoryLayout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, inMemoryLayout->GetChannelFormat(0));
  ASSERT_TRUE(inMemoryAccessManager->WaitForCompletion(inMemoryRequest));
  inMemoryAccessManager->DestroyVolumeDataPageAccessor(inMemoryPageAccessor);

  ASSERT_EQ(inMemoryData.size(), networkData.size());
  ASSERT_TRUE(memcmp(inMemoryData.data(), networkData.data(), networkData.size()) == 0);

}
