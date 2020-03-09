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
  case OpenOptions::AWS:
    return new IOManagerAWS(static_cast<const AWSOpenOptions &>(options), error);
  case OpenOptions::Azure:
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
