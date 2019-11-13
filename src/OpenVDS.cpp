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

#include <VDS/ParseVDSJson.h>

#include <memory>

#include <OpenVDS/VolumeDataAccess.h>

#include "VDS/VolumeDataLayoutImpl.h"
#include "VDS/VolumeDataPageAccessorImpl.h"
#include "VDS/VolumeDataAccessManagerImpl.h"
#include "VDS/VolumeDataRequestProcessor.h"

#include <fmt/format.h>

namespace OpenVDS
{

VDSHandle* Open(IOManager *ioManager, Error& error)
{
  std::unique_ptr<VDSHandle> ret(new VDSHandle(ioManager));

  error = Error();
  if (!DownloadAndParseVolumeDataLayoutAndLayerStatus(*ret.get(), error))
  {
    return nullptr;
  }
  ret->DataAccessManager.reset(new VolumeDataAccessManagerImpl(*ret.get()));
  ret->RequestProcessor.reset(new VolumeDataRequestProcessor(*ret->DataAccessManager.get()));
  return ret.release();
}

VDSHandle *Open(const OpenOptions &options, Error &error)
{
  error = Error();
  IOManager* ioManager = IOManager::CreateIOManager(options, error);
  if (error.Code)
    return nullptr;

  return Open(ioManager, error);
}

VolumeDataLayout *GetLayout(VDSHandle *handle)
{
  if (!handle)
    return nullptr;
  return handle->VolumeDataLayout.get();
}

VolumeDataAccessManager *GetDataAccessManager(VDSHandle *handle)
{
  if (!handle)
    return nullptr;
  return handle->DataAccessManager.get();
}

const char *AddDescriptorString(std::string const &descriptorString, VDSHandle &handle)
{
  char *data = new char[descriptorString.size() + 1];
  memcpy(data, descriptorString.data(), descriptorString.size());
  data[descriptorString.size()] = 0;
  handle.DescriptorStrings.emplace_back(data);
  return data;
}

static int32_t GetInternalCubeSizeLOD0(const VolumeDataLayoutDescriptor &desc)
{
  int32_t size = int32_t(1) << desc.GetBrickSize();

  size -= desc.GetNegativeMargin();
  size -= desc.GetPositiveMargin();

  assert(size > 0);

  return size;
}

static int32_t GetLODCount(const VolumeDataLayoutDescriptor &desc)
{
  return desc.GetLODLevels() + 1;
}

std::string GetLayerName(VolumeDataLayer const &volumeDataLayer)
{
  if(volumeDataLayer.GetChannelIndex() == 0)
  {
    return fmt::format("{}LOD{}", DimensionGroupUtil::GetDimensionGroupName(volumeDataLayer.GetChunkDimensionGroup()), volumeDataLayer.GetLOD());
  }
  else
  {
    assert(std::string(volumeDataLayer.GetVolumeDataChannelDescriptor().GetName()) != "");
    return fmt::format("{}{}LOD{}", volumeDataLayer.GetVolumeDataChannelDescriptor().GetName(), DimensionGroupUtil::GetDimensionGroupName(volumeDataLayer.GetPrimaryChannelLayer().GetChunkDimensionGroup()), volumeDataLayer.GetLOD());
  }
}

MetadataManager *FindMetadataManager(LayerMetadataContainer const &layerMetadataContainer, std::string const &layerName)
{
  std::unique_lock<std::mutex> metadataManagersMutexLock(layerMetadataContainer.mutex);
  auto it = layerMetadataContainer.managers.find(layerName);
  return (it != layerMetadataContainer.managers.end()) ? it->second.get() : nullptr;
}

MetadataManager *CreateMetadataManager(VDSHandle &handle, std::string const &layerName, MetadataStatus const &metadataStatus)
{
  std::unique_lock<std::mutex> metadataManagersMutexLock(handle.LayerMetadataContainer.mutex);

  assert(handle.LayerMetadataContainer.managers.find(layerName) == handle.LayerMetadataContainer.managers.end());
  int pageLimit = handle.AxisDescriptors.size() <= 3 ? 64 : 1024;
  return handle.LayerMetadataContainer.managers.insert(std::make_pair(layerName, std::unique_ptr<MetadataManager>(new MetadataManager(handle.IoManager.get(), layerName, metadataStatus, pageLimit)))).first->second.get();
}

void CreateVolumeDataLayout(VDSHandle &handle)
{
  //handle.volumeDataLayout.reset(new VolumeDataLayout(handle.channelDescriptors)
  int32_t dimensionality = int32_t(handle.AxisDescriptors.size());

  // Check if input layouts are valid so we can create a new layout
  if (dimensionality < 2)
  {
    handle.VolumeDataLayout.reset();
    return;
  }

  handle.VolumeDataLayout.reset(
    new VolumeDataLayoutImpl(
      handle,
      handle.LayoutDescriptor,
      handle.AxisDescriptors,
      handle.ChannelDescriptors,
      0, //MIA for now
      { 1, 0 }, //MIA for now
      VolumeDataHash::GetUniqueHash(),
      CompressionMethod::None,
      0,
      false,
      0));

  for(int32_t dimensionGroupIndex = 0; dimensionGroupIndex < DimensionGroup_3D_Max; dimensionGroupIndex++)
  {
    DimensionGroup dimensionGroup = (DimensionGroup)dimensionGroupIndex;

    int32_t nChunkDimensionality = DimensionGroupUtil::GetDimensionality(dimensionGroup);

        // Check if highest dimension in chunk is higher than the highest dimension in the dataset or 1D
    if(DimensionGroupUtil::GetDimension(dimensionGroup, nChunkDimensionality - 1) >= dimensionality ||
       nChunkDimensionality == 1)
    {
      continue;
    }

    assert(nChunkDimensionality == 2 || nChunkDimensionality == 3);

    int32_t physicalLODLevels = (nChunkDimensionality == 3 || handle.LayoutDescriptor.IsCreate2DLODs()) ? GetLODCount(handle.LayoutDescriptor) : 1;
    int32_t brickSize = GetInternalCubeSizeLOD0(handle.LayoutDescriptor) * (nChunkDimensionality == 2 ? handle.LayoutDescriptor.GetBrickSizeMultiplier2D() : 1);

    handle.VolumeDataLayout->CreateLayers(dimensionGroup, brickSize, physicalLODLevels, handle.ProduceStatuses[DimensionGroupUtil::GetDimensionsNDFromDimensionGroup(dimensionGroup)]);
  }
}

VDSHandle* Create(IOManager *ioManager, VolumeDataLayoutDescriptor const &layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const &axisDescriptors, std::vector<VolumeDataChannelDescriptor> const &channelDescriptors, MetadataContainer const &metadataContainer, Error &error)
{
  error = Error();
  std::unique_ptr<VDSHandle> handle(new VDSHandle(ioManager));

  handle->LayoutDescriptor = layoutDescriptor;

  for(auto axisDescriptor : axisDescriptors)
  {
    handle->AxisDescriptors.push_back(VolumeDataAxisDescriptor(axisDescriptor.GetNumSamples(), AddDescriptorString(axisDescriptor.GetName(), *handle), AddDescriptorString(axisDescriptor.GetUnit(), *handle), axisDescriptor.GetCoordinateMin(), axisDescriptor.GetCoordinateMax()));
  }

  for(auto channelDescriptor : channelDescriptors)
  {
    VolumeDataChannelDescriptor::Flags flags = VolumeDataChannelDescriptor::Default;

    if(channelDescriptor.IsDiscrete())                     flags = flags | VolumeDataChannelDescriptor::DiscreteData;
    if(!channelDescriptor.IsAllowLossyCompression())       flags = flags | VolumeDataChannelDescriptor::NoLossyCompression;
    if(channelDescriptor.IsUseZipForLosslessCompression()) flags = flags | VolumeDataChannelDescriptor::NoLossyCompressionUseZip;
    if(!channelDescriptor.IsRenderable())                  flags = flags | VolumeDataChannelDescriptor::NotRenderable;

    if(channelDescriptor.IsUseNoValue())
    {
      handle->ChannelDescriptors.push_back(VolumeDataChannelDescriptor(channelDescriptor.GetFormat(), channelDescriptor.GetComponents(), AddDescriptorString(channelDescriptor.GetName(), *handle), AddDescriptorString(channelDescriptor.GetUnit(), *handle), channelDescriptor.GetValueRangeMin(), channelDescriptor.GetValueRangeMax(), channelDescriptor.GetMapping(), channelDescriptor.GetMappedValueCount(), flags, channelDescriptor.GetNoValue(), channelDescriptor.GetIntegerScale(), channelDescriptor.GetIntegerOffset()));
    }
    else
    {
      handle->ChannelDescriptors.push_back(VolumeDataChannelDescriptor(channelDescriptor.GetFormat(), channelDescriptor.GetComponents(), AddDescriptorString(channelDescriptor.GetName(), *handle), AddDescriptorString(channelDescriptor.GetUnit(), *handle), channelDescriptor.GetValueRangeMin(), channelDescriptor.GetValueRangeMax(), channelDescriptor.GetMapping(), channelDescriptor.GetMappedValueCount(), flags, channelDescriptor.GetIntegerScale(), channelDescriptor.GetIntegerOffset()));
    }
  }

  handle->MetadataContainer = metadataContainer;

  handle->ProduceStatuses.clear();
  handle->ProduceStatuses.resize(int(Dimensions_45) + 1, VolumeDataLayer::ProduceStatus_Unavailable);

  CreateVolumeDataLayout(*handle);

  if (error.Code)
    return nullptr;

  if (!SerializeAndUploadVolumeDataLayout(*handle, error))
    return nullptr;

  handle->DataAccessManager.reset(new VolumeDataAccessManagerImpl(*handle.get()));
  handle->RequestProcessor.reset(new VolumeDataRequestProcessor(*handle->DataAccessManager.get()));

  return handle.release();
}

VDSHandle* Create(const OpenOptions& options, VolumeDataLayoutDescriptor const& layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const& axisDescriptors, std::vector<VolumeDataChannelDescriptor> const& channelDescriptors, MetadataContainer const& metadataContainer, Error& error)
{
  error = Error();
  IOManager* ioManager = IOManager::CreateIOManager(options, error);
  if (error.Code)
    return nullptr;

  return Create(ioManager, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, error);
}

void Close(VDSHandle *handle)
{
  delete handle;
}

}
