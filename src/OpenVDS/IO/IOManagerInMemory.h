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

#ifndef IOMANAGERINMEMORY_H
#define IOMANAGERINMEMORY_H

#include "IOManager.h"
#include <VDS/ThreadPool.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <mutex>
#include <cstdint>

namespace OpenVDS
{
  struct Object
  {
    std::vector<std::pair<std::string, std::string>> metaHeader;
    std::vector<uint8_t> data;
  };

  class IOManagerInMemory : public IOManager
  {
    public:
      static IOManager *CreateIOManagerInMemory(const InMemoryOpenOptions& openOptions, Error& error);
      ~IOManagerInMemory() override;

      std::shared_ptr<Request> ReadObjectInfo(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler) override;
      std::shared_ptr<Request> ReadObject(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range = IORange()) override;
      std::shared_ptr<Request> WriteObject(const std::string &objectName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback = nullptr) override;
    private:
      IOManagerInMemory(const InMemoryOpenOptions &openOptions, Error &error);
      std::mutex m_mutex;
      std::unordered_map<std::string, Object> m_data;
      ThreadPool m_threadPool;
  };
}

#endif //IOMANAGERINMEMORY_H
