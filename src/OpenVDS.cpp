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

#include <OpenVDS/OpenVDS.h>

#include "VDS/VDS.h"

#include "VDS/ParseVDSJson.h"

#include <memory>

#include <OpenVDS/VolumeDataAccess.h>

#include "VDS/VolumeDataLayoutImpl.h"
#include "VDS/VolumeDataPageAccessorImpl.h"
#include "VDS/VolumeDataAccessManagerImpl.h"
#include "VDS/VolumeDataRequestProcessor.h"

#include <fmt/format.h>

namespace OpenVDS
{

VDS* Open(IOManager *ioManager, Error& error)
{
  std::unique_ptr<VDS> ret(new VDS(ioManager));

  error = Error();
  if (!DownloadAndParseVolumeDataLayoutAndLayerStatus(*ret.get(), error))
  {
    return nullptr;
  }
  ret->DataAccessManager.reset(new VolumeDataAccessManagerImpl(*ret.get()));
  ret->RequestProcessor.reset(new VolumeDataRequestProcessor(*ret->DataAccessManager.get()));
  return ret.release();
}

VDS *Open(const OpenOptions &options, Error &error)
{
  error = Error();
  IOManager* ioManager = IOManager::CreateIOManager(options, error);
  if (error.Code)
    return nullptr;

  return Open(ioManager, error);
}

VolumeDataLayout *GetLayout(VDS *vds)
{
  if (!vds)
    return nullptr;
  return vds->VolumeDataLayout.get();
}

VolumeDataAccessManager *GetDataAccessManager(VDS *vds)
{
  if (!vds)
    return nullptr;
  return vds->DataAccessManager.get();
}

const char *AddDescriptorString(std::string const &descriptorString, VDS &vds)
{
  char *data = new char[descriptorString.size() + 1];
  memcpy(data, descriptorString.data(), descriptorString.size());
  data[descriptorString.size()] = 0;
  vds.DescriptorStrings.emplace_back(data);
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

MetadataManager *CreateMetadataManager(VDS &vds, std::string const &layerName, MetadataStatus const &metadataStatus)
{
  std::unique_lock<std::mutex> metadataManagersMutexLock(vds.LayerMetadataContainer.mutex);

  assert(vds.LayerMetadataContainer.managers.find(layerName) == vds.LayerMetadataContainer.managers.end());
  int pageLimit = vds.AxisDescriptors.size() <= 3 ? 64 : 1024;
  return vds.LayerMetadataContainer.managers.insert(std::make_pair(layerName, std::unique_ptr<MetadataManager>(new MetadataManager(vds.IoManager.get(), layerName, metadataStatus, pageLimit)))).first->second.get();
}

void CreateVolumeDataLayout(VDS &vds)
{
  int32_t dimensionality = int32_t(vds.AxisDescriptors.size());

  // Check if input layouts are valid so we can create a new layout
  if (dimensionality < 2)
  {
    vds.VolumeDataLayout.reset();
    return;
  }

  vds.VolumeDataLayout.reset(
    new VolumeDataLayoutImpl(
      vds,
      vds.LayoutDescriptor,
      vds.AxisDescriptors,
      vds.ChannelDescriptors,
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

    int32_t physicalLODLevels = (nChunkDimensionality == 3 || vds.LayoutDescriptor.IsCreate2DLODs()) ? GetLODCount(vds.LayoutDescriptor) : 1;
    int32_t brickSize = GetInternalCubeSizeLOD0(vds.LayoutDescriptor) * (nChunkDimensionality == 2 ? vds.LayoutDescriptor.GetBrickSizeMultiplier2D() : 1);

    vds.VolumeDataLayout->CreateLayers(dimensionGroup, brickSize, physicalLODLevels, vds.ProduceStatuses[DimensionGroupUtil::GetDimensionsNDFromDimensionGroup(dimensionGroup)]);
  }
}

VDS* Create(IOManager *ioManager, VolumeDataLayoutDescriptor const &layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const &axisDescriptors, std::vector<VolumeDataChannelDescriptor> const &channelDescriptors, MetadataContainer const &metadataContainer, Error &error)
{
  error = Error();
  std::unique_ptr<VDS> vds(new VDS(ioManager));

  vds->LayoutDescriptor = layoutDescriptor;

  for(auto axisDescriptor : axisDescriptors)
  {
    vds->AxisDescriptors.push_back(VolumeDataAxisDescriptor(axisDescriptor.GetNumSamples(), AddDescriptorString(axisDescriptor.GetName(), *vds), AddDescriptorString(axisDescriptor.GetUnit(), *vds), axisDescriptor.GetCoordinateMin(), axisDescriptor.GetCoordinateMax()));
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
      vds->ChannelDescriptors.push_back(VolumeDataChannelDescriptor(channelDescriptor.GetFormat(), channelDescriptor.GetComponents(), AddDescriptorString(channelDescriptor.GetName(), *vds), AddDescriptorString(channelDescriptor.GetUnit(), *vds), channelDescriptor.GetValueRangeMin(), channelDescriptor.GetValueRangeMax(), channelDescriptor.GetMapping(), channelDescriptor.GetMappedValueCount(), flags, channelDescriptor.GetNoValue(), channelDescriptor.GetIntegerScale(), channelDescriptor.GetIntegerOffset()));
    }
    else
    {
      vds->ChannelDescriptors.push_back(VolumeDataChannelDescriptor(channelDescriptor.GetFormat(), channelDescriptor.GetComponents(), AddDescriptorString(channelDescriptor.GetName(), *vds), AddDescriptorString(channelDescriptor.GetUnit(), *vds), channelDescriptor.GetValueRangeMin(), channelDescriptor.GetValueRangeMax(), channelDescriptor.GetMapping(), channelDescriptor.GetMappedValueCount(), flags, channelDescriptor.GetIntegerScale(), channelDescriptor.GetIntegerOffset()));
    }
  }

  vds->MetadataContainer = metadataContainer;

  vds->ProduceStatuses.clear();
  vds->ProduceStatuses.resize(int(Dimensions_45) + 1, VolumeDataLayer::ProduceStatus_Unavailable);

  CreateVolumeDataLayout(*vds);

  if (error.Code)
    return nullptr;

  if (!SerializeAndUploadVolumeDataLayout(*vds, error))
    return nullptr;

  vds->DataAccessManager.reset(new VolumeDataAccessManagerImpl(*vds.get()));
  vds->RequestProcessor.reset(new VolumeDataRequestProcessor(*vds->DataAccessManager.get()));

  return vds.release();
}

VDS* Create(const OpenOptions& options, VolumeDataLayoutDescriptor const& layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const& axisDescriptors, std::vector<VolumeDataChannelDescriptor> const& channelDescriptors, MetadataContainer const& metadataContainer, Error& error)
{
  error = Error();
  IOManager* ioManager = IOManager::CreateIOManager(options, error);
  if (error.Code)
    return nullptr;

  return Create(ioManager, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, error);
}

void Close(VDS *vds)
{
  delete vds;
}

}
