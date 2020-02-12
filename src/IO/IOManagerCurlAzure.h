#ifndef IOMANAGERCURLAZURE_H
#define IOMANAGERCURLAZURE_H

#include "IOManagerCurl.h"
#include <OpenVDS/OpenVDS.h>

namespace OpenVDS
{

struct AzureParsedConnectionString
{
  bool useHttps;
  std::string accountName;
  std::string accountKey;
  std::string endpointSuffix;
};
class IOManagerCurlAzure : public IOManagerCurl
{
public:
  IOManagerCurlAzure(const AzureOpenOptions& openOptions, Error& error);
  ~IOManagerCurlAzure() override;

  std::shared_ptr<Request> Download(const std::string requestName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range = IORange()) override;
  std::shared_ptr<Request> Upload(const std::string requestName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback = nullptr) override;
protected:
  AzureOpenOptions m_openOptions;
  AzureParsedConnectionString m_parsedConnectionString;
  std::string m_baseUrl;
};

}

#endif //IOMANAGERCURLAZURE_H
