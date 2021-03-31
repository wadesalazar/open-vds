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

#include "IOManagerHttp.h"

#include <fmt/format.h>

namespace OpenVDS
{
  IOManagerHttp::IOManagerHttp(const HttpOpenOptions &openOptions, Error &error)
    : IOManager(OpenOptions::Http)
    , m_curlHandler(error)
  {
    const std::string &url = openOptions.url;
    if (url.empty())
    {
      error.code = -1;
      error.string = "IOManagerHttp: empty url";
      return;
    }

    auto it = std::find(url.begin(), url.end(), '?');
    if (it != url.end())
    {
      m_base = std::string(url.begin(), it);
      m_suffix = std::string(it, url.end());
    } else
    {
      m_base = url;
    }

    if (m_base.back() != '/')
      m_base.push_back('/');

    if (m_suffix.size())
    {
      if (m_suffix[0] != '?')
        m_suffix.insert(m_suffix.begin(), '?');
    }
  }

  std::shared_ptr<Request> IOManagerHttp::ReadObjectInfo(const std::string& objectName, std::shared_ptr<TransferDownloadHandler> handler)
  {
    std::string url = m_base + objectName + m_suffix;
    std::shared_ptr<DownloadRequestCurl> request = std::make_shared<DownloadRequestCurl>(objectName, handler);
    std::vector<std::string> headers;
    m_curlHandler.addDownloadRequest(request, url, headers, convertToISO8601, CurlDownloadHandler::HEADER);
    return request;
  }
  
  std::shared_ptr<Request> IOManagerHttp::ReadObject(const std::string& objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range)
  {
    std::string url = m_base + objectName + m_suffix;
    std::shared_ptr<DownloadRequestCurl> request = std::make_shared<DownloadRequestCurl>(objectName, handler);
    std::vector<std::string> headers;
    if (range.start != range.end)
    {
      headers.emplace_back();
      auto& header = headers.back();
      header = fmt::format("range: bytes={}-{}", range.start, range.end);
    }
    m_curlHandler.addDownloadRequest(request, url, headers, convertToISO8601, CurlDownloadHandler::GET);
    return request;
  }

  class ErrorRequest : public Request
  {
    public:
    ErrorRequest(const std::string &objectName) : Request(objectName) {}
    bool WaitForFinish(Error &error) override
    {
      error.code = -1;
      error.string = "The http IO backend does not support writing data, use one of the cloud vendor specific backends such as S3 Azure or Google.";
      return false;
    }
    void Cancel() override {}
  };

  std::shared_ptr<Request> IOManagerHttp::WriteObject(const std::string& objectName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request& request, const Error& error)> completedCallback)
  {
    return std::make_shared<ErrorRequest>(objectName);
  }
}