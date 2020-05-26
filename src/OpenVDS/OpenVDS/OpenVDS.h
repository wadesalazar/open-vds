/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
** Copyright 2020 Microsoft Corp.
** Copyright 2020 Google, Inc.
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

#include <OpenVDS/openvds_export.h>
#include <OpenVDS/Metadata.h>
#include <OpenVDS/VolumeData.h>


#include <cstdint>
#include <string>
#include <vector>

namespace OpenVDS
{
class VolumeDataLayoutDescriptor;
class VolumeDataAxisDescriptor;
class VolumeDataChannelDescriptor;
class IOManager;

struct OpenOptions
{
  enum ConnectionType
  {
    AWS,
    Azure,
    AzurePresigned,
    GoogleStorage,
    File,
    InMemory
  };

  ConnectionType connectionType;

protected:
  OpenOptions(ConnectionType connectionType) : connectionType(connectionType) {}
};

/// <summary>
/// Options for opening a VDS in AWS
/// </summary>
struct AWSOpenOptions : OpenOptions
{
  std::string bucket;
  std::string key;
  std::string region;
  std::string endpointOverride;

  AWSOpenOptions() : OpenOptions(AWS) {}
  /// <summary>
  /// AWSOpenOptions constructor
  /// </summary>
  /// <param name="bucket">
  /// The bucket of the VDS
  /// </param>
  /// <param name="key">
  /// The key prefix of the VDS
  /// </param>
  /// <param name="region">
  /// The region of the bucket of the VDS
  /// </param>
  /// <param name="endpointOverride">
  /// This parameter allows to override the endpoint url
  /// </param>
  AWSOpenOptions(std::string const & bucket, std::string const & key, std::string const & region, std::string const & endpointOverride = std::string()) : OpenOptions(AWS), bucket(bucket), key(key), region(region), endpointOverride(endpointOverride) {}
};

/// <summary>
/// Options for opening a VDS in Azure
/// </summary>
struct AzureOpenOptions : OpenOptions
{
  std::string connectionString;
  std::string container;
  std::string blob;

  int parallelism_factor = 4;
  int max_execution_time = 100000;

  AzureOpenOptions() : OpenOptions(Azure) {}

  /// <summary>
  /// AzureOpenOptions constructor
  /// </summary>
  /// <param name="connectionString">
  /// The connectionString for the VDS
  /// </param>
  /// <param name="container">
  /// The container of the VDS
  /// </param>
  /// <param name="blob">
  /// The blob prefix of the VDS
  /// </param>
  AzureOpenOptions(std::string const& connectionString, std::string const& container, std::string const& blob) : OpenOptions(Azure), connectionString(connectionString), container(container), blob(blob) {}

  /// <summary>
  /// AzureOpenOptions constructor
  /// </summary>
  /// <param name="connectionString">
  /// The connectionString for the VDS
  /// </param>
  /// <param name="container">
  /// The container of the VDS
  /// </param>
  /// <param name="blob">
  /// The blob prefix of the VDS
  /// </param>
  /// <param name="parallelism_factor">
  /// The parallelism factor setting for the Azure Blob Storage library
  /// </param>
  /// <param name="max_execution_time">
  /// The max execution time setting for the Azure Blob Storage library
  /// </param>
  AzureOpenOptions(std::string const& connectionString, std::string const& container, std::string const& blob, int& parallelism_factor, int& max_execution_time) : OpenOptions(Azure), connectionString(connectionString), container(container), blob(blob), parallelism_factor(parallelism_factor), max_execution_time(max_execution_time) {}
};

/// <summary>
/// Options for opening a VDS with presigned Azure url
/// </summary>
struct AzurePresignedOpenOptions : OpenOptions
{
  std::string baseUrl;
  std::string urlSuffix;

  AzurePresignedOpenOptions() : OpenOptions(AzurePresigned) {}

