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

#include <mutex>
#include <functional>

namespace OpenVDS
{

  static int initialize_sdk = 0;
  static std::mutex initialize_sdk_mutex;
  static Aws::SDKOptions initialize_sdk_options;

  static void initializeAWSSDK()
  {
    std::unique_lock<std::mutex> lock(initialize_sdk_mutex);
    initialize_sdk++;
    if (initialize_sdk == 1)
    {
      Aws::InitAPI(initialize_sdk_options);
    }
  }

  static void deinitizlieAWSSDK()
  {
    std::unique_lock<std::mutex> lock(initialize_sdk_mutex);
    initialize_sdk--;
    if (!initialize_sdk)
    {
      Aws::ShutdownAPI(initialize_sdk_options);
    }

  }

  struct AsyncCallerContext
  {
    AsyncCallerContext(ObjectRequesterAWS *back)
      : back(back)
    {}
    ObjectRequesterAWS *back;
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

  static void callback(const Aws::S3::S3Client *client, const Aws::S3::Model::GetObjectRequest& objreq, const Aws::S3::Model::GetObjectOutcome &getObjectOutcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>&awsContext, std::shared_ptr<AsyncCallerContext> context)
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
      objReq->m_handler->handleError(objReq->m_error);
      return;
    }

    Aws::S3::Model::GetObjectResult result = const_cast<Aws::S3::Model::GetObjectOutcome &>(getObjectOutcome).GetResultWithOwnership();
    auto &retrieved_object = result.GetBody();
    auto content_length = result.GetContentLength();
    if (content_length > 0)
    {
      std::vector<uint8_t> data;
      data.resize(content_length);
      retrieved_object.read((char *)&data[0], content_length);
      objReq->m_handler->handleData(std::move(data));
    }
  }

  ObjectRequesterAWS::ObjectRequesterAWS(Aws::S3::S3Client& client, const std::string& bucket, const std::string& id, const std::shared_ptr<TransferHandler>& handler)
    : m_handler(handler)
    , m_context(std::make_shared<AsyncCallerContext>(this))
    , m_done(false)
  {
    Aws::S3::Model::GetObjectRequest object_request;
    object_request.SetBucket(bucket.c_str());
    object_request.SetKey(id.c_str());

    using namespace std::placeholders;
    auto bounded_callback = std::bind(&callback, _1, _2, _3, _4, m_context);
    client.GetObjectAsync(object_request, bounded_callback);
  }

  ObjectRequesterAWS::~ObjectRequesterAWS()
  {
    cancel(); 
  }

  void ObjectRequesterAWS::waitForFinish()
  {
    std::unique_lock<std::mutex> lock(m_context->mutex);
    if (m_done)
      return;
    m_waitForFinish.wait(lock);
  }
  bool ObjectRequesterAWS::isDone() const
  {
    std::unique_lock<std::mutex> lock(m_context->mutex);
    return m_done;
  }
  bool ObjectRequesterAWS::isSuccess(Error& error) const
  {
    std::unique_lock<std::mutex> lock(m_context->mutex);
    error = m_error;
    return m_error.code == 0;
  }

  void ObjectRequesterAWS::cancel()
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

  std::shared_ptr<ObjectRequester> IOManagerAWS::requestObject(const std::string objectName, std::shared_ptr<TransferHandler> handler)
  {
    std::string id = objectName.empty()? m_objectId : m_objectId + "/" + objectName;
    return std::make_shared<ObjectRequesterAWS>(*m_s3Client.get(), m_bucket, id, handler);
  }
}
