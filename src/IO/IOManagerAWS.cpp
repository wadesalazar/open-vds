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

#include "IOManagerAWS.h"

#include <aws/core/Aws.h>
#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/GetBucketLocationRequest.h>
#include <aws/s3/model/GetBucketLocationResult.h>
#include <aws/s3/model/BucketLocationConstraint.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/core/utils/logging/DefaultLogSystem.h>
#include <aws/core/utils/logging/AWSLogging.h>
#include <mutex>
#include <functional>

#include <fmt/format.h>

namespace OpenVDS
{

  static int initialize_sdk = 0;
  static std::mutex initialize_sdk_mutex;
  static Aws::SDKOptions initialize_sdk_options;

  static Aws::String convertStdString(const std::string &s)
  {
    return Aws::String(s.begin(), s.end());
  }

  static std::string convertAwsString(const Aws::String &s)
  {
    return std::string(s.begin(), s.end());
  }

  static void initializeAWSSDK()
  {

    std::unique_lock<std::mutex> lock(initialize_sdk_mutex);
    initialize_sdk++;
    if (initialize_sdk == 1)
    {

      Aws::Utils::Logging::InitializeAWSLogging(
        Aws::MakeShared<Aws::Utils::Logging::DefaultLogSystem>(
          "OpenVDS-S3 Integration", Aws::Utils::Logging::LogLevel::Trace, "aws_sdk_"));
      Aws::InitAPI(initialize_sdk_options);
    }
  }

  static void deinitizlieAWSSDK()
  {
    std::unique_lock<std::mutex> lock(initialize_sdk_mutex);
    initialize_sdk--;
    if (!initialize_sdk)
    {
      Aws::Utils::Logging::ShutdownAWSLogging();
      Aws::ShutdownAPI(initialize_sdk_options);
    }

  }

  template<typename T>
  struct AsyncContext
  {
    AsyncContext(T *back)
      : back(back)
    {}
    T *back;
    std::mutex mutex;
  };

  struct NotifyAll
  {
    NotifyAll(std::condition_variable &to_notify)
      : to_notify(to_notify)
    {}
    ~NotifyAll()
    {
      to_notify.notify_all();
    }
    std::condition_variable &to_notify;
  };

  static void download_callback(const Aws::S3::S3Client *client, const Aws::S3::Model::GetObjectRequest& objreq, const Aws::S3::Model::GetObjectOutcome &getObjectOutcome, std::weak_ptr<DownloadRequestAWS> weak_request)
  {
    auto objReq =  weak_request.lock();

    if (!objReq || objReq->m_cancelled)
      return;

    std::unique_lock<std::mutex> lock(objReq->m_mutex, std::defer_lock);
    if (getObjectOutcome.IsSuccess())
    {
      Aws::S3::Model::GetObjectResult result = const_cast<Aws::S3::Model::GetObjectOutcome&>(getObjectOutcome).GetResultWithOwnership();
      for (auto it : result.GetMetadata())
      {
        objReq->m_handler->handleMetadata(convertAwsString(it.first), convertAwsString(it.second));
      }
      auto& retrieved_object = result.GetBody();
      auto content_length = result.GetContentLength();
      std::vector<uint8_t> data;

      if (content_length > 0)
      {
        data.resize(content_length);
        retrieved_object.read((char*)&data[0], content_length);
        objReq->m_handler->handleData(std::move(data));
      }
      lock.lock();
    }
    else
    {
      lock.lock();
      auto s3error = getObjectOutcome.GetError();
      objReq->m_error.code = int(s3error.GetResponseCode());
      objReq->m_error.string = (s3error.GetExceptionName() + " : " + s3error.GetMessage()).c_str();
    }

    objReq->m_done = true;
    objReq->m_waitForFinish.notify_all();
    lock.unlock();
    objReq->m_handler->completed(*objReq, objReq->m_error);
  }

  DownloadRequestAWS::DownloadRequestAWS(const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler)
    : Request(id)
    , m_handler(handler)
    , m_cancelled(false)
    , m_done(false)
  {
  }

  DownloadRequestAWS::~DownloadRequestAWS()
  {
    DownloadRequestAWS::cancel(); 
  }

  void DownloadRequestAWS::run(Aws::S3::S3Client& client, const std::string& bucket, const IORange& range, std::weak_ptr<DownloadRequestAWS> downloadRequest)
  {
    Aws::S3::Model::GetObjectRequest object_request;
    object_request.SetBucket(convertStdString(bucket));
    object_request.SetKey(convertStdString(getObjectName()));
    if (range.end)
    {
      object_request.SetRange(convertStdString(fmt::format("bytes={}-{}", range.start, range.end)));
    }
    Aws::S3::GetObjectResponseReceivedHandler bounded_callback = [downloadRequest](const Aws::S3::S3Client *client, const Aws::S3::Model::GetObjectRequest &request, const Aws::S3::Model::GetObjectOutcome &outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>&) 
    {
      download_callback(client, request, outcome, downloadRequest);
    };
    client.GetObjectAsync(object_request, bounded_callback);
  }

