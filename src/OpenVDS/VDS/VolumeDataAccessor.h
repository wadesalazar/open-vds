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
#include <OpenVDS/VolumeSampler.h>

namespace OpenVDS
{
template <typename INDEX> inline INDEX NdPosToVector(const int (&pos)[Dimensionality_Max]){ assert(false); }

template <> inline IntVector2 NdPosToVector<IntVector2>(const int (&pos)[Dimensionality_Max]) { return { pos[1], pos[0]}; }
template <> inline IntVector3 NdPosToVector<IntVector3>(const int (&pos)[Dimensionality_Max]) { return { pos[2], pos[1], pos[0]}; }
template <> inline IntVector4 NdPosToVector<IntVector4>(const int (&pos)[Dimensionality_Max]) { return { pos[3], pos[2], pos[1], pos[0]}; }
template <> inline FloatVector2 NdPosToVector<FloatVector2>(const int (&pos)[Dimensionality_Max]) { return { (float)pos[1], (float)pos[0]}; }
template <> inline FloatVector3 NdPosToVector<FloatVector3>(const int (&pos)[Dimensionality_Max]) { return { (float)pos[2], (float)pos[1], (float)pos[0]}; }
template <> inline FloatVector4 NdPosToVector<FloatVector4>(const int (&pos)[Dimensionality_Max]) { return { (float)pos[3], (float)pos[2], (float)pos[1], (float)pos[0]}; }

template <typename INDEX> inline void VectorToNDPos(INDEX const &index, int (&pos)[Dimensionality_Max]) { assert(false); }
template <> inline void VectorToNDPos(IntVector2 const &index, int (&pos)[Dimensionality_Max]) { pos[0] = index[0]; pos[1] = index[1]; }
template <> inline void VectorToNDPos(IntVector3 const &index, int (&pos)[Dimensionality_Max]) { pos[0] = index[0]; pos[1] = index[1]; pos[2] = index[2]; }
template <> inline void VectorToNDPos(IntVector4 const &index, int (&pos)[Dimensionality_Max]) { pos[0] = index[0]; pos[1] = index[1]; pos[2] = index[2]; pos[3] = index[3]; }
template <> inline void VectorToNDPos(FloatVector2 const &index, int (&pos)[Dimensionality_Max]) { pos[0] = (int)floorf(index[0]); pos[1] = (int)floorf(index[1]); }
template <> inline void VectorToNDPos(FloatVector3 const &index, int (&pos)[Dimensionality_Max]) { pos[0] = (int)floorf(index[0]); pos[1] = (int)floorf(index[1]); pos[2] = (int)floorf(index[2]); }
template <> inline void VectorToNDPos(FloatVector4 const &index, int (&pos)[Dimensionality_Max]) { pos[0] = (int)floorf(index[0]); pos[1] = (int)floorf(index[1]); pos[2] = (int)floorf(index[2]); pos[3] = (int)floorf(index[3]); }


class AccessorRegion : public IndexRegion<IntVector4>
{
public:
    AccessorRegion() {}
    AccessorRegion(IntVector4 min, IntVector4 max)
      : IndexRegion<IntVector4>(min, max) {}

