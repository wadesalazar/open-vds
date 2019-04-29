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

#include "S3_Downloader.h"

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>

void s3_function()
{
  Aws::SDKOptions options;
  Aws::InitAPI(options);
  Aws::S3::S3Client s3_client;
  auto outcome = s3_client.ListBuckets();

  if (outcome.IsSuccess())
  {
    fprintf(stderr ,"Your Amazon S3 buckets:\n");

    Aws::Vector<Aws::S3::Model::Bucket> bucket_list = outcome.GetResult().GetBuckets();

    for (auto const& bucket : bucket_list)
    {
      fprintf(stderr, "  * %s\n", bucket.GetName().c_str());
    }
  }
  else
  {
    fprintf(stderr, "ListBuckets error: %s - %s\n", outcome.GetError().GetExceptionName().c_str(), outcome.GetError().GetMessage().c_str());
  }
  Aws::ShutdownAPI(options);
}
