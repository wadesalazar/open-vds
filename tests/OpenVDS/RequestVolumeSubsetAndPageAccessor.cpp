#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeData.h>

#include <array>

#include <gtest/gtest.h>

TEST(OpenVDS, MixedRequests)
{
  std::string url = TEST_URL;
  std::string connectionString = TEST_CONNECTION;
  if(url.empty())
  {
    GTEST_SKIP() << "Test Environment for connecting to VDS is not set";
  }

  OpenVDS::Error error;

  for (int i = 0; i < 20; i++)
  {
    std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(url, connectionString, error), &OpenVDS::Close);
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
    (void)value;

    accessManager->WaitForCompletion(request);
  }
}
