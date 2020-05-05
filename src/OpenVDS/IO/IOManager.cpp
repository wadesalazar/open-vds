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

#include "IOManagerInMemory.h"

#ifndef OPENVDS_NO_AWS_IOMANAGER
#include "IOManagerAWS.h"
#endif
#ifndef OPENVDS_NO_AZURE_IOMANAGER
#include "IOManagerAzure.h"
#endif

#ifndef OPENVDS_NO_CURL_IOMANAGER
#include "IOManagerAzurePresigned.h"
#endif

namespace OpenVDS
{
Request::Request(const std::string& objectName)
  : m_objectName(objectName)
{
}
Request::~Request()
{}

IOManager::~IOManager()
{}
IOManager* IOManager::CreateIOManager(const OpenOptions& options, Error &error)
{
  switch(options.connectionType)
  {
#ifndef OPENVDS_NO_AWS_IOMANAGER
  case OpenOptions::AWS:
    return new IOManagerAWS(static_cast<const AWSOpenOptions &>(options), error);
#endif
#ifndef OPENVDS_NO_AZURE_IOMANAGER
  case OpenOptions::Azure:
      return new IOManagerAzure(static_cast<const AzureOpenOptions&>(options), error);
#endif
#ifndef OPENVDS_NO_CURL_IOMANAGER
  case OpenOptions::AzurePresigned:
    return new IOManagerAzurePresigned(static_cast<const AzurePresignedOpenOptions&>(options).baseUrl, static_cast<const AzurePresignedOpenOptions&>(options).urlSuffix, error);
#endif
  case OpenOptions::InMemory:
    return new IOManagerInMemory(static_cast<const InMemoryOpenOptions &>(options), error);
  default:
    error.code = -1;
    error.string = "Unknwon type for OpenOptions";
    return nullptr;
  }
}
}
