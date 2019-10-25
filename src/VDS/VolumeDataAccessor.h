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

#ifndef VOLUMEDATAACCESSOR_H
#define VOLUMEDATAACCESSOR_H

#include <OpenVDS/Vector.h>

#include "VolumeDataPageImpl.h"
#include "VolumeDataPageAccessorImpl.h"
#include "VolumeSampler.h"


namespace OpenVDS
{
template <typename INDEX> INDEX ndPosToVector(const int (&pos)[Dimensionality_Max]){ assert(false); } ;

template <> IntVector2 ndPosToVector<IntVector2>(const int (&pos)[Dimensionality_Max]) { return { pos[1], pos[0]}; }
template <> IntVector3 ndPosToVector<IntVector3>(const int (&pos)[Dimensionality_Max]) { return { pos[2], pos[1], pos[0]}; }
template <> IntVector4 ndPosToVector<IntVector4>(const int (&pos)[Dimensionality_Max]) { return { pos[3], pos[2], pos[1], pos[0]}; }
template <> FloatVector2 ndPosToVector<FloatVector2>(const int (&pos)[Dimensionality_Max]) { return { (float)pos[1], (float)pos[0]}; }
template <> FloatVector3 ndPosToVector<FloatVector3>(const int (&pos)[Dimensionality_Max]) { return { (float)pos[2], (float)pos[1], (float)pos[0]}; }
template <> FloatVector4 ndPosToVector<FloatVector4>(const int (&pos)[Dimensionality_Max]) { return { (float)pos[3], (float)pos[2], (float)pos[1], (float)pos[0]}; }

template <typename INDEX> void vectorToNDPos(INDEX const &index, int (&pos)[Dimensionality_Max]) { assert(false); };
template <> void vectorToNDPos(IntVector2 const &index, int (&pos)[Dimensionality_Max]) { pos[0] = index[0]; pos[1] = index[1]; }
template <> void vectorToNDPos(IntVector3 const &index, int (&pos)[Dimensionality_Max]) { pos[0] = index[0]; pos[1] = index[1]; pos[2] = index[2]; }
template <> void vectorToNDPos(IntVector4 const &index, int (&pos)[Dimensionality_Max]) { pos[0] = index[0]; pos[1] = index[1]; pos[2] = index[2]; pos[3] = index[3]; }
template <> void vectorToNDPos(FloatVector2 const &index, int (&pos)[Dimensionality_Max]) { pos[0] = (int)floorf(index[0]); pos[1] = (int)floorf(index[1]); }
template <> void vectorToNDPos(FloatVector3 const &index, int (&pos)[Dimensionality_Max]) { pos[0] = (int)floorf(index[0]); pos[1] = (int)floorf(index[1]); pos[2] = (int)floorf(index[2]); }
template <> void vectorToNDPos(FloatVector4 const &index, int (&pos)[Dimensionality_Max]) { pos[0] = (int)floorf(index[0]); pos[1] = (int)floorf(index[1]); pos[2] = (int)floorf(index[2]); pos[3] = (int)floorf(index[3]); }


class AccessorRegion : public IndexRegion<IntVector4>
{
public:
    AccessorRegion() {}
    AccessorRegion(IntVector4 min, IntVector4 max)
      : IndexRegion<IntVector4>(min, max) {}

