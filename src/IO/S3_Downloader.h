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


//void s3_function();
#include <OpenVDS/openvds_export.h>
#include <OpenVDS/OpenVDS.h>

namespace OpenVDS
{
namespace S3
{
OPENVDS_EXPORT void test_function();

bool DownloadJson(const std::string &region, const std::string& bucket, const std::string &key, std::string &json, Error &error);
}
}
