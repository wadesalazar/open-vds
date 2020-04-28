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

#include "IOManagerGS.h"

namespace OpenVDS
{
  GetOrHeadRequestGS::GetOrHeadRequestGS(const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler)
    : Request(id)
    , m_handler(handler)
    , m_cancelled(false)
    , m_done(false)
  {
  }

  GetOrHeadRequestGS::~GetOrHeadRequestGS()
  {
    GetOrHeadRequestGS::Cancel();
  }

  void GetOrHeadRequestGS::WaitForFinish()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_waitForFinish.wait(lock, [this]{ return m_done; });
  }
  bool GetOrHeadRequestGS::IsDone() const
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_done;
  }

  bool GetOrHeadRequestGS::IsSuccess(Error& error) const
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_done)
    {
      error.code = -1;
      error.string = "Download not done.";
      return false;
    }
    error = m_error;
    return m_error.code == 0;
  }

  void GetOrHeadRequestGS::Cancel()
  {
    m_cancelled = true;
  }

  ReadObjectInfoRequestGS::ReadObjectInfoRequestGS(const std::string &id, const std::shared_ptr<TransferDownloadHandler>& handler)
    : GetOrHeadRequestGS(id, handler)
  {
  }

  void ReadObjectInfoRequestGS::run(const std::string& bucket, std::weak_ptr<ReadObjectInfoRequestGS> readObjectInfoRequest)
  {
  }

  DownloadRequestGS::DownloadRequestGS(const std::string &id, const std::shared_ptr<TransferDownloadHandler>& handler)
    : GetOrHeadRequestGS(id, handler)
  {
  }

  void DownloadRequestGS::run(const std::string& bucket, const IORange& range, std::weak_ptr<DownloadRequestGS> downloadRequest)
  {
  }

  UploadRequestGS::UploadRequestGS(const std::string& id, std::function<void(const Request & request, const Error & error)> completedCallback)
    : Request(id)
    , m_cancelled(false)
    , m_done(false)
  {
  }
 
  void UploadRequestGS::run(const std::string& bucket, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::weak_ptr<UploadRequestGS> uploadRequest)
  {
  }

  void UploadRequestGS::WaitForFinish()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_waitForFinish.wait(lock, [this]{ return this->m_done; });
  }
  bool UploadRequestGS::IsDone() const
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_done;
  }
  bool UploadRequestGS::IsSuccess(Error& error) const
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_done)
    {
      error.code = -1;
      error.string = "Download not done.";
      return false;
    }
    error = m_error;
    return m_error.code == 0;
  }
  void UploadRequestGS::Cancel()
  {
    m_cancelled = true;
  }

  IOManagerGS::IOManagerGS(const GSOpenOptions& openOptions, Error &error)
    : m_bucket(openOptions.bucket)
    , m_objectId(openOptions.key)
  {
    if (m_bucket.empty())
    {
      error.code = -1;
      error.string = "GS Config error. Empty bucket or region";
      return;
    }
  }

  IOManagerGS::~IOManagerGS()
  {
  }

  std::shared_ptr<Request> IOManagerGS::ReadObjectInfo(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler)
  {
    std::string id = objectName.empty()? m_objectId : m_objectId + "/" + objectName;
    auto ret = std::make_shared<ReadObjectInfoRequestGS>(id, handler);
    ret->run(m_bucket, ret);
    return ret;
  }

  std::shared_ptr<Request> IOManagerGS::ReadObject(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange &range)
  {
    std::string id = objectName.empty()? m_objectId : m_objectId + "/" + objectName;
    auto ret = std::make_shared<DownloadRequestGS>(id, handler);
    ret->run(m_bucket, range, ret);
    return ret;
  }

  std::shared_ptr<Request> IOManagerGS::WriteObject(const std::string &objectName, const std::string& contentDispositionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback)
  {
    std::string id = objectName.empty()? m_objectId : m_objectId + "/" + objectName;
    auto ret = std::make_shared<UploadRequestGS>(id, completedCallback);
    ret->run(m_bucket, contentDispositionFilename, contentType, metadataHeader, data, ret);
    return ret;
  }
}
