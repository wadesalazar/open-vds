/****************************************************************************
** Copyright 2020 The Open Group
** Copyright 2020 Bluware, Inc.
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

#include "IOManagerAzure.h"

#include <cpprest/filestream.h>

#include <fmt/format.h>
#include <mutex>
#include <string>
#include <functional>
#include <algorithm>

#ifdef WIN32
#undef WIN32_LEAN_AND_MEAN // avoid warnings if defined on command line
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace OpenVDS
{
#ifdef WIN32
static utility::string_t convertToUtilString(const std::string& str)
{
  utility::string_t ret;
  int len;
  int slength = (int)str.length();
  len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), slength, nullptr, 0);
  ret.resize(len);
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), slength, &ret[0], len);
  return ret;
}

static std::string convertFromUtilString(const utility::string_t& s)
{
  int len;
  int slength = (int)s.length() + 1;
  len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), slength, nullptr, 0, nullptr, nullptr);
  char* buf = new char[len];
  WideCharToMultiByte(CP_UTF8, 0, s.c_str(), slength, buf, len, nullptr, nullptr);
  std::string r(buf);
  delete[] buf;
  return r;
}

#else
static utility::string_t convertToUtilString(const std::string& str)
{
  return str;
}

static std::string convertFromUtilString(const utility::string_t& str)
{
  return str;
}
#endif

GetHeadRequestAzure::GetHeadRequestAzure(const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler)
  : RequestImpl(id)
  , m_handler(handler)
{
}

GetHeadRequestAzure::~GetHeadRequestAzure()
{
}

void GetHeadRequestAzure::Cancel()
{
  m_cancelTokenSrc.cancel();
  RequestImpl::Cancel();
}

ReadObjectInfoRequestAzure::ReadObjectInfoRequestAzure(const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler)
  : GetHeadRequestAzure(id, handler)
{
}

void ReadObjectInfoRequestAzure::run(azure::storage::cloud_blob_container& container, azure::storage::blob_request_options options, const std::string & requestName, std::weak_ptr<ReadObjectInfoRequestAzure> request)
{
  // set options, we should probably get these through AzureOpenOptions instead of haddong here - default set in the IOMangerAzure
  azure::storage::blob_request_options local_options;
  local_options.set_parallelism_factor(options.parallelism_factor()); //example: (4)
  local_options.set_maximum_execution_time(options.maximum_execution_time()); //example: (std::chrono::milliseconds(10000));

  // set the cancellation token
  m_cancelTokenSrc = pplx::cancellation_token_source();
  m_context = azure::storage::operation_context();

  m_blob = container.get_block_blob_reference(convertToUtilString(requestName));
  //m_task = m_blob.download_range_to_stream_async(m_outStream.create_ostream(), range.start, range.end - range.start, azure::storage::access_condition(), local_options, m_context, m_cancelTokenSrc.get_token());
  m_task = m_blob.download_attributes_async(azure::storage::access_condition(), local_options, m_context, m_cancelTokenSrc.get_token());
  m_task.then([request, this](pplx::task<void> task)
    {
      auto readObjectRequest = request.lock();
      if (!readObjectRequest)
        return;
      RequestStateHandler requestStateHandler(*readObjectRequest);
      if (requestStateHandler.isCancelledRequested())
      {
        return;
      }

      try
      {
        // when the task is completed
        task.get();
        m_handler->HandleObjectSize(m_blob.properties().size());

        m_handler->HandleObjectLastWriteTime(convertFromUtilString(m_blob.properties().last_modified().to_string(utility::datetime::ISO_8601)));

        // send metadata one at a time to the metadata handler
        for (auto it : m_blob.metadata())
        {
          m_handler->HandleMetadata(convertFromUtilString(it.first), convertFromUtilString(it.second));
        }
      }
      catch (const azure::storage::storage_exception & e)
      {
        // display the erro message, set completion (error) status and return the error to the handler
        m_error.code = -1;
        m_error.string = e.what();
      }
      m_handler->Completed(*this, m_error);
    });
}

DownloadRequestAzure::DownloadRequestAzure(const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler)
  : GetHeadRequestAzure(id, handler)
{
}

void DownloadRequestAzure::run(azure::storage::cloud_blob_container& container, azure::storage::blob_request_options options, const std::string& requestName, const IORange& range, std::weak_ptr<DownloadRequestAzure> request)
{
  // set options, we should probably get these through AzureOpenOptions instead of haddong here - default set in the IOMangerAzure
  azure::storage::blob_request_options local_options;
  local_options.set_parallelism_factor(options.parallelism_factor()); //example: (4)
  local_options.set_maximum_execution_time(options.maximum_execution_time()); //example: (std::chrono::milliseconds(10000));

  // set the cancellation token
  m_cancelTokenSrc = pplx::cancellation_token_source();
  m_context = azure::storage::operation_context();
  m_requestedRange = range;

  m_blob = container.get_block_blob_reference(convertToUtilString(requestName));
  if (range.end - range.start)
  {
    m_task = m_blob.download_range_to_stream_async(m_outStream.create_ostream(), range.start, range.end - range.start, azure::storage::access_condition(), local_options, m_context, m_cancelTokenSrc.get_token());
  }
  else
  {
    m_task = m_blob.download_to_stream_async(m_outStream.create_ostream(), azure::storage::access_condition(), local_options, m_context, m_cancelTokenSrc.get_token());
  }
  m_task.then([request, this](pplx::task<void> downloadTask)
    {
      auto downloadRequest = request.lock();
      if (!downloadRequest)
        return;
      RequestStateHandler requestStateHandler(*downloadRequest);
      if (requestStateHandler.isCancelledRequested())
      {
        return;
      }
      try
      {
        // when the task is completed
        downloadTask.get();
        // download properties and metadata
        //m_blob.download_attributes();
        std::vector<unsigned char> &data = m_outStream.collection();

        if (m_context.request_results().size() == 2 && m_context.request_results()[0].http_status_code() == 416 && m_requestedRange.start == 0 && m_requestedRange.end == 0)
          data.clear();

        m_handler->HandleObjectSize(m_blob.properties().size());

        m_handler->HandleObjectLastWriteTime(convertFromUtilString(m_blob.properties().last_modified().to_string(utility::datetime::ISO_8601)));

        // send metadata one at a time to the metadata handler
        for (auto it : m_blob.metadata())
        {
          m_handler->HandleMetadata(convertFromUtilString(it.first), convertFromUtilString(it.second));
        }
        // send data to the data handler
        m_handler->HandleData(std::move(data));
      }
      catch (const azure::storage::storage_exception & e)
      {
        // display the erro message, set completion (error) status and return the error to the handler
        m_error.code = -1;
        m_error.string = e.what();
      }
      m_handler->Completed(*this, m_error);
    });
}

UploadRequestAzure::UploadRequestAzure(const std::string& id, std::function<void(const Request & request, const Error & error)> completedCallback)
  : RequestImpl(id)
  , m_completedCallback(completedCallback)
{
}

void UploadRequestAzure::run(azure::storage::cloud_blob_container& container, azure::storage::blob_request_options options, const std::string& requestName, const std::string& contentDispositionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::weak_ptr<UploadRequestAzure> uploadRequest)
{
  // set options, we should probably get these through AzureOpenOptions instead of haddong here - default set in the IOMangerAzure
  azure::storage::blob_request_options local_options;
  local_options.set_parallelism_factor(options.parallelism_factor()); // Example:(4);
  local_options.set_maximum_execution_time(options.maximum_execution_time()); // Example: (std::chrono::seconds(10000));

  m_data = data;
  // Set teh cancellation token
  m_cancelTokenSrc = pplx::cancellation_token_source();
  m_blob = container.get_block_blob_reference(convertToUtilString(requestName));

  m_context = azure::storage::operation_context();
  // Get the provided metadata ready for upload
  for (auto it : metadataHeader)
  {
    m_blob.metadata()[convertToUtilString(it.first)] = convertToUtilString(it.second);
  }
  m_blob.properties().set_content_type(convertToUtilString(contentType));
  m_blob.properties().set_content_disposition(convertToUtilString(contentDispositionFilename));

  m_taskResult = m_blob.upload_from_stream_async(concurrency::streams::bytestream::open_istream(*data), data->size(), azure::storage::access_condition(), local_options, m_context, m_cancelTokenSrc.get_token());
  m_taskResult.then(
    [this, uploadRequest](pplx::task<void> uploadTask)
    {
      auto request = uploadRequest.lock();
      if (!request)
        return;
      RequestStateHandler requestStateHandler(*request);
      if (requestStateHandler.isCancelledRequested())
      {
        return;
      }
      try
      {
        uploadTask.get();
        m_data.reset();
      }
      catch (azure::storage::storage_exception & e)
      {
        // On error set the completion (error) status and call the completion callback
        std::string ex_msg;
        ex_msg = std::string(e.what());
        m_error.code = -1;
        m_error.string = ex_msg;
      }

      if (m_completedCallback)
        m_completedCallback(*this, m_error);
    });
}

void UploadRequestAzure::Cancel()
{
  RequestImpl::Cancel();
  m_cancelTokenSrc.cancel();
}

IOManagerAzure::IOManagerAzure(const AzureOpenOptions& openOptions, Error& error)
  : IOManager(OpenOptions::Azure)
  , m_containerStr(openOptions.container)
  , m_prefix(openOptions.blob)
{
  if (openOptions.connectionString.empty() && openOptions.bearerToken.empty())
  {
    error.code = -1;
    error.string = "Azure Config error. Must provide a connection string or a bearer token";
    return;
  }
  if (m_containerStr.empty())
  {
    error.code = -1;
    error.string = "Azure Config error. Empty container or blob name";
    return;
  }

  if (openOptions.connectionString.size())
  {
    m_storage_account = azure::storage::cloud_storage_account::parse(convertToUtilString(openOptions.connectionString));
  } else
  {
    assert(openOptions.bearerToken.size());
    if (openOptions.accountName.empty())
    {
      error.code = -1;
      error.string = "Azure Config error. Account Name is mandatory when specifying bearer token";
    }
    azure::storage::storage_credentials::bearer_token_credential bearerToken(convertToUtilString(openOptions.bearerToken));
    auto storage_credentials = azure::storage::storage_credentials(convertToUtilString(openOptions.accountName), bearerToken);
    m_storage_account = azure::storage::cloud_storage_account(storage_credentials, true);
  }
  
  try
  {
    m_blobClient = m_storage_account.create_cloud_blob_client();
    m_container = m_blobClient.get_container_reference(convertToUtilString(m_containerStr));
    m_container.create_if_not_exists();
  } catch (azure::storage::storage_exception &ex)
  {
    error.code = -1;
    error.string = ex.what();
    return;
  }

  //m_options.set_server_timeout(std::chrono::seconds(60000));

  m_options = azure::storage::blob_request_options();
  m_options.set_parallelism_factor(openOptions.parallelism_factor);
  m_options.set_maximum_execution_time(std::chrono::seconds(openOptions.max_execution_time));
}

IOManagerAzure::~IOManagerAzure()
{
}

static std::string create_id(const std::string& prefix, const std::string& objectName)
{
  if (objectName.empty())
  {
    return prefix;
  }
  if (prefix.empty())
  {
    return objectName;
  }
  return prefix + "/" + objectName;
}

std::shared_ptr<Request> IOManagerAzure::ReadObjectInfo(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler)
{
  std::string id = create_id(m_prefix, objectName);
  std::shared_ptr<ReadObjectInfoRequestAzure> azureRequest = std::make_shared<ReadObjectInfoRequestAzure>(id, handler);
  azureRequest->run(m_container, m_options, id, azureRequest);
  return azureRequest;
}

std::shared_ptr<Request> IOManagerAzure::ReadObject(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range)
{
  std::string id = create_id(m_prefix, objectName);
  std::shared_ptr<DownloadRequestAzure> azureRequest = std::make_shared<DownloadRequestAzure>(id, handler);
  azureRequest->run(m_container, m_options, id, range, azureRequest);
  return azureRequest;
}

std::shared_ptr<Request> IOManagerAzure::WriteObject(const std::string &objectName, const std::string& contentDispositionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback)
{
  std::string id = create_id(m_prefix, objectName);
  std::shared_ptr<UploadRequestAzure> azureRequest = std::make_shared<UploadRequestAzure>(id, completedCallback);
  azureRequest->run(m_container, m_options, id, contentDispositionFilename, contentType, metadataHeader, data, azureRequest);
  return azureRequest;
}
}
