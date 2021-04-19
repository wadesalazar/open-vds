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

#ifndef VOLUMEDATAACCESS_H
#define VOLUMEDATAACCESS_H

#include <OpenVDS/VolumeData.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/Vector.h>
#include <OpenVDS/Exceptions.h>

namespace OpenVDS {

enum class VDSProduceStatus
{
  Normal,
  Remapped,
  Unavailable
};

class VolumeDataPageAccessor;
class VolumeDataLayout;
struct VDS;

template <typename INDEX, typename T> class IVolumeDataReadAccessor;
template <typename INDEX, typename T> class IVolumeDataReadWriteAccessor;

template <typename T>
struct IndexRegion
{
    T Min;
    T Max;

    IndexRegion() {}
    IndexRegion(T Min, T Max) : Min(Min), Max(Max) {}
};

class IVolumeDataAccessor
{
protected:
                IVolumeDataAccessor() {}
  virtual      ~IVolumeDataAccessor() {}
public:
  class Manager
  {
  protected:
                  Manager() {}
    virtual      ~Manager() {}
  public:
    virtual void  DestroyVolumeDataAccessor(IVolumeDataAccessor *accessor) = 0;
                  
    virtual IVolumeDataAccessor *
                  CloneVolumeDataAccessor(IVolumeDataAccessor const &accessor) = 0;
  };

  virtual Manager &
                GetManager() = 0;

  virtual VolumeDataLayout const *
                GetLayout() = 0;

  using IndexOutOfRangeException = OpenVDS::IndexOutOfRangeException;
  using ReadErrorException = OpenVDS::ReadErrorException;
};

template <typename INDEX>
class IVolumeDataRegions
{
protected:
  virtual      ~IVolumeDataRegions() {}

public:
  virtual int64_t
                RegionCount() = 0;

  virtual IndexRegion<INDEX>
                Region(int64_t region) = 0;

  virtual int64_t
                RegionFromIndex(INDEX index) = 0;
};

template <typename INDEX>
class IVolumeDataAccessorWithRegions : public IVolumeDataAccessor, public IVolumeDataRegions<INDEX>
{
public:
  virtual IndexRegion<INDEX>
                CurrentRegion() = 0;
};

template <typename INDEX, typename T>
class IVolumeDataReadAccessor : public IVolumeDataAccessorWithRegions<INDEX>
{
public:
  virtual T     GetValue(INDEX index) = 0;
};

template <typename INDEX, typename T>
class IVolumeDataReadWriteAccessor : public IVolumeDataReadAccessor<INDEX, T>
{
public:
  virtual void SetValue(INDEX index, T value) = 0;
  virtual void Commit() = 0;
  virtual void Cancel() = 0;
};

class VolumeDataPage
{
public:
  struct Error
  {
    const char *message;
    int         errorCode; 
  };
protected:
                VolumeDataPage() {}
  virtual      ~VolumeDataPage() {}
public:
  virtual VolumeDataPageAccessor &
                GetVolumeDataPageAccessor() const = 0;
  virtual void  GetMinMax(int (&min)[Dimensionality_Max], int (&max)[Dimensionality_Max]) const = 0;
  virtual void  GetMinMaxExcludingMargin(int (&minExcludingMargin)[Dimensionality_Max], int (&maxExcludingMargin)[Dimensionality_Max]) const = 0;
  virtual Error GetError() const = 0;
  virtual const void *
                GetBuffer(int (&pitch)[Dimensionality_Max]) = 0;
  virtual void *GetWritableBuffer(int (&pitch)[Dimensionality_Max]) = 0;
  virtual void  UpdateWrittenRegion(const int (&writtenMin)[Dimensionality_Max], const int (&writtenMax)[Dimensionality_Max]) = 0;
  virtual void  Release() = 0;
};

class VolumeDataPageAccessor
{
public:
  enum AccessMode
  {
    AccessMode_ReadOnly,
    AccessMode_ReadWrite,
    AccessMode_Create
  };

protected:
                VolumeDataPageAccessor() {}
  virtual      ~VolumeDataPageAccessor() {}
public:
  virtual VolumeDataLayout const *GetLayout() const = 0;

