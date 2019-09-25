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

#ifndef VOLUMEDATAHASH_H
#define VOLUMEDATAHASH_H

#include <cstdint>
#include <assert.h>
namespace OpenVDS
{
class VolumeDataHash
{
public:
  static const uint64_t UNKNOWN  = 0;
  static const uint64_t NOVALUE  = ~0ULL;
  static uint64_t getUniqueHash();

private:
  static const uint32_t CONSTANT = 0x01010101;

  union
  {
    uint64_t m_dataHash;
    struct
    {
      float value;
      uint32_t magic;
    } m_constant;
  };
public:
  bool      isDefined() const { return m_dataHash != UNKNOWN; }
  bool      isNoValue() const { return m_dataHash == NOVALUE; }
  bool      isConstant() const { return isNoValue() || m_constant.magic == CONSTANT; }
  float     getConstantValue() const { assert(isConstant() && !isNoValue()); return m_constant.value; }
  float     getConstantValue(float rNoValue) const { assert(isConstant()); return isNoValue() ? rNoValue : m_constant.value; }
  uint64_t  calculateHash() const { return m_dataHash; }

  VolumeDataHash & operator= (uint64_t dataHash) { m_dataHash = dataHash; return *this; }
  VolumeDataHash() : m_dataHash(UNKNOWN) {}
  VolumeDataHash(VolumeDataHash const &volumeDataHash) : m_dataHash(volumeDataHash.m_dataHash) {}
  VolumeDataHash(uint64_t dataHash) : m_dataHash(dataHash) {}
  explicit VolumeDataHash(float constantValue) { m_constant.value = constantValue; m_constant.magic = CONSTANT; }

  bool operator== (uint64_t dataHash) { return m_dataHash == dataHash; }
  explicit operator uint64_t () const { return m_dataHash; }
};

}
#endif //VOLUMEDATAHASH_H
