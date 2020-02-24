/****************************************************************************
** Copyright 2020 The Open Group
** Copyright 2020 Bluware, Inc.
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

#ifndef FACADEIOMANAGER_H
#define FACADEIOMANAGER_H

#include <OpenVDS/OpenVDS.h>

#include <IO/IOManager.h>
#include <VDS/ThreadPool.h>

class FacadeRequest : public OpenVDS::Request
{
public:
  FacadeRequest(const std::string &objectName, const OpenVDS::Error &error)
    : Request(objectName)
    , m_error(error)
  {

  }
  void WaitForFinish() override
  {
  }
  bool IsDone() const override
  {
    return true;
  }
  bool IsSuccess(OpenVDS::Error &error) const override
  {
    error = m_error;
    return m_error.code == 0;
  }
  void Cancel() override
  {
  }

private:
  OpenVDS::Error m_error;
};

struct Object
{
  std::vector<std::pair<std::string, std::string>> metaHeader;
  std::vector<uint8_t> data;
  OpenVDS::Error error;
};

class IOManagerFacadeLight : public OpenVDS::IOManager
{
public:
  IOManagerFacadeLight(OpenVDS::IOManager *backend)
    : backend(backend)
  {}

  std::shared_ptr<OpenVDS::Request> Download(const std::string objectName, std::shared_ptr<OpenVDS::TransferDownloadHandler> handler, const OpenVDS::IORange& range = OpenVDS::IORange()) override
  {
    return backend->Download(objectName, handler, range);
  }

  std::shared_ptr<OpenVDS::Request> Upload(const std::string objectName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const OpenVDS::Request & request, const OpenVDS::Error & error)> completedCallback = nullptr) override
  {
    return backend->Upload(objectName, contentDispostionFilename, contentType, metadataHeader, data, completedCallback);
  }

  IOManager *backend;
};
class IOManagerFacade : public OpenVDS::IOManager
{
public:
  IOManagerFacade(OpenVDS::IOManager *backend)
    : backend(backend)
    , threadPool(1)
  {}

  std::shared_ptr<OpenVDS::Request> Download(const std::string objectName, std::shared_ptr<OpenVDS::TransferDownloadHandler> handler, const OpenVDS::IORange& range = OpenVDS::IORange()) override
  {
    auto object_it = m_data.find(objectName);
    OpenVDS::Error error;
    if (object_it != m_data.end())
    {
      auto &object = (*object_it).second;
      error = object.error;
      for (auto& meta : object.metaHeader)
      {
        handler->HandleMetadata(meta.first, meta.second);
      }
      std::vector<uint8_t> data = object.data;
      handler->HandleData(std::move(data));
      auto request = std::make_shared<FacadeRequest>(objectName, error);
      threadPool.Enqueue([handler, error, request]
        {
          handler->Completed(*request, error);
        });
      return request;
    }

    return backend->Download(objectName, handler, range);
  }

  std::shared_ptr<OpenVDS::Request> Upload(const std::string objectName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const OpenVDS::Request & request, const OpenVDS::Error & error)> completedCallback = nullptr) override
  {
    auto object_it = m_data.find(objectName);
    OpenVDS::Error error;
    if (object_it != m_data.end())
    {
      auto &object = (*object_it).second;
      error = object.error;
      auto request = std::make_shared<FacadeRequest>(objectName, error);
      if (completedCallback)
      {
        threadPool.Enqueue([completedCallback, error, request] { completedCallback(*request, error); });
      }
      return request;
    }

    return backend->Upload(objectName, contentDispostionFilename, contentType, metadataHeader, data, completedCallback);
  }

  IOManager *backend;
  ThreadPool threadPool;
  std::unordered_map<std::string, Object> m_data;
};

#endif
