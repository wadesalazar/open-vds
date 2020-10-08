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
  static const std::string GOOGLEAPIS = "https://storage.googleapis.com";
  static const std::string HEADER = "HEADER";
  static const std::string GET = "GET";
  static const std::string PUT = "PUT";

  class CredentialManagerGoogle
  {
  public:
      typedef std::pair<const std::string, const std::string> Option;

      virtual bool Authorize(
          std::string& url,
          std::vector<std::string>& headers,
          const std::string& verb,
          const std::string& bucket_name,
          const std::string& path_prefix,
          const std::string& object_name,
          const Option& option = Option()) = 0;
  
      virtual ~CredentialManagerGoogle() = default;
  };

  class CredentialManagerGoogleOAuthToken : public CredentialManagerGoogle
  {
      std::string token;

  public:
      explicit CredentialManagerGoogleOAuthToken(const std::string& token) : token(token) {}

      bool Authorize(
          std::string& url,
          std::vector<std::string>& headers,
          const std::string& verb,
          const std::string& bucket_name,
          const std::string& path_prefix,
          const std::string& object_name,
          const Option& option = Option()) override;
  };

  bool CredentialManagerGoogleOAuthToken::Authorize(
      std::string& url,
      std::vector<std::string>& headers,
      const std::string& verb,
      const std::string& bucket_name,
      const std::string& path_prefix,
      const std::string& object_name,
      const Option& option)
  {
      headers.push_back(fmt::format("Authorization: Bearer {}", token));
      return true;
  }

  class CredentialManagerGoogleOAuth2 : public CredentialManagerGoogle
  {
      typedef std::shared_ptr<google::cloud::storage::v1::oauth2::Credentials> CredentialsPtr;

      CredentialsPtr m_credentials;

  public:
      explicit CredentialManagerGoogleOAuth2(CredentialsPtr&& credentials) : m_credentials(std::move(credentials)) {}

      bool Authorize(
          std::string& url,
          std::vector<std::string>& headers,
          const std::string& verb,
          const std::string& bucket_name,
          const std::string& path_prefix,
          const std::string& object_name,
          const Option& option = Option()) override;
  };

  bool CredentialManagerGoogleOAuth2::Authorize(
      std::string& url,
      std::vector<std::string>& headers,
      const std::string& verb,
      const std::string& bucket_name,
      const std::string& path_prefix,
      const std::string& object_name,
      const Option& option)
  {
      auto authorization_header = m_credentials->AuthorizationHeader();
      if (!authorization_header) {
          return false;
      }
      
      headers.push_back(*authorization_header);
      return true;
  }

  class CredentialManagerGoogleSignedUrl : public CredentialManagerGoogle
  {
      typedef std::shared_ptr<google::cloud::storage::v1::oauth2::Credentials> CredentialsPtr;

      const std::chrono::seconds duration = std::chrono::minutes(15);
      google::cloud::storage::v1::Client m_client;

  public:
      explicit CredentialManagerGoogleSignedUrl(CredentialsPtr&& credentials) : m_client(std::move(credentials)) {}

      bool Authorize(
          std::string& url,
          std::vector<std::string>& headers,
          const std::string& verb,
          const std::string& bucket_name,
          const std::string& path_prefix,
          const std::string& object_name,
          const Option& option = Option()) override;
  };

  bool CredentialManagerGoogleSignedUrl::Authorize(
      std::string& url,
      std::vector<std::string>& headers,
      const std::string& verb,
      const std::string& bucket_name,
      const std::string& path_prefix,
      const std::string& object_name,
      const Option& option)
  {
      namespace gcs = google::cloud::storage;

      std::string object_prefix_name = (path_prefix.empty())
          ? object_name
          : fmt::format("{}/{}", path_prefix, object_name);

      auto signed_url = option.first.empty()
          ? m_client.CreateV4SignedUrl(verb, bucket_name, object_prefix_name, gcs::SignedUrlDuration(duration))
          : m_client.CreateV4SignedUrl(verb, bucket_name, object_prefix_name, gcs::SignedUrlDuration(duration),
              gcs::AddExtensionHeader(option.first, option.second));
      
      if (!signed_url) {
          return false;
      }

      url = *signed_url;
      return true;
  }

  IOManagerGoogle::IOManagerGoogle(const GoogleOpenOptions& openOptions, Error &error)
    : IOManager(OpenOptions::GoogleStorage)
    , m_curlHandler(error)
    , m_bucket(openOptions.bucket)
    , m_pathPrefix(openOptions.pathPrefix)
    , m_storageClass(openOptions.storageClass)
    , m_region(openOptions.region)
  {
    if (m_bucket.empty())
    {
      error.code = -1;
      error.string = "Google Cloud Storage Config error. Empty bucket";
      return;
    }

    switch (openOptions.credentialsType) {
    case GoogleOpenOptions::CredentialsType::Default:
      {
        auto credentials = google::cloud::storage::v1::oauth2::GoogleDefaultCredentials();
        if (!credentials) {
            error.code = -2;
            error.string = "Google Cloud Storage Config error. Unable to get Google Default Credentials.";
            return;
        }
        m_credentialsManager.reset(new CredentialManagerGoogleOAuth2(std::move(*credentials)));
      }
      break;
    case GoogleOpenOptions::CredentialsType::AccessToken:
      if (openOptions.credentials.empty()) {
          error.code = -2;
          error.string = "Google Cloud Storage Config error. Authorization Token is empty";
          return;
      }
      m_credentialsManager.reset(new CredentialManagerGoogleOAuthToken(openOptions.credentials));
      break;
    case GoogleOpenOptions::CredentialsType::JsonPath:
      {
        auto credentials = google::cloud::storage::v1::oauth2::CreateServiceAccountCredentialsFromJsonFilePath(openOptions.credentials);
        if (!credentials) {
            error.code = -2;
            error.string = "Google Cloud Storage Config error. Unable to create service account credentials fromJson file path.";
            return;
        }
        m_credentialsManager.reset(new CredentialManagerGoogleOAuth2(std::move(*credentials)));
      }
      break;
    case GoogleOpenOptions::CredentialsType::Json:
      {
        auto credentials = google::cloud::storage::v1::oauth2::CreateServiceAccountCredentialsFromJsonContents(openOptions.credentials);
        if (!credentials) {
            error.code = -2;
            error.string = "Google Cloud Storage Config error. Unable to create service account credentials from json contents";
            return;
        }
        m_credentialsManager.reset(new CredentialManagerGoogleOAuth2(std::move(*credentials)));
      }
      break;
    case GoogleOpenOptions::CredentialsType::SignedUrl:
      {
        auto credentials = google::cloud::storage::v1::oauth2::GoogleDefaultCredentials();
        if (!credentials) {
            error.code = -2;
            error.string = "Google Cloud Storage Config error. Unable to get Google Default Credentials.";
            return;
        }
        m_credentialsManager.reset(new CredentialManagerGoogleSignedUrl(std::move(*credentials)));
      }
      break;
    case GoogleOpenOptions::CredentialsType::SignedUrlJsonPath:
      {
        auto credentials = google::cloud::storage::v1::oauth2::CreateServiceAccountCredentialsFromJsonFilePath(openOptions.credentials);
        if (!credentials) {
            error.code = -2;
            error.string = "Google Cloud Storage Config error. Unable to create service account credentials fromJson file path.";
            return;
        }
        m_credentialsManager.reset(new CredentialManagerGoogleSignedUrl(std::move(*credentials)));
      }
      break;
    case GoogleOpenOptions::CredentialsType::SignedUrlJson:
      {
        auto credentials = google::cloud::storage::v1::oauth2::CreateServiceAccountCredentialsFromJsonContents(openOptions.credentials);
        if (!credentials) {
            error.code = -2;
            error.string = "Google Cloud Storage Config error. Unable to create service account credentials from json contents";
            return;
        }
        m_credentialsManager.reset(new CredentialManagerGoogleSignedUrl(std::move(*credentials)));
      }
      break;
    }
  }

  static std::string downloadUrl(const std::string& googleapi, const std::string& bucket, const std::string& pathPrefix, const std::string& objectName)
  {
    //std::string url = fmt::format("{}/storage/v1/b/{}/o/{}?alt=media", GOOGLEAPIS, m_bucket, objectName); //I cant make this scheme work
    if (pathPrefix.size())
      return fmt::format("{}/{}/{}/{}", googleapi, bucket, pathPrefix, objectName);
    return fmt::format("{}/{}/{}", googleapi, bucket, objectName);
  }

  std::shared_ptr<Request> IOManagerGoogle::ReadObjectInfo(const std::string& objectName, std::shared_ptr<TransferDownloadHandler> handler)
  {
    std::string url = downloadUrl(GOOGLEAPIS, m_bucket, m_pathPrefix, objectName);
    std::shared_ptr<DownloadRequestCurl> request = std::make_shared<DownloadRequestCurl>(objectName, handler);
    std::vector<std::string> headers;

    if (!m_credentialsManager->Authorize(url, headers, HEADER, m_bucket, m_pathPrefix, objectName)) {
      request->m_done = true;
      request->m_cancelled = true;
      request->m_error.code = -3;
      request->m_error.string = "Google Cloud Storage Config error. Unable to generate Authorization Header.";
      return request;
    }
    m_curlHandler.addDownloadRequest(request, url, headers, convertToISO8601, CurlDownloadHandler::HEADER);
    return request;
  }

  std::shared_ptr<Request> IOManagerGoogle::ReadObject(const std::string& objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range)
  {
    std::string url = downloadUrl(GOOGLEAPIS, m_bucket, m_pathPrefix, objectName);
    std::shared_ptr<DownloadRequestCurl> request = std::make_shared<DownloadRequestCurl>(objectName, handler);
    std::vector<std::string> headers;
    std::string option_name;
    std::string option_value;
    if (range.start != range.end)
    {
      option_name = "range";
      option_value = fmt::format("bytes={}-{}", range.start, range.end);

      headers.emplace_back();
      auto& header = headers.back();
      header = fmt::format("{}: {}", option_name, option_value);
    }
    if (!m_credentialsManager->Authorize(url, headers, GET, m_bucket, m_pathPrefix, objectName, std::make_pair(option_name, option_value))) {
        request->m_done = true;
        request->m_cancelled = true;
        request->m_error.code = -3;
        request->m_error.string = "Google Cloud Storage Config error. Unable to generate Authorization Header.";
        return request;
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
    headers.push_back(fmt::format("Content-Type: multipart/related; boundary={}", delimiterStr));

    Json::Value jsonHeader;
    if (m_pathPrefix.size())
      jsonHeader["name"] = fmt::format("{}/{}", m_pathPrefix, objectName);
    else
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

    if (!m_storageClass.empty())
    {
      headers.push_back(fmt::format("x-goog-storage-class: {}", m_storageClass));
    }

    if (!m_credentialsManager->Authorize(url, headers, PUT, m_bucket, m_pathPrefix, objectName)) {
        request->m_done = true;
        request->m_cancelled = true;
        request->m_error.code = -3;
        request->m_error.string = "Google Cloud Storage Config error. Unable to generate Authorization Header.";
        return request;
    }

    m_curlHandler.addUploadRequest(request, url, headers, true, std::move(upload_buffers), uploadSize);
    return request;
  }
}