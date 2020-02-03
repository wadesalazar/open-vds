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

DownloadRequestAzure::DownloadRequestAzure(const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler)
  : Request(id)
  , m_handler(handler)
  , m_cancelled(false)
  , m_done(false)
{
}

DownloadRequestAzure::~DownloadRequestAzure()
{
  DownloadRequestAzure::Cancel();
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

  m_blob = container.get_block_blob_reference(convertToUtilString(requestName));
  m_downloadTask = m_blob.download_range_to_stream_async(m_outStream.create_ostream(), range.start, range.end - range.start, azure::storage::access_condition(), local_options, m_context, m_cancelTokenSrc.get_token());
  m_downloadTask.then([request, this]
    {
      auto downloadRequest = request.lock();
      if (!downloadRequest)
        return;
      try
      {
        // when the task is completed
        m_downloadTask.get();
        // download properties and metadata
        m_blob.download_attributes();
        std::vector<unsigned char> data = m_outStream.collection();

        if (auto tmp = request.lock())
        {
          // send metadata one at a time to the metadata handler
          for (auto it : m_blob.metadata())
          {
            m_handler->HandleMetadata(convertFromUtilString(it.first), convertFromUtilString(it.second));
          }
          // send data to the data handler
          auto blobData = m_outStream.collection();
          m_handler->HandleData(std::move(blobData));

          // declare success and set completion status
          m_error.code = 0;
          m_done = true;
          m_waitForFinish.notify_all();
          m_handler->Completed(*this, m_error);
        }

      }
      catch (const azure::storage::storage_exception & e)
      {
        // display the erro message, set completion (error) status and return the error to the handler
        ucout << _XPLATSTR("Error message is: ") << e.what() << std::endl;
        m_error.code = -1;
        m_error.string = e.what();
        m_done = true;
        m_waitForFinish.notify_all();
        m_handler->Completed(*this, m_error);
      }
    });
}

void DownloadRequestAzure::WaitForFinish()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_waitForFinish.wait(lock, [this]
    {
      return m_done;
    });

}

bool DownloadRequestAzure::IsDone() const
{
  std::unique_lock<std::mutex> lock(m_mutex);
  return m_done;
}

bool DownloadRequestAzure::IsSuccess(Error& error) const
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

void DownloadRequestAzure::Cancel()
{
  //m_cancelTokenSrc.cancel();
  m_cancelled = true;
}

UploadRequestAzure::UploadRequestAzure(const std::string& id, std::function<void(const Request & request, const Error & error)> completedCallback)
  : Request(id)
  , m_completedCallback(completedCallback)
  , m_cancelled(false)
  , m_done(false)
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
    [this, uploadRequest]()
    {
      auto request = uploadRequest.lock();
      if (!request)
        return;

      m_data.reset();

      try
      {
        m_error.code = 0;
        m_done = true;
        m_waitForFinish.notify_all();
        if (m_completedCallback) m_completedCallback(*this, m_error);
      }

      catch (azure::storage::storage_exception & e)
      {
        // On error set the completion (error) status and call the completion callback
        std::string ex_msg;
        ex_msg = std::string(e.what());
        m_error.code = -1;
        m_error.string = ex_msg;
        m_done = true;
        m_waitForFinish.notify_all();
        if (m_completedCallback) m_completedCallback(*this, m_error);
      }
    });
}

void UploadRequestAzure::WaitForFinish()
{
  std::unique_lock<std::mutex> lock(m_mutex);

  m_waitForFinish.wait(lock, [this]
    {
      return this->m_done;
    });
}

bool UploadRequestAzure::IsDone() const
{
  std::unique_lock<std::mutex> lock(m_mutex);
  return m_done;
}

bool UploadRequestAzure::IsSuccess(Error& error) const
{
  std::unique_lock<std::mutex> lock(m_mutex);
  if (!m_done)
  {
    error.code = -1;
    error.string = "Upload not done.";
    return false;
  }
  error = m_error;
  return m_error.code == 0;
}

void UploadRequestAzure::Cancel()
{
  m_cancelled = true;
  m_cancelTokenSrc.cancel();
}

IOManagerAzure::IOManagerAzure(const AzureOpenOptions& openOptions, Error& error)
  : m_connStr(openOptions.connectionString)
  , m_containerStr(openOptions.container)
{
  if (m_connStr.empty())
  {
    error.code = -1;
    error.string = "Azure Config error. Must provide a connection string";
    return;
  }
  if (m_containerStr.empty())
  {
    error.code = -1;
    error.string = "Azure Config error. Empty container or blob name";
    return;
  }
  m_storage_account = azure::storage::cloud_storage_account::parse(convertToUtilString(m_connStr));
  m_blobClient = m_storage_account.create_cloud_blob_client();
  m_container = m_blobClient.get_container_reference(convertToUtilString(m_containerStr));
  m_container.create_if_not_exists();

  //m_options.set_server_timeout(std::chrono::seconds(60000));

  m_options = azure::storage::blob_request_options();
  m_options.set_parallelism_factor(openOptions.parallelism_factor);
  m_options.set_maximum_execution_time(std::chrono::seconds(openOptions.max_execution_time));
}

IOManagerAzure::~IOManagerAzure()
{
}

std::shared_ptr<Request> IOManagerAzure::Download(const std::string requestName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range)
{
  std::shared_ptr<DownloadRequestAzure> azureRequest = std::make_shared<DownloadRequestAzure>(requestName, handler);
  azureRequest->run(m_container, m_options, requestName, range, azureRequest);
  return azureRequest;
}

std::shared_ptr<Request> IOManagerAzure::Upload(const std::string requestName, const std::string& contentDispositionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback)
{
  std::shared_ptr<UploadRequestAzure> azureRequest = std::make_shared<UploadRequestAzure>(requestName, completedCallback);
  azureRequest->run(m_container, m_options, requestName, contentDispositionFilename, contentType, metadataHeader, data, azureRequest);
  return azureRequest;
}
}
