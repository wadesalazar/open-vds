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

#ifndef IOMANAGERCURL_H
#define IOMANAGERCURL_H

#include "IOManager.h"

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <deque>
#include <condition_variable>

#undef WIN32_LEAN_AND_MEAN // avoid warnings if defined on command line
#define WIN32_LEAN_AND_MEAN
#undef NOMINMAX
#define NOMINMAX 1
#include <uv.h>
#include <curl/curl.h>

#include <fmt/format.h>

namespace OpenVDS
{

static std::string trimInBothEnds(const std::string& str)
{
    auto it_begin = std::find_if(str.begin(), str.end(), [](const char a) { return !std::isspace(a); });
    auto it_end = std::find_if(str.rbegin(), str.rend(), [](const char a) { return !std::isspace(a); }).base();
    if (it_end < it_begin)
        return std::string(); 
    return std::string(it_begin, it_end);
}

static void getKeyValueFromLine(const char *line, size_t size, std::string &key, std::string &value, char delimiter)
{
  const char *end = line + size;
  const char *colon = std::find(line, end, delimiter);
  if (colon >= end - 1 )
    return;

  std::string k(line, colon);
  std::string v(colon + 1, end);

  key = trimInBothEnds(k);
  value = trimInBothEnds(v);
}

class IOManagerCurl;

struct UVEventLoopData;
struct CurlEasyHandler
{
  CurlEasyHandler(UVEventLoopData *eventLoopData)
    : eventLoopData(eventLoopData)
    , curlEasy(nullptr)
  {}
  UVEventLoopData *eventLoopData;
  CURL* curlEasy;
  virtual void handleDone(int responsCode, const Error &error) = 0;
  virtual bool shouldRetry(int responseCode) { return false; }
  virtual void handleHeaderData(char* buffer, size_t size) = 0;
  virtual void handleWriteData(char* ptr, size_t size) = 0;
  virtual size_t handleReadRequest(char* buffer, size_t size) = 0;
};

struct SocketContext
{
  SocketContext(CurlEasyHandler *request, curl_socket_t socket);

  CURLM *curlMulti;
  curl_socket_t socket;
  uv_poll_t pollHandle;
};

struct CurlDownloadHandler;
class DownloadRequestCurl : public Request
{
public:
  DownloadRequestCurl(const std::string& id, const std::shared_ptr<TransferDownloadHandler> &handler);
  ~DownloadRequestCurl() override;

  void WaitForFinish() override;
  bool IsDone() const override;
  bool IsSuccess(Error& error) const override;
  void Cancel() override;

  std::shared_ptr<CurlDownloadHandler> m_downloadHandler;

  std::string m_requestName;
  std::shared_ptr<TransferDownloadHandler> m_handler;
  bool m_cancelled;
  bool m_done;
  Error m_error;
  std::condition_variable m_waitForFinish;
  mutable std::mutex m_mutex;
};

struct CurlDownloadHandler : public CurlEasyHandler
{
  CurlDownloadHandler(UVEventLoopData *eventLoopData, const std::shared_ptr<DownloadRequestCurl> &request, std::string url, std::vector<std::string> headers)
    : CurlEasyHandler(eventLoopData)
    , request(request)
    , url(std::move(url))
    , headers(std::move(headers))
    , data(std::move(data))
  {
  }

  void handleDone(int responsCode, const Error &error) override;
  void handleHeaderData(char* buffer, size_t size) override;
  void handleWriteData(char* ptr, size_t size) override;
  size_t handleReadRequest(char* buffer, size_t size) override;

  std::weak_ptr<DownloadRequestCurl> request;
  std::string url;
  std::vector<std::string> headers;
  std::vector<uint8_t> data;
};

struct CurlUploadHandler;
class UploadRequestCurl : public Request
{
public:
  UploadRequestCurl(const std::string& id, std::function<void(const Request & request, const Error & error)> completedCallback);
  void WaitForFinish() override;
  bool IsDone() const override;
  bool IsSuccess(Error& error) const override;
  void Cancel() override;
  
  std::shared_ptr<CurlUploadHandler> m_uploadHandler;

  std::function<void(const Request & request, const Error & error)> m_completedCallback;
  bool m_cancelled;
  bool m_done;
  Error m_error;
  std::condition_variable m_waitForFinish;
  mutable std::mutex m_mutex;
};

struct CurlUploadHandler : public CurlEasyHandler
{
  CurlUploadHandler(UVEventLoopData *eventLoopData, std::weak_ptr<UploadRequestCurl> request, const std::string &url, std::vector<std::string> headers, const std::shared_ptr<std::vector<uint8_t>> &data)
    : CurlEasyHandler(eventLoopData)
    , request(request)
    , url(url)
    , headers(std::move(headers))
    , data(data)
    , data_offset(0)
    , retry_count(0)
  {}

  void handleDone(int responsCode, const Error &error) override;
  void handleHeaderData(char* buffer, size_t size) override;
  void handleWriteData(char* ptr, size_t size) override;
  size_t handleReadRequest(char* buffer, size_t size) override;
  
  bool shouldRetry(int responseCode) override;

  std::weak_ptr<UploadRequestCurl> request;
  std::string url;
  std::vector<std::string> headers;
  std::shared_ptr<std::vector<uint8_t>> data;
  size_t data_offset;
  int retry_count;
};

typedef void(*TransformHeaderName)(std::string &toTransform);

struct UVEventLoopData
{
  uv_loop_t *loop;
  CURLM *curlMulti;
  
  std::vector<std::shared_ptr<CurlDownloadHandler>> incommingDownloadRequests;
  std::vector<std::shared_ptr<CurlDownloadHandler>> cancelledDownloads;
  std::vector<std::shared_ptr<CurlUploadHandler>> incommingUploadRequests;
  std::vector<std::shared_ptr<CurlUploadHandler>> cancelledUploads;

  std::vector<std::shared_ptr<CurlEasyHandler>> queuedRequests;
  std::vector<std::shared_ptr<CurlEasyHandler>> processingRequests;

  std::mutex mutex;
  
  uv_async_t asyncAddDownload;
  uv_async_t asyncCancelledDownload;
  uv_async_t asyncAddUpload;
  uv_async_t asyncCancelledUpload;
  uv_async_t asyncStop;

  uv_prepare_t beforeBlock;
  uv_timer_t curlTimeout;

  TransformHeaderName headerNameTransformer;
};


class IOManagerCurl : public IOManager
{
public:
  IOManagerCurl(TransformHeaderName headerTransformer, Error& error);
  ~IOManagerCurl() override;

protected:
  void addDownloadRequest(const std::shared_ptr<DownloadRequestCurl> &request, const std::string &url, const std::vector<std::string> &headers);
  void addUploadRequest(const std::shared_ptr<UploadRequestCurl> &request, const std::string &url, const std::vector<std::string> &headers, const std::shared_ptr<std::vector<uint8_t>> &data);

private:
  UVEventLoopData m_eventLoopData;
  std::unique_ptr<std::thread> m_thread;
};
}
#endif //IOMANAGERCURL_H