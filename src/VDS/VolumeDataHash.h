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
  static uint64_t GetUniqueHash();

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
  bool      IsDefined() const { return m_dataHash != UNKNOWN; }
  bool      IsNoValue() const { return m_dataHash == NOVALUE; }
  bool      IsConstant() const { return IsNoValue() || m_constant.magic == CONSTANT; }
  float       GetConstantValue() const { assert(IsConstant() && !IsNoValue()); return m_constant.value; }
  float       GetConstantValue(float rNoValue) const { assert(IsConstant()); return IsNoValue() ? rNoValue : m_constant.value; }
  uint64_t       CalculateHash() const { return m_dataHash; }

  VolumeDataHash & operator= (uint64_t uDataHash) { m_dataHash = uDataHash; return *this; }
  VolumeDataHash() : m_dataHash(UNKNOWN) {}
  VolumeDataHash(VolumeDataHash const &cVolumeDataHash) : m_dataHash(cVolumeDataHash.m_dataHash) {}
  VolumeDataHash(uint64_t uDataHash) : m_dataHash(uDataHash) {}
  explicit VolumeDataHash(float rConstantValue) { m_constant.value = rConstantValue; m_constant.magic = CONSTANT; }

  bool operator== (uint64_t uDataHash) { return m_dataHash == uDataHash; }
  operator uint64_t () const { return m_dataHash; }
};

}
#endif //VOLUMEDATAHASH_H
