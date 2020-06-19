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

#include "IOManagerGoogle.h"

#include <fmt/format.h>
#include <sstream>
#include <iomanip>

namespace OpenVDS
{
  constexpr char GOOGLEAPIS[] = "https://storage.googleapis.com";

  IOManagerGoogle::IOManagerGoogle(const GoogleOpenOptions& openOptions, Error &error)
    : m_curlHandler(error)
    , m_bucket(openOptions.bucket)
  {
    if (m_bucket.empty())
    {
      error.code = -1;
      error.string = "Google Cloud Storage Config error. Empty bucket";
      return;
    }

    auto credentials = google::cloud::storage::v1::oauth2::GoogleDefaultCredentials();
    if (!credentials) {
        error.code = -2;
        error.string = "Google Cloud Storage Config error. Unable to get Google Default Credentials.";
        return;
    }
    m_credentials = *credentials;

  }

  std::string convertToISO8601(const std::string& value);

  std::shared_ptr<Request> IOManagerGoogle::ReadObjectInfo(const std::string& objectName, std::shared_ptr<TransferDownloadHandler> handler)
  {
    //std::string url = fmt::format("{}/storage/v1/b/{}/o/{}?alt=media", GOOGLEAPIS, m_bucket, objectName);
    std::string url = fmt::format("{}/{}/{}", GOOGLEAPIS, m_bucket, objectName);
    std::shared_ptr<DownloadRequestCurl> request = std::make_shared<DownloadRequestCurl>(objectName, handler);
    std::vector<std::string> headers;

    auto authorization_header = m_credentials->AuthorizationHeader();
    if (!authorization_header) {
      request->m_done = true;
      request->m_cancelled = true;
      request->m_error.code = -3;
      request->m_error.string = "Google Cloud Storage Config error. Unable to generate Authorization Header.";
      return request;
    }
    headers.push_back(*authorization_header);
    m_curlHandler.addDownloadRequest(request, url, headers, convertToISO8601, CurlDownloadHandler::HEADER);
    return request;
  }

  std::shared_ptr<Request> IOManagerGoogle::ReadObject(const std::string& objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range)
  {
    //std::string url = fmt::format("{}/storage/v1/b/{}/o/{}?alt=media", GOOGLEAPIS, m_bucket, objectName); //I cant make this work with sub-paths!!!
    std::string url = fmt::format("{}/{}/{}", GOOGLEAPIS, m_bucket, objectName);
    std::shared_ptr<DownloadRequestCurl> request = std::make_shared<DownloadRequestCurl>(objectName, handler);
    std::vector<std::string> headers;
    auto authorization_header = m_credentials->AuthorizationHeader();
    if (!authorization_header) {
      request->m_done = true;
      request->m_cancelled = true;
      request->m_error.code = -3;
      request->m_error.string = "Google Cloud Storage Config error. Unable to generate Authorization Header.";
      return request;
    }
    headers.push_back(*authorization_header);
    if (range.start != range.end)
    {
      headers.emplace_back();
      auto& header = headers.back();
      header = fmt::format("range: bytes={}-{}", range.start, range.end);
    }
    m_curlHandler.addDownloadRequest(request, url, headers, convertToISO8601, CurlDownloadHandler::GET);
    return request;
  }

  std::shared_ptr<Request> IOManagerGoogle::WriteObject(const std::string& objectName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request& request, const Error& error)> completedCallback)
  {
    std::string url = fmt::format("{}/upload/storage/v1/b/{}/o?uploadType=multipart", GOOGLEAPIS, m_bucket, objectName);
    std::shared_ptr<UploadRequestCurl> request = std::make_shared<UploadRequestCurl>(objectName, completedCallback);
    static const std::string delimiterStr = "foo_openvds_delimiter_baz";
    std::vector<std::string> headers;
    auto authorization_header = m_credentials->AuthorizationHeader();
    if (!authorization_header) {
      request->m_done = true;
      request->m_cancelled = true;
      request->m_error.code = -3;
      request->m_error.string = "Google Cloud Storage Config error. Unable to generate Authorization Header.";
      return request;
    }
    headers.push_back(*authorization_header);
    headers.push_back(fmt::format("Content-Type: multipart/related; boundary={}", delimiterStr));

    Json::Value jsonHeader;
    jsonHeader["name"] = objectName;
    if (contentDispostionFilename.size())
      jsonHeader["Content-Disposition"] = fmt::format("attachment; filename={}", contentDispostionFilename);
    if (metadataHeader.size())
    {
      auto& metaJson = jsonHeader["metadata"];
      for (auto metaTag : metadataHeader)
      {
        metaJson[metaTag.first] = metaTag.second;
      }
    }
    std::string jsonHeaderString = Json::writeString(m_jsonWriterBuilder, jsonHeader);

    std::string multipartMsgHeader = fmt::format("--{}\nContent-Type: application/json; charset=UTF-8\n\n{}\n\n--{}\nContent-Type: {}\n\n", delimiterStr, jsonHeaderString, delimiterStr, contentType);
    
    std::string multipartMsgFooter = fmt::format("\n--{}--", delimiterStr);

    std::vector<std::shared_ptr<std::vector<uint8_t>>> upload_buffers;
    upload_buffers.reserve(3);
    upload_buffers.emplace_back(std::make_shared<std::vector<uint8_t>>(multipartMsgHeader.begin(), multipartMsgHeader.end()));
    upload_buffers.emplace_back(data);
    upload_buffers.emplace_back(std::make_shared<std::vector<uint8_t>>(multipartMsgFooter.begin(), multipartMsgFooter.end()));

    size_t uploadSize = multipartMsgHeader.size() + data->size() + multipartMsgFooter.size();

    headers.push_back(fmt::format("Content-Length: {}", uploadSize));

    m_curlHandler.addUploadRequest(request, url, headers, true, std::move(upload_buffers), uploadSize);
    return request;
  }
}