    AccessorRegion Intersection(AccessorRegion const &region);
    bool           IsEmpty();
    bool           Contains(IntVector2 index);
    bool           Contains(IntVector3 index);
    bool           Contains(IntVector4 index);
    void           Expand(IntVector2 index);
    void           Expand(IntVector3 index);
    void           Expand(IntVector4 index);
};

inline bool AccessorRegion::Contains(IntVector4 index)
{
  return index[0] >= Min[0] && index[0] < Max[0] &&
         index[1] >= Min[1] && index[1] < Max[1] &&
         index[2] >= Min[2] && index[2] < Max[2] &&
         index[3] >= Min[3] && index[3] < Max[3];
}

inline bool AccessorRegion::Contains(IntVector3 index)
{
  return index[0] >= Min[1] && index[0] < Max[1] &&
         index[1] >= Min[2] && index[1] < Max[2] &&
         index[2] >= Min[3] && index[2] < Max[3];
}

inline bool AccessorRegion::Contains(IntVector2 index)
{
  return index[0] >= Min[2] && index[0] < Max[2] &&
         index[1] >= Min[3] && index[1] < Max[3];
}

inline void AccessorRegion::Expand(IntVector4 index)
{
  if(Min[0] > index[0])
  {
    Min[0] = index[0];
  }
  if(Max[0] <= index[0])
  {
    Max[0] = index[0] + 1;
  }
  if(Min[1] > index[1])
  {
    Min[1] = index[1];
  }
  if(Max[1] <= index[1])
  {
    Max[1] = index[1] + 1;
  }
  if(Min[2] > index[2])
  {
    Min[2] = index[2];
  }
  if(Max[2] <= index[2])
  {
    Max[2] = index[2] + 1;
  }
  if(Min[3] > index[3])
  {
    Min[3] = index[3];
  }
  if(Max[3] <= index[3])
  {
    Max[3] = index[3] + 1;
  }
}

inline void AccessorRegion::Expand(IntVector3 index)
{
  if(Min[1] > index[0])
  {
    Min[1] = index[0];
  }
  if(Max[1] <= index[0])
  {
    Max[1] = index[0] + 1;
  }
  if(Min[2] > index[1])
  {
    Min[2] = index[1];
  }
  if(Max[2] <= index[1])
  {
    Max[2] = index[1] + 1;
  }
  if(Min[3] > index[2])
  {
    Min[3] = index[2];
  }
  if(Max[3] <= index[2])
  {
    Max[3] = index[2] + 1;
  }
}

inline void AccessorRegion::Expand(IntVector2 index)
{
  if(Min[2] > index[0])
  {
    Min[2] = index[0];
  }
  if(Max[2] <= index[0])
  {
    Max[2] = index[0] + 1;
  }
  if(Min[3] > index[1])
  {
    Min[3] = index[1];
  }
  if(Max[3] <= index[1])
  {
    Max[3] = index[1] + 1;
  }
}

inline bool AccessorRegion::IsEmpty()
{
  return Max[0] <= Min[0] ||
         Max[1] <= Min[1] ||
         Max[2] <= Min[2] ||
         Max[3] <= Min[3];
}

inline AccessorRegion AccessorRegion::Intersection(AccessorRegion const &region)
{
  IntVector4 minIntersection = { Min[0] >= region.Min[0] ? Min[0] : region.Min[0],
                                 Min[1] >= region.Min[1] ? Min[1] : region.Min[1],
                                 Min[2] >= region.Min[2] ? Min[2] : region.Min[2],
                                 Min[3] >= region.Min[3] ? Min[3] : region.Min[3]};

  IntVector4 maxIntersection = { Max[0] <= region.Max[0] ? Max[0] : region.Max[0],
                                 Max[1] <= region.Max[1] ? Max[1] : region.Max[1],
                                 Max[2] <= region.Max[2] ? Max[2] : region.Max[2],
                                 Max[3] <= region.Max[3] ? Max[3] : region.Max[3]};

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

  virtual void  ReadPageAtPosition(IntVector4 index, bool enableWriting);
  void          MakeCurrentPageWritable();
  void          UpdateWrittenRegion();
public:
  VolumeDataLayout const *GetLayout();

  void          Commit();
  void          Cancel();

  virtual IVolumeDataAccessor *Clone(VolumeDataPageAccessor &volumeDataPageAccessor) const = 0;

public:
  VolumeDataPageAccessor *GetVolumeDataPageAccessor() const { return m_volumeDataPageAccessor; }

  VolumeDataAccessorBase(VolumeDataPageAccessor &VolumeDataPageAccessor);
  virtual ~VolumeDataAccessorBase();
};


template<typename T>
class RawVolumeDataAccessor : public VolumeDataAccessorBase
{
protected:
  T             GetValue(IntVector4 index);
  InterpolatedRealType<T> GetValue(FloatVector4 index);
  void          SetValue(IntVector4 index, T value);

  T             GetValue(IntVector3 index);
  void          SetValue(IntVector3 index, T value);

  T             GetValue(IntVector2 index);
  void          SetValue(IntVector2 index, T value);

  RawVolumeDataAccessor(VolumeDataPageAccessor &volumeDataPageAccessor)
    : VolumeDataAccessorBase(volumeDataPageAccessor)
  {}
};

template<typename INDEX, typename T1, typename T2, bool useNoValue>
class ConvertingVolumeDataAccessor : public RawVolumeDataAccessor<T2>, public IVolumeDataReadWriteAccessor<INDEX, T1>
{
  using RawVolumeDataAccessor<T2>::m_min;
  using RawVolumeDataAccessor<T2>::m_max;
  using RawVolumeDataAccessor<T2>::m_validRegion;
  using RawVolumeDataAccessor<T2>::m_buffer;
  using RawVolumeDataAccessor<T2>::m_pitch;
  using RawVolumeDataAccessor<T2>::m_volumeDataPageAccessor;

