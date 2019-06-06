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

#ifndef VOLUMEDATACHANNELDESCRIPTOR_H
#define VOLUMEDATACHANNELDESCRIPTOR_H

#include <string>
#include <Math/Range.h>

#include "VolumeData.h"

namespace OpenVDS
{
/// \class VolumeDataChannelDescriptor
/// \brief Describes a channel of a VDS
class VolumeDataChannelDescriptor
{
public:
  /// Flags for this channel
  enum Flags
  {
    Default = 0,
    DiscreteData = (1 << 0),                                      ///< This channel contains discrete data
    NoLossyCompression = (1 << 1),                                ///< Do not allow lossy compression on this channel
    NotRenderable = (1 << 2),                                     ///< This channel is not renderable
    NoLossyCompressionUseZip = NoLossyCompression | (1 << 3),     ///< Use Zip when compressing this channel
  };
  
  enum Format
  {
    FormatAny = -1, ///< The DataBlock can be in any format
    Format1Bit,     ///< data is in packed 1-bit format
    FormatU8,       ///< data is in unsigned 8 bit
    FormatU16,      ///< data is in unsigned 16 bit
    FormatR32,      ///< data is in 32 bit float
    FormatU32,      ///< data is in unsigned 32 bit
    FormatR64,      ///< data is in 64 bit double
    FormatU64       ///< data is in unsigned 64 bit
  };

  enum Components
  {
    Components_1 = 1,
    Components_2 = 2,
    Components_4 = 4,
    ComponentsMax = Components_4
  };


private:
  Format format;
  Components components;
  const char *pName;
  const char *pUnit;
  Range<float> valueRange;

  VolumeDataMapping mapping;
  int         mappedValueCount;

  Flags       flags;

  bool        useNoValue;
  float       noValue;

  float       integerScale;
  float       integerOffset;

public:
  VolumeDataChannelDescriptor()
    : format(FormatAny), components(Components_1), pName(nullptr), pUnit(nullptr), valueRange({0.0f,0.0f}), mapping(VolumeDataMapping::Direct), mappedValueCount(1), flags(Default), useNoValue(false), noValue(0.0f), integerScale(1.0f), integerOffset(0.0f) {}