    AccessorRegion intersection(AccessorRegion const &region);
    bool           isEmpty();
    bool           contains(IntVector2 index);
    bool           contains(IntVector3 index);
    bool           contains(IntVector4 index);
    void           expand(IntVector2 index);
    void           expand(IntVector3 index);
    void           expand(IntVector4 index);
};

bool AccessorRegion::contains(IntVector4 index)
{
  return index[0] >= min[0] && index[0] < max[0] &&
         index[1] >= min[1] && index[1] < max[1] &&
         index[2] >= min[2] && index[2] < max[2] &&
         index[3] >= min[3] && index[3] < max[3];
}

bool AccessorRegion::contains(IntVector3 index)
{
  return index[0] >= min[1] && index[0] < max[1] &&
         index[1] >= min[2] && index[1] < max[2] &&
         index[2] >= min[3] && index[2] < max[3];
}

bool AccessorRegion::contains(IntVector2 index)
{
  return index[0] >= min[2] && index[0] < max[2] &&
         index[1] >= min[3] && index[1] < max[3];
}

void AccessorRegion::expand(IntVector4 index)
{
  if(min[0] > index[0])
  {
    min[0] = index[0];
  }
  if(max[0] <= index[0])
  {
    max[0] = index[0] + 1;
  }
  if(min[1] > index[1])
  {
    min[1] = index[1];
  }
  if(max[1] <= index[1])
  {
    max[1] = index[1] + 1;
  }
  if(min[2] > index[2])
  {
    min[2] = index[2];
  }
  if(max[2] <= index[2])
  {
    max[2] = index[2] + 1;
  }
  if(min[3] > index[3])
  {
    min[3] = index[3];
  }
  if(max[3] <= index[3])
  {
    max[3] = index[3] + 1;
  }
}

void AccessorRegion::expand(IntVector3 index)
{
  if(min[1] > index[0])
  {
    min[1] = index[0];
  }
  if(max[1] <= index[0])
  {
    max[1] = index[0] + 1;
  }
  if(min[2] > index[1])
  {
    min[2] = index[1];
  }
  if(max[2] <= index[1])
  {
    max[2] = index[1] + 1;
  }
  if(min[3] > index[2])
  {
    min[3] = index[2];
  }
  if(max[3] <= index[2])
  {
    max[3] = index[2] + 1;
  }
}

void AccessorRegion::expand(IntVector2 index)
{
  if(min[2] > index[0])
  {
    min[2] = index[0];
  }
  if(max[2] <= index[0])
  {
    max[2] = index[0] + 1;
  }
  if(min[3] > index[1])
  {
    min[3] = index[1];
  }
  if(max[3] <= index[1])
  {
    max[3] = index[1] + 1;
  }
}

bool AccessorRegion::isEmpty()
{
  return max[0] <= min[0] ||
         max[1] <= min[1] ||
         max[2] <= min[2] ||
         max[3] <= min[3];
}

AccessorRegion AccessorRegion::intersection(AccessorRegion const &region)
{
  IntVector4 minIntersection = { min[0] >= region.min[0] ? min[0] : region.min[0],
                                 min[1] >= region.min[1] ? min[1] : region.min[1],
                                 min[2] >= region.min[2] ? min[2] : region.min[2],
                                 min[3] >= region.min[3] ? min[3] : region.min[3]};

  IntVector4 maxIntersection = { max[0] <= region.max[0] ? max[0] : region.max[0],
                                 max[1] <= region.max[1] ? max[1] : region.max[1],
                                 max[2] <= region.max[2] ? max[2] : region.max[2],
                                 max[3] <= region.max[3] ? max[3] : region.max[3]};

  return AccessorRegion(minIntersection, maxIntersection);
}


class VolumeDataAccessorBase
{
protected:
  VolumeDataPageAccessorImpl *m_volumeDataPageAccessor;

  VolumeDataPage *m_currentPage;

  bool          m_canceled;

                // This is the location of the current data buffer (including render margins which don't contain valid data)
  IntVector4    m_min;
  IntVector4    m_max;

                // This is the region that is valid for indexing (i.e. m_buffer contains valid values in this region)
  AccessorRegion m_validRegion;

                // This is the written region which is used for invalidates and margin copies
  AccessorRegion m_writtenRegion;

  IntVector4    m_numSamples;

  bool          m_writable;

  void         *m_buffer;

  IntVector4    m_pitch;

