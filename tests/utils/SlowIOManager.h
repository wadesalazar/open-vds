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

#ifndef SLOWIOMANAGER_H
#define SLOWIOMANAGER_H

#include <OpenVDS/IO/IOManager.h>

#include <mutex>
#include <thread>
#include <chrono>

class BlockUntil
{
public:
  BlockUntil(std::chrono::high_resolution_clock::time_point until)
    : until(until)
    , done(false)
  {}

  void block()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (done)
      return;
    auto duration = until - std::chrono::high_resolution_clock::now();
    if (duration.count() < 0)
      return;
    std::this_thread::sleep_for(duration);
    done = true;
  }

private:
  std::mutex m_mutex;
  std::chrono::high_resolution_clock::time_point until;
  bool done;
};

class SlowRequest : public OpenVDS::Request
{
public:
  SlowRequest(const std::string &objectName, int delayMs)
    : OpenVDS::Request(objectName)
    , m_blockUntil(std::make_shared<BlockUntil>(std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(delayMs)))
  {
  }

  void WaitForFinish() override
  {
    assert(m_target);
    m_blockUntil->block();
    m_target->WaitForFinish();
  }

  bool IsDone() const override
  {
    assert(m_target);
    m_blockUntil->block();
    return m_target->IsDone();
  }

  bool IsSuccess(OpenVDS::Error& error) const override
  {
    assert(m_target);
    m_blockUntil->block();
    return m_target->IsSuccess(error);
  }

  void Cancel() override
  {
    assert(m_target);
    m_blockUntil->block();
    m_target->Cancel();
  }

  std::shared_ptr<BlockUntil> m_blockUntil;
  std::shared_ptr<OpenVDS::Request> m_target;
};

class SlowTransferDownloadHandler : public OpenVDS::TransferDownloadHandler
{
public:
  SlowTransferDownloadHandler(std::shared_ptr<BlockUntil> blockUntil, const std::shared_ptr<OpenVDS::TransferDownloadHandler>& target)
    : m_blockUntil(blockUntil)
    , m_target(target)
  {}
  void HandleObjectSize(int64_t size)
  {
    m_blockUntil->block();
    m_target->HandleObjectSize(size);
  }

  void HandleObjectLastWriteTime(const std::string& lastWriteTimeISO8601)
  {
    m_blockUntil->block();
    m_target->HandleObjectLastWriteTime(lastWriteTimeISO8601);
  }
  void HandleMetadata(const std::string& key, const std::string& header)
  {
    m_blockUntil->block();
    m_target->HandleMetadata(key, header);
  }
  void HandleData(std::vector<uint8_t>&& data)
  {
    m_blockUntil->block();
    m_target->HandleData(std::move(data));
  }
  void Completed(const OpenVDS::Request& request, const OpenVDS::Error& error)
  {
    m_blockUntil->block();
    m_target->Completed(request, error);
  }

  std::shared_ptr<BlockUntil> blockUntil()
  {
    return m_blockUntil;
  }

private:
  std::shared_ptr<BlockUntil> m_blockUntil;
  std::shared_ptr<OpenVDS::TransferDownloadHandler> m_target;
};

class SlowIOManager : public OpenVDS::IOManager
{
public:
  SlowIOManager(int delayMs, OpenVDS::IOManager *target)
    : m_delayMs(delayMs)
    , m_target(target)
  {}

  std::shared_ptr<OpenVDS::Request> ReadObjectInfo(const std::string& objectName, std::shared_ptr<OpenVDS::TransferDownloadHandler> handler) override
  {
    auto slowRequest = std::make_shared<SlowRequest>(objectName, m_delayMs);
    auto slowTransfer = std::make_shared<SlowTransferDownloadHandler>(slowRequest->m_blockUntil, handler);
    slowRequest->m_target = m_target->ReadObjectInfo(objectName, slowTransfer);
    return slowRequest;
  }
  
  std::shared_ptr<OpenVDS::Request> ReadObject(const std::string& objectName, std::shared_ptr<OpenVDS::TransferDownloadHandler> handler, const OpenVDS::IORange& range = OpenVDS::IORange()) override
  {
    auto slowRequest = std::make_shared<SlowRequest>(objectName, m_delayMs);
    auto slowTransfer = std::make_shared<SlowTransferDownloadHandler>(slowRequest->m_blockUntil, handler);
    slowRequest->m_target = m_target->ReadObject(objectName, slowTransfer, range);
    return slowRequest;
  }

  std::shared_ptr<OpenVDS::Request> WriteObject(const std::string& objectName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const OpenVDS::Request& request, const OpenVDS::Error& error)> completedCallback = nullptr) override
  {
    auto slowRequest = std::make_shared<SlowRequest>(objectName, m_delayMs);
    std::function<void(const OpenVDS::Request& request, const OpenVDS::Error& error)> slowCompletedCallback;
    if (completedCallback)
    {
      slowCompletedCallback = [slowRequest, completedCallback](const OpenVDS::Request& request, const OpenVDS::Error& error) { slowRequest->m_blockUntil->block(); completedCallback(request, error); };
    }
    slowRequest->m_target = m_target->WriteObject(objectName, contentDispostionFilename, contentType, metadataHeader, data, slowCompletedCallback);
    return slowRequest;
  }

private:
  int m_delayMs;
  OpenVDS::IOManager *m_target;
};

#endif //SLOWIOMANAGER_H
