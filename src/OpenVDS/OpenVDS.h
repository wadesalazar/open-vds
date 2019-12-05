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
    File,
    InMemory
  };

  ConnectionType connectionType;

protected:
  OpenOptions(ConnectionType connectionType) : connectionType(connectionType) {}
};

struct AWSOpenOptions : OpenOptions
{
  std::string bucket;
  std::string key;
  std::string region;

  AWSOpenOptions() : OpenOptions(AWS) {}
  AWSOpenOptions(std::string const & bucket, std::string const & key, std::string const & region) : OpenOptions(AWS), bucket(bucket), key(key), region(region) {}
};

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
OPENVDS_EXPORT VolumeDataLayout *GetLayout(VDSHandle handle);

/// <summary>
/// Get the VolumeDataAccessManager for a VDS
/// </summary>
/// <param name="handle">
/// The handle of the VDS
/// </param>
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
