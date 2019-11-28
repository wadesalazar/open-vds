#include "IOManagerInMemory.h"

namespace OpenVDS
{

class InMemoryRequest : public Request
{
public:
  InMemoryRequest(const std::string &objectName, const Error &error)
    : Request(objectName)
    , m_error(error)
  {

  }
  ~InMemoryRequest() override
  {

  }
  void WaitForFinish() override
  {
  }
  bool IsDone() const override
  {
    return true;
  }
  bool IsSuccess(Error &error) const override
  {
    error = m_error;
    return m_error.code == 0;
  }
  void Cancel() override
  {
  }

private:
  Error m_error;
};

IOManagerInMemory::IOManagerInMemory(const InMemoryOpenOptions &openOptions, Error &error)
  : m_threadPool(1)
{

}

IOManagerInMemory::~IOManagerInMemory()
{

}

std::shared_ptr<OpenVDS::Request> IOManagerInMemory::Download(const std::string objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange &range)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  auto object = m_data[objectName].get();
  Error error;
  if (object)
  {
    for (auto &meta : object->metaHeader)
    {
      handler->HandleMetadata(meta.first, meta.second);
    }
    std::vector<uint8_t> data = object->data;
    handler->HandleData(std::move(data));
  }
  else
  {
    error.string = std::string("Object: ") + objectName + " not found.";
    error.code = 404;
  }

  auto request = std::make_shared<InMemoryRequest>(objectName, error);
  m_threadPool.Enqueue([handler, error, request]{ handler->Completed(*request, error); });
  return request;
}

std::shared_ptr<Request> IOManagerInMemory::Upload(const std::string objectName, const std::string &contentDispostionFilename, const std::string &contentType, const std::vector<std::pair<std::string, std::string> > &metadataHeader, std::shared_ptr<std::vector<uint8_t> > data, std::function<void (const Request &, const Error &)> completedCallback)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  std::unique_ptr<Object> object(new Object());
  object->metaHeader = metadataHeader;
  object->data = *data;
  m_data[objectName] = std::move(object);
  auto r = std::make_shared<InMemoryRequest>(objectName, Error());
  if (completedCallback)
    m_threadPool.Enqueue([completedCallback, r] { completedCallback(*r, Error()); });
  return r;
}

}
