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

#ifndef VDS_H
#define VDS_H

#include <OpenVDS/VolumeDataLayoutDescriptor.h>
#include <OpenVDS/VolumeDataAxisDescriptor.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/Metadata.h>
#include <OpenVDS/Vector.h>

#include "VDS/VolumeDataLayoutImpl.h"
#include "VDS/VolumeDataLayer.h"
#include "VDS/MetadataManager.h"
#include "VDS/VolumeDataAccessManagerImpl.h"
#include "VDS/VolumeDataRequestProcessor.h"
#include "IO/IOManager.h"

#include <vector>
#include <map>
#include <memory>
#include <unordered_map>

namespace OpenVDS
{

class LayerMetadataContainer
{
public:
  mutable std::mutex mutex;
  std::map<std::string, std::unique_ptr<MetadataManager>> managers;
};

struct VDS
{
  VDS(IOManager *ioManager)
    : ioManager(ioManager)
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

  MetadataContainer metadataContainer;

  std::unique_ptr<VolumeDataLayoutImpl>
                    volumeDataLayout;
  std::unique_ptr<VolumeDataAccessManagerImpl>
                    accessManager;
  std::unique_ptr<IOManager>
                    ioManager;
  LayerMetadataContainer
                    layerMetadataContainer;
  std::unique_ptr<VolumeDataRequestProcessor>
                    requestProcessor;
};

const char *AddDescriptorString(std::string const &descriptorString, VDS &handle);

void CreateVolumeDataLayout(VDS &handle);

std::string GetLayerName(VolumeDataLayer const &volumeDataLayer);
MetadataManager *FindMetadataManager(LayerMetadataContainer const &layerMetadataContainer, std::string const &layerName);
MetadataManager *CreateMetadataManager(VDS &handle, std::string const &layerName, MetadataStatus const &metadataStatus);

}

#endif //VDS_H
