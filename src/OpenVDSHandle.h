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

#ifndef OPENVDSHANDLE_H
#define OPENVDSHANDLE_H

#include <OpenVDS/VolumeDataLayoutDescriptor.h>
#include <OpenVDS/VolumeDataAxisDescriptor.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/Metadata.h>

#include <Math/Vector.h>

#include "VDS/VolumeDataLayer.h"
#include "VDS/VolumeDataLayout.h"
#include "IO/IOManager.h"

#include <vector>
#include <memory>
#include <unordered_map>

namespace OpenVDS
{

struct VDSHandle
{
  VDSHandle(const OpenOptions &openOptions, Error &error)
    : ioManager(IOManager::createIOManager(openOptions, error))
  {
  }
  VolumeDataLayoutDescriptor
                    layoutDescriptor;

  std::vector<VolumeDataAxisDescriptor>
                    axisDescriptors;

  std::vector<VolumeDataChannelDescriptor>
                    channelDescriptors;

  std::vector<std::unique_ptr<char[]>>
                    descriptorStrings;

  std::vector<VolumeDataLayer::ProduceStatus>
                    produceStatuses;

  MetadataContainer metaDataContainer;

  std::unique_ptr<VolumeDataLayout>
                    volumeDataLayout;
  std::vector<VolumeDataPageAccessor *>
                    pageAccessors;
  std::unique_ptr<IOManager>
                    ioManager;
};

}

#endif //OPENVDSHANDLE_H
