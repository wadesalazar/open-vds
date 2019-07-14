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
  return ret.release();
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

static VolumeDataLayer *getVolumeDataLayer(VolumeDataLayout *layout, DimensionsND dimension, int channel, int lod, bool isAllowFailure)
{
  if(!layout)
  {
    fprintf(stderr, "Volume data layout is NULL, this is usually because the VDS setup is invalid");
    return nullptr;
  }

  if(channel > layout->getChannelCount())
  {
    fprintf(stderr, "Specified channel doesn't exist");
    return nullptr;
  }

  VolumeDataLayer *layer = layout->getBaseLayer(DimensionGroupUtil::getDimensionGroupFromDimensionsND(dimension), channel);

  if(!layer && !isAllowFailure)
  {
    fprintf(stderr, "Specified dimension group doesn't exist");
    return nullptr;
  }

  while(layer && layer->getLod() < lod)
  {
    layer = layer->getParentLayer();
  }

  if((!layer || layer->getLayerType() == VolumeDataLayer::Virtual) && !isAllowFailure)
  {
    fprintf(stderr, "Specified LOD doesn't exist");
  }

  assert(layer || isAllowFailure);
  return (layer->getLayerType() != VolumeDataLayer::Virtual) ? layer : nullptr;
}

VolumeDataPageAccessor *createVolumeDataPageAccessor(VolumeDataLayout *layout, DimensionsND dimension, int lod, int channel, int maxPages, Access access)
{
  VolumeDataLayer* layer = getVolumeDataLayer(layout, dimension, channel, lod, true);

  if (!layer) return NULL;

  return new VolumeDataPageAccessorImpl(layer, maxPages, access == Access::Write);
}
}
