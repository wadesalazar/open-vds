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
#include <map>
#include <functional>

namespace OpenVDS
{
  class Request;
  class TransferDownloadHandler
  {
  public:
    virtual ~TransferDownloadHandler();
    virtual void handleMetadata(const std::string &key, const std::string &header);
    virtual void handleData(std::vector<uint8_t> &&data) = 0;
    virtual void completed(const Request &request, const Error &error) = 0;
  };

  class Request
  {
  public:
    Request(const std::string &objectName);
    virtual ~Request();
    virtual void waitForFinish() = 0;
    virtual bool isDone() const = 0;
    virtual bool isSuccess(Error &error) const = 0;
    virtual void cancel() = 0;
    const std::string &getObjectName() const { return m_objectName; }

  private:
    std::string m_objectName;
  };

  struct IORange
  {
    size_t start;
    size_t end;
  };

  class IOManager
  {
  public:
    virtual ~IOManager();
    virtual std::shared_ptr<Request> downloadObject(const std::string objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange &range = IORange()) = 0;
    virtual std::shared_ptr<Request> uploadObject(const std::string objectName, std::shared_ptr<std::vector<uint8_t>> data, const std::map<std::string, std::string> &metadataHeader, std::function<void(const Request &request, const Error &error)> completedCallback = nullptr) = 0;
    std::shared_ptr<Request> uploadObject(const std::string objectName, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request &request, const Error &error)> completedCallback = nullptr)
    {
      return uploadObject(objectName, data, std::map<std::string, std::string>(), completedCallback);
    }


    static IOManager *createIOManager(const OpenOptions &options, Error &error);
  };

}

#endif
