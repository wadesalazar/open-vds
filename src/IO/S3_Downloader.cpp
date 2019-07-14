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
#include "OpenVDS/openvds_export.h"
#include <vector>

#include "OpenVDSHandle.h"

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/GetBucketLocationRequest.h>
#include <aws/s3/model/GetBucketLocationResult.h>
#include <aws/s3/model/BucketLocationConstraint.h>
#include <aws/s3/model/GetObjectRequest.h>

namespace OpenVDS
{
namespace S3
{
static std::string convert_aws_string(const Aws::String &str)
{
  return std::string(str.data(), str.size());
}

static void initializeAWSSDK()
{
  static bool initialized = false;
  if (initialized)
    return;

  initialized = true;
  Aws::SDKOptions options;
  Aws::InitAPI(options);
}

void test_function()
{
  initializeAWSSDK();
  std::string bucket_name = "";
  std::string location;
  {
    Aws::Client::ClientConfiguration config;
    config.region = "us-east-2";
    Aws::S3::S3Client bucket_client(config);

    auto outcome = bucket_client.ListBuckets();

    if (outcome.IsSuccess())
    {
      fprintf(stderr, "Your Amazon S3 buckets:\n");

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
    Aws::S3::Model::GetBucketLocationRequest getbucketlocation;
    getbucketlocation.WithBucket(bucket_name.c_str());
    auto bucketlocation_result = bucket_client.GetBucketLocation(getbucketlocation);
    if (bucketlocation_result.IsSuccess())
    {
      location = convert_aws_string(Aws::S3::Model::BucketLocationConstraintMapper::GetNameForBucketLocationConstraint(bucketlocation_result.GetResult().GetLocationConstraint()));
    } else
    {
      fprintf(stderr, "Bucket error: %s - %s\n", bucketlocation_result.GetError().GetExceptionName().c_str(), bucketlocation_result.GetError().GetMessage().c_str());
      return;
    }
  }
  Aws::Client::ClientConfiguration config;
  config.region = location.c_str();
  Aws::S3::S3Client s3_client(config);
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

  Aws::S3::Model::ListObjectsRequest objects_request;
  objects_request.WithBucket(bucket_name.c_str());
  auto list_objects_outcome = s3_client.ListObjects(objects_request);
  if (list_objects_outcome.IsSuccess())
  {
      Aws::Vector<Aws::S3::Model::Object> object_list =
          list_objects_outcome.GetResult().GetContents();
  
      for (auto const &s3_object : object_list)
      {
          std::cout << "* " << s3_object.GetKey() << std::endl;
      }
  }
  else
  {
      std::cout << "ListObjects error: " <<
          list_objects_outcome.GetError().GetExceptionName() << " " <<
          list_objects_outcome.GetError().GetMessage() << std::endl;
  }

  {
    Aws::S3::Model::GetObjectRequest object_request;
    object_request.SetBucket(bucket_name.c_str());
    //object_request.SetKey("Object name here");

    // Get the object
    auto get_object_outcome = s3_client.GetObject(object_request);
    if (get_object_outcome.IsSuccess())
    {
      // Get an Aws::IOStream reference to the retrieved file
      auto result = get_object_outcome.GetResultWithOwnership();
      auto& retrieved_file = result.GetBody();

      auto content_length = result.GetContentLength();
    
      FILE *f = fopen("filename here", "wb");
      if (!f)
      {
        fprintf(stderr, "Failed to open file\n");
        return;
      }

      std::vector<uint8_t> buffer(content_length);
      retrieved_file.read((char *)buffer.data(), content_length);
      fwrite(buffer.data(), 1, buffer.size(), f);
      fclose(f);
    }
    else
    {
      auto error = get_object_outcome.GetError();
      std::cout << "ERROR: " << error.GetExceptionName() << ": "
        << error.GetMessage() << std::endl;
    }
  }

}
}
}
