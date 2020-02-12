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

#include "IOManagerCurl.h"

#include <fmt/printf.h>

namespace OpenVDS
{

SocketContext::SocketContext(CurlEasyHandler* request, curl_socket_t socket)
  : curlMulti(request->eventLoopData->curlMulti)
  , socket(socket)
{
  uv_poll_init_socket(request->eventLoopData->loop, &pollHandle, socket);
  pollHandle.data = this;
}

static void socketContextCloseCB(uv_handle_t* handle)
{
  SocketContext *socketContext = static_cast<SocketContext *>(handle->data);
  delete(socketContext);
}

static std::string CURLErrorMessage(CURL *curlEasy, CURLcode curlCode)
{
  char
    * url = nullptr;

  const char
    * curlErrorString = curl_easy_strerror(curlCode);

  CURLcode
    res = curl_easy_getinfo(curlEasy, CURLINFO_EFFECTIVE_URL, &url);

  if ((CURLE_OK == res) && url)
  {
    return fmt::format("CURL error received for: '{}'. CURL error code: {}, CURL error string: '{}'", std::string(url), curlCode, curlErrorString);
  }
  return std::string();
}

static std::string CurlHttpErrorMessage(int responseCode, char *url)
{
  return fmt::format("Http error respons: {} -> {}\n", responseCode, url);
}

static size_t curlHeaderCallbackCB(char *buffer, size_t size, size_t nitems, void *userdata)
{
  static_cast<CurlEasyHandler *>(userdata)->handleHeaderData(buffer, nitems);
  return nitems;
}

static size_t curlWriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
  static_cast<CurlEasyHandler *>(userdata)->handleWriteData(ptr, nmemb);
  return nmemb;
}

static size_t curlReadCallback(char *buffer, size_t size, size_t nitems, void *userdata)
{
  return static_cast<CurlEasyHandler *>(userdata)->handleReadRequest(buffer, nitems);
}

static void curlAddRequests(UVEventLoopData *eventLoopData)
{
  int maxConcurrentRequests = 64;
  int to_add = maxConcurrentRequests - eventLoopData->processingRequests.size();
  assert(to_add >= 0);
  to_add = std::min(to_add, int(eventLoopData->queuedRequests.size()));
  for (int i = 0; i < to_add; i++)
  {
    eventLoopData->processingRequests.emplace_back(eventLoopData->queuedRequests[i]);
    curl_multi_add_handle(eventLoopData->curlMulti, eventLoopData->processingRequests.back()->curlEasy);
  }
  eventLoopData->queuedRequests.erase(eventLoopData->queuedRequests.begin(), eventLoopData->queuedRequests.begin() + to_add);
}

#if UV_VERSION_MAJOR < 1
static void addDownloadCB(uv_async_t *handle, int status)
#else
static void addDownloadCB(uv_async_t *handle)
#endif
{
  UVEventLoopData *eventLoopData = static_cast<UVEventLoopData *>(handle->data);
  std::vector<std::shared_ptr<CurlDownloadHandler>> downloadRequests;
  downloadRequests.reserve(64);
  {
    std::unique_lock<std::mutex> lock(eventLoopData->mutex);
    std::swap(downloadRequests, eventLoopData->incommingDownloadRequests);
  }
  for (auto &downloadRequest : downloadRequests)
  {
    downloadRequest->curlEasy = curl_easy_init();
    CurlEasyHandler *context = downloadRequest.get();

    if (downloadRequest->headers.size())
    {
      curl_slist *headers = nullptr;
      for (auto &header : downloadRequest->headers)
      {
        headers = curl_slist_append(headers, header.c_str());
      }
      curl_easy_setopt(downloadRequest->curlEasy, CURLOPT_HTTPHEADER, headers);
    }

    curl_easy_setopt(downloadRequest->curlEasy, CURLOPT_PRIVATE, context);
    curl_easy_setopt(downloadRequest->curlEasy, CURLOPT_URL, downloadRequest->url.c_str());
    curl_easy_setopt(downloadRequest->curlEasy, CURLOPT_HEADERFUNCTION, &curlHeaderCallbackCB);
    curl_easy_setopt(downloadRequest->curlEasy, CURLOPT_HEADERDATA, downloadRequest.get());
    curl_easy_setopt(downloadRequest->curlEasy, CURLOPT_WRITEFUNCTION, &curlWriteCallback);
    curl_easy_setopt(downloadRequest->curlEasy, CURLOPT_WRITEDATA, downloadRequest.get());
  }
  
  eventLoopData->queuedRequests.insert(eventLoopData->queuedRequests.end(), downloadRequests.begin(), downloadRequests.end());
  curlAddRequests(eventLoopData);
}

