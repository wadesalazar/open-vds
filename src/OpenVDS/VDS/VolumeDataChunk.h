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

#ifndef VOLUMEDATACHUNK_H
#define VOLUMEDATACHUNK_H

#include <cstdint>
#include "Hash.h"

namespace OpenVDS
{
class VolumeDataLayer;
struct VolumeDataChunk
{
  const VolumeDataLayer *layer;
  int64_t index;
};

inline bool operator==(VolumeDataChunk const &lhs, VolumeDataChunk const &rhs)
{
  return lhs.index == rhs.index && lhs.layer == rhs.layer;
}

bool operator<(VolumeDataChunk const& lhs, VolumeDataChunk const& rhs);
}

namespace std {
template <>
struct hash<OpenVDS::VolumeDataChunk>
{
  std::size_t operator()(const OpenVDS::VolumeDataChunk& volumeDataChunk) const;
};
}

#endif //VOLUMEDATACHUNK_H