  void DownloadRequestAWS::waitForFinish()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    while(!m_done)
      m_waitForFinish.wait(lock);
  }
  bool DownloadRequestAWS::isDone() const
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_done;
  }

  bool DownloadRequestAWS::isSuccess(Error& error) const
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_done)
    {
      error.code = -1;
      error.string = "Download not done.";
      return false;
    }
    error = m_error;
    return m_error.code == 0;
  }

  void DownloadRequestAWS::cancel()
  {
    m_cancelled = true;
  }

  static void upload_callback(const Aws::S3::S3Client* client, const Aws::S3::Model::PutObjectRequest&putRequest, const Aws::S3::Model::PutObjectOutcome &outcome, std::weak_ptr<UploadRequestAWS> weak_upload)
  {
    auto objReq =  weak_upload.lock();
    if (!objReq || objReq->m_cancelled)
      return;

    std::unique_lock<std::mutex> lock(objReq->m_mutex);
    if (!outcome.IsSuccess())
    {
      auto s3error = outcome.GetError();
      objReq->m_error.code = int(s3error.GetResponseCode());
      objReq->m_error.string = (s3error.GetExceptionName() + " : " + s3error.GetMessage()).c_str();
    }

    objReq->m_done = true;
    objReq->m_waitForFinish.notify_all();
    Error error = objReq->m_error;
    lock.unlock();
    if (objReq->m_completedCallback)
      objReq->m_completedCallback(*objReq, error);
  }

  UploadRequestAWS::UploadRequestAWS(const std::string& id, std::function<void(const Request & request, const Error & error)> completedCallback)
    : Request(id)
    , m_completedCallback(completedCallback)
    , m_cancelled(false)
    , m_done(false)
  {
  }
 
  void UploadRequestAWS::run(Aws::S3::S3Client& client, const std::string& bucket, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::weak_ptr<UploadRequestAWS> uploadRequest)
  {
    m_stream = std::make_shared<IOStream>(data);

    Aws::S3::Model::PutObjectRequest put;
    put.SetBucket(convertStdString(bucket));
    put.SetKey(convertStdString(getObjectName()));
    put.SetBody(m_stream);
    put.SetContentType(convertStdString(contentType));
    put.SetContentLength(data->size());
    if (contentDispostionFilename.size())
      put.SetContentDisposition(Aws::String("attachment; filename=" + convertStdString(contentDispostionFilename)));
    for (auto &metaPair : metadataHeader)
    {
      put.AddMetadata(convertStdString(metaPair.first), convertStdString(metaPair.second.c_str()));
    }
    
    Aws::S3::PutObjectResponseReceivedHandler bounded_callback = [uploadRequest] (const Aws::S3::S3Client* client, const Aws::S3::Model::PutObjectRequest&putRequest, const Aws::S3::Model::PutObjectOutcome &outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>&) { upload_callback(client, putRequest, outcome, uploadRequest);};
    client.PutObjectAsync(put, bounded_callback);

  }

  void UploadRequestAWS::waitForFinish()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    while(!m_done)
      m_waitForFinish.wait(lock);
  }
  bool UploadRequestAWS::isDone() const
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_done;
  }
  bool UploadRequestAWS::isSuccess(Error& error) const
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_done)
    {
      error.code = -1;
      error.string = "Download not done.";
      return false;
    }
    error = m_error;
    return m_error.code == 0;
  }
  void UploadRequestAWS::cancel()
  {
    m_cancelled = true;
  }

  IOManagerAWS::IOManagerAWS(const AWSOpenOptions& openOptions, Error &error)
    : m_region(openOptions.region)
    , m_bucket(openOptions.bucket)
    , m_objectId(openOptions.key)
  {
    if (m_region.empty() || m_bucket.empty())
    {
      error.code = -1;
      error.string = "AWS Config error. Empty bucket or region";
      return;
    }

    if (m_objectId[m_objectId.size() -1] == '/')
      m_objectId.resize(m_objectId.size() - 1);
    initializeAWSSDK();

    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.scheme = Aws::Http::Scheme::HTTPS;
    clientConfig.region = m_region.c_str();
    clientConfig.connectTimeoutMs = 3000;
    clientConfig.requestTimeoutMs = 6000;

    m_s3Client.reset(new Aws::S3::S3Client(clientConfig));
  }

  IOManagerAWS::~IOManagerAWS()
  {
    deinitizlieAWSSDK();
  }

  std::shared_ptr<Request> IOManagerAWS::download(const std::string objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange &range)
  {
    std::string id = objectName.empty()? m_objectId : m_objectId + "/" + objectName;
    auto ret = std::make_shared<DownloadRequestAWS>(id, handler);
    ret->run(*m_s3Client.get(), m_bucket, range, ret);
    return ret;
  }

  std::shared_ptr<Request> IOManagerAWS::upload(const std::string objectName, const std::string& contentDispositionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback)
  {
    std::string id = objectName.empty()? m_objectId : m_objectId + "/" + objectName;
    auto ret = std::make_shared<UploadRequestAWS>(id, completedCallback);
    ret->run(*m_s3Client.get(), m_bucket, contentDispositionFilename, contentType, metadataHeader, data, ret);
    return ret;
  }
}
