#ifndef EXTENTALLOCATOR_H
#define EXTENTALLOCATOR_H
////////////////////////////////////////////////////////////////////////////////
// <copyright>
// Copyright (c) 2016 Hue AS. All rights reserved.
//
// All rights are reserved. Reproduction or transmission in whole or in part, in
// any form or by any means, electronic, mechanical or otherwise, is prohibited
// without the prior written permission of the copyright owner.
// </copyright>
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER < 1600
#ifndef INTTYPES_DEFINED
#define INTTYPES_DEFINED
typedef signed __int8  int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef signed __int64 int64_t;
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#endif
#else
#include <stdint.h>
#endif

#include <map>
#include <cassert>

class ExtentAllocator
{
public:
  enum ExtentType
  {
    FreeExtent,
    DataStoreHeaderExtent,
    FileTableExtent,
    PageDirectoryExtent,
    IndexPageExtent,
    ChunkExtent,
    MixedExtent
  };

  class Extent
  {
  public:
    int     m_referenceCount;
    ExtentType
      m_extentType;
    int     m_combinedExtents;

    Extent(int referenceCount, ExtentType type) : m_referenceCount(referenceCount), m_extentType(type), m_combinedExtents(1) {};
    Extent(Extent const &extent) : m_referenceCount(extent.m_referenceCount), m_extentType(extent.m_extentType), m_combinedExtents(1) {};
  };

  typedef std::map<int64_t, Extent> Extents;

  Extents m_extents;

private:
  void  CombineCompatibleExtents(Extents::iterator position);

  Extents::iterator  GetPosition(int64_t offset);

public:
  ExtentAllocator(int64_t initialSize);

  int64_t  Allocate(int32_t size, ExtentType type);

  void  AddReference(int64_t offset, int32_t size, ExtentType type);
};

#endif //EXTENTALLOCATOR_H
