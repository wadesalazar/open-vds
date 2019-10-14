/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
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

#ifndef IOMANAGERAWS_H
#define IOMANAGERAWS_H

#include "IOManager.h"

#include <vector>
#include <string>
#include <aws/s3/S3Client.h>
namespace OpenVDS
{
  class DownloadRequestAWS;
  class UploadRequestAWS;
  template<typename> struct AsyncContext;
  using AsyncDownloadContext = AsyncContext<DownloadRequestAWS>;
  using AsyncUploadContext = AsyncContext<UploadRequestAWS>;

  class DownloadRequestAWS : public Request
  {
  public:
    DownloadRequestAWS(const std::string &id, const std::shared_ptr<TransferDownloadHandler>& handler);
    ~DownloadRequestAWS() override;

    void run(Aws::S3::S3Client& client, const std::string& bucket, const IORange& range, std::weak_ptr<DownloadRequestAWS> request);

    void waitForFinish() override;
    bool isDone() const override;
    bool isSuccess(Error &error) const override;
    void cancel() override;

    std::shared_ptr<TransferDownloadHandler> m_handler;
    std::atomic_bool m_cancelled;
    bool m_done;
    Error m_error;
    std::condition_variable m_waitForFinish;
    mutable std::mutex m_mutex;
  };

  class VectorBuf : public std::basic_streambuf<char, std::char_traits<char>>
  {
  public:
    VectorBuf(std::vector<uint8_t>& vec)
    {
      setg((char *) vec.data(), (char *) vec.data(), (char *) vec.data() + vec.size());
    }
  };

  class IOStream : public Aws::IOStream
  {
  public:
    IOStream(std::shared_ptr<std::vector<uint8_t>> data)
      : Aws::IOStream(&m_buffer)
      , m_data(data)
      , m_buffer(*data)
    {}
    std::shared_ptr<std::vector<uint8_t>> m_data;
    VectorBuf m_buffer;
  };

  class UploadRequestAWS : public Request
  {
  public:
    UploadRequestAWS(const std::string &id, std::function<void(const Request & request, const Error & error)> completedCallback);
    void run(Aws::S3::S3Client& client, const std::string& bucket, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::weak_ptr<UploadRequestAWS> uploadRequest);
    void waitForFinish() override;
    bool isDone() const override;
    bool isSuccess(Error &error) const override;
    void cancel() override;

    std::function<void(const Request &request, const Error &error)> m_completedCallback;
    std::shared_ptr<IOStream> m_stream;
    std::atomic_bool m_cancelled;
    bool m_done;
    Error m_error;
    std::condition_variable m_waitForFinish;
    mutable std::mutex m_mutex;
  };

  class IOManagerAWS : public IOManager
  {
    public:
      IOManagerAWS(const AWSOpenOptions &openOptions, Error &error);
      ~IOManagerAWS() override;

      std::shared_ptr<Request> download(const std::string objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range = IORange()) override;
      std::shared_ptr<Request> upload(const std::string objectName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback = nullptr) override;
    private:
      std::string m_region;
      std::string m_bucket;
      std::string m_objectId;
      std::unique_ptr<Aws::S3::S3Client> m_s3Client;
  };
}
#endif //IOMANAGERAWS_H