  virtual void  readPageAtPosition(IntVector4 index, bool enableWriting);
  void          makeCurrentPageWritable();
  void          updateWrittenRegion();
public:
  VolumeDataLayout const *getLayout();

  void          commit();
  void          cancel();

  //virtual VolumeDataAccessor *clone(VolumeDataPageAccessor &volumeDataPageAccessor) = 0;

public:
  VolumeDataPageAccessor *volumeDataPageAccessor() { return m_volumeDataPageAccessor; }

  VolumeDataAccessorBase(VolumeDataPageAccessor &volumeDataPageAccessor);
  virtual ~VolumeDataAccessorBase();
};


template<typename T>
class RawVolumeDataAccessor : public VolumeDataAccessorBase
{
protected:
  T             getValue(IntVector4 index);
  InterpolatedRealType<T> getValue(FloatVector4 index);
  void          setValue(IntVector4 index, T value);

  T             getValue(IntVector3 index);
  void          setValue(IntVector3 index, T value);

  T             getValue(IntVector2 index);
  void          setValue(IntVector2 index, T value);

  RawVolumeDataAccessor(VolumeDataPageAccessor &volumeDataPageAccessor)
    : VolumeDataAccessorBase(volumeDataPageAccessor)
  {}
};

template<typename INDEX, typename T1, typename T2, bool isUseNoValue>
class ConvertingVolumeDataAccessor : public RawVolumeDataAccessor<T2>, public VolumeDataReadWriteAccessor<INDEX, T1>
{
  using RawVolumeDataAccessor<T2>::m_min;
  using RawVolumeDataAccessor<T2>::m_max;
  using RawVolumeDataAccessor<T2>::m_validRegion;
  using RawVolumeDataAccessor<T2>::m_buffer;
  using RawVolumeDataAccessor<T2>::m_pitch;
  using RawVolumeDataAccessor<T2>::m_volumeDataPageAccessor;

  QuantizingValueConverterWithNoValue<T1, T2, isUseNoValue> m_readValueConverter;

  QuantizingValueConverterWithNoValue<T2, T1, isUseNoValue> m_writeValueConverter;

  float m_replacementNoValue;

  void getCurrentRegionMinMax(IntVector2 &min, IntVector2 &max)
  {
    min = {m_validRegion.min[2], m_validRegion.min[3]};
    max = {m_validRegion.max[2], m_validRegion.max[3]};
  }

  void getCurrentRegionMinMax(IntVector3 &min, IntVector3 &max)
  {
    min = {m_validRegion.min[1], m_validRegion.min[2], m_validRegion.min[3]};
    max = {m_validRegion.max[1], m_validRegion.max[2], m_validRegion.max[3]};
  }

  void getCurrentRegionMinMax(IntVector4 &min, IntVector4 &max)
  {
    min = {m_validRegion.min[0], m_validRegion.min[1], m_validRegion.min[2], m_validRegion.min[3]};
    max = {m_validRegion.max[0], m_validRegion.max[1], m_validRegion.max[2], m_validRegion.max[3]};
  }

public:
  ConvertingVolumeDataAccessor(VolumeDataPageAccessor &volumeDataPageAccessor, float replacementNoValue)
    : RawVolumeDataAccessor<T2>(volumeDataPageAccessor)
  {
    VolumeDataChannelDescriptor const &channelDescriptor = m_volumeDataPageAccessor->getChannelDescriptor();

    bool isConvertWithValueRange = true;

    if (channelDescriptor.getFormat() == VolumeDataChannelDescriptor::Format_U8 || channelDescriptor.getFormat() == VolumeDataChannelDescriptor::Format_U16)
    {
      isConvertWithValueRange = false;
    }

    float valueRangeMin = channelDescriptor.getValueRangeMin();
    float valueRangeMax = channelDescriptor.getValueRangeMax();
    float integerScale  = channelDescriptor.getIntegerScale();
    float integerOffset = channelDescriptor.getIntegerOffset();
    float noValue       = channelDescriptor.getNoValue();

    m_readValueConverter  = QuantizingValueConverterWithNoValue<T1, T2, isUseNoValue>(valueRangeMin, valueRangeMax, integerScale, integerOffset, noValue, replacementNoValue, isConvertWithValueRange);
    m_writeValueConverter = QuantizingValueConverterWithNoValue<T2, T1, isUseNoValue>(valueRangeMin, valueRangeMax, integerScale, integerOffset, replacementNoValue, noValue, isConvertWithValueRange);
    m_replacementNoValue  = replacementNoValue;
  }