#if UV_VERSION_MAJOR < 1
static void cancelledDownloadCB(uv_async_t *handle, int status)
#else
static void cancelledDownloadCB(uv_async_t *handle)
#endif
{
  UVEventLoopData *eventLoopData = static_cast<UVEventLoopData *>(handle->data);
  std::vector<std::shared_ptr<CurlDownloadHandler>> cancelledDownloads;
  cancelledDownloads.reserve(16);
  {
    std::unique_lock<std::mutex> lock(eventLoopData->mutex);
    std::swap(cancelledDownloads, eventLoopData->cancelledDownloads);
    for (auto &cancelled : cancelledDownloads)
    {
      auto it = std::find(eventLoopData->incommingDownloadRequests.begin(), eventLoopData->incommingDownloadRequests.end(), cancelled);
      if (it != eventLoopData->incommingDownloadRequests.end())
        eventLoopData->incommingDownloadRequests.erase(it);
    }
  }

  if (cancelledDownloads.empty())
    return;
  for (auto &cancelled : cancelledDownloads)
  {
    auto it_queued = std::find(eventLoopData->queuedRequests.begin(), eventLoopData->queuedRequests.end(), cancelled);
    if (it_queued != eventLoopData->queuedRequests.end())
    {
      eventLoopData->queuedRequests.erase(it_queued);
    }
    else
    {
      auto it_processing = std::find(eventLoopData->processingRequests.begin(), eventLoopData->processingRequests.end(), cancelled);
      if (it_processing != eventLoopData->processingRequests.end())
      {
        eventLoopData->processingRequests.erase(it_processing);
        curl_multi_remove_handle(eventLoopData->curlMulti, cancelled->curlEasy);
      }
    }
    curl_easy_cleanup(cancelled->curlEasy);
  }
}

#if UV_VERSION_MAJOR < 1
static void addUploadCB(uv_async_t *handle, int status)
#else
static void addUploadCB(uv_async_t *handle)
#endif
{
  UVEventLoopData *eventLoopData = static_cast<UVEventLoopData *>(handle->data);
  std::vector<std::shared_ptr<CurlUploadHandler>> uploadRequests;
  uploadRequests.reserve(64);
  {
    std::unique_lock<std::mutex> lock(eventLoopData->mutex);
    std::swap(uploadRequests, eventLoopData->incommingUploadRequests);
  }
  for (auto &uploadRequest : uploadRequests)
  {
    uploadRequest->curlEasy = curl_easy_init();
    CurlEasyHandler *context = uploadRequest.get();

    if (uploadRequest->headers.size())
    {
      curl_slist *headers = nullptr;
      for (auto &header : uploadRequest->headers)
      {
        headers = curl_slist_append(headers, header.c_str());
      }
      curl_easy_setopt(uploadRequest->curlEasy, CURLOPT_HTTPHEADER, headers);
    }

    curl_easy_setopt(uploadRequest->curlEasy, CURLOPT_PRIVATE, context);
    curl_easy_setopt(uploadRequest->curlEasy, CURLOPT_URL, uploadRequest->url.c_str());
    curl_easy_setopt(uploadRequest->curlEasy, CURLOPT_HEADERFUNCTION, &curlHeaderCallbackCB);
    curl_easy_setopt(uploadRequest->curlEasy, CURLOPT_HEADERDATA, uploadRequest.get());
    curl_easy_setopt(uploadRequest->curlEasy, CURLOPT_READFUNCTION, &curlReadCallback);
    curl_easy_setopt(uploadRequest->curlEasy, CURLOPT_READDATA, uploadRequest.get());
    curl_easy_setopt(uploadRequest->curlEasy, CURLOPT_UPLOAD, 1);
    curl_easy_setopt(uploadRequest->curlEasy, CURLOPT_INFILESIZE_LARGE, uploadRequest->data->size());
  }
  
  eventLoopData->queuedRequests.insert(eventLoopData->queuedRequests.end(), uploadRequests.begin(), uploadRequests.end());
  curlAddRequests(eventLoopData);
}

