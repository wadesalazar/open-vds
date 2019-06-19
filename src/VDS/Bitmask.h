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

#ifndef BITMASK_H_INCLUDE
#define BITMASK_H_INCLUDE

#include <type_traits>
namespace OpenVDS
{
namespace Internal
{
template <typename T>
struct bit_mask {
  using U = typename std::underlying_type<T>::type;
  constexpr bit_mask()
    : _flags()
  {
  }
  constexpr bit_mask(const bit_mask<T> &other) = default;

  template <typename... ARGS>
    bit_mask(T flag, ARGS... args)
    : bit_mask(args...)
    {
      _flags |= static_cast<U>(flag);
    }
  constexpr bit_mask(T flag)
    : _flags(static_cast<U>(flag))
  {
  }

  explicit constexpr bit_mask(U u)
    : _flags(u)
  {
  }

  constexpr  bit_mask<T> operator~() const { return bit_mask(~_flags); }
  constexpr  bit_mask<T> operator|(const bit_mask<T> &other) const { return bit_mask<T>(_flags | other._flags); }
  constexpr  bit_mask<T> operator&(const bit_mask<T> &other) const { return bit_mask<T>(_flags & other._flags); }
  constexpr  bit_mask<T> operator^(const bit_mask<T> &other) const { return bit_mask<T>(_flags ^ other._flags); }
  bit_mask<T>& operator|= (const bit_mask<T> &other) { _flags |= other._flags; return *this; }
  bit_mask<T>& operator&= (const bit_mask<T> &other) { _flags &= other._flags; return *this; }
  bit_mask<T>& operator^= (const bit_mask<T> &other) { _flags ^= other._flags; return *this; }

  template<typename V>
    friend constexpr bit_mask<V> operator|(V a, bit_mask<V> &b);
  template<typename V>
    friend constexpr bit_mask<V> operator&(V a, const bit_mask<V> &b);
  template<typename V>
    friend constexpr bit_mask<V> operator^(V a, const bit_mask<V> &b);
  T to_enum() const { return T(_flags); }
  U _flags;
};

template<typename T>
  constexpr bit_mask<T> operator|(T a, bit_mask<T> &b) { return bit_mask<T>(static_cast<typename std::underlying_type<T>::type>(a) | b._flags); }
template<typename T>
  constexpr  bit_mask<T> operator&(T a, const bit_mask<T> &b) { return bit_mask<T>(static_cast<typename std::underlying_type<T>::type>(a) & b._flags); }
template<typename T>
  constexpr  bit_mask<T> operator^(T a, const bit_mask<T> &b) { return bit_mask<T>(static_cast<typename std::underlying_type<T>::type>(a) ^ b._flags); }

//this is to eager
//template<typename T>
//constexpr  inline bit_mask<T> operator|(T a, T b) { return bit_mask<T>(static_cast<typename std::underlying_type<T>::type>(a) | static_cast<typename std::underlying_type<T>::type>(b)); }
//template<typename T>
//constexpr  inline bit_mask<T> operator&(T a, T b) { return bit_mask<T>(static_cast<typename std::underlying_type<T>::type>(a) & static_cast<typename std::underlying_type<T>::type>(b)); }
//template<typename T>
//constexpr  inline bit_mask<T> operator^(T a, T b) { return bit_mask<T>(static_cast<typename std::underlying_type<T>::type>(a) ^ static_cast<typename std::underlying_type<T>::type>(b)); }
}
}
#endif // BITMASK_H_INCLUDE
