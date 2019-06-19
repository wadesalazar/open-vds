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

#include <VDS/VolumeDataLayoutDescriptor.h>
#include <VDS/VolumeDataAxisDescriptor.h>
#include <VDS/VolumeDataChannelDescriptor.h>
#include <VDS/VolumeDataLayer.h>

#include <Math/Vector.h>

#include <vector>
#include <memory>
#include <unordered_map>

namespace OpenVDS
{
struct VDSMetaDataKey
{
  std::string category;
  std::string name;
};
inline bool operator==(const VDSMetaDataKey& a, const VDSMetaDataKey& b)
{
  return a.category == b.category
    && a.name == b.name;
}
}

namespace std
{
template<>
struct hash<OpenVDS::VDSMetaDataKey>
{
  std::size_t operator()(const OpenVDS::VDSMetaDataKey &k) const
  { 
    size_t const h1= std::hash<std::string>()(k.category);
    size_t const h2= std::hash<std::string>()(k.name);
    return h1 ^ (h2 << 1);
  }
};
}

namespace OpenVDS
{
struct VDSMetaContainer
{
  std::unordered_map<VDSMetaDataKey, int> intData;
  std::unordered_map<VDSMetaDataKey, IntVec2> intVec2Data;
  std::unordered_map<VDSMetaDataKey, IntVec3> intVec3Data;
  std::unordered_map<VDSMetaDataKey, IntVec4> intVec4Data;
  std::unordered_map<VDSMetaDataKey, float> floatData;
  std::unordered_map<VDSMetaDataKey, FloatVec2> floatVec2Data;
  std::unordered_map<VDSMetaDataKey, FloatVec3> floatVec3Data;
  std::unordered_map<VDSMetaDataKey, FloatVec4> floatVec4Data;
  std::unordered_map<VDSMetaDataKey, double> doubleData;
  std::unordered_map<VDSMetaDataKey, DoubleVec2> doubleVec2Data;
  std::unordered_map<VDSMetaDataKey, DoubleVec3> doubleVec3Data;
  std::unordered_map<VDSMetaDataKey, DoubleVec4> doubleVec4Data;
  std::unordered_map<VDSMetaDataKey, std::string> stringData;
  std::unordered_map<VDSMetaDataKey, std::vector<uint8_t>> blobData;
};

struct VDSFreeDelete
{
    void operator()(void* x) { free(x); }
};

struct VDSDescriptorString
{
  uint32_t size;
  char data[];
};

struct VDSHandle
{
  std::string url;
  VolumeDataLayoutDescriptor layoutDescriptor;
  std::vector<VolumeDataAxisDescriptor> axisDescriptors;
  std::vector<VolumeDataChannelDescriptor> channelDescriptors;
  std::vector<std::unique_ptr<VDSDescriptorString, VDSFreeDelete>> descriptorStrings;
  std::vector<VolumeDataLayer::ProduceStatus> produceStatuses;
  VDSMetaContainer metaDataContainer;
};
}

#endif //OPENVDSHANDLE_H