  int64_t regionCount() override
  {
    return m_volumeDataPageAccessor->getChunkCount();
  }

  IndexRegion<INDEX> region(int64_t region) override
  {
    int32_t minExcludingMargin[Dimensionality_Max];
    int32_t maxExcludingMargin[Dimensionality_Max];

    m_volumeDataPageAccessor->getChunkMinMaxExcludingMargin(region, minExcludingMargin, maxExcludingMargin);

    return IndexRegion<INDEX>(ndPosToVector<INDEX>(minExcludingMargin), ndPosToVector<INDEX>(maxExcludingMargin));
  }

  int64_t regionFromIndex(INDEX index) override
  {
    int position[Dimensionality_Max];
    vectorToNDPos(index, position);
    return m_volumeDataPageAccessor->getChunkIndex(position);
  }

  IndexRegion<INDEX> currentRegion() override
  {
    INDEX min, max;

    getCurrentRegionMinMax(min, max);
    return IndexRegion<INDEX>(min, max);
  }

  T1 getValue(INDEX index) override { return m_readValueConverter.convertValue(RawVolumeDataAccessor<T2>::getValue(index)); }

  void setValue(INDEX index, T1 value) override { return RawVolumeDataAccessor<T2>::setValue(index, m_writeValueConverter.convertValue(value)); }

  void commit() override { return RawVolumeDataAccessor<T2>::commit(); }
  void cancel() override { return RawVolumeDataAccessor<T2>::cancel(); }

  VolumeDataAccessManager &getManager() override { return *m_volumeDataPageAccessor->getManager(); }

  VolumeDataLayout const *getLayout() override { return VolumeDataAccessorBase::getLayout(); }

  VolumeDataAccessor *clone(VolumeDataPageAccessor &volumeDataPageAccessor) { volumeDataPageAccessor.addReference(); return new ConvertingVolumeDataAccessor(volumeDataPageAccessor, m_replacementNoValue); }
};

template<typename INDEX, typename T1, typename T2, bool isUseNoValue, InterpolationMethod interpolationMethod>
class InterpolatingVolumeDataAccessor : public RawVolumeDataAccessor<T2>, public VolumeDataReadAccessor<INDEX, T1>
{
  using RawVolumeDataAccessor<T2>::m_min;
  using RawVolumeDataAccessor<T2>::m_max;
  using RawVolumeDataAccessor<T2>::m_validRegion;
  using RawVolumeDataAccessor<T2>::m_buffer;
  using RawVolumeDataAccessor<T2>::m_pitch;
  using RawVolumeDataAccessor<T2>::m_volumeDataPageAccessor;

  float m_valueRangeMin;
  float m_valueRangeMax;

  float m_noValue;

  float m_integerScale;
  float m_integerOffset;

  VolumeSampler<T2, interpolationMethod, isUseNoValue> m_volumeSampler;

  float m_replacementNoValue;

  void getCurrentRegionMinMax(FloatVector2 &min, FloatVector2 &max)
  {
    min = {(float)m_validRegion.min[2], (float)m_validRegion.min[3]};
    max = {(float)m_validRegion.max[2], (float)m_validRegion.max[3]};
  }

  void getCurrentRegionMinMax(FloatVector3 &min, FloatVector3 &max)
  {
    min = {(float)m_validRegion.min[1], (float)m_validRegion.min[2], (float)m_validRegion.min[3]};
    max = {(float)m_validRegion.max[1], (float)m_validRegion.max[2], (float)m_validRegion.max[3]};
  }

