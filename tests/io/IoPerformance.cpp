#include <gtest/gtest.h>
#include <iostream>
#include <IO/IOManager.h>
#include <OpenVDS/OpenVDS.h>
#include <chrono>
#include <random>

#include <fmt/printf.h>

//#define IN_MEMORY 1
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

  void HandleObjectSize(int64_t size) override
  {
  }

  void HandleObjectLastWriteTime(const std::string &lastWriteTimeISO8601) override
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
}

TEST(IOTests, performance)
{
#ifdef IN_MEMORY
  OpenVDS::InMemoryOpenOptions options;
#else
  std::string url = TEST_URL;
  std::string connectionString = TEST_CONNECTION;
  if(url.empty())
  {
    GTEST_SKIP() << "Test Environment for connecting to VDS is not set";
  }
#endif

  GTEST_SKIP() << "This test has to be enabled manually";
  OpenVDS::Error error;
  OpenVDS::IOManager* ioManager = OpenVDS::IOManager::CreateIOManager(url, connectionString, OpenVDS::IOManager::AccessPattern::ReadWrite, error);

  int mega = 1 << 20;
  int chunkSize = mega * 4;
#ifdef IN_MEMORY
  int chunkCount = 1000;
#else
  int chunkCount = 100;
#endif
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
    downloadRequests.push_back(ioManager->ReadObject(std::to_string(i), downloadTransfers.back()));
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
