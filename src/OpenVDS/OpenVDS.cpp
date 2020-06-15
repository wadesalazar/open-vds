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
#include <map>
#include <limits>

#include <OpenVDS/VolumeDataAccess.h>

#include "VDS/VolumeDataLayoutImpl.h"
#include "VDS/VolumeDataPageAccessorImpl.h"
#include "VDS/VolumeDataAccessManagerImpl.h"
#include "VDS/VolumeDataRequestProcessor.h"

#include <fmt/format.h>

#include <cctype>

namespace OpenVDS
{

static char asciitolower(char in) {
  if (in <= 'Z' && in >= 'A')
    return in - ('Z' - 'z');
  return in;
}

static std::string trim(const char *start, const char *end)
{
  while (std::isspace(*start) && start < end)
    start++;
  end--;
  while (end > start && std::isspace(*end))
    end--;
  return std::string(start, end + 1);
}

static std::map<std::string, std::string> parseConnectionString(const char* connectionString, size_t connectionStringSize, Error &error)
{
  std::map<std::string, std::string> ret;
  auto it = connectionString;
  auto end = connectionString + connectionStringSize;
 
  const char* name_begin = nullptr;
  const char* name_end = nullptr;
  while (it < end)
  {
    auto keyValueEnd = std::find(it, end, ';');
    auto equals = std::find(it, keyValueEnd, '=');
    name_begin = it;
    name_end = equals;
    it = equals + 1;
    if (it >= keyValueEnd)
    {
      error.code = -1;
      std::string name(name_begin, name_end);
      error.string = fmt::format("Invalid connection string. Name {} has no value.", name);
      return ret;
    }

    std::string name = trim(name_begin, name_end);
    if (name.empty())
    {
      error.code = - 1;
      error.string = fmt::format("Empty name in connection string. Name must consist of more than empty spaces.");
      return ret;
    }
    std::string value = trim(it, keyValueEnd);
    if (value.empty())
    {
      error.code = -1;
      error.string = fmt::format("Empty value in connection string. Name {} has empty value.", name);
      return ret;
    }
    
    std::transform(name.begin(), name.end(), name.begin(), asciitolower);
    ret.emplace(std::move(name), std::move(value));
    it = keyValueEnd + 1;
  }
  return ret;
}

template<int SIZE>
static bool isProtocol(const StringWrapper &str, const char(&literal)[SIZE])
{
  if (str.size < SIZE - 1)
    return false;
  std::string protocol(str.data, str.data + SIZE - 1);
  std::transform(protocol.begin(), protocol.end(), protocol.begin(), asciitolower);
  return memcmp(protocol.data(), literal, SIZE - 1) == 0;
}

template<int SIZE>
static StringWrapper removeProtocol(const StringWrapper& str, const char(&literal)[SIZE])
{
  StringWrapper ret(str);
  ret.data += SIZE - 1;
  ret.size -= SIZE - 1;
  return ret;
}

static std::unique_ptr<OpenOptions> createS3OpenOptions(const StringWrapper &url, const StringWrapper &connectionString, Error &error)
{
  std::unique_ptr<AWSOpenOptions> openOptions(new AWSOpenOptions());

  auto connectionStringMap = parseConnectionString(connectionString.data, connectionString.size, error);
  if (error.code)
  {
    return nullptr;
  }

  if (url.size < 1)
  {
    error.code = -1;
    error.string = "S3 url is missing bucket";
  }
  auto end = url.data + url.size;
  auto bucket_end = std::find(url.data, end, '/');
  openOptions->bucket = std::string(url.data, bucket_end);

  auto urlKeyBegin = bucket_end + 1;
  if (urlKeyBegin < end)
  {
    openOptions->key = std::string(urlKeyBegin, end);
  }

  for (auto& connectionPair : connectionStringMap)
  {
    if (connectionPair.first == "region")
    {
      openOptions->region = connectionPair.second;
    }
    else if (connectionPair.first == "endpointoverride")
    {
      openOptions->endpointOverride = connectionPair.second;
    }
    else if (connectionPair.first == "accesskeyid")
    {
      openOptions->accessKeyId = connectionPair.second;
    }
    else if (connectionPair.first == "secretkey")
    {
      openOptions->secretKey = connectionPair.second;
    }
    else if (connectionPair.first == "sessiontoken")
    {
      openOptions->sessionToken = connectionPair.second;
    }
    else if (connectionPair.first == "expiration")
    {
      openOptions->expiration = connectionPair.second;
    }
    else
    {
      error.code = -1;
      error.string = fmt::format("Invalid key \"{}\" in S3 connection string.", connectionPair.first);
      return openOptions;
    }
  }
  return openOptions;
}

static std::unique_ptr<OpenOptions> createAzureOpenOptions(const StringWrapper &url, const StringWrapper &connectionString, Error &error)
{
  std::unique_ptr<AzureOpenOptions> openOptions(new AzureOpenOptions());
  if (url.size < 1)
  {
    error.code = -1;
    error.string = "Azure url is missing container";
  }
  auto end = url.data + url.size;
  auto container_end = std::find(url.data, end, '/');
  openOptions->container = std::string(url.data, container_end);

  auto urlBlobBegin = container_end + 1;
  if (urlBlobBegin < end)
  {
    openOptions->blob = std::string(urlBlobBegin, end);
  }
  openOptions->connectionString = std::string(connectionString.data, connectionString.data + connectionString.size);
  return openOptions;
}


static std::unique_ptr<OpenOptions> createAzureSASOpenOptions(const StringWrapper &url, const StringWrapper &connectionString, Error& error)
{
  std::unique_ptr<AzurePresignedOpenOptions> openOptions(new AzurePresignedOpenOptions());
  const char http[] = "https://";
  openOptions->baseUrl.reserve(sizeof(http) - 1 + url.size);
  openOptions->baseUrl.insert(0, http, sizeof(http) - 1);
  openOptions->baseUrl.append(url.data, url.data + url.size);

  auto connectionStringMap = parseConnectionString(connectionString.data, connectionString.size, error);
  for (auto& connectionPair : connectionStringMap)
  {
    if (connectionPair.first == "suffix")
    {
      openOptions->urlSuffix = connectionPair.second;
    }
    else
    {
      error.code = -1;
      error.string = fmt::format("Invalid key \"{}\" in AzureSAS connection string.", connectionPair.first);
      return openOptions;
    }
  }
  return openOptions;
}

static std::unique_ptr<OpenOptions> createInMemoryOpenOptions(const StringWrapper& url, const StringWrapper& connectionString, Error& error)
{
  std::unique_ptr<InMemoryOpenOptions> openOptions(new InMemoryOpenOptions());
  openOptions->name.insert(0, url.data, url.size);
  return openOptions;
}

OpenOptions* CreateOpenOptions(StringWrapper url, StringWrapper connectionString, Error& error)
{
  error = Error();
  std::unique_ptr<OpenOptions> openOptions;

  if (isProtocol(url, "s3://"))
  {
    openOptions = createS3OpenOptions(removeProtocol(url, "s3://"), connectionString, error);
  }
  else if (isProtocol(url, "azure://"))
  {
    openOptions = createAzureOpenOptions(removeProtocol(url, "azure://"), connectionString, error);
  }
  else if (isProtocol(url, "azuresas://"))
  {
    openOptions = createAzureSASOpenOptions(removeProtocol(url, "azuresas://"), connectionString, error);
  }
  else if (isProtocol(url, "inmemory://"))
  {
    openOptions = createInMemoryOpenOptions(removeProtocol(url, "inmemory://"), connectionString, error);
  }
  else
  {
    error.code = -1;
    error.string = fmt::format("Unknown url scheme for {}.", std::string(url.data, url.data + url.size));
    return nullptr;
  }

  if (error.code)
  {
    return nullptr;
  }
  return openOptions.release();
}

VDS *Open(StringWrapper url, StringWrapper connectionString, Error& error)
{
  error = Error();
  std::unique_ptr<IOManager> ioManager(IOManager::CreateIOManager(url, connectionString, error));
  if (error.code)
    return nullptr;

  return Open(ioManager.release(), error);
}

VDS* Open(IOManager *ioManager, Error& error)
{
  std::unique_ptr<VDS> ret(new VDS(ioManager));

  error = Error();
  if (!DownloadAndParseVolumeDataLayoutAndLayerStatus(*ret.get(), error))
  {
    return nullptr;
  }
  ret->accessManager.reset(new VolumeDataAccessManagerImpl(*ret.get()));
  return ret.release();
}

VDS *Open(const OpenOptions &options, Error &error)
{
  error = Error();
  std::unique_ptr<IOManager> ioManager(IOManager::CreateIOManager(options, error));
  if (error.code)
    return nullptr;

  return Open(ioManager.release(), error);
}

VolumeDataLayout *GetLayout(VDS *vds)
{
  if (!vds)
    return nullptr;
  return vds->volumeDataLayout.get();
}

VolumeDataAccessManager *GetAccessManager(VDS *vds)
{
  if (!vds)
    return nullptr;
  return vds->accessManager.get();
}

const char *AddDescriptorString(std::string const &descriptorString, VDS &vds)
{
  char *data = new char[descriptorString.size() + 1];
  memcpy(data, descriptorString.data(), descriptorString.size());
  data[descriptorString.size()] = 0;
  vds.descriptorStrings.emplace_back(data);
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
  std::unique_lock<std::mutex> metadataManagersMutexLock(vds.layerMetadataContainer.mutex);

  assert(vds.layerMetadataContainer.managers.find(layerName) == vds.layerMetadataContainer.managers.end());
  int pageLimit = vds.axisDescriptors.size() <= 3 ? 64 : 1024;
  return vds.layerMetadataContainer.managers.insert(std::make_pair(layerName, std::unique_ptr<MetadataManager>(new MetadataManager(vds.ioManager.get(), layerName, metadataStatus, pageLimit)))).first->second.get();
}

void CreateVolumeDataLayout(VDS &vds)
{
  int32_t dimensionality = int32_t(vds.axisDescriptors.size());

  // Check if input layouts are valid so we can create a new layout
  if (dimensionality < 2)
  {
    vds.volumeDataLayout.reset();
    return;
  }

  const int actualValueRangeChannel = -1;
  const FloatRange actualValueRange = FloatRange(1, 0);

  vds.volumeDataLayout.reset(
    new VolumeDataLayoutImpl(
      vds,
      vds.layoutDescriptor,
      vds.axisDescriptors,
      vds.channelDescriptors,
      actualValueRangeChannel,
      actualValueRange,
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

    int32_t physicalLODLevels = (nChunkDimensionality == 3 || vds.layoutDescriptor.IsCreate2DLODs()) ? GetLODCount(vds.layoutDescriptor) : 1;
    int32_t brickSize = GetInternalCubeSizeLOD0(vds.layoutDescriptor) * (nChunkDimensionality == 2 ? vds.layoutDescriptor.GetBrickSizeMultiplier2D() : 1);

    vds.volumeDataLayout->CreateLayers(dimensionGroup, brickSize, physicalLODLevels, vds.produceStatuses[DimensionGroupUtil::GetDimensionsNDFromDimensionGroup(dimensionGroup)]);
  }
}

static void copyMetadataToContainer(MetadataContainer &container, const MetadataReadAccess &readAccess)
{
  std::unordered_set<std::string> categories;
  for (auto &key : readAccess.GetMetadataKeys())
  {
    categories.insert(key.GetCategory());
  }
  for (auto &category : categories)
  {
    container.CopyMetadata(category.c_str(), &readAccess);
  }
}

VDSHandle Create(IOManager* ioManager, VolumeDataLayoutDescriptor const &layoutDescriptor, VectorWrapper<VolumeDataAxisDescriptor> axisDescriptors, VectorWrapper<VolumeDataChannelDescriptor> channelDescriptors, MetadataReadAccess const &metadata, Error &error)
{
  error = Error();
  std::unique_ptr<VDS> vds(new VDS(ioManager));

  vds->layoutDescriptor = layoutDescriptor;

  for(size_t i = 0; i < axisDescriptors.size; i++)
  {
    auto &axisDescriptor = axisDescriptors.data[i];
    vds->axisDescriptors.push_back(VolumeDataAxisDescriptor(axisDescriptor.GetNumSamples(), AddDescriptorString(axisDescriptor.GetName(), *vds), AddDescriptorString(axisDescriptor.GetUnit(), *vds), axisDescriptor.GetCoordinateMin(), axisDescriptor.GetCoordinateMax()));
  }

  for(size_t i = 0; i < channelDescriptors.size; i++)
  {
    auto &channelDescriptor = channelDescriptors.data[i];
    VolumeDataChannelDescriptor::Flags flags = VolumeDataChannelDescriptor::Default;

    if(channelDescriptor.IsDiscrete())                     flags = flags | VolumeDataChannelDescriptor::DiscreteData;
    if(!channelDescriptor.IsAllowLossyCompression())       flags = flags | VolumeDataChannelDescriptor::NoLossyCompression;
    if(channelDescriptor.IsUseZipForLosslessCompression()) flags = flags | VolumeDataChannelDescriptor::NoLossyCompressionUseZip;
    if(!channelDescriptor.IsRenderable())                  flags = flags | VolumeDataChannelDescriptor::NotRenderable;

    if(channelDescriptor.IsUseNoValue())
    {
      vds->channelDescriptors.push_back(VolumeDataChannelDescriptor(channelDescriptor.GetFormat(), channelDescriptor.GetComponents(), AddDescriptorString(channelDescriptor.GetName(), *vds), AddDescriptorString(channelDescriptor.GetUnit(), *vds), channelDescriptor.GetValueRangeMin(), channelDescriptor.GetValueRangeMax(), channelDescriptor.GetMapping(), channelDescriptor.GetMappedValueCount(), flags, channelDescriptor.GetNoValue(), channelDescriptor.GetIntegerScale(), channelDescriptor.GetIntegerOffset()));
    }
    else
    {
      vds->channelDescriptors.push_back(VolumeDataChannelDescriptor(channelDescriptor.GetFormat(), channelDescriptor.GetComponents(), AddDescriptorString(channelDescriptor.GetName(), *vds), AddDescriptorString(channelDescriptor.GetUnit(), *vds), channelDescriptor.GetValueRangeMin(), channelDescriptor.GetValueRangeMax(), channelDescriptor.GetMapping(), channelDescriptor.GetMappedValueCount(), flags, channelDescriptor.GetIntegerScale(), channelDescriptor.GetIntegerOffset()));
    }
  }

  copyMetadataToContainer(vds->metadataContainer, metadata);

  vds->produceStatuses.clear();
  vds->produceStatuses.resize(int(Dimensions_45) + 1, VolumeDataLayer::ProduceStatus_Unavailable);

  CreateVolumeDataLayout(*vds);

  if (error.code)
    return nullptr;

  if (!SerializeAndUploadVolumeDataLayout(*vds, error))
    return nullptr;

  vds->accessManager.reset(new VolumeDataAccessManagerImpl(*vds.get()));

  return vds.release();
}

VDSHandle Create(StringWrapper url, StringWrapper connectionString, VolumeDataLayoutDescriptor const& layoutDescriptor, VectorWrapper<VolumeDataAxisDescriptor> axisDescriptors, VectorWrapper<VolumeDataChannelDescriptor> channelDescriptors, MetadataReadAccess const& metadata, Error& error)
{
  error = Error();
  std::unique_ptr<IOManager> ioManager(IOManager::CreateIOManager(url, connectionString, error));
  if (error.code)
    return nullptr;

  return Create(ioManager.release(), layoutDescriptor, axisDescriptors, channelDescriptors, metadata, error);
}

VDSHandle Create(const OpenOptions& options, VolumeDataLayoutDescriptor const& layoutDescriptor, VectorWrapper<VolumeDataAxisDescriptor> axisDescriptors, VectorWrapper<VolumeDataChannelDescriptor> channelDescriptors, MetadataReadAccess const& metadata, Error& error)
{
  error = Error();
  std::unique_ptr<IOManager> ioManager(IOManager::CreateIOManager(options, error));
  if (error.code)
    return nullptr;

  return Create(ioManager.release(), layoutDescriptor, axisDescriptors, channelDescriptors, metadata, error);
}

void Close(VDS *vds)
{
  delete vds;
}

}