#if UV_VERSION_MAJOR < 1
static void cancelledUploadCB(uv_async_t *handle, int status)
#else
static void cancelledUploadCB(uv_async_t *handle)
#endif
{
  UVEventLoopData *eventLoopData = static_cast<UVEventLoopData *>(handle->data);
  std::vector<std::shared_ptr<CurlUploadHandler>> cancelledUploads;
  cancelledUploads.reserve(16);
  {
    std::unique_lock<std::mutex> lock(eventLoopData->mutex);
    std::swap(cancelledUploads, eventLoopData->cancelledUploads);
    for (auto &cancelled : cancelledUploads)
    {
      auto it = std::find(eventLoopData->incommingUploadRequests.begin(), eventLoopData->incommingUploadRequests.end(), cancelled);
      if (it != eventLoopData->incommingUploadRequests.end())
        eventLoopData->incommingUploadRequests.erase(it);
    }
  }

  if (cancelledUploads.empty())
    return;
  for (auto &cancelled : cancelledUploads)
  {
    auto it_queued = std::find(eventLoopData->queuedRequests.begin(), eventLoopData->queuedRequests.end(), cancelled);
    if (it_queued != eventLoopData->queuedRequests.end())
    {
      eventLoopData->queuedRequests.erase(it_queued);
    }
    else
    {
      auto it_processing = std::find(eventLoopData->processingRequests.begin(), eventLoopData->processingRequests.end(), cancelled);
      if (it_processing != eventLoopData->processingRequests.end())
      {
        eventLoopData->processingRequests.erase(it_processing);
        curl_multi_remove_handle(eventLoopData->curlMulti, cancelled->curlEasy);
      }
    }
    curl_easy_cleanup(cancelled->curlEasy);
  }

}

