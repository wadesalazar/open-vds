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
#include <OpenVDS/MetadataContainer.h>
#include <OpenVDS/Vector.h>

#include "VDS/VolumeDataLayoutImpl.h"
#include "VDS/VolumeDataLayer.h"
#include "VDS/MetadataManager.h"
#include "VDS/VolumeDataAccessManagerImpl.h"
#include "VDS/VolumeDataRequestProcessor.h"
#include "VDS/VolumeDataStore.h"
#include "VDS/GlobalStateImpl.h"

#include <vector>
#include <map>
#include <memory>
#include <unordered_map>

namespace OpenVDS
{

class LayerMetadataContainer
{
public:
  virtual bool GetMetadataStatus(std::string const &layerName, MetadataStatus &metadataStatus) const = 0;
  virtual void SetMetadataStatus(std::string const &layerName, MetadataStatus &metadataStatus, int pageLimit) = 0;
};

class DescriptorStringContainer
{
  std::vector<std::unique_ptr<char[]>> m_descriptorStrings;
public:
  const char *Add(std::string const &descriptorString)
  {
    char *data = new char[descriptorString.size() + 1];
    memcpy(data, descriptorString.data(), descriptorString.size());
    data[descriptorString.size()] = 0;
    m_descriptorStrings.emplace_back(data);
    return data;
  }
};

void ReleaseVolumeDataAccessManager(VolumeDataAccessManagerImpl *);

struct VDS
{
  VolumeDataLayoutDescriptor
                    layoutDescriptor;

  std::vector<VolumeDataAxisDescriptor>
                    axisDescriptors;

  std::vector<VolumeDataChannelDescriptor>
                    channelDescriptors;

  DescriptorStringContainer
                    descriptorStrings;

  std::vector<VolumeDataLayer::ProduceStatus>
                    produceStatuses;

  MetadataContainer metadataContainer;

  std::unique_ptr<VolumeDataLayoutImpl>
                    volumeDataLayout;
  std::shared_ptr<VolumeDataAccessManagerImpl>
                    accessManager;
  std::unique_ptr<VolumeDataStore>
                    volumeDataStore;
};

void CreateVolumeDataLayout(VDS &handle, CompressionMethod compressionMethod = CompressionMethod::None, float compressionTolerance = 0);

std::string GetLayerName(VolumeDataLayer const &volumeDataLayer);

}

#endif //VDS_H
