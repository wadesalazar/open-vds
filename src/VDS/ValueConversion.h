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

#ifndef VALUECONVERSION_H
#define VALUECONVERSION_H

#include <type_traits>
#include <limits>

namespace OpenVDS
{

// Real/Integer -> Bool conversion
template<typename T1, typename T2>
inline typename std::enable_if<std::is_same<T1,bool>::value && !std::is_same<T2,bool>::value, T1>::type
convertValue(T2 value)
{
  return value != 0;
}

// Bool -> Real/Integer/Bool conversion
template<typename T1, typename T2>
inline typename std::enable_if<std::is_same<T2,bool>::value, T1>::type
convertValue(T2 value)
{
  return value;
}

// Integer -> Integer conversion
template<typename T1, typename T2>
inline typename std::enable_if<std::numeric_limits<T1>::is_integer && !std::is_same<T1,bool>::value && std::numeric_limits<T2>::is_integer && !std::is_same<T2,bool>::value, T1>::type
convertValue(T2 value)
{
  if((std::make_signed<T1>::type)((std::numeric_limits<T1>::min)()) > (std::make_signed<T2>::type)((std::numeric_limits<T2>::min)()))
  {
    if(value < (T2)((std::numeric_limits<T1>::min)())) return ((std::numeric_limits<T1>::min)());
  }

  if((std::make_unsigned<T1>::type)((std::numeric_limits<T1>::max)()) < (std::make_unsigned<T2>::type)((std::numeric_limits<T2>::max)()))
  {
    if(value > (T2)((std::numeric_limits<T1>::max)())) return ((std::numeric_limits<T1>::max)());
  }

  return (T1)value;
}

// Real -> Integer conversion
template<typename T1, typename T2>
inline typename std::enable_if<std::numeric_limits<T1>::is_integer && !std::is_same<T1,bool>::value && !std::numeric_limits<T2>::is_integer, T1>::type
convertValue(T2 value)
{
  if(value < ((std::numeric_limits<T1>::min)())) return ((std::numeric_limits<T1>::min)());
  if(value > ((std::numeric_limits<T1>::max)())) return ((std::numeric_limits<T1>::max)());
  if(std::numeric_limits<T1>::is_signed)
  {
    return (T1)std::floor(value + 0.5f);
  }
  else
  {
    return (T1)(value + 0.5f);
  }
}

// Integer/Real -> Real conversion
template<typename T1, typename T2>
inline typename std::enable_if<!std::numeric_limits<T1>::is_integer && !std::is_same<T2,bool>::value, T1>::type
convertValue(T2 value)
{
  return (T1)value;
}

template <typename T>
inline T convertNoValue(float noValue)
{
  return convertValue<T, float>(noValue);
}

// Conversion of NoValue that takes quantized types into account
template <>
inline bool convertNoValue<bool>(float noValue)
{
  return false;
}

// Conversion of NoValue that takes quantized types into account
template <>
inline uint8_t convertNoValue<uint8_t>(float noValue)
{
  return 0xff;
}

// Conversion of NoValue that takes quantized types into account
template <>
inline uint16_t convertNoValue<uint16_t>(float noValue)
{
  return 0xffff;
}

// Conversion of NoValue that takes quantized types into account
template <typename T>
inline T convertNoValue(double noValue)
{
  return convertValue<T, double>(noValue);
}

// Conversion of NoValue that takes quantized types into account
template <>
inline bool convertNoValue<bool>(double noValue)
{
  return false;
}

// Conversion of NoValue that takes quantized types into account
template <>
inline uint8_t convertNoValue<uint8_t>(double noValue)
{
  return 0xff;
}

// Conversion of NoValue that takes quantized types into account
template <>
inline uint16_t convertNoValue<uint16_t>(double noValue)
{
  return 0xffff;
}

}

#endif //VALUECONVERSION_H