  QuantizingValueConverterWithNoValue<T1, T2, useNoValue> m_readValueConverter;

  QuantizingValueConverterWithNoValue<T2, T1, useNoValue> m_writeValueConverter;

  float m_replacementNoValue;

  void GetCurrentRegionMinMax(IntVector2 &min, IntVector2 &max)
  {
    min = {m_validRegion.Min[2], m_validRegion.Min[3]};
    max = {m_validRegion.Max[2], m_validRegion.Max[3]};
  }

  void GetCurrentRegionMinMax(IntVector3 &min, IntVector3 &max)
  {
    min = {m_validRegion.Min[1], m_validRegion.Min[2], m_validRegion.Min[3]};
    max = {m_validRegion.Max[1], m_validRegion.Max[2], m_validRegion.Max[3]};
  }

  void GetCurrentRegionMinMax(IntVector4 &min, IntVector4 &max)
  {
    min = {m_validRegion.Min[0], m_validRegion.Min[1], m_validRegion.Min[2], m_validRegion.Min[3]};
    max = {m_validRegion.Max[0], m_validRegion.Max[1], m_validRegion.Max[2], m_validRegion.Max[3]};
  }

public:
  ConvertingVolumeDataAccessor(VolumeDataPageAccessor &volumeDataPageAccessor, float replacementNoValue)
    : RawVolumeDataAccessor<T2>(volumeDataPageAccessor)
  {
    VolumeDataChannelDescriptor const &channelDescriptor = m_volumeDataPageAccessor->GetChannelDescriptor();

    bool isConvertWithValueRange = true;

    if (channelDescriptor.GetFormat() == VolumeDataChannelDescriptor::Format_U8 || channelDescriptor.GetFormat() == VolumeDataChannelDescriptor::Format_U16)
    {
      isConvertWithValueRange = false;
    }

    float valueRangeMin = channelDescriptor.GetValueRangeMin();
    float valueRangeMax = channelDescriptor.GetValueRangeMax();
    float integerScale  = channelDescriptor.GetIntegerScale();
    float integerOffset = channelDescriptor.GetIntegerOffset();
    float noValue       = channelDescriptor.GetNoValue();

    m_readValueConverter  = QuantizingValueConverterWithNoValue<T1, T2, useNoValue>(valueRangeMin, valueRangeMax, integerScale, integerOffset, noValue, replacementNoValue, isConvertWithValueRange);
    m_writeValueConverter = QuantizingValueConverterWithNoValue<T2, T1, useNoValue>(valueRangeMin, valueRangeMax, integerScale, integerOffset, replacementNoValue, noValue, isConvertWithValueRange);
    m_replacementNoValue  = replacementNoValue;
  }

  int64_t RegionCount() override
  {
    return m_volumeDataPageAccessor->GetChunkCount();
  }

  IndexRegion<INDEX> Region(int64_t region) override
  {
    int32_t minExcludingMargin[Dimensionality_Max];
    int32_t maxExcludingMargin[Dimensionality_Max];

    m_volumeDataPageAccessor->GetChunkMinMaxExcludingMargin(region, minExcludingMargin, maxExcludingMargin);

    return IndexRegion<INDEX>(NdPosToVector<INDEX>(minExcludingMargin), NdPosToVector<INDEX>(maxExcludingMargin));
  }

  int64_t RegionFromIndex(INDEX index) override
  {
    int position[Dimensionality_Max];
    VectorToNDPos(index, position);
    return m_volumeDataPageAccessor->GetChunkIndex(position);
  }

  IndexRegion<INDEX> CurrentRegion() override
  {
    INDEX min, max;

    GetCurrentRegionMinMax(min, max);
    return IndexRegion<INDEX>(min, max);
  }

  T1 GetValue(INDEX index) override { return m_readValueConverter.ConvertValue(RawVolumeDataAccessor<T2>::GetValue(index)); }

  void SetValue(INDEX index, T1 value) override { return RawVolumeDataAccessor<T2>::SetValue(index, m_writeValueConverter.ConvertValue(value)); }

