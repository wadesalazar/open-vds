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

#include <string>
#include <vector>

namespace OpenVDS
{

enum class MetadataType
{
  Int,
  IntVector2,
  IntVector3,
  IntVector4,
  Float,
  FloatVector2,
  FloatVector3,
  FloatVector4,
  Double,
  DoubleVector2,
  DoubleVector3,
  DoubleVector4,
  String,
  BLOB
};

struct MetadataKey
{
  MetadataType type;
  std::string  category;
  std::string  name;
};

inline bool operator==(const MetadataKey& a, const MetadataKey& b) { return a.type == b.type && a.category == b.category && a.name == b.name; }

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
  std::unordered_map<MetadataKey, int>        intData;
  std::unordered_map<MetadataKey, IntVector2> intVector2Data;
  std::unordered_map<MetadataKey, IntVector3> intVector3Data;
  std::unordered_map<MetadataKey, IntVector4> intVector4Data;

  std::unordered_map<MetadataKey, float>        floatData;
  std::unordered_map<MetadataKey, FloatVector2> floatVector2Data;
  std::unordered_map<MetadataKey, FloatVector3> floatVector3Data;
  std::unordered_map<MetadataKey, FloatVector4> floatVector4Data;

  std::unordered_map<MetadataKey, double>        doubleData;
  std::unordered_map<MetadataKey, DoubleVector2> doubleVector2Data;
  std::unordered_map<MetadataKey, DoubleVector3> doubleVector3Data;
  std::unordered_map<MetadataKey, DoubleVector4> doubleVector4Data;
 
  std::unordered_map<MetadataKey, std::string> stringData;

  std::unordered_map<MetadataKey, std::vector<uint8_t>> blobData;

  std::vector<MetadataKey> keys;
};
} // end namespace OpenVDS

#endif // OPENVDS_METADATA_H
