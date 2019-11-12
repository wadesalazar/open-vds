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

#ifndef OPENVDS_H
#define OPENVDS_H

#include "openvds_export.h"
#include "Metadata.h"
#include "VolumeData.h"

#include <cstdint>
#include <string>
#include <vector>

namespace OpenVDS
{
class VolumeDataLayoutDescriptor;
class VolumeDataAxisDescriptor;
class VolumeDataChannelDescriptor;

struct OpenOptions
{
  enum ConnectionType
  {
    AWS,
    Azure,
    File
  };

  ConnectionType connectionType;

protected:
  OpenOptions(ConnectionType connectionType) : connectionType(connectionType) {}
};

struct AWSOpenOptions : OpenOptions
{
  std::string Bucket;
  std::string Key;
  std::string Region;

  AWSOpenOptions() : OpenOptions(AWS) {}
  AWSOpenOptions(std::string const & bucket, std::string const & key, std::string const & region) : OpenOptions(AWS), Bucket(bucket), Key(key), Region(region) {}
};

struct Error
{
  int Code = 0;
  std::string String;
};

enum class Access
{
  Read = 1 << 0,
  Write = 1 << 1
};
struct VDSHandle;
class VolumeDataLayout;
class VolumeDataAccessManager;
class VolumeDataPageAccessor;

OPENVDS_EXPORT VDSHandle* Open(const OpenOptions& options, Error &error);
OPENVDS_EXPORT VDSHandle* Create(const OpenOptions& options, VolumeDataLayoutDescriptor const &layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const &axisDescriptors, std::vector<VolumeDataChannelDescriptor> const &channelDescriptors, MetadataContainer const &metadataContainer, Error &error);
OPENVDS_EXPORT void       Destroy(VDSHandle *handle);

OPENVDS_EXPORT VolumeDataLayout *GetLayout(VDSHandle *handle);
OPENVDS_EXPORT VolumeDataAccessManager *GetDataAccessManager(VDSHandle *handle);
}

#endif //OPENVDS_H
