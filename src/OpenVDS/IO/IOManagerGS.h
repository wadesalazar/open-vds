/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
** Copyright 2020 Google, Inc.
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

#ifndef IOMANAGERGS_H
#define IOMANAGERGS_H

#include "IOManager.h"

#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>
		
namespace OpenVDS
{
  class GetOrHeadRequestGS : public Request
  {
  public:
    GetOrHeadRequestGS(const std::string &id, const std::shared_ptr<TransferDownloadHandler>& handler);
    ~GetOrHeadRequestGS() override;


    void WaitForFinish() override;
    bool IsDone() const override;
    bool IsSuccess(Error &error) const override;
    void Cancel() override;

    std::shared_ptr<TransferDownloadHandler> m_handler;
    std::atomic_bool m_cancelled;
    bool m_done;
    Error m_error;
    std::condition_variable m_waitForFinish;
    mutable std::mutex m_mutex;
  };

  class ReadObjectInfoRequestGS : public GetOrHeadRequestGS
  {
  public:
    ReadObjectInfoRequestGS(const std::string &id, const std::shared_ptr<TransferDownloadHandler>& handler);
    void run(const std::string& bucket, std::weak_ptr<ReadObjectInfoRequestGS> request);
  };

  class DownloadRequestGS : public GetOrHeadRequestGS
  {
  public:
    DownloadRequestGS(const std::string &id, const std::shared_ptr<TransferDownloadHandler>& handler);
    void run(const std::string& bucket, const IORange& range, std::weak_ptr<DownloadRequestGS> request);
  };

  class UploadRequestGS : public Request
  {
  public:
    UploadRequestGS(const std::string &id, std::function<void(const Request & request, const Error & error)> completedCallback);
    void run(const std::string& bucket, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::weak_ptr<UploadRequestGS> uploadRequest);
    void WaitForFinish() override;
    bool IsDone() const override;
    bool IsSuccess(Error &error) const override;
    void Cancel() override;

    std::atomic_bool m_cancelled;
    bool m_done;
    Error m_error;
    std::condition_variable m_waitForFinish;
    mutable std::mutex m_mutex;
  };

  class IOManagerGS : public IOManager
  {
    public:
      IOManagerGS(const GSOpenOptions &openOptions, Error &error);
      ~IOManagerGS() override;

      std::shared_ptr<Request> ReadObjectInfo(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler) override;
      std::shared_ptr<Request> ReadObject(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range = IORange()) override;
      std::shared_ptr<Request> WriteObject(const std::string &objectName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback = nullptr) override;
    private:
      std::string m_bucket;
      std::string m_objectId;
  };
}
#endif //IOMANAGERGS_H
