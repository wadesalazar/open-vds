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

#ifndef IOMANAGER_H
#define IOMANAGER_H

#include <OpenVDS/OpenVDS.h>

#include <memory>
#include <functional>

namespace OpenVDS
{
  class Request;
  class TransferDownloadHandler
  {
  public:
    virtual ~TransferDownloadHandler() {};
    virtual void HandleObjectSize(int64_t size) = 0;
    virtual void HandleObjectLastWriteTime(const std::string &lastWriteTimeISO8601) = 0;
    virtual void HandleMetadata(const std::string &key, const std::string &header) = 0;
    virtual void HandleData(std::vector<uint8_t> &&data) = 0;
    virtual void Completed(const Request &request, const Error &error) = 0;
  };

  class Request
  {
  public:
    Request(const std::string &objectName);
    virtual ~Request();
    virtual bool WaitForFinish(Error &error) = 0;
    virtual void Cancel() = 0;
    const std::string &GetObjectName() const { return m_objectName; }

  private:
    std::string m_objectName;
  };

  struct IORange
  {
    int64_t start;
    int64_t end;
  };

  class IOManager
  {
  public:
    enum AccessPattern
    {
      ReadOnly,
      ReadWrite
    };

    IOManager(OpenOptions::ConnectionType connectionType);
    virtual ~IOManager();
    virtual std::shared_ptr<Request> ReadObjectInfo(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler) = 0;
    virtual std::shared_ptr<Request> ReadObject(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange &range = IORange()) = 0;
    virtual std::shared_ptr<Request> WriteObject(const std::string &objectName, const std::string &contentDispostionFilename, const std::string &contentType, const std::vector<std::pair<std::string, std::string>> &metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request &request, const Error &error)> completedCallback = nullptr) = 0;
    std::shared_ptr<Request> UploadBinary(const std::string &objectName, const std::string &contentDispositionFilename, const std::vector<std::pair<std::string, std::string>> &metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request &request, const Error &error)> completedCallback = nullptr)
    {
      return WriteObject(objectName, contentDispositionFilename, "application/octet-stream", metadataHeader, data, completedCallback);
    }
    std::shared_ptr<Request> UploadJson(const std::string &objectName, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request &request, const Error &error)> completedCallback = nullptr)
    {
      return WriteObject(objectName, "", "application/json", std::vector<std::pair<std::string, std::string>>(), data, completedCallback);
    }

    OpenOptions::ConnectionType connectionType() const { return m_connectionType; }

    OPENVDS_EXPORT
    static IOManager *CreateIOManager(const OpenOptions &options, IOManager::AccessPattern accessPattern, Error &error);

    OPENVDS_EXPORT
    static IOManager *CreateIOManager(const StringWrapper& url, const StringWrapper& connectionString, IOManager::AccessPattern accessPattern, Error& error);

  protected:
    OpenOptions::ConnectionType m_connectionType;
  };

}

#endif
