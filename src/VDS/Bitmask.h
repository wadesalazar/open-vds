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
struct BitMask {
  using U = typename std::underlying_type<T>::type;
  constexpr BitMask()
    : _flags()
  {
  }
  constexpr BitMask(const BitMask<T> &other) = default;

  template <typename... ARGS>
    BitMask(T flag, ARGS... args)
    : BitMask(args...)
    {
      _flags |= static_cast<U>(flag);
    }
  constexpr BitMask(T flag)
    : _flags(static_cast<U>(flag))
  {
  }

  explicit constexpr BitMask(U u)
    : _flags(u)
  {
  }

  constexpr  BitMask<T> operator~() const { return BitMask(~_flags); }
  constexpr  BitMask<T> operator|(const BitMask<T> &other) const { return BitMask<T>(_flags | other._flags); }
  constexpr  BitMask<T> operator&(const BitMask<T> &other) const { return BitMask<T>(_flags & other._flags); }
  constexpr  BitMask<T> operator^(const BitMask<T> &other) const { return BitMask<T>(_flags ^ other._flags); }
  BitMask<T>& operator|= (const BitMask<T> &other) { _flags |= other._flags; return *this; }
  BitMask<T>& operator&= (const BitMask<T> &other) { _flags &= other._flags; return *this; }
  BitMask<T>& operator^= (const BitMask<T> &other) { _flags ^= other._flags; return *this; }

  template<typename V>
    friend constexpr BitMask<V> operator|(V a, BitMask<V> &b);
  template<typename V>
    friend constexpr BitMask<V> operator&(V a, const BitMask<V> &b);
  template<typename V>
    friend constexpr BitMask<V> operator^(V a, const BitMask<V> &b);
  T to_enum() const { return T(_flags); }
  U _flags;
};

template<typename T>
  constexpr BitMask<T> operator|(T a, BitMask<T> &b) { return BitMask<T>(static_cast<typename std::underlying_type<T>::type>(a) | b._flags); }
template<typename T>
  constexpr  BitMask<T> operator&(T a, const BitMask<T> &b) { return BitMask<T>(static_cast<typename std::underlying_type<T>::type>(a) & b._flags); }
template<typename T>
  constexpr  BitMask<T> operator^(T a, const BitMask<T> &b) { return BitMask<T>(static_cast<typename std::underlying_type<T>::type>(a) ^ b._flags); }

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
