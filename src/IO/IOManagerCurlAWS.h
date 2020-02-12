#ifndef IOMANAGERCURLAWS_H
#define IOMANAGERCURLAWS_H

#include "IOManagerCurl.h"
#include <OpenVDS/OpenVDS.h>

#include "AWSCredentialsHandler.h"

namespace OpenVDS
{

class IOManagerCurlAWS : public IOManagerCurl
{
public:
  IOManagerCurlAWS(const AWSOpenOptions& openOptions, Error& error);
  ~IOManagerCurlAWS() override;

  std::shared_ptr<Request> Download(const std::string requestName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range = IORange()) override;
  std::shared_ptr<Request> Upload(const std::string requestName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback = nullptr) override;
protected:
  AWSCredentialsHandler m_credentialsHandler;
  AWSOpenOptions m_openOptions;
  std::string m_hostname;
  std::string m_baseUrl;
};

}

#endif //IOMANAGERCURLAWS_H
