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

#include <memory>
#include <OpenVDS/OpenVDS.h>

namespace OpenVDS
{
  class TransferHandler
  {
  public:
    virtual ~TransferHandler();
    virtual void handleMetadata(const std::string &key, const std::string &header);
    virtual void handleData(std::vector<uint8_t> &&data) = 0;
    virtual void handleError(Error &error) = 0;
  };

  class Request
  {
  public:
    virtual ~Request();
    virtual void waitForFinish() = 0;
    virtual bool isDone() const = 0;
    virtual bool isSuccess(Error &error) const = 0;
    virtual void cancel() = 0;
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
    virtual std::shared_ptr<Request> downloadObject(const std::string objectName, std::shared_ptr<TransferHandler> handler, const IORange &range = IORange()) = 0;
    virtual std::shared_ptr<Request> uploadObject(const std::string objectName, std::shared_ptr<std::vector<uint8_t>> data, const IORange &range = IORange()) = 0;

    static IOManager *createIOManager(const OpenOptions &options, Error &error);
  };

}

#endif
