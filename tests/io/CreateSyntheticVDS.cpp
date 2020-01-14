#include <gtest/gtest.h>

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataAccess.h>

#include <VDS/VolumeDataLayoutImpl.h>

#include "../utils/GenerateVDS.h"

#include <chrono>
#include <fmt/format.h>
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
  auto full_start = std::chrono::high_resolution_clock::now();
  OpenVDS::Error error;
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> inMemoryVDS(generateSimpleInMemory3DVDS(100,100,100, OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32), OpenVDS::Close);
  fill3DVDSWithNoise(inMemoryVDS.get());
  OpenVDS::VolumeDataLayout *inMemoryLayout = OpenVDS::GetLayout(inMemoryVDS.get());

  OpenVDS::AzureOpenOptions options;
  options.connectionString = TEST_AZURE_CONNECTION;
  options.container = "SIMPLE_NOISE_VDS";
  options.parallelism_factor = 8;

  if (options.connectionString.empty())
  {
    GTEST_SKIP() << "Environment variables not set";
  }

  //OpenVDS::AWSOpenOptions options;
  //options.region = TEST_AWS_REGION;
  //options.bucket = TEST_AWS_BUCKET;
  //options.key = "SIMPLE_NOISE_VDS";


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
  for (auto &meta : *inMemoryLayout)
  {
    switch(meta.type)
    {
    case OpenVDS::MetadataType::Int:        metadata.SetMetadataInt(meta.category, meta.name, inMemoryLayout->GetMetadataInt(meta.category, meta.name)); break;
    case OpenVDS::MetadataType::IntVector2: metadata.SetMetadataIntVector2(meta.category, meta.name, inMemoryLayout->GetMetadataIntVector2(meta.category, meta.name)); break;
    case OpenVDS::MetadataType::IntVector3: metadata.SetMetadataIntVector2(meta.category, meta.name, inMemoryLayout->GetMetadataIntVector2(meta.category, meta.name)); break;
    case OpenVDS::MetadataType::IntVector4: metadata.SetMetadataIntVector2(meta.category, meta.name, inMemoryLayout->GetMetadataIntVector2(meta.category, meta.name)); break;
    case OpenVDS::MetadataType::Float:        metadata.SetMetadataFloat(meta.category, meta.name, inMemoryLayout->GetMetadataFloat(meta.category, meta.name)); break;
    case OpenVDS::MetadataType::FloatVector2: metadata.SetMetadataFloatVector2(meta.category, meta.name, inMemoryLayout->GetMetadataFloatVector2(meta.category, meta.name)); break;
    case OpenVDS::MetadataType::FloatVector3: metadata.SetMetadataFloatVector2(meta.category, meta.name, inMemoryLayout->GetMetadataFloatVector2(meta.category, meta.name)); break;
    case OpenVDS::MetadataType::FloatVector4: metadata.SetMetadataFloatVector2(meta.category, meta.name, inMemoryLayout->GetMetadataFloatVector2(meta.category, meta.name)); break;
    case OpenVDS::MetadataType::Double:        metadata.SetMetadataDouble(meta.category, meta.name, inMemoryLayout->GetMetadataDouble(meta.category, meta.name)); break;
    case OpenVDS::MetadataType::DoubleVector2: metadata.SetMetadataDoubleVector2(meta.category, meta.name, inMemoryLayout->GetMetadataDoubleVector2(meta.category, meta.name)); break;
    case OpenVDS::MetadataType::DoubleVector3: metadata.SetMetadataDoubleVector2(meta.category, meta.name, inMemoryLayout->GetMetadataDoubleVector2(meta.category, meta.name)); break;
    case OpenVDS::MetadataType::DoubleVector4: metadata.SetMetadataDoubleVector2(meta.category, meta.name, inMemoryLayout->GetMetadataDoubleVector2(meta.category, meta.name)); break;
    case OpenVDS::MetadataType::String: metadata.SetMetadataString(meta.category, meta.name, std::string(inMemoryLayout->GetMetadataString(meta.category, meta.name))); break;

    case OpenVDS::MetadataType::BLOB:
    {
      std::vector<uint8_t> blob;
      inMemoryLayout->GetMetadataBLOB(meta.category, meta.name, blob);
      metadata.SetMetadataBLOB(meta.category, meta.name, blob);
      break;
    }
    }
  }

  auto create_start = std::chrono::high_resolution_clock::now();
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> networkVDS(OpenVDS::Create(options, layoutDescriptor, axisDescriptors, channelDescriptors, metadata, error), &OpenVDS::Close);
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
  networkVDS.reset(OpenVDS::Open(options, error));
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

  int64_t networkRequest = networkAccessManager->RequestVolumeSubset(networkData.data(), networkLayout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, networkLayout->GetChannelFormat(0));

  ASSERT_TRUE(networkAccessManager->WaitForCompletion(networkRequest));

  networkVDS.reset();

  auto end_time = std::chrono::high_resolution_clock::now();
  auto read_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - read_start);
  auto full_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - full_start);
  fmt::print(stderr, "Create time {} - Read time {} - Full time {}\n", create_time.count(), read_time.count(), full_time.count());

  std::vector<uint8_t> inMemoryData;
  inMemoryData.resize(inMemoryAccessManager->GetVolumeSubsetBufferSize(inMemoryLayout, minPos, maxPos, inMemoryLayout->GetChannelFormat(0), 0));
  int64_t inMemoryRequest = inMemoryAccessManager->RequestVolumeSubset(inMemoryData.data(), inMemoryLayout, OpenVDS::Dimensions_012, 0, 0, minPos, maxPos, inMemoryLayout->GetChannelFormat(0));
  ASSERT_TRUE(inMemoryAccessManager->WaitForCompletion(inMemoryRequest));
  inMemoryAccessManager->DestroyVolumeDataPageAccessor(inMemoryPageAccessor);

  ASSERT_EQ(inMemoryData.size(), networkData.size());
  ASSERT_TRUE(memcmp(inMemoryData.data(), networkData.data(), networkData.size()) == 0);

}
