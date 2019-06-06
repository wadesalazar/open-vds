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

#include "VolumeDataHash.h"

#include <mutex>
#include <random>

namespace OpenVDS
{
uint64_t VolumeDataHash::GetUniqueHash()
{
  static std::mutex hashMutex;

  std::lock_guard<std::mutex> lock(hashMutex); 

  static std::default_random_engine upper;
  static std::default_random_engine lower;

  std::uniform_int_distribution<uint32_t> distribution;

  return uint64_t((uint64_t(distribution(upper)) << 32) | uint64_t(distribution(lower)));
}
}