  void getCurrentRegionMinMax(FloatVector4 &min, FloatVector4 &max)
  {
    min = {(float)m_validRegion.min[0], (float)m_validRegion.min[1], (float)m_validRegion.min[2], (float)m_validRegion.min[3]};
    max = {(float)m_validRegion.max[0], (float)m_validRegion.max[1], (float)m_validRegion.max[2], (float)m_validRegion.max[3]};
  }

  void readPageAtPosition(IntVector4 index, bool enableWriting) override
  {
    VolumeDataAccessorBase::readPageAtPosition(index, enableWriting);

   int32_t size[DataStoreDimensionality_Max] = { m_max[3] - m_min[3],  m_max[2] - m_min[2], m_max[1] - m_min[1],  m_max[0] - m_min[0] };
   int32_t pitch[DataStoreDimensionality_Max] = { m_pitch[3], m_pitch[2], m_pitch[1], m_pitch[0] };

    m_volumeSampler = VolumeSampler<T2, (InterpolationMethod)interpolationMethod, isUseNoValue>(size, pitch, m_valueRangeMin, m_valueRangeMax, m_integerScale, m_integerOffset, m_noValue, m_replacementNoValue);
  }

public:
  InterpolatingVolumeDataAccessor(VolumeDataPageAccessor &volumeDataPageAccessor, float replacementNoValue)
    : RawVolumeDataAccessor<T2>(volumeDataPageAccessor)
  {
    VolumeDataChannelDescriptor const &channelDescriptor = this->m_volumeDataPageAccessor->getChannelDescriptor();

    m_valueRangeMin = channelDescriptor.getValueRangeMin();
    m_valueRangeMax = channelDescriptor.getValueRangeMax();
    m_noValue       = channelDescriptor.getNoValue();
    m_integerScale  = channelDescriptor.getIntegerScale();
    m_integerOffset = channelDescriptor.getIntegerOffset();

    m_replacementNoValue  = replacementNoValue;
  }

  int64_t regionCount() override
  {
    return m_volumeDataPageAccessor->getChunkCount();
  }

  IndexRegion<INDEX>
  region(int64_t region) override
  {
    int32_t minExcludingMargin[Dimensionality_Max];
    int32_t maxExcludingMargin[Dimensionality_Max];

    m_volumeDataPageAccessor->getChunkMinMaxExcludingMargin(region, minExcludingMargin, maxExcludingMargin);

    return IndexRegion<INDEX>(ndPosToVector<INDEX>(minExcludingMargin), ndPosToVector<INDEX>(maxExcludingMargin));
  }

  int64_t regionFromIndex(INDEX index) override
  {
    int position[Dimensionality_Max];
    vectorToNDPos(index, position);
    return m_volumeDataPageAccessor->getChunkIndex(position);
  }

  IndexRegion<INDEX> currentRegion() override
  {
    INDEX min;
    INDEX max;

    getCurrentRegionMinMax(min, max);
    return IndexRegion<INDEX>(min, max);
  }

  T1 getValue_t(FloatVector2 pos)
  {
    IntVector2 index = {(int)floorf(pos[0]), (int)floorf(pos[1])};

    if(!m_validRegion.contains(index))
    {
      readPageAtPosition({m_validRegion.min[0], m_validRegion.min[1], index[0], index[1]}, false);
      if(!m_buffer)
      {
        return 0;
      }
    }

    return convertValue<T1>(m_volumeSampler.sample2D((T2 *)m_buffer, {pos[1] - m_min[3], pos[0] - m_min[2]}));
  }