  virtual int   GetLOD() const = 0;
  virtual int   GetChannelIndex() const = 0;
  virtual VolumeDataChannelDescriptor const &GetChannelDescriptor() const = 0;
  virtual void  GetNumSamples(int (&numSamples)[Dimensionality_Max]) const = 0;

  virtual int64_t GetChunkCount() const = 0;
  virtual void  GetChunkMinMax(int64_t chunk, int (&min)[Dimensionality_Max], int (&max)[Dimensionality_Max]) const = 0;
  virtual void  GetChunkMinMaxExcludingMargin(int64_t chunk, int (&minExcludingMargin)[Dimensionality_Max], int (&maxExcludingMargin)[Dimensionality_Max]) const = 0;
  virtual int64_t GetChunkIndex(const int (&position)[Dimensionality_Max]) const = 0;

  /// <summary>
  /// Get the chunk index for this VolumeDataPageAccessor corresponding to the given chunk index in the primary channel.
  /// Because some channels can have mappings (e.g. one value per trace), the number of chunks can be less than in the primary
  /// channel and we need to have a mapping to figure out the chunk index in each channel that is produced together.
  /// </summary>
  /// <param name="primaryChannelChunkIndex">
  /// The index of the chunk in the primary channel (channel 0) that we want to map to a chunk index for this VolumeDataPageAccessor.
  /// </param>
  /// <returns>
  /// The chunk index for this VolumeDataPageAccessor corresponding to the given chunk index in the primary channel.
  /// </returns>
  virtual int64_t GetMappedChunkIndex(int64_t primaryChannelChunkIndex) const = 0;

  /// <summary>
  /// Get the primary channel chunk index corresponding to the given chunk index of this VolumeDataPageAccessor.
  /// In order to avoid creating duplicates requests when a channel is mapped, we need to know which primary channel chunk index is representative of
  /// a particular mapped chunk index.
  /// </summary>
  /// <param name="chunkIndex">
  /// The chunk index for this VolumeDataPageAccessor that we want the representative primary channel chunk index of.
  /// </param>
  /// <returns>
  /// The primary channel chunk index corresponding to the given chunk index for this VolumeDataPageAccessor.
  /// </returns>
  virtual int64_t GetPrimaryChannelChunkIndex(int64_t chunkIndex) const = 0;

  virtual int   AddReference() = 0;
  virtual int   RemoveReference() = 0;

  virtual int   GetMaxPages() = 0;
  virtual void  SetMaxPages(int maxPages) = 0;

  virtual VolumeDataPage *CreatePage(int64_t chunkIndex) = 0;
  virtual VolumeDataPage *ReadPage(int64_t chunkIndex) = 0;
  virtual VolumeDataPage *ReadPageAtPosition(const int (&position)[Dimensionality_Max]) = 0;

  virtual void  Commit() = 0;
};

/// \class VolumeDataReadAccessor
/// \brief A class that provides random read access to the voxel values of a VDS
template <typename INDEX, typename T>
class VolumeDataReadAccessor
{
protected:
  IVolumeDataReadAccessor<INDEX, T> *
                        m_accessor;
public:
  VolumeDataLayout const *
                        GetLayout() const { return m_accessor ? m_accessor->GetLayout() : NULL; }

  int64_t               RegionCount() const { return m_accessor ? m_accessor->RegionCount() : 0; }

  IndexRegion<INDEX>    Region(int64_t region) const { return m_accessor ? m_accessor->Region(region) : IndexRegion<INDEX>(); }

  int64_t               RegionFromIndex(INDEX index) { return m_accessor ? m_accessor->RegionFromIndex(index) : 0; }

  IndexRegion<INDEX>    CurrentRegion() const { return m_accessor ? m_accessor->CurrentRegion() : IndexRegion<INDEX>(); }

  T                     GetValue(INDEX index) const { return m_accessor ? m_accessor->GetValue(index) : T(); }

                        VolumeDataReadAccessor() : m_accessor() {}

                        VolumeDataReadAccessor(IVolumeDataReadAccessor<INDEX, T> *accessor) : m_accessor(accessor) {}

