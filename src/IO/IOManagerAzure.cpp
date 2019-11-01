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

#include <was/common.h>
#include <was/storage_account.h>
#include <was/blob.h>

#include <fmt/format.h>

void downloadObject()
{
  azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(_XPLATSTR("DefaultEndpointsProtocol=https;AccountName=myaccountname;AccountKey=myaccountkey"));
  azure::storage::cloud_blob_client blob_client = storage_account.create_cloud_blob_client();
  azure::storage::cloud_blob_container container = blob_client.get_container_reference(_XPLATSTR("my-sample-container"));
           
  concurrency::streams::container_buffer<std::vector<uint8_t>> buffer;
  concurrency::streams::ostream output_stream(buffer);
  azure::storage::cloud_block_blob binary_blob = container.get_block_blob_reference(_XPLATSTR("my-blob-1"));
  binary_blob.download_to_stream(output_stream);
}
