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

#include "IOManager.h"

#include "IOManagerAWS.h"

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
void TransferDownloadHandler::handleMetadata(const std::string& key, const std::string& header)
{
}

IOManager::~IOManager()
{}
IOManager* IOManager::createIOManager(const OpenOptions& options, Error &error)
{
  switch(options.connectionType)
  {
  case OpenOptions::AWS:
    return new IOManagerAWS(static_cast<const AWSOpenOptions &>(options), error);
  default:
    error.code = -1;
    error.string = "Unknwon type for OpenOptions";
    return nullptr;
  }
}
}