                        VolumeDataReadAccessor(VolumeDataReadAccessor const &readAccessor) : m_accessor(readAccessor.m_accessor ? static_cast<IVolumeDataReadAccessor<INDEX, T>*>(readAccessor.m_accessor->GetManager().CloneVolumeDataAccessor(*readAccessor.m_accessor)) : NULL) {}

                       ~VolumeDataReadAccessor() { if(m_accessor) m_accessor->GetManager().DestroyVolumeDataAccessor(m_accessor); }
};

/// \class VolumeDataReadWriteAccessor
/// \brief A class that provides random read/write access to the voxel values of a VDS
template <typename INDEX, typename T>
class VolumeDataReadWriteAccessor : public VolumeDataReadAccessor<INDEX, T>
{
protected:
  using VolumeDataReadAccessor<INDEX, T>::m_accessor;
  IVolumeDataReadWriteAccessor<INDEX, T> *
                        Accessor() { return static_cast<IVolumeDataReadWriteAccessor<INDEX, T> *>(m_accessor); }
public:
  void                  SetValue(INDEX index, T value) { if(Accessor()) return Accessor()->SetValue(index, value); }
  void                  Commit() { if(Accessor()) return Accessor()->Commit(); }
  void                  Cancel() { if(Accessor()) return Accessor()->Cancel(); }

                        VolumeDataReadWriteAccessor() : VolumeDataReadAccessor<INDEX, T>() {}

                        VolumeDataReadWriteAccessor(IVolumeDataReadWriteAccessor<INDEX, T> *accessor) : VolumeDataReadAccessor<INDEX, T>(accessor) {}