  void Commit() override { return RawVolumeDataAccessor<T2>::Commit(); }
  void Cancel() override { return RawVolumeDataAccessor<T2>::Cancel(); }

  VolumeDataAccessManagerImpl &GetManager() override { return *m_volumeDataPageAccessor->GetManager(); }

  VolumeDataLayout const *GetLayout() override { return VolumeDataAccessorBase::GetLayout(); }

  IVolumeDataAccessor *Clone(VolumeDataPageAccessor &volumeDataPageAccessor) const override { volumeDataPageAccessor.AddReference(); return new ConvertingVolumeDataAccessor(volumeDataPageAccessor, m_replacementNoValue); }
};

template<typename INDEX, typename T1, typename T2, bool useNoValue, int interpolationMethod>
class InterpolatingVolumeDataAccessor : public RawVolumeDataAccessor<T2>, public IVolumeDataReadAccessor<INDEX, T1>
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

  VolumeSampler<T2, (InterpolationMethod)interpolationMethod, useNoValue> m_volumeSampler;

  float m_replacementNoValue;

  void GetCurrentRegionMinMax(FloatVector2 &min, FloatVector2 &max)
  {
    min = {(float)m_validRegion.Min[2], (float)m_validRegion.Min[3]};
    max = {(float)m_validRegion.Max[2], (float)m_validRegion.Max[3]};
  }

  void GetCurrentRegionMinMax(FloatVector3 &min, FloatVector3 &max)
  {
    min = {(float)m_validRegion.Min[1], (float)m_validRegion.Min[2], (float)m_validRegion.Min[3]};
    max = {(float)m_validRegion.Max[1], (float)m_validRegion.Max[2], (float)m_validRegion.Max[3]};
  }

  void GetCurrentRegionMinMax(FloatVector4 &min, FloatVector4 &max)
  {
    min = {(float)m_validRegion.Min[0], (float)m_validRegion.Min[1], (float)m_validRegion.Min[2], (float)m_validRegion.Min[3]};
    max = {(float)m_validRegion.Max[0], (float)m_validRegion.Max[1], (float)m_validRegion.Max[2], (float)m_validRegion.Max[3]};
  }

  void ReadPageAtPosition(IntVector4 index, bool enableWriting) override
  {
    VolumeDataAccessorBase::ReadPageAtPosition(index, enableWriting);

   int32_t size[DataBlock::Dimensionality_Max] = { m_max[3] - m_min[3],  m_max[2] - m_min[2], m_max[1] - m_min[1],  m_max[0] - m_min[0] };
   int32_t pitch[DataBlock::Dimensionality_Max] = { m_pitch[3], m_pitch[2], m_pitch[1], m_pitch[0] };

    m_volumeSampler = VolumeSampler<T2, (InterpolationMethod)interpolationMethod, useNoValue>(size, pitch, m_valueRangeMin, m_valueRangeMax, m_integerScale, m_integerOffset, m_noValue, m_replacementNoValue);
  }

public:
  InterpolatingVolumeDataAccessor(VolumeDataPageAccessor &volumeDataPageAccessor, float replacementNoValue)
    : RawVolumeDataAccessor<T2>(volumeDataPageAccessor)
  {
    VolumeDataChannelDescriptor const &channelDescriptor = this->m_volumeDataPageAccessor->GetChannelDescriptor();

    m_valueRangeMin = channelDescriptor.GetValueRangeMin();
    m_valueRangeMax = channelDescriptor.GetValueRangeMax();
    m_noValue       = channelDescriptor.GetNoValue();
    m_integerScale  = channelDescriptor.GetIntegerScale();
    m_integerOffset = channelDescriptor.GetIntegerOffset();

    m_replacementNoValue  = replacementNoValue;
  }

  int64_t RegionCount() override
  {
    return m_volumeDataPageAccessor->GetChunkCount();
  }

  IndexRegion<INDEX>
  Region(int64_t region) override
  {
    int32_t minExcludingMargin[Dimensionality_Max];
    int32_t maxExcludingMargin[Dimensionality_Max];

    m_volumeDataPageAccessor->GetChunkMinMaxExcludingMargin(region, minExcludingMargin, maxExcludingMargin);

    return IndexRegion<INDEX>(NdPosToVector<INDEX>(minExcludingMargin), NdPosToVector<INDEX>(maxExcludingMargin));
  }

  int64_t RegionFromIndex(INDEX index) override
  {
    int position[Dimensionality_Max];
    VectorToNDPos(index, position);
    return m_volumeDataPageAccessor->GetChunkIndex(position);
  }

