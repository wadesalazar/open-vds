#include "IOManagerInMemory.h"
#include "IOManagerRequestImpl.h"

#include <fmt/format.h>

namespace OpenVDS
{

IOManagerInMemory::IOManagerInMemory(const InMemoryOpenOptions &openOptions, Error &error)
  : IOManager(OpenOptions::InMemory)
  , m_threadPool(std::thread::hardware_concurrency())
{

}

IOManagerInMemory::~IOManagerInMemory()
{

}

std::shared_ptr<Request> IOManagerInMemory::ReadObjectInfo(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler)
{
  auto request = std::make_shared<RequestImpl>(objectName);
  m_threadPool.Enqueue([this, objectName, handler, request]
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      RequestStateHandler requestStateHandler(*request);
      if (requestStateHandler.isCancelledRequested())
      {
        return;
      }
      auto it = m_data.find(objectName);
      Error error;
      if (it != m_data.end())
      {
        auto object = it->second;
        lock.unlock();
        handler->HandleObjectSize(int64_t(object.data.size()));
        for (auto& meta : object.metaHeader)
        {
          handler->HandleMetadata(meta.first, meta.second);
        }
      }
      else
      {
        lock.unlock();
        error.string = std::string("Object: ") + objectName + " not found.";
        error.code = 404;
      }

      handler->Completed(*request, error);
    });
  std::shared_ptr<OpenVDS::Request>  retRequest = request;
  return retRequest;
}

std::shared_ptr<OpenVDS::Request> IOManagerInMemory::ReadObject(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange &range)
{
  auto request = std::make_shared<RequestImpl>(objectName);
  m_threadPool.Enqueue([this, objectName, handler, request]
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      RequestStateHandler requestStateHandler(*request);
      if (requestStateHandler.isCancelledRequested())
      {
        return;
      }

      auto it = m_data.find(objectName);
      Error error;
      if (it != m_data.end())
      {
        auto object = it->second;
        lock.unlock();
        handler->HandleObjectSize(int64_t(object.data.size()));
        for (auto& meta : object.metaHeader)
        {
          handler->HandleMetadata(meta.first, meta.second);
        }
        handler->HandleData(std::move(object.data));
      }
      else
      {
        lock.unlock();
        error.string = std::string("Object: ") + objectName + " not found.";
        error.code = 404;
      }
      handler->Completed(*request, error); 
    });
  std::shared_ptr<OpenVDS::Request>  retRequest = request;
  return retRequest;
}

std::shared_ptr<Request> IOManagerInMemory::WriteObject(const std::string &objectName, const std::string &contentDispostionFilename, const std::string &contentType, const std::vector<std::pair<std::string, std::string> > &metadataHeader, std::shared_ptr<std::vector<uint8_t> > data, std::function<void (const Request &, const Error &)> completedCallback)
{
  auto request = std::make_shared<RequestImpl>(objectName);
  m_threadPool.Enqueue([objectName, metadataHeader, data, completedCallback, request, this]
    {
      Object object;
      object.metaHeader = metadataHeader;
      object.data = *data;
      std::unique_lock<std::mutex> lock(m_mutex);
      RequestStateHandler requestStateHandler(*request);
      if (requestStateHandler.isCancelledRequested())
      {
        return;
      }
      m_data[objectName] = std::move(object);
      lock.unlock();
     
      if (completedCallback)
        completedCallback(*request, Error());
    });
  std::shared_ptr<OpenVDS::Request>  retRequest = request;
  return retRequest;
}


class IOManagerProxy : public OpenVDS::IOManager
{
public:
  IOManagerProxy(OpenVDS::IOManager *backend)
    : IOManager(backend->connectionType())
    , backend(backend)
  {}

  std::shared_ptr<OpenVDS::Request> ReadObjectInfo(const std::string &objectName, std::shared_ptr<OpenVDS::TransferDownloadHandler> handler) override
  {
    return backend->ReadObjectInfo(objectName, handler);
  }

  std::shared_ptr<OpenVDS::Request> ReadObject(const std::string &objectName, std::shared_ptr<OpenVDS::TransferDownloadHandler> handler, const OpenVDS::IORange& range = OpenVDS::IORange()) override
  {
    return backend->ReadObject(objectName, handler, range);
  }

  std::shared_ptr<OpenVDS::Request> WriteObject(const std::string &objectName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const OpenVDS::Request & request, const OpenVDS::Error & error)> completedCallback = nullptr) override
  {
    return backend->WriteObject(objectName, contentDispostionFilename, contentType, metadataHeader, data, completedCallback);
  }

  IOManager *backend;
};

IOManager* IOManagerInMemory::CreateIOManagerInMemory(const InMemoryOpenOptions& openOptions, Error& error)
{
  if (openOptions.name.empty())
  {
    return new IOManagerInMemory(openOptions, error);
  }

  static std::unordered_map<std::string, std::unique_ptr<IOManagerInMemory>> manager_map;
  static std::mutex mutex;

  std::unique_lock<std::mutex> lock(mutex);
  auto& manager = manager_map[openOptions.name];
  if (!manager)
    manager.reset(new IOManagerInMemory(openOptions, error));

  return new IOManagerProxy(manager.get());
}

}