  /// <summary>
  /// AzurePresignedOpenOptions constructor
  /// </summary>
  /// <param name="baseUrl">
  /// The base url for the VDS
  /// </param>
  /// <param name="urlSuffix">
  /// The suffix of the presigned url
  /// </param>
  AzurePresignedOpenOptions(const std::string &baseUrl, const std::string &urlSuffix) : OpenOptions(AzurePresigned), baseUrl(baseUrl), urlSuffix(urlSuffix) {}
};

/// <summary>
/// Options for opening a VDS in Google Cloud Storage
/// </summary>
struct GSOpenOptions : OpenOptions
{
  std::string bucket;
  std::string key;

  GSOpenOptions() : OpenOptions(GoogleStorage) {}
  /// <summary>
  /// GSOpenOptions constructor
  /// </summary>
  /// <param name="bucket">
  /// The bucket of the VDS
  /// </param>
  /// <param name="key">
  /// The key prefix of the VDS
  /// </param>
  GSOpenOptions(std::string const & bucket, std::string const & key) : OpenOptions(GoogleStorage), bucket(bucket), key(key) {}
};

/// <summary>
/// Options for opening a VDS which is stored in memory (for testing)
/// </summary>
struct InMemoryOpenOptions : OpenOptions
{
  InMemoryOpenOptions() : OpenOptions(InMemory) {}
};

struct Error
{
  int code = 0;
  std::string string;
};

template<typename T>
struct VectorWrapper
{
  VectorWrapper(const std::vector<T> &toWrap)
    : data(toWrap.data())
    , size(toWrap.size())
  {}

  const T *data;
  size_t size;
};

typedef struct VDS *VDSHandle;
class VolumeDataLayout;
class VolumeDataAccessManager;
class VolumeDataPageAccessor;

/// <summary>
/// Open an existing VDS
/// </summary>
/// <param name="options">
/// The options for the connection
/// </param>
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
/// </returns>
OPENVDS_EXPORT VDSHandle Open(const OpenOptions& options, Error& error);

/// <summary>
/// Open an existing VDS
/// </summary>
/// <param name="ioManager">
/// The IOManager for the connection, it will be deleted automatically when the VDS handle is closed
/// </param>
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
/// </returns>
OPENVDS_EXPORT VDSHandle Open(IOManager*ioManager, Error &error);

/// <summary>
/// Create a new VDS
/// </summary>
/// <param name="options">
/// The options for the connection
/// </param>
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
/// </returns>
OPENVDS_EXPORT VDSHandle Create(const OpenOptions& options, VolumeDataLayoutDescriptor const& layoutDescriptor, VectorWrapper<VolumeDataAxisDescriptor> axisDescriptors, VectorWrapper<VolumeDataChannelDescriptor> channelDescriptors, MetadataReadAccess const& metadata, Error& error);

/// <summary>
/// Create a new VDS
/// </summary>
/// <param name="ioManager">
/// The IOManager for the connection, it will be deleted automatically when the VDS handle is closed
/// </param>
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
/// </returns>
OPENVDS_EXPORT VDSHandle Create(IOManager* ioManager, VolumeDataLayoutDescriptor const &layoutDescriptor, VectorWrapper<VolumeDataAxisDescriptor> axisDescriptors, VectorWrapper<VolumeDataChannelDescriptor> channelDescriptors, MetadataReadAccess const &metadata, Error &error);

/// <summary>
/// Get the VolumeDataLayout for a VDS
/// </summary>
/// <param name="handle">
/// The handle of the VDS
/// </param>
/// <returns>
/// The VolumeDataLayout of the VDS
/// </returns>
OPENVDS_EXPORT VolumeDataLayout *GetLayout(VDSHandle handle);

/// <summary>
/// Get the VolumeDataAccessManager for a VDS
/// </summary>
/// <param name="handle">
/// The handle of the VDS
/// </param>
/// <returns>
/// The VolumeDataAccessManager of the VDS
/// </returns>
OPENVDS_EXPORT VolumeDataAccessManager *GetAccessManager(VDSHandle handle);

/// <summary>
/// Close a VDS and free up all associated resources
/// </summary>
/// <param name="handle">
/// The handle of the VDS
/// </param>
OPENVDS_EXPORT void Close(VDSHandle handle);

}

#endif //OPENVDS_H
