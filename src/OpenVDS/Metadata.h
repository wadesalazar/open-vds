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

#ifndef OPENVDS_METADATA_H
#define OPENVDS_METADATA_H

#include <unordered_map>
#include <Math/Vector.h>

namespace OpenVDS
{
struct MetadataKey
{
  std::string category;
  std::string name;
};

inline bool operator==(const MetadataKey& a, const MetadataKey& b) { return a.category == b.category && a.name == b.name; }

} // end namespace OpenVDS

namespace std
{
template<>
struct hash<OpenVDS::MetadataKey>
{
  std::size_t operator()(const OpenVDS::MetadataKey &k) const
  { 
    size_t const h1= std::hash<std::string>()(k.category);
    size_t const h2= std::hash<std::string>()(k.name);
    return h1 ^ (h2 << 1);
  }
};
} // end namespace std

namespace OpenVDS
{
struct MetadataContainer
{
  std::unordered_map<MetadataKey, int>     intData;
  std::unordered_map<MetadataKey, IntVec2> intVec2Data;
  std::unordered_map<MetadataKey, IntVec3> intVec3Data;
  std::unordered_map<MetadataKey, IntVec4> intVec4Data;

  std::unordered_map<MetadataKey, float>     floatData;
  std::unordered_map<MetadataKey, FloatVec2> floatVec2Data;
  std::unordered_map<MetadataKey, FloatVec3> floatVec3Data;
  std::unordered_map<MetadataKey, FloatVec4> floatVec4Data;

  std::unordered_map<MetadataKey, double>     doubleData;
  std::unordered_map<MetadataKey, DoubleVec2> doubleVec2Data;
  std::unordered_map<MetadataKey, DoubleVec3> doubleVec3Data;
  std::unordered_map<MetadataKey, DoubleVec4> doubleVec4Data;

 
  std::unordered_map<MetadataKey, std::string> stringData;
  std::unordered_map<MetadataKey, std::vector<uint8_t>> blobData;
};
} // end namespace OpenVDS

#endif // OPENVDS_METADATA_H
