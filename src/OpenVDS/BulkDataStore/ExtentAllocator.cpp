////////////////////////////////////////////////////////////////////////////////
// <copyright>
// Copyright (c) 2016 Hue AS. All rights reserved.
//
// All rights are reserved. Reproduction or transmission in whole or in part, in
// any form or by any means, electronic, mechanical or otherwise, is prohibited
// without the prior written permission of the copyright owner.
// </copyright>
////////////////////////////////////////////////////////////////////////////////
#include "ExtentAllocator.h"


ExtentAllocator::ExtentAllocator(int64_t initialSize)
{
  m_extents.insert(Extents::value_type(0,           Extent(0, FreeExtent)));
  m_extents.insert(Extents::value_type(initialSize, Extent(0, FreeExtent)));
}

int64_t
ExtentAllocator::Allocate(int32_t size, ExtentType type)
{
  assert(m_extents.rbegin()->second.m_referenceCount == 0 && "Last position has to have referenceCount 0 or allocation will fail");

  Extents::iterator it = m_extents.begin();

  while(true)
  {
    if(it->second.m_referenceCount == 0)
    {
      assert(it->second.m_extentType == FreeExtent);
      Extents::iterator next = it; ++next;

      // If we've reached the end, insert a large enough FreeExtent at the end so allocation will succeed
      if(next == m_extents.end())
      {
        assert(it != m_extents.begin());

        Extents::iterator before = it; --before;

        int64_t
          end = it->first + size;

        if(before != m_extents.end() && before->second.m_extentType == FreeExtent)
        {
          end = before->first + size;
          m_extents.erase(it);
          it = before;
        }

        next = m_extents.insert(Extents::value_type(end, Extent(0, FreeExtent))).first;
      }

      int64_t
        extentSize = next->first - it->first;

      if(extentSize >= size)
      {
        it->second.m_referenceCount++;
        it->second.m_extentType = type;
        it->second.m_combinedExtents = 1;

        int64_t
          pos = it->first;

        CombineCompatibleExtents(it);

        // If there is room left in the FreeExtent we need to make a new FreeExtent from the end of the allocated extent
        if(extentSize > size)
        {
          m_extents.insert(Extents::value_type(pos + size, Extent(0, FreeExtent)));
        }

        return pos;
      }
    }
    ++it;
    assert(it != m_extents.end());
  }
}

void
ExtentAllocator::CombineCompatibleExtents(Extents::iterator position)
{
  if(position->second.m_extentType != FreeExtent &&
     position->second.m_extentType != ChunkExtent &&
     position->second.m_extentType != IndexPageExtent)
  {
    return;
  }

  // combine extents if they have the same reference count and type
  if(position != m_extents.begin())
  {
    Extents::iterator
      before = position; --before;

    if(before->second.m_referenceCount == position->second.m_referenceCount && before->second.m_extentType == position->second.m_extentType)
    {
      before->second.m_combinedExtents += position->second.m_combinedExtents;
      m_extents.erase(position);
    }
  }
}

ExtentAllocator::Extents::iterator
ExtentAllocator::GetPosition(int64_t offset)
{
  Extents::iterator
    position = m_extents.lower_bound(offset);

  assert(position != m_extents.end() && "The position cannot be beyond the end of the file");

  // split the extent if the position is in the middle of an existing extent
  if(position->first != offset)
  {
    assert(position != m_extents.begin() && "The set starts out with a position at 0");

    Extents::iterator
      before = position; --before;

    position = m_extents.insert(Extents::value_type(offset, before->second)).first;
  }

  return position;
}

void
ExtentAllocator::AddReference(int64_t offset, int32_t size, ExtentType type)
{
  assert(offset >= 0 && size > 0);

  Extents::iterator
    start = GetPosition(offset),
    stop  = GetPosition(offset + size);

  assert(start != m_extents.end() && "The extent cannot be beyond the end of the file");
  assert(stop  != m_extents.end() && "The extent cannot be beyond the end of the file");

  for(Extents::iterator it = start; it != stop; ++it)
  {
    it->second.m_referenceCount++;

    if (it->second.m_extentType == FreeExtent)
    {
      it->second.m_extentType = type;
    }
    else if (it->second.m_extentType != type)
    {
      it->second.m_extentType = MixedExtent;
    }
  }

  CombineCompatibleExtents(start);
  CombineCompatibleExtents(stop);
}
