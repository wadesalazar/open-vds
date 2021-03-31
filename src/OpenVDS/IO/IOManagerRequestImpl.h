/****************************************************************************
** Copyright 2021 The Open Group
** Copyright 2021 Bluware, Inc.
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

#ifndef IOMANAGERREQUESTIMPL_H
#define IOMANAGERREQUESTIMPL_H

#include "IOManager.h"

#include <mutex>
#include <condition_variable>
namespace OpenVDS
{
class RequestImpl : public Request
{
public:
  RequestImpl(const std::string& objectName)
    : Request(objectName)
    , m_done(false)
    , m_cancelled(false)
    , m_cancelledRequested(false)
  {}

  bool WaitForFinish(OpenVDS::Error& error) override
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_waitForFinish.wait(lock, [this]
    {
      return m_done;
    });

    error = m_error;
    return error.code == 0;
  }

  void Cancel()
  {
    m_cancelledRequested = true;
  }

  bool m_done;
  bool m_cancelled;
  bool m_cancelledRequested;
  Error m_error;
  std::condition_variable m_waitForFinish;
  std::mutex m_mutex;
};

class RequestStateHandler
{
public:
  RequestStateHandler(RequestImpl &request)
    : request(request)
    , lock(request.m_mutex)
  {
  }
  ~RequestStateHandler()
  {
    if (request.m_cancelledRequested)
    {
      request.m_cancelled = true;
    }
    request.m_done = true;
    request.m_waitForFinish.notify_all();
  }

  bool isCancelledRequested() { return request.m_cancelledRequested;  }

  RequestImpl& request;
  std::unique_lock<std::mutex> lock;
};
}
#endif
