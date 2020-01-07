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

#include "IOManagerAzure.h"

//#include <stdafx.h>
#include <cpprest/filestream.h>

#include <was/common.h>
#include <was/storage_account.h>
#include <was/blob.h>

#include <fmt/format.h>
#include <mutex>
#include <codecvt>
#include <string>

namespace OpenVDS
{
    static int initialize_azure_sdk = 0;
    static std::mutex initialize_azure_sdk_mutex;

    utility::string_t to_string(const std::vector<uint8_t>& data)
    {
        return utility::string_t(data.cbegin(), data.cend());
    }

    std::wstring StringToWString(const std::string& s)
    {
        std::wstring wsTmp(s.begin(), s.end());
        return wsTmp;
    }

    std::string WStringToString(const std::wstring& wStr)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(wStr);
    }

    void IOManagerAzure::initializeAzureBlob()
    {

        std::unique_lock<std::mutex> lock(initialize_azure_sdk_mutex);
        initialize_azure_sdk++;
        if (initialize_azure_sdk == 1)
        {

            azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(StringToWString(m_conn_str));
            m_blobClient = storage_account.create_cloud_blob_client();
            azure::storage::cloud_blob_container container = m_blobClient.get_container_reference(StringToWString(m_container));
            container.create_if_not_exists();

            m_blobRef = container.get_block_blob_reference(StringToWString(m_blob));
           
        }
    }

    void DownloadObject()
    {
        azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(_XPLATSTR("DefaultEndpointsProtocol=https;AccountName=myaccountname;AccountKey=myaccountkey"));
        azure::storage::cloud_blob_client blob_client = storage_account.create_cloud_blob_client();
        azure::storage::cloud_blob_container container = blob_client.get_container_reference(_XPLATSTR("my-sample-container"));

        concurrency::streams::container_buffer<std::vector<uint8_t>> buffer;
        concurrency::streams::ostream output_stream(buffer);
        azure::storage::cloud_block_blob binary_blob = container.get_block_blob_reference(_XPLATSTR("my-blob-1"));
        //binary_blob.download_to_stream(output_stream);
    }

    DownloadRequestAzure::DownloadRequestAzure(const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler)
        : Request(id)
        , m_handler(handler)
        , m_cancelled(false)
        , m_done(false)
    {
    }

    DownloadRequestAzure::~DownloadRequestAzure()
    {
        DownloadRequestAzure::Cancel();
    }

    void DownloadRequestAzure::run(azure::storage::cloud_block_blob& client, azure::storage::blob_request_options options, const IORange& range, std::weak_ptr<DownloadRequestAzure> request)
    {
        // set options, we should probably get these through AzureOpenOptions instead of haddong here - default set in the IOMangerAzure
        azure::storage::blob_request_options local_options;
        local_options.set_parallelism_factor(options.parallelism_factor()); //example: (4)
        local_options.set_maximum_execution_time(options.maximum_execution_time()); //example: (std::chrono::milliseconds(10000));

        // set the cancellation token
        m_cancel_token_src = pplx::cancellation_token_source();
        m_context = azure::storage::operation_context();

        concurrency::streams::container_buffer<std::vector<uint8_t>> out_stream;// .create_ostream();


        auto download_task = client.download_range_to_stream_async(out_stream.create_ostream(), range.start, range.end - range.start, azure::storage::access_condition(),
            local_options, m_context, m_cancel_token_src.get_token());
        try
        {
            // when the task is completed
            download_task.get();
            // download properties and metadata
            client.download_attributes();
            std::vector<unsigned char> data = out_stream.collection();
            
            // we want to print a part of the data to cout first 80 bytes or all if shorter than 80
            std::string dataString(data.begin(), (data.size() > 80 ? data.begin()+80 : data.end()));
            ucout << _XPLATSTR("Text downloaded successfully, part of text is: ") << dataString.c_str() << std::endl;

            if (auto tmp = request.lock())
            {
                // send metadata one at a time to the metadata handler
                for (auto it : client.metadata())
                {
                    m_handler->HandleMetadata(WStringToString(it.first), WStringToString(it.second));
                }
                // send data to the data handler
                auto blobData = out_stream.collection();
                m_handler->HandleData(std::move(blobData));

                // declare success and set completion status
                m_error.code = 0;
                m_error.string = "Success";
                m_done = true;
                m_waitForFinish.notify_all();
                m_handler->Completed(*this, m_error);                
            }
            
        }
        catch (const azure::storage::storage_exception & e)
        {
            // display the erro message, set completion (error) status and return the error to the handler
            ucout << _XPLATSTR("Error message is: ") << e.what() << std::endl;
            m_error.code = -1;
            m_error.string = e.what();
            m_done = true;
            m_waitForFinish.notify_all();
            m_handler->Completed(*this, m_error);
        }
    }

    void DownloadRequestAzure::WaitForFinish()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_waitForFinish.wait(lock, [this] { return m_done; });

    }

    bool DownloadRequestAzure::IsDone() const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_done;
    }

    bool DownloadRequestAzure::IsSuccess(Error& error) const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_done)
        {
            error.code = -1;
            error.string = "Download not done.";
            return false;
        }
        error = m_error;
        return m_error.code == 0;
    }

    void DownloadRequestAzure::Cancel()
    {
        m_cancel_token_src.cancel();
        m_cancelled = true;
    }

    UploadRequestAzure::UploadRequestAzure(const std::string& id, std::function<void(const Request & request, const Error & error)> completedCallback)
        : Request(id)
        , m_completedCallback(completedCallback)
        , m_cancelled(false)
        , m_done(false)
    {
    }

    void UploadRequestAzure::run(azure::storage::cloud_block_blob& client, azure::storage::blob_request_options options, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::weak_ptr<UploadRequestAzure> uploadRequest)
    {

        // set options, we should probably get these through AzureOpenOptions instead of haddong here - default set in the IOMangerAzure
		auto local_options = azure::storage::blob_request_options();
        
        local_options.set_parallelism_factor(options.parallelism_factor()); // Example:(4);
        local_options.set_maximum_execution_time(options.maximum_execution_time()); // Example: (std::chrono::seconds(10000));

        // Set teh cancellation token
        m_cancel_token_src = pplx::cancellation_token_source();

        m_context = azure::storage::operation_context();
        // Get the provided metadata ready for upload
        for (auto it : metadataHeader)
        {
            client.metadata()[StringToWString(it.first)] = StringToWString(it.second);
        }


		std::string ex_msg;
        auto task_result = client.upload_from_stream_async(concurrency::streams::bytestream::open_istream(*data), data->size(), azure::storage::access_condition(), local_options, m_context, m_cancel_token_src.get_token());
		try
		{
            // On completion of task
			task_result.get();
            // Set the completion status and call the completion callback
            m_error.code = 0;
            m_error.string = "Success";
            m_done = true;
            m_waitForFinish.notify_all();
            if (m_completedCallback) m_completedCallback(*this, m_error);
		}

		catch (azure::storage::storage_exception & e)
		{
            // On error set the completion (error) status and call the completion callback
			ex_msg = std::string(e.what());
            m_error.code = -1;
            m_error.string = ex_msg;
            m_done = true;
            m_waitForFinish.notify_all();
            if (m_completedCallback) m_completedCallback(*this, m_error);
		}

	}

    void UploadRequestAzure::WaitForFinish()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_waitForFinish.wait(lock, [this] { return this->m_done; });
    }

    bool UploadRequestAzure::IsDone() const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_done;
    }

    bool UploadRequestAzure::IsSuccess(Error& error) const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_done)
        {
            error.code = -1;
            error.string = "Upload not done.";
            return false;
        }
        error = m_error;
        return m_error.code == 0;
    }

    void UploadRequestAzure::Cancel()
    {
        m_cancelled = true;
        m_cancel_token_src.cancel();
    }

    IOManagerAzure::IOManagerAzure(const AzureOpenOptions& openOptions, Error& error)
        : m_conn_str(openOptions.connection_string)
        , m_container(openOptions.container)
        , m_blob(openOptions.blob)
        
    {
        if (m_conn_str.empty())
        {
            error.code = -1;
            error.string = "Azure Config error. Must provide a connection string";
            return;
        }
        if (m_container.empty() || m_blob.empty())
        {
            error.code = -1;
            error.string = "Azure Config error. Empty container or blob name";
            return;
        }
        initializeAzureBlob();
        //m_options.set_server_timeout(std::chrono::seconds(60000));
        
        m_options = azure::storage::blob_request_options();     
        m_options.set_parallelism_factor(openOptions.parallelism_factor);
        m_options.set_maximum_execution_time(std::chrono::seconds(openOptions.max_execution_time));
    }

    IOManagerAzure::~IOManagerAzure()
    {
        std::unique_lock<std::mutex> lock(initialize_azure_sdk_mutex);
        initialize_azure_sdk--;
        if (initialize_azure_sdk == 0)
        {
            //cleanup here
        }
    }

    std::shared_ptr<Request> IOManagerAzure::Download(const std::string requestName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range)
    {
        std::string blobId = m_blob + "/" + m_container;
        std::string id = requestName.empty() ? blobId : blobId + "/" + requestName;
        std::shared_ptr<DownloadRequestAzure> azureRequest;
        azureRequest.reset(new DownloadRequestAzure(id, handler));
        std::weak_ptr<DownloadRequestAzure> ret = azureRequest;
        azureRequest->run(m_blobRef, m_options, range, ret);
        return azureRequest;
    }

    std::shared_ptr<Request> IOManagerAzure::Upload(const std::string requestName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback)
    {
        std::string blobId = m_blob + "/" + m_container;
        std::string id = requestName.empty() ? blobId : blobId + "/" + requestName;
        std::shared_ptr<UploadRequestAzure> azureRequest;
        azureRequest.reset(new UploadRequestAzure(id, completedCallback));
        std::weak_ptr<UploadRequestAzure> ret = azureRequest;
        azureRequest->run(m_blobRef, m_options, contentType, metadataHeader, data, ret);
        return azureRequest; 
    }
}
