/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
** Copyright 2020 Microsoft Corp.
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

#include "IOManager.h"

#include "IOManagerAWS.h"
#include "IOManagerAzure.h"
#include "IOManagerInMemory.h"
#include "IOManagerCurlAWS.h"
#include "IOManagerCurlAzure.h"

#include <VDS/Env.h>

namespace OpenVDS
{
Request::Request(const std::string& objectName)
  : m_objectName(objectName)
{
}
Request::~Request()
{}

TransferDownloadHandler::~TransferDownloadHandler()
{
}
void TransferDownloadHandler::HandleMetadata(const std::string& key, const std::string& header)
{
}

IOManager::~IOManager()
{}
IOManager* IOManager::CreateIOManager(const OpenOptions& options, Error &error)
{
  bool use_curl = getBooleanEnvironmentVariable("OPENVDS_USE_CURL");
  switch(options.connectionType)
  {
  case OpenOptions::AWS:
    if (use_curl)
      return new IOManagerCurlAWS(static_cast<const AWSOpenOptions&>(options), error);

    return new IOManagerAWS(static_cast<const AWSOpenOptions&>(options), error);
  case OpenOptions::Azure:
    if (use_curl)
      return new IOManagerCurlAzure(static_cast<const AzureOpenOptions&>(options), error);

    return new IOManagerAzure(static_cast<const AzureOpenOptions&>(options), error);
  case OpenOptions::InMemory:
    return new IOManagerInMemory(static_cast<const InMemoryOpenOptions &>(options), error);
  default:
    error.code = -1;
    error.string = "Unknwon type for OpenOptions";
    return nullptr;
  }
}
}