                        VolumeDataReadWriteAccessor(VolumeDataReadWriteAccessor const &readWriteAccessor) :  VolumeDataReadAccessor<INDEX, T>(readWriteAccessor.m_accessor ? static_cast<IVolumeDataReadWriteAccessor<INDEX, T>*>(readWriteAccessor.m_accessor->GetManager().CloneVolumeDataAccessor(*readWriteAccessor.m_accessor)) : NULL) {}
};

//-----------------------------------------------------------------------------
// 2D VolumeDataAccessors
//-----------------------------------------------------------------------------

typedef VolumeDataReadAccessor<FloatVector2, double> VolumeData2DInterpolatingAccessorR64;
typedef VolumeDataReadAccessor<FloatVector2, float>  VolumeData2DInterpolatingAccessorR32;

typedef VolumeDataReadAccessor<IntVector2, double> VolumeData2DReadAccessorR64;
typedef VolumeDataReadAccessor<IntVector2, uint64_t> VolumeData2DReadAccessorU64;
typedef VolumeDataReadAccessor<IntVector2, float>  VolumeData2DReadAccessorR32;
typedef VolumeDataReadAccessor<IntVector2, uint32_t> VolumeData2DReadAccessorU32;
typedef VolumeDataReadAccessor<IntVector2, uint16_t> VolumeData2DReadAccessorU16;
typedef VolumeDataReadAccessor<IntVector2, uint8_t>  VolumeData2DReadAccessorU8;
typedef VolumeDataReadAccessor<IntVector2, bool>   VolumeData2DReadAccessor1Bit;

typedef VolumeDataReadWriteAccessor<IntVector2, double> VolumeData2DReadWriteAccessorR64;
typedef VolumeDataReadWriteAccessor<IntVector2, uint64_t> VolumeData2DReadWriteAccessorU64;
typedef VolumeDataReadWriteAccessor<IntVector2, float>  VolumeData2DReadWriteAccessorR32;
typedef VolumeDataReadWriteAccessor<IntVector2, uint32_t> VolumeData2DReadWriteAccessorU32;
typedef VolumeDataReadWriteAccessor<IntVector2, uint16_t> VolumeData2DReadWriteAccessorU16;
typedef VolumeDataReadWriteAccessor<IntVector2, uint8_t>  VolumeData2DReadWriteAccessorU8;
typedef VolumeDataReadWriteAccessor<IntVector2, bool>   VolumeData2DReadWriteAccessor1Bit;

//-----------------------------------------------------------------------------
// 3D VolumeDataAccessors
//-----------------------------------------------------------------------------

typedef VolumeDataReadAccessor<FloatVector3, double> VolumeData3DInterpolatingAccessorR64;
typedef VolumeDataReadAccessor<FloatVector3, float>  VolumeData3DInterpolatingAccessorR32;

typedef VolumeDataReadAccessor<IntVector3, double> VolumeData3DReadAccessorR64;
typedef VolumeDataReadAccessor<IntVector3, uint64_t> VolumeData3DReadAccessorU64;
typedef VolumeDataReadAccessor<IntVector3, float>  VolumeData3DReadAccessorR32;
typedef VolumeDataReadAccessor<IntVector3, uint32_t> VolumeData3DReadAccessorU32;
typedef VolumeDataReadAccessor<IntVector3, uint16_t> VolumeData3DReadAccessorU16;
typedef VolumeDataReadAccessor<IntVector3, uint8_t>  VolumeData3DReadAccessorU8;
typedef VolumeDataReadAccessor<IntVector3, bool>   VolumeData3DReadAccessor1Bit;

typedef VolumeDataReadWriteAccessor<IntVector3, double> VolumeData3DReadWriteAccessorR64;
typedef VolumeDataReadWriteAccessor<IntVector3, uint64_t> VolumeData3DReadWriteAccessorU64;
typedef VolumeDataReadWriteAccessor<IntVector3, float>  VolumeData3DReadWriteAccessorR32;
typedef VolumeDataReadWriteAccessor<IntVector3, uint32_t> VolumeData3DReadWriteAccessorU32;
typedef VolumeDataReadWriteAccessor<IntVector3, uint16_t> VolumeData3DReadWriteAccessorU16;
typedef VolumeDataReadWriteAccessor<IntVector3, uint8_t>  VolumeData3DReadWriteAccessorU8;
typedef VolumeDataReadWriteAccessor<IntVector3, bool>   VolumeData3DReadWriteAccessor1Bit;

//-----------------------------------------------------------------------------
// 4D VolumeDataAccessors
//-----------------------------------------------------------------------------

typedef VolumeDataReadAccessor<FloatVector4, double> VolumeData4DInterpolatingAccessorR64;
typedef VolumeDataReadAccessor<FloatVector4, float>  VolumeData4DInterpolatingAccessorR32;

typedef VolumeDataReadAccessor<IntVector4, double> VolumeData4DReadAccessorR64;
typedef VolumeDataReadAccessor<IntVector4, uint64_t> VolumeData4DReadAccessorU64;
typedef VolumeDataReadAccessor<IntVector4, float>  VolumeData4DReadAccessorR32;
typedef VolumeDataReadAccessor<IntVector4, uint32_t> VolumeData4DReadAccessorU32;
typedef VolumeDataReadAccessor<IntVector4, uint16_t> VolumeData4DReadAccessorU16;
typedef VolumeDataReadAccessor<IntVector4, uint8_t>  VolumeData4DReadAccessorU8;
typedef VolumeDataReadAccessor<IntVector4, bool>   VolumeData4DReadAccessor1Bit;

typedef VolumeDataReadWriteAccessor<IntVector4, double> VolumeData4DReadWriteAccessorR64;
typedef VolumeDataReadWriteAccessor<IntVector4, uint64_t> VolumeData4DReadWriteAccessorU64;
typedef VolumeDataReadWriteAccessor<IntVector4, float>  VolumeData4DReadWriteAccessorR32;
typedef VolumeDataReadWriteAccessor<IntVector4, uint32_t> VolumeData4DReadWriteAccessorU32;
typedef VolumeDataReadWriteAccessor<IntVector4, uint16_t> VolumeData4DReadWriteAccessorU16;
typedef VolumeDataReadWriteAccessor<IntVector4, uint8_t>  VolumeData4DReadWriteAccessorU8;
typedef VolumeDataReadWriteAccessor<IntVector4, bool>   VolumeData4DReadWriteAccessor1Bit;

} /* namespace OpenVDS*/

#endif //VOLUMEDATAACCESS_H
