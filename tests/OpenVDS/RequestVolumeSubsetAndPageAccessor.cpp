#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeData.h>

#include <array>

#include <gtest/gtest.h>

TEST(OpenVDS, MixedRequests)
{
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

  if(options.bucket.empty() || options.key.empty())
  {
    GTEST_SKIP() << "Environment variables not set";
  }

  OpenVDS::Error error;

  for (int i = 0; i < 20; i++)
  {
    std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(options, error), &OpenVDS::Close);
    ASSERT_TRUE(handle);

    OpenVDS::VolumeDataLayout* layout = OpenVDS::GetLayout(handle.get());

    OpenVDS::VolumeDataAccessManager* accessManager = OpenVDS::GetAccessManager(handle.get());
    ASSERT_TRUE(accessManager);

    std::vector<uint8_t> buffer;
    buffer.resize(accessManager->GetVolumeSubsetBufferSize(layout, { 0, 0, 0 }, { 100, 100, 100 }, layout->GetChannelFormat(0)));
    auto request = accessManager->RequestVolumeSubset(buffer.data(), layout, OpenVDS::Dimensions_012, 0, 0, { 0, 0, 0 }, { 100, 100, 100 }, layout->GetChannelFormat(0));

    OpenVDS::VolumeDataPageAccessor* pageAccessor = accessManager->CreateVolumeDataPageAccessor(layout, OpenVDS::Dimensions_012, 0, 0, 256, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly);
    ASSERT_TRUE(pageAccessor);
    auto dataAccessor = accessManager->Create3DVolumeDataAccessorR32(pageAccessor, -1000);
    float value = dataAccessor->GetValue({ 100, 100, 100 });

    accessManager->WaitForCompletion(request);
  }
}