#if UV_VERSION_MAJOR < 1
static void beforeBlockCB(uv_prepare_t *handle, int status)
#else
static void beforeBlockCB(uv_prepare_t *handle)
#endif
{
  std::vector<CurlEasyHandler *> to_remove;
  UVEventLoopData *eventLoopData = static_cast<UVEventLoopData *>(handle->data);

  CURLMsg* message;
  int pending;
  CURL* easy_handle;
  CurlEasyHandler* socketContext;

  while ((message = curl_multi_info_read(eventLoopData->curlMulti, &pending)))
  {
    switch (message->msg)
    {
    case CURLMSG_DONE:
    {
      /* Do not use message data after calling curl_multi_remove_handle() and
         curl_easy_cleanup(). As per curl_multi_info_read() docs:
         "WARNING: The data the returned pointer points to will not survive
         calling curl_multi_cleanup, curl_multi_remove_handle or
         curl_easy_cleanup." */
      easy_handle = message->easy_handle;
      curl_easy_getinfo(easy_handle, CURLINFO_PRIVATE, &socketContext);
      assert(easy_handle == socketContext->curlEasy);

      Error error;
      CURLcode code = message->data.result;
      int responseCode = 0;


      if (code == CURLE_OK)
      {
        CURLcode curlCode = curl_easy_getinfo(socketContext->curlEasy, CURLINFO_HTTP_CODE, &responseCode);

        if (curlCode == CURLE_OK)
        {
          char* url = NULL;

          curl_easy_getinfo(socketContext->curlEasy, CURLINFO_EFFECTIVE_URL, &url);

          switch (responseCode)
          {
          case 201: // CREATED
          case 202: // ACCEPTED
          case 203: // NON-AUTHORITATIVE INFORMATION
          case 204: // NO CONTENT
          case 205: // RESET CONTENT
          case 207: // MULTI-STATUS
          case 208: // ALREADY REPORTED
          case 226: // IM USED
          case 200: // OK
          case 206: // PARTIAL CONTENT. Will happen when using range-headers for adaptive compression.
            break; //success

          case 409: // CONFLICT
          case 500: // INTERNAL_SERVER_ERROR
          case 503: // SERVICE_UNAVAILABLE
            if (socketContext->shouldRetry(responseCode))
            {
              fmt::print(stderr, "CURL respons error {}. Automatic rety {}", responseCode, url);
              curl_multi_remove_handle(eventLoopData->curlMulti, socketContext->curlEasy);

              CURL* dup = curl_easy_duphandle(socketContext->curlEasy);
              curl_easy_cleanup(socketContext->curlEasy);
              socketContext->curlEasy = dup;
              curl_multi_add_handle(eventLoopData->curlMulti, dup);
              continue;
            }
            else
            {
              error.code = responseCode;
              error.string = CurlHttpErrorMessage(responseCode, url);
            }
            break;
          default:
            error.code = responseCode;
            error.string = CurlHttpErrorMessage(responseCode, url);
          }
        }
        else
        {
          error.code = curlCode;
          error.string = CURLErrorMessage(socketContext->curlEasy, code);
        }
      }
      else
      {
        error.code = code;
        error.string = CURLErrorMessage(socketContext->curlEasy, code);
      }

      socketContext->handleDone(responseCode, error);

      curl_multi_remove_handle(eventLoopData->curlMulti, easy_handle);
      curl_easy_cleanup(easy_handle);
      socketContext->curlEasy = nullptr;
      to_remove.push_back(socketContext);
      break;
    }
    default:
      fprintf(stderr, "CURLMSG default\n");
      break;
    }
  }
  if (to_remove.size())
  {
    std::unique_lock<std::mutex> lock(eventLoopData->mutex);
    for (auto remove : to_remove)
    {
      auto it = std::find_if(eventLoopData->processingRequests.begin(), eventLoopData->processingRequests.end(), [remove](const std::shared_ptr<CurlEasyHandler>& req)
        {
          return remove == req.get();
        });
      if (it != eventLoopData->processingRequests.end())
        eventLoopData->processingRequests.erase(it);
    }
    curlAddRequests(eventLoopData);
  }
}

#if UV_VERSION_MAJOR < 1
static void exitEventLoopCB(uv_async_t *handle, int status)
#else
static void exitEventLoopCB(uv_async_t *handle)
#endif
{
  UVEventLoopData *eventLoopData = static_cast<UVEventLoopData *>(handle->data);
  uv_close((uv_handle_t *) &eventLoopData->asyncStop, nullptr);
  uv_close((uv_handle_t *) &eventLoopData->asyncAddDownload, nullptr);
  uv_close((uv_handle_t *) &eventLoopData->asyncCancelledDownload, nullptr);
  uv_close((uv_handle_t *) &eventLoopData->asyncAddUpload, nullptr);
  uv_close((uv_handle_t *) &eventLoopData->asyncCancelledUpload, nullptr);

  uv_prepare_stop(&eventLoopData->beforeBlock);
  uv_close((uv_handle_t *) &eventLoopData->beforeBlock, nullptr);
  
  uv_timer_stop(&eventLoopData->curlTimeout);
  uv_close((uv_handle_t *) &eventLoopData->curlTimeout, nullptr);
}

static void curlPollCB(uv_poll_t* handle, int status, int events)
{
  SocketContext *socketContext = static_cast<SocketContext*>(handle->data);

  int running_handles;
  int flags = 0;
 
  if(events & UV_READABLE)
    flags |= CURL_CSELECT_IN;
  if(events & UV_WRITABLE)
    flags |= CURL_CSELECT_OUT;
 
  curl_multi_socket_action(socketContext->curlMulti, socketContext->socket, flags, &running_handles);
}

