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

  static void download_callback(const Aws::S3::S3Client *client, const Aws::S3::Model::GetObjectRequest& objreq, const Aws::S3::Model::GetObjectOutcome &getObjectOutcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>&awsContext, std::shared_ptr<AsyncDownloadContext> context)
  {
    std::unique_lock<std::mutex> lock(context->mutex);
    auto objReq =  context->back;
    if (!objReq)
      return;

    NotifyAll notify(objReq->m_waitForFinish);
    objReq->m_done = true;
    if (!getObjectOutcome.IsSuccess())
    {
      auto s3error = getObjectOutcome.GetError();
      objReq->m_error.code = int(s3error.GetResponseCode());
      objReq->m_error.string = (s3error.GetExceptionName() + " : " + s3error.GetMessage()).c_str();
      objReq->m_handler->completed(*objReq, objReq->m_error);
      return;
    }

    Aws::S3::Model::GetObjectResult result = const_cast<Aws::S3::Model::GetObjectOutcome &>(getObjectOutcome).GetResultWithOwnership();
    for (auto it : result.GetMetadata())
    {
      objReq->m_handler->handleMetadata(convertAwsString(it.first), convertAwsString(it.second));
    }
    auto &retrieved_object = result.GetBody();
    auto content_length = result.GetContentLength();
    std::vector<uint8_t> data;

    if (content_length > 0)
    {
      data.resize(content_length);
      retrieved_object.read((char *)&data[0], content_length);
      objReq->m_handler->handleData(std::move(data));
    }
    objReq->m_handler->completed(*objReq, objReq->m_error);
  }

  DownloadRequestAWS::DownloadRequestAWS(Aws::S3::S3Client& client, const std::string& bucket, const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler, const IORange &range)
    : Request(id)
    , m_handler(handler)
    , m_context(std::make_shared<AsyncDownloadContext>(this))
    , m_done(false)
  {
    Aws::S3::Model::GetObjectRequest object_request;
    object_request.SetBucket(convertStdString(bucket));
    object_request.SetKey(convertStdString(id));
    if (range.end)
    {
      char rangeHeaderBuffer[100];
      snprintf(rangeHeaderBuffer, sizeof(rangeHeaderBuffer), "bytes=%zu-%zu", range.start, range.end);
      object_request.SetRange(rangeHeaderBuffer);
    }
    using namespace std::placeholders;
    auto bounded_callback = std::bind(&download_callback, _1, _2, _3, _4, m_context);
    client.GetObjectAsync(object_request, bounded_callback);
  }

  DownloadRequestAWS::~DownloadRequestAWS()
  {
    cancel(); 
  }

  void DownloadRequestAWS::waitForFinish()
  {
    std::unique_lock<std::mutex> lock(m_context->mutex);
    if (m_done)
      return;
    m_waitForFinish.wait(lock);
  }
  bool DownloadRequestAWS::isDone() const
  {
    std::unique_lock<std::mutex> lock(m_context->mutex);
    return m_done;
  }
  bool DownloadRequestAWS::isSuccess(Error& error) const
  {
    std::unique_lock<std::mutex> lock(m_context->mutex);
    error = m_error;
    return m_error.code == 0;
  }

  void DownloadRequestAWS::cancel()
  {
    std::unique_lock<std::mutex> lock(m_context->mutex);
    m_context->back = nullptr;
  }

  static void upload_callback(const Aws::S3::S3Client* client, const Aws::S3::Model::PutObjectRequest&putRequest, const Aws::S3::Model::PutObjectOutcome &outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>&,  std::shared_ptr<AsyncUploadContext> context)
  {
    std::unique_lock<std::mutex> lock(context->mutex);
    auto uploadReq =  context->back;
    if (!uploadReq)
      return;

    NotifyAll notify(uploadReq->m_waitForFinish);
    uploadReq->m_done = true;
    if (!outcome.IsSuccess())
    {
      auto s3error = outcome.GetError();
      uploadReq->m_error.code = int(s3error.GetResponseCode());
      uploadReq->m_error.string = (s3error.GetExceptionName() + " : " + s3error.GetMessage()).c_str();
    }
    if (uploadReq->m_completedCallback)
      uploadReq->m_completedCallback(*uploadReq, uploadReq->m_error);
  }

  UploadRequestAWS::UploadRequestAWS(Aws::S3::S3Client& client, const std::string& bucket, const std::string& id, std::shared_ptr<std::vector<uint8_t>> data, const std::vector<std::pair<std::string, std::string>> &metadataHeader, std::function<void(const Request &request, const Error &error)> completedCallback)
    : Request(id)
    , m_context(std::make_shared<AsyncUploadContext>(this))
    , m_data(data)
    , m_completedCallback(completedCallback)
    , m_vectorBuf(*data)
    , m_stream(std::make_shared<Aws::IOStream>(&m_vectorBuf))
    , m_done(false)
  {
    Aws::S3::Model::PutObjectRequest put;
    put.SetBucket(convertStdString(bucket));
    put.SetKey(convertStdString(id));
    put.SetBody(m_stream);
    put.SetContentType("binary/octet-stream");
    put.SetContentLength(data->size());
    for (auto &metaPair : metadataHeader)
    {
      put.AddMetadata(convertStdString(metaPair.first), convertStdString(metaPair.second.c_str()));
    }
    
    using namespace std::placeholders;
    auto bounded_callback = std::bind(&upload_callback, _1, _2, _3, _4, m_context);

    client.PutObjectAsync(put, bounded_callback);
  }
  void UploadRequestAWS::waitForFinish()
  {
    std::unique_lock<std::mutex> lock(m_context->mutex);
    if (m_done)
      return;
    m_waitForFinish.wait(lock);
  }
  bool UploadRequestAWS::isDone() const
  {
    std::unique_lock<std::mutex> lock(m_context->mutex);
    return m_done;
  }
  bool UploadRequestAWS::isSuccess(Error& error) const
  {
    std::unique_lock<std::mutex> lock(m_context->mutex);
    error = m_error;
    return m_error.code == 0;
  }
  void UploadRequestAWS::cancel()
  {
    std::unique_lock<std::mutex> lock(m_context->mutex);
    m_context->back = nullptr;
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

    Aws::Client::ClientConfiguration config;
    config.region = m_region.c_str();
    m_s3Client.reset(new Aws::S3::S3Client(config));
  }

  IOManagerAWS::~IOManagerAWS()
  {
    deinitizlieAWSSDK();
  }

  std::shared_ptr<Request> IOManagerAWS::downloadObject(const std::string objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange &range)
  {
    std::string id = objectName.empty()? m_objectId : m_objectId + "/" + objectName;
    return std::make_shared<DownloadRequestAWS>(*m_s3Client.get(), m_bucket, id, handler, range);
  }
  
  std::shared_ptr<Request> IOManagerAWS::uploadObject(const std::string objectName, std::shared_ptr<std::vector<uint8_t>> data, const std::vector<std::pair<std::string, std::string>> &metadataHeader, std::function<void(const Request &request, const Error &error)> completedCallback)
  {
    std::string id = objectName.empty()? m_objectId : m_objectId + "/" + objectName;
    return std::make_shared<UploadRequestAWS>(*m_s3Client.get(), m_bucket, id, data, metadataHeader, completedCallback);
  }
}
