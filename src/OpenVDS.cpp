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

#include "OpenVDS/OpenVDS.h"

#include <OpenVDSHandle.h>

#include <IO/S3_Downloader.h>
#include <VDS/ParseVDSJson.h>

#include <memory>

#include <OpenVDS/VolumeDataAccess.h>

#include "VDS/VolumeDataLayout.h"
#include "VDS/VolumeDataPageAccessorImpl.h"
#include "VDS/VolumeDataAccessManagerImpl.h"

namespace OpenVDS
{
VDSHandle *open(const OpenOptions &options, Error &error)
{
  error = Error();
  std::unique_ptr<VDSHandle> ret(new VDSHandle(options, error));
  if (error.code)
    return nullptr;

  if (!downloadAndParseVDSJson(*ret.get(), error))
  {
    return nullptr;
  }
  ret->dataAccessManager.reset(new VolumeDataAccessManagerImpl(ret.get()));
  return ret.release();
}

VolumeDataLayout *layout(VDSHandle *handle)
{
  if (!handle)
    return nullptr;
  return handle->volumeDataLayout.get();
}

VDSHandle* create(const OpenOptions& options, VolumeDataLayoutDescriptor const &layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const &axisDescriptors, std::vector<VolumeDataChannelDescriptor> const &channelDescriptors, MetadataContainer const &metadataContainer, Error &error)
{
  error = Error();
  std::unique_ptr<VDSHandle> ret(new VDSHandle(options, error));
  if (error.code)
    return nullptr;

  if (!serializeAndUploadVDSJson(*ret.get(), error))
    return nullptr;
  
  return ret.release();
}

void destroy(VDSHandle *handle)
{
  delete handle;
}

}