static int curlSocketCB(CURL *easy, curl_socket_t s, int what, void *userp, void *socketp)
{
  CurlEasyHandler *curlRequest;
  curl_easy_getinfo(easy, CURLINFO_PRIVATE, &curlRequest);
  int events = 0;

  SocketContext *socketContext = static_cast<SocketContext *>(socketp);

  switch (what)
  {
  case CURL_POLL_IN:
  case CURL_POLL_OUT:
  case CURL_POLL_INOUT:
    if (what != CURL_POLL_IN)
      events |= UV_WRITABLE;
    if (what != CURL_POLL_OUT)
      events |= UV_READABLE;

    if (!socketp)
    {
      socketContext = new SocketContext(curlRequest, s);
      curl_multi_assign(curlRequest->eventLoopData->curlMulti, s, socketContext);
    }
    uv_poll_start(&socketContext->pollHandle, events, curlPollCB);
    break;
  case CURL_POLL_REMOVE:
    curl_multi_assign(curlRequest->eventLoopData->curlMulti, s, nullptr);
    socketContext->curlMulti = nullptr;
    uv_poll_stop(&socketContext->pollHandle);
    uv_close((uv_handle_t*)&socketContext->pollHandle, socketContextCloseCB);
    break;
  default:
    throw std::runtime_error("Error in Curl handling");
  }

  return 0;
}

#if UV_VERSION_MAJOR < 1
static void onCurlTimeout(uv_timer_t *req, int status)
#else
static void onCurlTimeout(uv_timer_t *req)
#endif
{
  UVEventLoopData *eventLoopData = static_cast<UVEventLoopData *>(req->data);
  int running_handles;
  curl_multi_socket_action(eventLoopData->curlMulti, CURL_SOCKET_TIMEOUT, 0, &running_handles);
}

static int curlTimerCallback(CURLM* multi, long timeout_ms, void* userp)
{
  UVEventLoopData *eventLoopData = static_cast<UVEventLoopData *>(userp);
  if (timeout_ms < 0)
  {
    uv_timer_stop(&eventLoopData->curlTimeout);
  }
  else
  {
    uv_timer_start(&eventLoopData->curlTimeout, onCurlTimeout, timeout_ms, 0);
  }
  return 0;
}

void CurlDownloadHandler::handleDone(int responseCode, const Error &error)
{
  auto downloadRequest = request.lock();
  if (!downloadRequest)
    return;
  switch (responseCode)
  {
  case 201: // CREATED
  case 202: // ACCEPTED
  case 203: // NON-AUTHORITATIVE INFORMATION
  case 204: // NO CONTENT
  case 205: // RESET CONTENT
  case 207: // MULTI-STATUS
  case 208: // ALREADY REPORTED
  case 226: // IM USED
    fmt::print(stderr, "Unexpected Success code: {} -> {}\n", responseCode, downloadRequest->GetObjectName());
  }
  std::unique_lock<std::mutex> lock(downloadRequest->m_mutex);
  downloadRequest->m_done = true;
  downloadRequest->m_error = error;
  if (downloadRequest->m_handler)
  {
    if (responseCode < 300 && data.size())
      downloadRequest->m_handler->HandleData(std::move(data));
    downloadRequest->m_handler->Completed(*downloadRequest, downloadRequest->m_error);
  }
  downloadRequest->m_waitForFinish.notify_all();
}

void CurlDownloadHandler::handleHeaderData(char* b, size_t size)
{
  std::string name, value;
  getKeyValueFromLine(b, size, name, value, ':');
  if (name.empty())
    return;
  auto downloadRequest = request.lock();
  if (!downloadRequest)
    return;
  if (!downloadRequest->m_handler)
    return;
  if (eventLoopData->headerNameTransformer)
    eventLoopData->headerNameTransformer(name);
  downloadRequest->m_handler->HandleMetadata(name, value);
  std::string contentlength = name;
  std::transform(contentlength.begin(), contentlength.end(), contentlength.begin(), [](char c)
    {
      return std::tolower(c);
    });
  if (contentlength == "content-length")
  {
    char* end = 0;
    long long length = strtoll(value.data(), &end, 10);
    if (end > value.data() && length)
    {
      data.reserve(length);
    }
  }
  return;
}
  
void CurlDownloadHandler::handleWriteData(char* ptr, size_t size)
{
  data.insert(data.end(), ptr, ptr + size);
}
  
size_t CurlDownloadHandler::handleReadRequest(char* buffer, size_t size)
{
  throw std::runtime_error("Downloads can not send data!");
}


