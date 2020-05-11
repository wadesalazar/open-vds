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
#include <cmath>
#include <stdint.h>

namespace OpenVDS
{

// Real/Integer -> Bool conversion
template<typename T1, typename T2>
inline typename std::enable_if<std::is_same<T1,bool>::value && !std::is_same<T2,bool>::value, T1>::type
ConvertValue(T2 value)
{
  return value != 0;
}

// Bool -> Real/Integer/Bool conversion
template<typename T1, typename T2>
inline typename std::enable_if<std::is_same<T2,bool>::value, T1>::type
ConvertValue(T2 value)
{
  return value;
}

// Integer -> Integer conversion
template<typename T1, typename T2>
inline typename std::enable_if<std::numeric_limits<T1>::is_integer && !std::is_same<T1,bool>::value && std::numeric_limits<T2>::is_integer && !std::is_same<T2,bool>::value, T1>::type
ConvertValue(T2 value)
{
  if((typename std::make_signed<T1>::type)((std::numeric_limits<T1>::min)()) > (typename std::make_signed<T2>::type)((std::numeric_limits<T2>::min)()))
  {
    if(value < (T2)((std::numeric_limits<T1>::min)())) return ((std::numeric_limits<T1>::min)());
  }

  if((typename std::make_unsigned<T1>::type)((std::numeric_limits<T1>::max)()) < (typename std::make_unsigned<T2>::type)((std::numeric_limits<T2>::max)()))
  {
    if(value > (T2)((std::numeric_limits<T1>::max)())) return ((std::numeric_limits<T1>::max)());
  }

  return (T1)value;
}

// Real -> Integer conversion
template<typename T1, typename T2>
inline typename std::enable_if<std::numeric_limits<T1>::is_integer && !std::is_same<T1,bool>::value && !std::numeric_limits<T2>::is_integer, T1>::type
ConvertValue(T2 value)
{
  if(value < T2((std::numeric_limits<T1>::min)())) return ((std::numeric_limits<T1>::min)());
  if(value > T2((std::numeric_limits<T1>::max)())) return ((std::numeric_limits<T1>::max)());
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
ConvertValue(T2 value)
{
  return (T1)value;
}

template <typename T>
inline T ConvertNoValue(float noValue)
{
  return ConvertValue<T, float>(noValue);
}

// Conversion of NoValue that takes quantized types into account
template <>
inline bool ConvertNoValue<bool>(float /*noValue*/)
{
  return false;
}

// Conversion of NoValue that takes quantized types into account
template <>
inline uint8_t ConvertNoValue<uint8_t>(float /*noValue*/)
{
  return 0xff;
}

// Conversion of NoValue that takes quantized types into account
template <>
inline uint16_t ConvertNoValue<uint16_t>(float /*noValue*/)
{
  return 0xffff;
}

// Conversion of NoValue that takes quantized types into account
template <typename T>
inline T ConvertNoValue(double noValue)
{
  return ConvertValue<T, double>(noValue);
}

// Conversion of NoValue that takes quantized types into account
template <>
inline bool ConvertNoValue<bool>(double /*noValue*/)
{
  return false;
}

// Conversion of NoValue that takes quantized types into account
template <>
inline uint8_t ConvertNoValue<uint8_t>(double /*noValue*/)
{
  return 0xff;
}

// Conversion of NoValue that takes quantized types into account
template <>
inline uint16_t ConvertNoValue<uint16_t>(double /*noValue*/)
{
  return 0xffff;
}

template<typename T1, bool isUseNoValue>
struct ResultConverter
{
  static float ReciprocalScale(float, float) { return 1.0f; }

  template<typename T2>
  static T1 ConvertValueT(T2 value, float, float)
  {
    return ConvertValue<T1, T2>(value);
  }
};

static inline int QuantizeValueWithReciprocalScale(float value, float offset, float reciprocalScale, int buckets)
{
  float  bucket = (value - offset) * reciprocalScale;
  return (bucket <= 0)           ? 0 :
         (bucket >= buckets - 1) ? (buckets - 1) :
                                   (int)(bucket + 0.5f);
}

template<bool isUseNoValue>
struct ResultConverter<uint8_t, isUseNoValue>
{
  static float ReciprocalScale(float valueRangeMin, float valueRangeMax) { return (255 - isUseNoValue) / (valueRangeMax - valueRangeMin); }

  template<typename T2>
  static uint8_t ConvertValueT(T2 value, float offset, float reciprocalScale)
  {
    return (uint8_t)QuantizeValueWithReciprocalScale((float)value, offset, reciprocalScale, 256 - isUseNoValue);
  }
};

template<bool isUseNoValue>
struct ResultConverter<uint16_t, isUseNoValue>
{
  static float ReciprocalScale(float valueRangeMin, float valueRangeMax) { return (65535 - isUseNoValue) / (valueRangeMax - valueRangeMin); }

  template<typename T2>
  static uint16_t ConvertValueT(T2 value, float offset, float reciprocalScale)
  {
    return (uint16_t)QuantizeValueWithReciprocalScale((float)value, offset, reciprocalScale, 65536 - isUseNoValue);
  }
};

template<typename T, bool isUseNoValue>
struct QuantizedTypesToFloatConverter
{
  QuantizedTypesToFloatConverter()
  {}
  QuantizedTypesToFloatConverter(float /*integerScale*/, float /*integerOffset*/, bool /*isRangeScale*/)
  {}

  T ConvertValue(T value) const
  {
    return value;
  }
};

template<bool isUseNoValue>
struct QuantizedTypesToFloatConverter<uint8_t, isUseNoValue>
{
  float m_integerScale;
  float m_integerOffset;

  QuantizedTypesToFloatConverter() : m_integerScale(1), m_integerOffset(0)
  {}

  QuantizedTypesToFloatConverter(float integerScale, float integerOffset, bool isRangeScale)
    : m_integerScale(integerScale / (isRangeScale ? 255.0f - isUseNoValue : 1.0f))
    , m_integerOffset(integerOffset)
  {}

  float ConvertValue(uint8_t value) const
  {
    return m_integerOffset + m_integerScale * value;
  }
};

template<bool isUseNoValue>
struct QuantizedTypesToFloatConverter<uint16_t, isUseNoValue>
{
  float m_integerScale;
  float m_integerOffset;

public:
  QuantizedTypesToFloatConverter()
    : m_integerScale(1)
    , m_integerOffset(0)
  {}
  QuantizedTypesToFloatConverter(float integerScale, float integerOffset, bool isRangeScale)
    : m_integerScale(integerScale / (isRangeScale ? 65535.0f - isUseNoValue : 1.0f))
    , m_integerOffset(integerOffset)
  {}

  float ConvertValue(uint16_t value) const
  {
    return m_integerOffset + m_integerScale * value;
  }
};

template<typename T1, typename T2, bool isUseNoValue>
class QuantizingValueConverterWithNoValue
{
  float m_integerOffset;
  float m_reciprocalScale;

  T2 m_noValue;

  T1 m_replacementNoValue;
  QuantizedTypesToFloatConverter<T2, isUseNoValue> m_quantizedTypesToFloatConverter;

public:
  QuantizingValueConverterWithNoValue()
    : m_integerOffset(0)
    , m_reciprocalScale(0)
    , m_noValue(0)
    , m_replacementNoValue(0)
  {}

  QuantizingValueConverterWithNoValue(float valueRangeMin, float valueRangeMax, float integerScale, float integerOffset, float noValue, float replacementNoValue)
    : m_integerOffset(valueRangeMin)
    , m_reciprocalScale(ResultConverter<T1, isUseNoValue>::ReciprocalScale(valueRangeMin, valueRangeMax))
    , m_noValue(ConvertNoValue<T2>(noValue))
    , m_replacementNoValue(ConvertNoValue<T1>(replacementNoValue))
    , m_quantizedTypesToFloatConverter(integerScale, integerOffset, false)
  {}

  QuantizingValueConverterWithNoValue(float valueRangeMin, float valueRangeMax, float integerScale, float integerOffset, double noValue, double replacementNoValue)
    : m_integerOffset(valueRangeMin)
    , m_reciprocalScale(ResultConverter<T1, isUseNoValue>::ReciprocalScale(valueRangeMin, valueRangeMax))
    , m_noValue(ConvertNoValue<T2>(noValue))
    , m_replacementNoValue(ConvertNoValue<T1>(replacementNoValue))
    , m_quantizedTypesToFloatConverter(integerScale, integerOffset, false)
  {}

  QuantizingValueConverterWithNoValue(float valueRangeMin, float valueRangeMax, float integerScale, float integerOffset, float noValue, float replacementNoValue, bool isConvertWithValueRangeOnly)
    : m_integerOffset(isConvertWithValueRangeOnly ? valueRangeMin : integerOffset)
    , m_reciprocalScale(isConvertWithValueRangeOnly ? ResultConverter<T1, isUseNoValue>::ReciprocalScale(valueRangeMin, valueRangeMax) : 1.0f / integerScale)
    , m_noValue(ConvertNoValue<T2>(noValue))
    , m_replacementNoValue(ConvertNoValue<T1>(replacementNoValue))
    , m_quantizedTypesToFloatConverter(isConvertWithValueRangeOnly ? valueRangeMax - valueRangeMin : integerScale, isConvertWithValueRangeOnly ? valueRangeMin : integerOffset, isConvertWithValueRangeOnly)
  {}

  QuantizingValueConverterWithNoValue(float valueRangeMin, float valueRangeMax, float integerScale, float integerOffset, double noValue, double replacementNoValue, bool isConvertWithValueRangeOnly)
    : m_integerOffset(isConvertWithValueRangeOnly ? valueRangeMin : integerOffset)
    , m_reciprocalScale(isConvertWithValueRangeOnly ? ResultConverter<T1, isUseNoValue>::ReciprocalScale(valueRangeMin, valueRangeMax) : 1.0f / integerScale)
    , m_noValue(ConvertNoValue<T2>(noValue))
    , m_replacementNoValue(ConvertNoValue<T1>(replacementNoValue))
    , m_quantizedTypesToFloatConverter(isConvertWithValueRangeOnly ? valueRangeMax - valueRangeMin : integerScale, isConvertWithValueRangeOnly ? valueRangeMin : integerOffset, isConvertWithValueRangeOnly)
  {}

  T1 ConvertValue(T2 value) const
  {
    if(isUseNoValue && value == m_noValue)
    {
      return m_replacementNoValue;
    }
    else
    {
      return ResultConverter<T1, isUseNoValue>::ConvertValueT(m_quantizedTypesToFloatConverter.ConvertValue(value), m_integerOffset, m_reciprocalScale);
    }
  }
};
}
#endif //VALUECONVERSION_H
