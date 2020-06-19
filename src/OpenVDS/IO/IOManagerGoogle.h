/****************************************************************************
** Copyright 2020 The Open Group
** Copyright 2020 Bluware, Inc.
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

#include "IOManagerCurl.h"

#include <google/cloud/storage/client.h>

namespace OpenVDS
{

class IOManagerGoogle : public IOManager
{
public:
  IOManagerGoogle(const GoogleOpenOptions& openOptions, Error& error);
  std::shared_ptr<Request> ReadObjectInfo(const std::string& objectName, std::shared_ptr<TransferDownloadHandler> handler) override;
  std::shared_ptr<Request> ReadObject(const std::string& objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range = IORange()) override;
  std::shared_ptr<Request> WriteObject(const std::string& objectName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request& request, const Error& error)> completedCallback = nullptr) override;
 
private:
  CurlHandler m_curlHandler;
  std::string m_bucket;
  std::shared_ptr<google::cloud::storage::v1::oauth2::Credentials> m_credentials;
};
}

#endif