DownloadRequestCurl::DownloadRequestCurl(const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler)
  : Request(id)
  , m_handler(handler)
  , m_cancelled(false)
  , m_done(false)
{
}

DownloadRequestCurl::~DownloadRequestCurl()
{
}
void DownloadRequestCurl::WaitForFinish()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_done)
    return;
  m_waitForFinish.wait(lock);
}
bool DownloadRequestCurl::IsDone() const
{
  std::unique_lock<std::mutex> lock(m_mutex);
  return m_done;
}
bool DownloadRequestCurl::IsSuccess(Error& error) const
{
  std::unique_lock<std::mutex> lock(m_mutex);
  if (!m_done)
    return false;
  error = m_error;
  return m_error.code == 0;
}
void DownloadRequestCurl::Cancel()
{
  std::unique_lock<std::mutex> lock(m_downloadHandler->eventLoopData->mutex);
  m_downloadHandler->eventLoopData->cancelledDownloads.push_back(m_downloadHandler);
  uv_async_send(&m_downloadHandler->eventLoopData->asyncCancelledDownload);
}

UploadRequestCurl::UploadRequestCurl(const std::string& id, std::function<void(const Request & request, const Error & error)> completedCallback)
  : Request(id)
  , m_completedCallback(completedCallback)
  , m_cancelled(false)
  , m_done(false)
{
}

void UploadRequestCurl::WaitForFinish()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_done)
    return;
  m_waitForFinish.wait(lock);
}
bool UploadRequestCurl::IsDone() const
{
  std::unique_lock<std::mutex> lock(m_mutex);
  return m_done;
}
bool UploadRequestCurl::IsSuccess(Error& error) const
{
  std::unique_lock<std::mutex> lock(m_mutex);
  if (!m_done)
    return false;
  error = m_error;
  return m_error.code == 0;
}

void UploadRequestCurl::Cancel()
{
  std::unique_lock<std::mutex> lock(m_uploadHandler->eventLoopData->mutex);
  m_uploadHandler->eventLoopData->cancelledUploads.push_back(m_uploadHandler);
  uv_async_send(&m_uploadHandler->eventLoopData->asyncCancelledUpload);
}

void CurlUploadHandler::handleDone(int responseCode, const Error& error)
{
  auto uploadRequest = request.lock();
  if (!uploadRequest)
    return;
  std::unique_lock<std::mutex> lock(uploadRequest->m_mutex);
  uploadRequest->m_done = true;
  uploadRequest->m_error = error;
  if (uploadRequest->m_completedCallback)
  {
    uploadRequest->m_completedCallback(*uploadRequest, error);
  }
  uploadRequest->m_waitForFinish.notify_all();
}
void CurlUploadHandler::handleHeaderData(char* buffer, size_t size)
{
}
void CurlUploadHandler::handleWriteData(char* ptr, size_t size)
{
  throw std::runtime_error("This function should not be called\n");
}
size_t CurlUploadHandler::handleReadRequest(char* buffer, size_t size)
{
  size_t to_copy = std::min(size, data->size() - data_offset);
  memcpy(buffer, data->data() + data_offset, to_copy);
  data_offset += to_copy;
  return to_copy;
}

bool CurlUploadHandler::shouldRetry(int responseCode)
{
  retry_count++;
  if (retry_count < 4)
    return true;
  return false;
}

struct Barrier
{
  std::mutex mutex;
  std::condition_variable wait;
};

