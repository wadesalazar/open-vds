/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
** Copyright 2020 Microsoft Corp.
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

#ifndef IOMANAGERAZURE_H
#define IOMANAGERAZURE_H


//#include "IOManagerAzure.h"
#include "IOManager.h"

#include <vector>
#include <string>

#include <was/common.h>
#include <was/storage_account.h>
#include <was/blob.h>

#include <fmt/format.h>

namespace OpenVDS
{
    class DownloadRequestAzure : public Request
    {
    public:
        DownloadRequestAzure(const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler);
        ~DownloadRequestAzure() override;

        void run(azure::storage::cloud_blob_container& container, azure::storage::blob_request_options options, const std::string & requestName, const IORange& range, std::weak_ptr<DownloadRequestAzure> request);

        void WaitForFinish() override;
        bool IsDone() const override;
        bool IsSuccess(Error& error) const override;
        void Cancel() override;

        std::shared_ptr<TransferDownloadHandler> m_handler;
        std::atomic_bool m_cancelled;
        bool m_done;
        Error m_error;
        std::condition_variable m_waitForFinish;
        mutable std::mutex m_mutex;
        azure::storage::cloud_block_blob  m_blob;
        concurrency::streams::container_buffer<std::vector<uint8_t>> m_outStream;
        pplx::cancellation_token_source m_cancelTokenSrc;
        pplx::task<void> m_downloadTask;
        azure::storage::operation_context m_context;
        IORange m_requestedRange;
    };
    class UploadRequestAzure : public Request
    {
    public:
        UploadRequestAzure(const std::string& id, std::function<void(const Request & request, const Error & error)> completedCallback);
        void run(azure::storage::cloud_blob_container& container, azure::storage::blob_request_options options, const std::string& requestName, const std::string& contentDispositionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::weak_ptr<UploadRequestAzure> uploadRequest);
        void WaitForFinish() override;
        bool IsDone() const override;
        bool IsSuccess(Error& error) const override;
        void Cancel() override;

        std::function<void(const Request & request, const Error & error)> m_completedCallback;
        std::shared_ptr<std::vector<uint8_t>> m_data;
        std::atomic_bool m_cancelled;
        bool m_done;
        Error m_error;
        std::condition_variable m_waitForFinish;
        mutable std::mutex m_mutex;
        azure::storage::cloud_block_blob  m_blob;
        azure::storage::operation_context m_context;
        pplx::cancellation_token_source m_cancelTokenSrc;
        pplx::task<void> m_taskResult;
    };

    class IOManagerAzure : public IOManager
    {
    public:
        IOManagerAzure(const AzureOpenOptions& openOptions, Error& error);
        ~IOManagerAzure() override;

        HeadInfo Head(const std::string &objectName, Error &error, const IORange& range = IORange());
        std::shared_ptr<Request> Download(const std::string requestName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range = IORange()) override;
        std::shared_ptr<Request> Upload(const std::string requestName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback = nullptr) override;
    private:
        std::string m_connStr;
        std::string m_containerStr;
        azure::storage::cloud_storage_account m_storage_account;
        azure::storage::cloud_blob_client m_blobClient;
        azure::storage::cloud_blob_container m_container;
        azure::storage::blob_request_options m_options;
    };
}


#endif //IOMANAGERAZURE_H