  T1 getValue_t(FloatVector3 pos)
  {
    IntVector3 index = {(int)floorf(pos[0]),
                     (int)floorf(pos[1]),
                     (int)floorf(pos[2])};

    if(!m_validRegion.contains(index))
    {
      readPageAtPosition({m_validRegion.min[0], index[0], index[1], index[2]}, false);
      if(!m_buffer)
      {
        return 0;
      }
    }

    // TODO: This should really use 2D sampling for 2D dimensiongroups
    return convertValue<T1>(m_volumeSampler.sample3D((T2 *)m_buffer, {pos[2] - m_min[3], pos[1] - m_min[2], pos[0] - m_min[1]}));
  }

  T1 getValue_t(FloatVector4 pos)
  {
    IntVector4 index = {(int)floorf(pos[0]),
                     (int)floorf(pos[1]),
                     (int)floorf(pos[2]),
                     (int)floorf(pos[3])};

    if(!m_validRegion.contains(index))
    {
      readPageAtPosition({index[0], index[1], index[2], index[3]}, false);
      if(!m_buffer)
      {
        return 0;
      }
    }

    // TODO: This doesn't work with dimensiongroup 0/1/3
    return convertValue<T1>(m_volumeSampler.sample3D((T2 *)m_buffer, {pos[3] - m_min[3], pos[2] - m_min[2], pos[1] - m_min[1]}));
  }

  T1 getValue(INDEX pos) override
  {
      return getValue_t(pos);
  }

  VolumeDataAccessManager &getManager() override { return *m_volumeDataPageAccessor->getManager(); }

  VolumeDataLayout const *getLayout() override { return VolumeDataAccessorBase::getLayout(); }

  VolumeDataAccessor *clone(VolumeDataPageAccessor &volumeDataPageAccessor) { volumeDataPageAccessor.addReference(); return InterpolatingVolumeDataAccessor(volumeDataPageAccessor, m_replacementNoValue); }
};


  template <typename T>
  T readBuffer(const void *buffer, int index)
  {
    return static_cast<const T *>(buffer)[index];
  }

  template <typename T>
  void writeBuffer(void *buffer, int index, T value)
  {
    static_cast<T *>(buffer)[index] = value;
  }

  template <>
  bool readBuffer<bool>(const void *buffer, int index)
  {
    return (static_cast<const unsigned char *>(buffer)[index / 8] & (1 << (index % 8))) != 0;
  }

