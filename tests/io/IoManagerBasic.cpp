#include <gtest/gtest.h>
#include <iostream>
#include <IO/IOManager.h>
#include <IO/IOManagerAzure.h>
#include <OpenVDS/OpenVDS.h>
#include <VDS/VolumeDataAccessManagerImpl.h>
#include <VDS/Base64.h>
#include <chrono>

class AzureTransfer : public OpenVDS::TransferDownloadHandler
{
public:
  AzureTransfer() {}
  OpenVDS::Error m_error;

  std::vector<uint8_t> m_data;
  std::vector<uint8_t> m_metadata;

  ~AzureTransfer() override
  {
  }

  void HandleObjectSize(int64_t size) override
  {
  }

  void HandleObjectLastWriteTime(const std::string &lastWriteTimeISO8601) override
  {
  }

  void HandleMetadata(const std::string& key, const std::string& header) override
  {
    if (key == "vdschunkmetadata")
    {
      bool decoded = OpenVDS::Base64Decode(header.data(), (int)header.size(), m_metadata);
      if (!decoded)
      {
        m_error.code = -1;
        m_error.string = "Failed to decode chunk metadata";
      }
    }
  }

  void HandleData(std::vector<uint8_t>&& data) override
  {
    m_data = data;
    std::string dataString(data.begin(), data.end());
  }

  void Completed(const OpenVDS::Request& req, const OpenVDS::Error& error) override
  {
    m_error = error;
  }
};

void completedCallback(const OpenVDS::Request& request, const OpenVDS::Error& error)
{
  if (error.code != 0)
  {
    std::cout << "Failed Upload, Error=" << error.code << ", Message=" << error.string << std::endl;
  }
};

TEST(IOTests, basicAzure)
{
  OpenVDS::Error error;
  std::string url = TEST_URL;
  std::string connectionString = TEST_CONNECTION;
  if(url.empty())
  {
    GTEST_SKIP() << "Test Environment for connecting to VDS is not set";
  }

  OpenVDS::Error azureManagerError;

  OpenVDS::IOManagerAzure* m_ioManager = reinterpret_cast <OpenVDS::IOManagerAzure*> (OpenVDS::IOManager::CreateIOManager(url, connectionString, azureManagerError));

  const char hash[] = "Test text";
  std::vector<char> base64Hash;
  OpenVDS::Base64Encode((const unsigned char*)&hash, sizeof(hash), base64Hash);
  std::vector<std::pair<std::string, std::string>> meta_map;
  meta_map.emplace_back("vdschunkmetadata", std::string(base64Hash.begin(), base64Hash.end()));
  std::string str = "Data to write";
  std::vector<uint8_t> vec(str.begin(), str.end());
  std::shared_ptr<std::vector<uint8_t>> to_write = std::make_shared<std::vector<uint8_t>>(vec);

  // Test that we can upload and download some data
  {
    std::shared_ptr<OpenVDS::Request> uploadRequest = m_ioManager->WriteObject("aztest1", "NA", "Text", meta_map, to_write, completedCallback);
    while (!uploadRequest->IsDone())
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    bool uploadStatus = uploadRequest->IsSuccess(azureManagerError);
    ASSERT_TRUE(uploadStatus);
  }

  {
    auto transferHandler = std::make_shared<AzureTransfer>();

    std::shared_ptr<OpenVDS::Request> downloadRequest = m_ioManager->ReadObject("aztest1", transferHandler);

    downloadRequest->WaitForFinish();
    ASSERT_TRUE(downloadRequest->IsDone());

    bool downloadStatus = downloadRequest->IsSuccess(azureManagerError);
    ASSERT_TRUE(downloadStatus);
  }

  // Test that we can upload and download a 0 byte chunk
  {
    to_write->clear();
    std::shared_ptr<OpenVDS::Request> uploadRequest = m_ioManager->WriteObject("aztest2", "", "Text", meta_map, to_write, completedCallback);

    uploadRequest->WaitForFinish();
    ASSERT_TRUE(uploadRequest->IsDone());

    bool uploadStatus = uploadRequest->IsSuccess(azureManagerError);
    ASSERT_TRUE(uploadStatus);
  }

  {
    auto transferHandler = std::make_shared<AzureTransfer>();

    std::shared_ptr<OpenVDS::Request> downloadRequest = m_ioManager->ReadObject("aztest2", transferHandler);

    downloadRequest->WaitForFinish();
    ASSERT_TRUE(downloadRequest->IsDone());

    bool downloadStatus = downloadRequest->IsSuccess(azureManagerError);
    ASSERT_TRUE(downloadStatus);
  }

  // Test that we can catch an error if we download something that doesn't exist
  {
    auto transferHandler = std::make_shared<AzureTransfer>();

    std::shared_ptr<OpenVDS::Request> downloadRequest = m_ioManager->ReadObject("aztest_not_existing_blob", transferHandler);

    downloadRequest->WaitForFinish();
    ASSERT_TRUE(downloadRequest->IsDone());

    bool downloadStatus = downloadRequest->IsSuccess(azureManagerError);
    ASSERT_FALSE(downloadStatus);
  }
}