  IndexRegion<INDEX> CurrentRegion() override
  {
    INDEX min;
    INDEX max;

    GetCurrentRegionMinMax(min, max);
    return IndexRegion<INDEX>(min, max);
  }

  T1 GetValue_t(FloatVector2 pos)
  {
    IntVector2 index = {(int)floorf(pos[0]), (int)floorf(pos[1])};

    if(!m_validRegion.Contains(index))
    {
      ReadPageAtPosition({m_validRegion.Min[0], m_validRegion.Min[1], index[0], index[1]}, false);
      if(!m_buffer)
      {
        return 0;
      }
    }

    return ConvertValue<T1>(m_volumeSampler.Sample2D((T2 *)m_buffer, {pos[1] - m_min[3], pos[0] - m_min[2]}));
  }

  T1 GetValue_t(FloatVector3 pos)
  {
    IntVector3 index = {(int)floorf(pos[0]),
                     (int)floorf(pos[1]),
                     (int)floorf(pos[2])};

    if(!m_validRegion.Contains(index))
    {
      ReadPageAtPosition({m_validRegion.Min[0], index[0], index[1], index[2]}, false);
      if(!m_buffer)
      {
        return 0;
      }
    }

    // TODO: This should really use 2D sampling for 2D dimensiongroups
    return ConvertValue<T1>(m_volumeSampler.Sample3D((T2 *)m_buffer, {pos[2] - m_min[3], pos[1] - m_min[2], pos[0] - m_min[1]}));
  }

  T1 GetValue_t(FloatVector4 pos)
  {
    IntVector4 index = {(int)floorf(pos[0]),
                     (int)floorf(pos[1]),
                     (int)floorf(pos[2]),
                     (int)floorf(pos[3])};

    if(!m_validRegion.Contains(index))
    {
      ReadPageAtPosition({index[0], index[1], index[2], index[3]}, false);
      if(!m_buffer)
      {
        return 0;
      }
    }

    // TODO: This doesn't work with dimensiongroup 0/1/3
    return ConvertValue<T1>(m_volumeSampler.Sample3D((T2 *)m_buffer, {pos[3] - m_min[3], pos[2] - m_min[2], pos[1] - m_min[1]}));
  }

  T1 GetValue(INDEX pos) override
  {
      return GetValue_t(pos);
  }

  VolumeDataAccessManagerImpl &GetManager() override { return *m_volumeDataPageAccessor->GetManager(); }

  VolumeDataLayout const *GetLayout() override { return VolumeDataAccessorBase::GetLayout(); }

  IVolumeDataAccessor *Clone(VolumeDataPageAccessor &volumeDataPageAccessor) const override { volumeDataPageAccessor.AddReference(); return new InterpolatingVolumeDataAccessor(volumeDataPageAccessor, m_replacementNoValue); }
};


  template <typename T>
  inline T ReadBuffer(const void *buffer, int index)
  {
    return static_cast<const T *>(buffer)[index];
  }

  template <typename T>
  inline void WriteBuffer(void *buffer, int index, T value)
  {
    static_cast<T *>(buffer)[index] = value;
  }

  template <>
  inline bool ReadBuffer<bool>(const void *buffer, int index)
  {
    return (static_cast<const unsigned char *>(buffer)[index / 8] & (1 << (index % 8))) != 0;
  }

  template <>
  inline void WriteBuffer<bool>(void *buffer, int index, bool value)
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
T RawVolumeDataAccessor<T>::GetValue(IntVector4 index)
{
  if(!m_validRegion.Contains(index))
  {
    ReadPageAtPosition({index[0], index[1], index[2], index[3]}, false);
    if(!m_buffer)
    {
      return 0;
    }
  }

  return ReadBuffer<T>(m_buffer, (index[0] - m_min[0]) * m_pitch[0] +
                                 (index[1] - m_min[1]) * m_pitch[1] +
                                 (index[2] - m_min[2]) * m_pitch[2] +
                                 (index[3] - m_min[3]) * m_pitch[3]);
}

//-----------------------------------------------------------------------------