  template <>
  void writeBuffer<bool>(void *buffer, int index, bool value)
  {
    if(value)
    {
      static_cast<unsigned char *>(buffer)[index / 8] |= (1 << (index % 8));
    }
    else
    {
      static_cast<unsigned char *>(buffer)[index / 8] &= ~(1 << (index % 8));
    }
  }

//-----------------------------------------------------------------------------

template <typename T>
T RawVolumeDataAccessor<T>::getValue(IntVector4 index)
{
  if(!m_validRegion.contains(index))
  {
    readPageAtPosition({index[0], index[1], index[2], index[3]}, false);
    if(!m_buffer)
    {
      return 0;
    }
  }

  return readBuffer<T>(m_buffer, (index[0] - m_min[0]) * m_pitch[0] +
                                 (index[1] - m_min[1]) * m_pitch[1] +
                                 (index[2] - m_min[2]) * m_pitch[2] +
                                 (index[3] - m_min[3]) * m_pitch[3]);
}

//-----------------------------------------------------------------------------

template <typename T>
T RawVolumeDataAccessor<T>::getValue(IntVector3 index)
{
  if(!m_validRegion.contains(index))
  {
    readPageAtPosition({m_validRegion.min[0], index[0], index[1], index[2]}, false);
    if(!m_buffer)
    {
      return 0;
    }
  }

  return readBuffer<T>(m_buffer, (index[0] - m_min[1]) * m_pitch[1] +
                                 (index[1] - m_min[2]) * m_pitch[2] +
                                 (index[2] - m_min[3]) * m_pitch[3]);
}

//-----------------------------------------------------------------------------

template <typename T>
T RawVolumeDataAccessor<T>::getValue(IntVector2 index)
{
  if(!m_validRegion.contains(index))
  {
    readPageAtPosition({m_validRegion.min[0], m_validRegion.min[1], index[0], index[1]}, false);
    if(!m_buffer)
    {
      return 0;
    }
  }

  return readBuffer<T>(m_buffer, (index[0] - m_min[2]) * m_pitch[2] +
                                 (index[1] - m_min[3]) * m_pitch[3]);
}

//-----------------------------------------------------------------------------

template <typename T>
void RawVolumeDataAccessor<T>::setValue(IntVector4 index, T value)
{
  if(!m_writtenRegion.contains(index))
  {
    if(!m_validRegion.contains(index))
    {
      readPageAtPosition({index[0], index[1], index[2], index[3]}, true);
      if(!m_buffer)
      {
        return;
      }
    }
    else if(!m_writable)
    {
      makeCurrentPageWritable();
    }
    if(m_writtenRegion.max[0] == 0) // Has not been written to before (but can be locked because it has had margins copied into it)
    {
      m_writtenRegion = AccessorRegion({index[0], index[1], index[2], index[3]}, {index[0] + 1, index[1] + 1, index[2] + 1, index[3] + 1});
    }
    else
    {
      m_writtenRegion.expand(index);
    }
  }

  writeBuffer<T>(m_buffer, (index[0] - m_min[0]) * m_pitch[0] +
                           (index[1] - m_min[1]) * m_pitch[1] +
                           (index[2] - m_min[2]) * m_pitch[2] +
                           (index[3] - m_min[3]) * m_pitch[3], value);
}

//-----------------------------------------------------------------------------

template <typename T>
void RawVolumeDataAccessor<T>::setValue(IntVector3 index, T value)
{
  if(!m_writtenRegion.contains(index))
  {
    if(!m_validRegion.contains(index))
    {
      readPageAtPosition({m_validRegion.min[0], index[0], index[1], index[2]}, true);
      if(!m_buffer)
      {
        return;
      }
    }
    else if(!m_writable)
    {
      makeCurrentPageWritable();
    }
    if(m_writtenRegion.max[0] == 0) // Has not been written to before (but can be locked because it has had margins copied into it)
    {
      m_writtenRegion = AccessorRegion({m_validRegion.min[0], index[0], index[1], index[2]}, {m_validRegion.min[0] + 1, index[0] + 1, index[1] + 1, index[2] + 1});
    }
    else
    {
      m_writtenRegion.expand(index);
    }
  }

  writeBuffer<T>(m_buffer, (index[0] - m_min[1]) * m_pitch[1] +
                           (index[1] - m_min[2]) * m_pitch[2] +
                           (index[2] - m_min[3]) * m_pitch[3], value);
}

//-----------------------------------------------------------------------------

template <typename T>
void RawVolumeDataAccessor<T>::setValue(IntVector2 index, T value)
{
  if(!m_writtenRegion.contains(index))
  {
    if(!m_validRegion.contains(index))
    {
      readPageAtPosition({m_validRegion.min[0], m_validRegion.min[1], index[0], index[1]}, true);
      if(!m_buffer)
      {
        return;
      }
    }
    else if(!m_writable)
    {
      makeCurrentPageWritable();
    }
    if(m_writtenRegion.max[0] == 0) // Has not been written to before (but can be locked because it has had margins copied into it)
    {
      m_writtenRegion = AccessorRegion({m_validRegion.min[0], m_validRegion.min[1], index[0], index[1]}, {m_validRegion.min[0] + 1, m_validRegion.min[1] + 1, index[0] + 1, index[1] + 1});
    }
    else
    {
      m_writtenRegion.expand(index);
    }
  }

  writeBuffer<T>(m_buffer, (index[0] - m_min[2]) * m_pitch[2] +
                           (index[1] - m_min[3]) * m_pitch[3], value);
}
}

#endif
