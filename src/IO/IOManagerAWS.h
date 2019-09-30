/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
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

#ifndef IOMANAGERAWS_H
#define IOMANAGERAWS_H

#include "IOManager.h"

#include <vector>
#include <string>
#include <aws/s3/S3Client.h>
namespace OpenVDS
{
  struct AsyncCallerContext;
  class DownloadRequestAWS : public Request
  {
  public:
    DownloadRequestAWS(Aws::S3::S3Client &client, const std::string &bucket, const std::string &id, const std::shared_ptr<TransferHandler> &handler, const IORange &range);
    ~DownloadRequestAWS();
 
    void waitForFinish() override;
    bool isDone() const override;
    bool isSuccess(Error &error) const override;
    void cancel() override;

    std::shared_ptr<TransferHandler> m_handler;
    std::shared_ptr<AsyncCallerContext> m_context;
    Error m_error;
    bool  m_done;
    std::condition_variable m_waitForFinish;
  };

  class IOManagerAWS : public IOManager
  {
    public:
      IOManagerAWS(const AWSOpenOptions &openOptions, Error &error);
      ~IOManagerAWS();

      std::shared_ptr<Request> downloadObject(const std::string objectName, std::shared_ptr<TransferHandler> handler, const IORange &range = IORange()) override;
      std::shared_ptr<Request> uploadObject(const std::string objectName, std::shared_ptr<std::vector<uint8_t>> data, const IORange& range = IORange()) override;
    private:
      std::string m_region;
      std::string m_bucket;
      std::string m_objectId;
      std::unique_ptr<Aws::S3::S3Client> m_s3Client;
  };
}
#endif //IOMANAGERAWS_H
