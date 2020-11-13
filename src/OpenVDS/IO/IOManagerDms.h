#ifndef IOMANAGERDMS_H
#define IOMANAGERDMS_H

#include "IOManager.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
#include <SDManager.h>
#include <SDGenericDataset.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <atomic>
#include <condition_variable>
#include <memory>
#include <future>

#include "ThreadPool.h"

namespace OpenVDS
{
  class GetHeadRequestDms : public Request
  {
  public:
    GetHeadRequestDms(seismicdrive::SDGenericDataset &dataset, const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler);
    ~GetHeadRequestDms() override;

    void WaitForFinish() override;
    bool IsDone() const override;
    bool IsSuccess(Error& error) const override;
    void Cancel() override;

    seismicdrive::SDGenericDataset & m_dataset;
    std::shared_ptr<TransferDownloadHandler> m_handler;
    std::atomic_bool m_cancelled;
    bool m_done;
    Error m_error;
    std::condition_variable m_waitForFinish;
    mutable std::mutex m_mutex;
    std::future<void> m_job;
  };

  class ReadObjectInfoRequestDms : public GetHeadRequestDms
  {
  public:
    ReadObjectInfoRequestDms(seismicdrive::SDGenericDataset &dataset, const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler);

    void run(const std::string& requestName, std::weak_ptr<ReadObjectInfoRequestDms> request, ThreadPool &threadPool);

  };

  class DownloadRequestDms : public GetHeadRequestDms
  {
  public:
    DownloadRequestDms(seismicdrive::SDGenericDataset &dataset, const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler);

    void run(const std::string& requestName, const IORange& range, std::weak_ptr<DownloadRequestDms> request, ThreadPool &threadPool);

    IORange m_requestedRange;
    std::vector<uint8_t> m_data;
  };

  class UploadRequestDms : public Request
  {
  public:
    UploadRequestDms(seismicdrive::SDGenericDataset &dataset, const std::string& id, std::function<void(const Request& request, const Error& error)> completedCallback);
    void run(const std::string& requestName, const std::string& contentDispositionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::weak_ptr<UploadRequestDms> uploadRequest, ThreadPool &threadPool);
    void WaitForFinish() override;
    bool IsDone() const override;
    bool IsSuccess(Error& error) const override;
    void Cancel() override;

    seismicdrive::SDGenericDataset & m_dataset;
    std::function<void(const Request& request, const Error& error)> m_completedCallback;
    std::shared_ptr<std::vector<uint8_t>> m_data;
    std::atomic_bool m_cancelled;
    bool m_done;
    Error m_error;
    std::condition_variable m_waitForFinish;
    mutable std::mutex m_mutex;
    std::future<void> m_job;
  };

  class IOManagerDms : public IOManager
  {
  public:
    IOManagerDms(const DMSOpenOptions& openOptions, IOManager::AccessPattern accessPatttern, Error& error);
    ~IOManagerDms() override;

    std::shared_ptr<Request> ReadObjectInfo(const std::string& objectName, std::shared_ptr<TransferDownloadHandler> handler) override;
    std::shared_ptr<Request> ReadObject(const std::string& requestName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range = IORange()) override;
    std::shared_ptr<Request> WriteObject(const std::string& requestName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request& request, const Error& error)> completedCallback = nullptr) override;

  private:
    std::unique_ptr<seismicdrive::SDManager> m_sdManager;
    std::unique_ptr<seismicdrive::SDGenericDataset> m_dataset;
    bool m_opened;
    ThreadPool m_threadPool;
  };
}

#endif //IOMANAGERDMS_H
