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

#include <string>
#include <vector>

namespace OpenVDS
{
class VolumeDataLayoutDescriptor;
class VolumeDataAxisDescriptor;
class VolumeDataChannelDescriptor;

struct OpenOptions
{
  std::string bucket;
  std::string key;
  std::string region;
};

struct Error
{
  int code = 0;
  std::string string;
};

struct VDSHandle;

OPENVDS_EXPORT VDSHandle* open(const OpenOptions& options, Error &error);
OPENVDS_EXPORT VDSHandle* create(const OpenOptions& options, VolumeDataLayoutDescriptor const &layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const &axisDescriptors, std::vector<VolumeDataChannelDescriptor> const &channelDescriptors, Error &error);
OPENVDS_EXPORT void destroy(VDSHandle *handle);
}

#endif //OPENVDS_H