  /// The minimum constructor for a VolumeDataChannelDescriptor. This will use direct mapping, default flags, and no No Value
  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax)
    : format(format), components(components), pName(pName), pUnit(pUnit), valueRange({valueRangeMin, valueRangeMax}), mapping(VolumeDataMapping::Direct), mappedValueCount(1), flags(Default), useNoValue(false), noValue(0.0f), integerScale(1.0f), integerOffset(0.0f) {}

  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param mapping the mapping for this channel
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, enum VolumeDataMapping mapping)
    : format(format), components(components), pName(pName), pUnit(pUnit), valueRange({valueRangeMin, valueRangeMax}), mapping(mapping), mappedValueCount(1), flags(Default), useNoValue(false), noValue(0.0f), integerScale(1.0f), integerOffset(0.0f) {}

  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param flags the flags for this channel
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, enum Flags flags)
    : format(format), components(components), pName(pName), pUnit(pUnit), valueRange({valueRangeMin, valueRangeMax}), mapping(VolumeDataMapping::Direct), mappedValueCount(1), flags(flags), useNoValue(false), noValue(0.0f), integerScale(1.0f), integerOffset(0.0f) {}

  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param mapping the mapping for this channel
  /// \param flags the flags for this channel
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, enum VolumeDataMapping mapping, enum Flags flags)
    : format(format), components(components), pName(pName), pUnit(pUnit), valueRange({valueRangeMin, valueRangeMax}), mapping(mapping), mappedValueCount(1), flags(flags), useNoValue(false), noValue(0.0f), integerScale(1.0f), integerOffset(0.0f) {}

  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param mapping the mapping for this channel
  /// \param mappedValueCount When using per trace mapping, the number of values to store per trace
  /// \param flags the flags for this channel
  /// \param integerScale the scale to use for integer types
  /// \param integerOffset the offset to use for integer types
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, enum VolumeDataMapping mapping, int mappedValueCount, enum Flags flags, float integerScale, float integerOffset)
    : format(format), components(components), pName(pName), pUnit(pUnit), valueRange({valueRangeMin, valueRangeMax}), mapping(mapping), mappedValueCount(mappedValueCount), flags(flags), useNoValue(false), noValue(0.0f), integerScale(integerScale), integerOffset(integerOffset) {}

  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param noValue the No Value for this channel
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, float noValue)
    : format(format), components(components), pName(pName), pUnit(pUnit), valueRange({valueRangeMin, valueRangeMax}), mapping(VolumeDataMapping::Direct), mappedValueCount(1), flags(Default), useNoValue(true), noValue(noValue), integerScale(1.0f), integerOffset(0.0f) {}

  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param noValue the No Value for this channel
  /// \param mapping the mapping for this channel
  /// \param flags the flags for this channel
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, float noValue, enum VolumeDataMapping mapping, enum Flags flags)
    : format(format), components(components), pName(pName), pUnit(pUnit), valueRange({valueRangeMin, valueRangeMax}), mapping(mapping), mappedValueCount(1), flags(flags), useNoValue(true), noValue(noValue), integerScale(1.0f), integerOffset(0.0f) {}
  
  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param mapping the mapping for this channel
  /// \param mappedValueCount When using per trace mapping, the number of values to store per trace
  /// \param flags the flags for this channel
  /// \param noValue the No Value for this channel
  /// \param integerScale the scale to use for integer types
  /// \param integerOffset the offset to use for integer types
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, enum VolumeDataMapping mapping, int mappedValueCount, enum Flags flags, float noValue, float integerScale, float integerOffset)
    : format(format), components(components), pName(pName), pUnit(pUnit), valueRange({valueRangeMin, valueRangeMax}), mapping(mapping), mappedValueCount(mappedValueCount), flags(flags), useNoValue(true), noValue(noValue), integerScale(integerScale), integerOffset(integerOffset) {}

  Format      GetFormat()                       const { return format; }
  Components  GetComponents()                   const { return components; }
  bool        IsDiscrete()                      const { return (flags & DiscreteData) || format == Format1Bit; }
  bool        IsRenderable()                    const { return !(flags & NotRenderable); }
  bool        IsAllowLossyCompression()         const { return !(flags & NoLossyCompression) && !IsDiscrete(); }
  bool        IsUseZipForLosslessCompression()  const { return (flags & NoLossyCompressionUseZip) == NoLossyCompressionUseZip; }
  const char *GetName()                   const { return pName; }
  const char *GetUnit()                   const { return pUnit; }
  const Range<float> &GetValueRange()           const { return valueRange; }
  float       GetValueRangeMin()                const { return valueRange.min; }
  float       GetValueRangeMax()                const { return valueRange.max; }

  VolumeDataMapping GetMapping()                      const { return mapping; }
  int         GetMappedValueCount()             const { return mappedValueCount; }

  bool        IsUseNoValue()                    const { return useNoValue; }
  float       GetNoValue()                      const { return noValue; }

  float       GetIntegerScale()                 const { return integerScale; }
  float       GetIntegerOffset()                const { return integerOffset; }

  /// Named constructor for a trace mapped channel
  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param mappedValueCount When using per trace mapping, the number of values to store per trace
  /// \param flags the flags for this channel
  /// \return a trace mapped descriptor
  static VolumeDataChannelDescriptor
    TraceMappedVolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, int mappedValueCount, enum Flags flags)
  {
    return VolumeDataChannelDescriptor(format, components, pName, pUnit, valueRangeMin, valueRangeMax, VolumeDataMapping::PerTrace, mappedValueCount, flags, 1.0f, 0.0f);
  }

  /// Named constructor for a trace mapped channel
  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param mappedValueCount When using per trace mapping, the number of values to store per trace
  /// \param flags the flags for this channel
  /// \param noValue the No Value for this channel
  /// \return a trace mapped descriptor
  static VolumeDataChannelDescriptor
    TraceMappedVolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, int mappedValueCount, enum Flags flags, float noValue)
  {
    return VolumeDataChannelDescriptor(format, components, pName, pUnit, valueRangeMin, valueRangeMax, VolumeDataMapping::PerTrace, mappedValueCount, flags, noValue, 1.0f, 0.0f);
  }
};
}
#endif //VOLUMEDATACHANNELDESCRIPTOR_H
