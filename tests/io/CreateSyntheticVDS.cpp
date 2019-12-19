#include <gtest/gtest.h>

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataAccess.h>

#include <VDS/VolumeDataLayoutImpl.h>

#include "../utils/GenerateVDS.h"

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
  OpenVDS::Error error;
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> inMemoryVDS(generateSimpleInMemory3DVDS(60,60,60, OpenVDS::VolumeDataChannelDescriptor::Format_R32), OpenVDS::Close);
  fill3DVDSWithNoise(inMemoryVDS.get());
  OpenVDS::VolumeDataLayout *inMemoryLayout = OpenVDS::GetLayout(inMemoryVDS.get());

  OpenVDS::AWSOpenOptions options;

  options.region = TEST_AWS_REGION;
  options.bucket = TEST_AWS_BUCKET;
  options.key = "SIMPLE_NOISE_VDS";

  if(options.region.empty() || options.bucket.empty() || options.key.empty())
  {
    GTEST_SKIP() << "Environment variables not set";
  }
  ASSERT_TRUE(options.region.size() && options.bucket.size() && options.key.size());


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

  networkVDS.reset(OpenVDS::Open(options, error));
  ASSERT_TRUE(networkVDS);

  OpenVDS::VolumeDataLayout *networkLayout = OpenVDS::GetLayout(networkVDS.get());
  OpenVDS::VolumeDataAccessManager *networkAccessManager = OpenVDS::GetAccessManager(networkVDS.get());
  OpenVDS::VolumeDataPageAccessor *networkPageAccessor = networkAccessManager->CreateVolumeDataPageAccessor(networkLayout, OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly);


  for (int64_t i = 0; i < chunkCount; i++)
  {
    int32_t min[OpenVDS::Dimensionality_Max];
    int32_t max[OpenVDS::Dimensionality_Max];
    int32_t size[OpenVDS::Dimensionality_Max];
    inMemoryPageAccessor->GetChunkMinMax(i, min, max);
    for (int dim = 0; dim < OpenVDS::Dimensionality_Max; dim++)
    {
      size[dim] = max[dim] - min[dim];
    }
      int32_t inMemoryPitch[OpenVDS::Dimensionality_Max];
      OpenVDS::VolumeDataPage *inMemoryPage = inMemoryPageAccessor->ReadPage(i);
      const void *inMemoryBuffer = inMemoryPage->GetBuffer(inMemoryPitch);

      int32_t networkPitch[OpenVDS::Dimensionality_Max];
      OpenVDS::VolumeDataPage *networkPage = networkPageAccessor->ReadPage(i);
      const void *networkBuffer = networkPage->GetBuffer(networkPitch);

      for (int dim = 0; dim < dimensions; dim++)
      {
        ASSERT_TRUE(inMemoryPitch[dim] ==networkPitch[dim]);
      }
      ASSERT_TRUE(memcmp(inMemoryBuffer, networkBuffer, inMemoryPitch[2] * size[2] * 4) == 0);
      inMemoryPage->Release();
      networkPage->Release();
  }

  inMemoryAccessManager->DestroyVolumeDataPageAccessor(inMemoryPageAccessor);
  networkAccessManager->DestroyVolumeDataPageAccessor(networkPageAccessor);
}
