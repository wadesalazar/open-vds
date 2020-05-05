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

#ifndef HASH_H_INCLUDE
#define HASH_H_INCLUDE

#include <string>
#include <cstring>
#include <OpenVDS/Range.h>

namespace OpenVDS
{

template <class T>
inline uint64_t ConvertToIntForHashing(T tValue)
{
  return tValue;
}

template <>
inline uint64_t ConvertToIntForHashing(float rValue)
{
  uint64_t ret = 0;
  memcpy(&ret, &rValue, sizeof(rValue));
  return ret;
}

template <>
inline uint64_t ConvertToIntForHashing(double rValue)
{
  uint64_t ret = 0;
  memcpy(&ret, &rValue, sizeof(rValue));
  return ret;
}


template <typename T>
struct InternalHasher
{
static uint64_t CalculateHash(const T &tValue)
{
  uint64_t uValue = tValue;
  uValue *= 0x87c37b91114253d5ULL;
  uValue = (uValue << 31) | (uValue >> (64 - 31)); // ROTL64(uValue,31);
  uValue *= 0x4cf5ad432745937fULL;
  return uValue;
}
};
template <>
struct InternalHasher<float>
{
static uint64_t CalculateHash(float rValue) { union { float _rValue; uint32_t _uValue; } convert; convert._rValue = rValue; return InternalHasher<uint32_t>::CalculateHash(convert._uValue); }
};

template <>
struct InternalHasher<double>
{
static uint64_t CalculateHash(double rValue) { union { double _rValue; uint64_t _uValue; } convert; convert._rValue = rValue; return InternalHasher<uint64_t>::CalculateHash(convert._uValue); }
};

template <>
struct InternalHasher<std::string>
{
static uint64_t CalculateHash(std::basic_string<char> const &str)
{
  uint64_t
    hash(0xff51afd7ed558ccdULL);

  int64_t length = str.length(), i = 0;
  while(i < length)
  {
    uint64_t
      chars = str[i++];

    if(i < length) chars |= str[i++] << 8;
    if(i < length) chars |= str[i++] << 16;
    if(i < length) chars |= str[i++] << 24;

    hash ^=  InternalHasher<uint64_t>::CalculateHash(chars);
    hash = (hash << 27) | (hash >> (64 - 27));
    hash = hash * 5 + 0x52dce729;
  }

  return hash;
}
};

template<typename T>
struct InternalHasher<Range<T>>
{
  static uint64_t CalculateHash(const Range<T>& value)
  {
    return ConvertToIntForHashing(value.Min) * 98953412543643LL +
           ConvertToIntForHashing(value.Max) * 45985432099125LL;
  }
};

template<typename Key>
class HashTraits
{

  // This will yield a substitution failure if the template parameter isn't a class (so a member function pointer type cannot be formed)
  template <typename T> struct TypeCheck;

public:
  static uint64_t CalculateHash(Key const &cA) { return InternalHasher<Key>::CalculateHash(cA); }

  static bool
    Equal(Key const &cA, Key const &cB) { return cA == cB; }
};

class HashCombiner
{
  uint64_t m_combinedHash;
public:
  HashCombiner()
    : m_combinedHash(0xff51afd7ed558ccdULL)
  {
  }

  template<typename T>
  explicit HashCombiner(const T& tValue)
    : m_combinedHash(0xff51afd7ed558ccdULL)
  {
    Add(tValue);
  }

  operator uint64_t() const
  {
    return m_combinedHash;
  }

  // Methods
  uint64_t GetCombinedHash() const { return m_combinedHash; }

  template<typename T>
  HashCombiner& Add(const T& tValue)
  {
    m_combinedHash ^= HashTraits<T>::CalculateHash(tValue);
    m_combinedHash = (m_combinedHash << 27) | (m_combinedHash >> (64 - 27));
    m_combinedHash = m_combinedHash * 5 + 0x52dce729;
    return *this;
  }

  template<typename T>
  HashCombiner& Add(const T* atValue, int32_t nSize)
  {
    for (int32_t i = 0; i < nSize; ++i)
    {
      Add(atValue[i]);
    }

    return *this;
  }
};

}

#endif //HASH_H_INCLUDE

