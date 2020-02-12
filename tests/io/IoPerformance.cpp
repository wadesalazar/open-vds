#include <gtest/gtest.h>
#include <iostream>
#include <IO/IOManager.h>
#include <OpenVDS/OpenVDS.h>
#include <chrono>
#include <random>

#include <fmt/printf.h>

class Transfer : public OpenVDS::TransferDownloadHandler
{
public:
    Transfer() {}
    OpenVDS::Error m_error;

  std::vector<uint8_t> m_data;
  std::vector<uint8_t> m_metadata;

    ~Transfer() override
    {
    }

    void HandleMetadata(const std::string& key, const std::string& header) override
    {
    }

    void HandleData(std::vector<uint8_t>&& data) override
    {
      m_data = std::move(data);
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

TEST(IOTests, performance)
{
  //OpenVDS::AzureOpenOptions options;
  //options.connectionString = TEST_AZURE_CONNECTION;
  //options.container = "SIMPLE_NOISE_VDS";
  //options.parallelism_factor = 8;

  //if (options.connectionString.empty())
  //{
  //  GTEST_SKIP() << "Environment variables not set";
  //}

  //OpenVDS::AWSOpenOptions options;
  //options.region = TEST_AWS_REGION;
  //options.bucket = TEST_AWS_BUCKET;
  //options.key = "SIMPLE_NOISE_VDS";
  OpenVDS::AWSOpenOptions options;
  options.region = TEST_AWS_REGION;
  options.bucket = TEST_AWS_BUCKET;
  options.key = "SIMPLE_NOISE_VDS2";

  if (options.region.empty() || options.bucket.empty())
  {
    GTEST_SKIP() << "Environment variables not set";
  }

  OpenVDS::Error error;
  OpenVDS::IOManager* ioManager = OpenVDS::IOManager::CreateIOManager(options, error);

  int mega = 1 << 20;
  int chunkSize = mega * 4;
  int chunkCount = 100;
  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<uint64_t> dis(0);
  std::vector<std::shared_ptr<std::vector<uint8_t>>> upload_data_buffers;
  upload_data_buffers.reserve(chunkCount);
  for (int i = 0; i < chunkCount; i++)
  {
    upload_data_buffers.push_back(std::make_shared<std::vector<uint8_t>>(chunkSize));
    auto& buf = *upload_data_buffers.back();
    for (int n = 0; n < chunkSize; n += sizeof(decltype(dis(gen))))
    {
      auto v = dis(gen);
      memcpy(buf.data() + n, &v, sizeof(v));
    }
  }

  std::vector<std::shared_ptr<OpenVDS::Request>> uploadRequests;
  uploadRequests.reserve(chunkCount);

  fmt::print("Done generating random data. File size is {}, number of chunks is {}\n", chunkSize, chunkCount);

  auto upload_start_time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < chunkCount; i++)
  {
    auto req = ioManager->UploadBinary(std::to_string(i), "", {}, upload_data_buffers[i], nullptr);
    uploadRequests.push_back(req);
  }

  for (int i = 0; i < chunkCount; i++)
  {
    uploadRequests[i]->WaitForFinish();
  }
  auto upload_end_time = std::chrono::high_resolution_clock::now();

  double upload_time = std::chrono::duration_cast<std::chrono::milliseconds>(upload_end_time - upload_start_time).count() / 1000.0;
  fmt::print("Done uploading data in {} seconds\n", upload_time);
  for (int i = 0; i < chunkCount; i++)
  {
    if (!uploadRequests[i]->IsSuccess(error))
    {
      fmt::print(stderr, "Failed to upload {} with error code {} : {}\n", uploadRequests[i]->GetObjectName(), error.code, error.string);
      ASSERT_TRUE(false);
    }
  }

  std::vector<std::shared_ptr<Transfer>> downloadTransfers;
  downloadTransfers.reserve(chunkCount);
  std::vector<std::shared_ptr<OpenVDS::Request>> downloadRequests;
  downloadRequests.reserve(chunkCount);

  auto download_start_time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < chunkCount; i++)
  {
    downloadTransfers.push_back(std::make_shared<Transfer>());
    downloadRequests.push_back(ioManager->Download(std::to_string(i), downloadTransfers.back()));
  }

  for (int i = 0; i < chunkCount; i++)
  {
    downloadRequests[i]->WaitForFinish();
  }
  auto download_end_time = std::chrono::high_resolution_clock::now();

  int64_t bytes = 0;
  for (int i = 0; i < chunkCount; i++)
  {
    if (!downloadRequests[i]->IsSuccess(error))
    {
      fmt::print(stderr, "Failed to download {} with error code {} : {}\n", downloadRequests[i]->GetObjectName(), error.code, error.string);
      ASSERT_TRUE(false);
    }
    bytes += downloadTransfers[i]->m_data.size();
  }

  double download_time = std::chrono::duration_cast<std::chrono::milliseconds>(download_end_time - download_start_time).count() / 1000.0;
  int64_t bits = bytes * 8;
  double mbytes = bytes / double(mega);
  double mbitsSecUpload = bits / upload_time / double(mega);
  double mbitsSecDownload = bits / download_time / double(mega);
  fmt::print(stderr, "Completed Uploading {} MB in {} Seconds giving {} Mbit/s\n", mbytes, upload_time, mbitsSecUpload);
  fmt::print(stderr, "Completed Downloading {} MB in {} Seconds giving {} Mbit/s\n", mbytes, download_time, mbitsSecDownload);
  fmt::print(stderr, "Asserting data is the same....\n");
  for (int i = 0; i < chunkCount; i++)
  {
    ASSERT_TRUE(downloadTransfers[i]->m_data.size() == upload_data_buffers[i]->size());
    ASSERT_TRUE(memcmp(downloadTransfers[i]->m_data.data(), upload_data_buffers[i]->data(), upload_data_buffers[i]->size()) == 0);
  }

}