template <typename T>
T RawVolumeDataAccessor<T>::GetValue(IntVector3 index)
{
  if(!m_validRegion.Contains(index))
  {
    ReadPageAtPosition({m_validRegion.Min[0], index[0], index[1], index[2]}, false);
    if(!m_buffer)
    {
      return 0;
    }
  }

  return ReadBuffer<T>(m_buffer, (index[0] - m_min[1]) * m_pitch[1] +
                                 (index[1] - m_min[2]) * m_pitch[2] +
                                 (index[2] - m_min[3]) * m_pitch[3]);
}

//-----------------------------------------------------------------------------

template <typename T>
T RawVolumeDataAccessor<T>::GetValue(IntVector2 index)
{
  if(!m_validRegion.Contains(index))
  {
    ReadPageAtPosition({m_validRegion.Min[0], m_validRegion.Min[1], index[0], index[1]}, false);
    if(!m_buffer)
    {
      return 0;
    }
  }

  return ReadBuffer<T>(m_buffer, (index[0] - m_min[2]) * m_pitch[2] +
                                 (index[1] - m_min[3]) * m_pitch[3]);
}

//-----------------------------------------------------------------------------

template <typename T>
void RawVolumeDataAccessor<T>::SetValue(IntVector4 index, T value)
{
  if(!m_writtenRegion.Contains(index))
  {
    if(!m_validRegion.Contains(index))
    {
      ReadPageAtPosition({index[0], index[1], index[2], index[3]}, true);
      if(!m_buffer)
      {
        return;
      }
    }
    else if(!m_writable)
    {
      MakeCurrentPageWritable();
    }
    if(m_writtenRegion.Max[0] == 0) // Has not been written to before (but can be locked because it has had margins copied into it)
    {
      m_writtenRegion = AccessorRegion({index[0], index[1], index[2], index[3]}, {index[0] + 1, index[1] + 1, index[2] + 1, index[3] + 1});
    }
    else
    {
      m_writtenRegion.Expand(index);
    }
  }

  WriteBuffer<T>(m_buffer, (index[0] - m_min[0]) * m_pitch[0] +
                           (index[1] - m_min[1]) * m_pitch[1] +
                           (index[2] - m_min[2]) * m_pitch[2] +
                           (index[3] - m_min[3]) * m_pitch[3], value);
}

//-----------------------------------------------------------------------------

template <typename T>
void RawVolumeDataAccessor<T>::SetValue(IntVector3 index, T value)
{
  if(!m_writtenRegion.Contains(index))
  {
    if(!m_validRegion.Contains(index))
    {
      ReadPageAtPosition({m_validRegion.Min[0], index[0], index[1], index[2]}, true);
      if(!m_buffer)
      {
        return;
      }
    }
    else if(!m_writable)
    {
      MakeCurrentPageWritable();
    }
    if(m_writtenRegion.Max[0] == 0) // Has not been written to before (but can be locked because it has had margins copied into it)
    {
      m_writtenRegion = AccessorRegion({m_validRegion.Min[0], index[0], index[1], index[2]}, {m_validRegion.Min[0] + 1, index[0] + 1, index[1] + 1, index[2] + 1});
    }
    else
    {
      m_writtenRegion.Expand(index);
    }
  }

  WriteBuffer<T>(m_buffer, (index[0] - m_min[1]) * m_pitch[1] +
                           (index[1] - m_min[2]) * m_pitch[2] +
                           (index[2] - m_min[3]) * m_pitch[3], value);
}

//-----------------------------------------------------------------------------

template <typename T>
void RawVolumeDataAccessor<T>::SetValue(IntVector2 index, T value)
{
  if(!m_writtenRegion.Contains(index))
  {
    if(!m_validRegion.Contains(index))
    {
      ReadPageAtPosition({m_validRegion.Min[0], m_validRegion.Min[1], index[0], index[1]}, true);
      if(!m_buffer)
      {
        return;
      }
    }
    else if(!m_writable)
    {
      MakeCurrentPageWritable();
    }
    if(m_writtenRegion.Max[0] == 0) // Has not been written to before (but can be locked because it has had margins copied into it)
    {
      m_writtenRegion = AccessorRegion({m_validRegion.Min[0], m_validRegion.Min[1], index[0], index[1]}, {m_validRegion.Min[0] + 1, m_validRegion.Min[1] + 1, index[0] + 1, index[1] + 1});
    }
    else
    {
      m_writtenRegion.Expand(index);
    }
  }

  WriteBuffer<T>(m_buffer, (index[0] - m_min[2]) * m_pitch[2] +
                           (index[1] - m_min[3]) * m_pitch[3], value);
}
}

#endif