IOManagerCurl::IOManagerCurl(TransformHeaderName headerTransformer, Error& error)
  : IOManager()
{
  m_eventLoopData.headerNameTransformer = headerTransformer;
  error.code = curl_global_init(CURL_GLOBAL_ALL);
  if (error.code)
  {
    error.string = "Could not init curl";
    error.code = -1;
    return;
  }

  Barrier barrier;
  std::unique_lock<std::mutex> lock(barrier.mutex);

  auto run = [&barrier, this]
  {
#if UV_VERSION_MAJOR < 1
    m_eventLoopData.loop = uv_loop_new();
#else
    m_eventLoopData.loop = (uv_loop_t*) malloc(sizeof *m_eventLoopData.loop);
    uv_loop_init(m_eventLoopData.loop);
#endif
    uv_async_init(m_eventLoopData.loop, &m_eventLoopData.asyncStop, &exitEventLoopCB);
    m_eventLoopData.asyncStop.data = &m_eventLoopData;
    uv_async_init(m_eventLoopData.loop, &m_eventLoopData.asyncAddDownload, &addDownloadCB);
    m_eventLoopData.asyncAddDownload.data = &m_eventLoopData;
    uv_async_init(m_eventLoopData.loop, &m_eventLoopData.asyncCancelledDownload, &cancelledDownloadCB);
    m_eventLoopData.asyncCancelledDownload.data = &m_eventLoopData;
    uv_async_init(m_eventLoopData.loop, &m_eventLoopData.asyncAddUpload, &addUploadCB);
    m_eventLoopData.asyncAddUpload.data = &m_eventLoopData;
    uv_async_init(m_eventLoopData.loop, &m_eventLoopData.asyncCancelledUpload, &cancelledUploadCB);
    m_eventLoopData.asyncCancelledUpload.data = &m_eventLoopData;

    m_eventLoopData.curlMulti = curl_multi_init();
    curl_multi_setopt(m_eventLoopData.curlMulti, CURLMOPT_SOCKETFUNCTION, curlSocketCB);
    curl_multi_setopt(m_eventLoopData.curlMulti, CURLMOPT_SOCKETDATA, &m_eventLoopData);
    curl_multi_setopt(m_eventLoopData.curlMulti, CURLMOPT_TIMERFUNCTION, curlTimerCallback);
    curl_multi_setopt(m_eventLoopData.curlMulti, CURLMOPT_TIMERDATA, &m_eventLoopData);

    uv_prepare_init(m_eventLoopData.loop, &m_eventLoopData.beforeBlock);
    m_eventLoopData.beforeBlock.data = &m_eventLoopData;
    uv_prepare_start(&m_eventLoopData.beforeBlock, &beforeBlockCB);

    uv_timer_init(m_eventLoopData.loop, &m_eventLoopData.curlTimeout);
    m_eventLoopData.curlTimeout.data = &m_eventLoopData;

    {
      std::unique_lock<std::mutex> lock(barrier.mutex);
      barrier.wait.notify_one();
    }
    uv_run(m_eventLoopData.loop, UV_RUN_DEFAULT);

    curl_multi_cleanup(m_eventLoopData.curlMulti);

#if UV_VERSION_MAJOR < 1
    uv_loop_delete(m_eventLoopData.loop);
#else
    uv_loop_close(m_eventLoopData.loop);
    free(m_eventLoopData.loop);
#endif
  };

  m_thread.reset(new std::thread(run));
  barrier.wait.wait(lock);
}

IOManagerCurl::~IOManagerCurl()
{
  uv_async_send(&m_eventLoopData.asyncStop);
  m_thread->join();
}

void IOManagerCurl::addDownloadRequest(const std::shared_ptr<DownloadRequestCurl>& request, const std::string& url, const std::vector<std::string>& headers)
{
 auto curlData =  std::make_shared<CurlDownloadHandler>(&m_eventLoopData, request, url, headers);
 request->m_downloadHandler = curlData;
 std::unique_lock<std::mutex> lock(m_eventLoopData.mutex);
 m_eventLoopData.incommingDownloadRequests.push_back(curlData);
 uv_async_send(&m_eventLoopData.asyncAddDownload);
}

void IOManagerCurl::addUploadRequest(const std::shared_ptr<UploadRequestCurl>& request, const std::string& url, const std::vector<std::string>& headers, const std::shared_ptr<std::vector<uint8_t>> &data)
{
  auto curlData = std::make_shared<CurlUploadHandler>(&m_eventLoopData, request, url, headers, data);
  request->m_uploadHandler = curlData;
  std::unique_lock<std::mutex> lock(m_eventLoopData.mutex);
  m_eventLoopData.incommingUploadRequests.push_back(curlData);
  uv_async_send(&m_eventLoopData.asyncAddUpload);
}

}
