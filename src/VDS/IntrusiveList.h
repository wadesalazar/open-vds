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

#ifndef INTRUSIVELIST_H
#define INTRUSIVELIST_H

#include <assert.h>

namespace OpenVDS
{

template <typename Item>
class IntrusiveListNode
{
public:
  Item *m_next;
  Item *m_prev;
  IntrusiveListNode() : m_next(nullptr), m_prev(nullptr) {}
};

template <typename Item, IntrusiveListNode<Item> Item::*PtrToNode = &Item::node>
class IntrusiveList
{
public:
  typedef IntrusiveListNode<Item> Node;
private:
  Item *m_firstItem;
  Item *m_lastItem;

#ifdef INTRUSIVE_LIST_MAINTAIN_COUNT
  int64_t m_count;
#endif

public:
  IntrusiveList()
    : m_firstItem(nullptr)
    , m_lastItem(nullptr)
#ifdef INTRUSIVE_LIST_MAINTAIN_COUNT
    , m_count(0)
#endif
  {}

  bool isEmpty() const
  {
    assert(!!m_firstItem == !!m_lastItem);
    return m_firstItem == NULL;
  }

  Item *getFirstItem() const { return m_firstItem; }
  Item *getLastItem() const { return m_lastItem; }

  void remove(Item *item)
  {
    Node &node = item->*PtrToNode;

    if(node.m_next)
    {
      assert((node.m_next->*PtrToNode).m_prev == item && "The prev pointer of the next element has to point to this element");
      (node.m_next->*PtrToNode).m_prev = node.m_prev;
    }
    else
    {
      assert(m_lastItem == item && "If next pointer is NULL the item has to be the last in the list");
      m_lastItem = node.m_prev;
    }

    if(node.m_prev)
    {
      assert((node.m_prev->*PtrToNode).m_next == item && "The next pointer of the prev element has to point to this element");
      (node.m_prev->*PtrToNode).m_next = node.m_next;
    }
    else
    {
      assert(m_firstItem == item && "If prev pointer is NULL the item has to be the first in the list");
      m_firstItem = node.m_next;
    }

    node.m_prev = nullptr;
    node.m_next = nullptr;

#ifdef INTRUSIVE_LIST_MAINTAIN_COUNT
    m_count--;
#endif
  }

  void insertFirst(Item *item)
  {
    Node &node = item->*PtrToNode;

    assert(!node.m_prev && "The item should be removed from a list before being inserted");
    assert(!node.m_next && "The item should be removed from a list before being inserted");

    if(!m_firstItem)
    {
      assert(!m_lastItem && "If there is no first element there can't be a last element");
      m_firstItem = item;
      m_lastItem = item;
    }
    else
    {
      assert(m_lastItem && "If there is a first element there must be a last element");
      (m_firstItem->*PtrToNode).m_prev = item;
      node.m_next = m_firstItem;
      m_firstItem = item;
    }

#ifdef INTRUSIVE_LIST_MAINTAIN_COUNT
    m_count++;
#endif
  }

  void insertLast(Item *item)
  {
    Node &node = item->*PtrToNode;

    assert(!node.m_prev && "The item should be removed from a list before being inserted");
    assert(!node.m_next && "The item should be removed from a list before being inserted");

    if(!m_lastItem)
    {
      assert(!m_firstItem && "If there is no last element there can't be a first element");
      m_lastItem = item;
      m_firstItem = item;
    }
    else
    {
      assert(m_firstItem && "If there is a last element there must be a first element");
      (m_lastItem->*PtrToNode).m_next = item;
      node.m_prev = m_lastItem;
      m_lastItem = item;
    }

#ifdef INTRUSIVE_LIST_MAINTAIN_COUNT
    m_count++;
#endif
  }

  void InsertAfter(Item *item, Item *insertionPoint)
  {
    Node &node = item->*PtrToNode;

    assert(!node.m_prev && "The item should be removed from a list before being inserted");
    assert(!node.m_next && "The item should be removed from a list before being inserted");

    if(!insertionPoint)
    {
      insertFirst(item);
    }
    else if(!((insertionPoint->*PtrToNode).m_next))
    {
      insertLast(item);
    }
    else
    {
      node.m_next = (insertionPoint->*PtrToNode).m_next;
      node.m_prev = insertionPoint;
      (node.m_prev->*PtrToNode).m_next = item;
      (node.m_next->*PtrToNode).m_prev = item;
    }

#ifdef INTRUSIVE_LIST_MAINTAIN_COUNT
    m_count++;
#endif
  }

  void insertBefore(Item *item, Item *insertionPoint)
  {
    Node &node = item->*PtrToNode;

    assert(!node.m_prev && "The item should be removed from a list before being inserted");
    assert(!node.m_next && "The item should be removed from a list before being inserted");

    if(!insertionPoint)
    {
      InsertLast(item);
    }
    else if(!(insertionPoint->*PtrToNode).m_prev)
    {
      InsertFirst(item);
    }
    else
    {
      node.m_next = insertionPoint;
      node.m_prev = (insertionPoint->*PtrToNode).m_prev;
      (node.m_prev->*PtrToNode).m_next = item;
      (node.m_next->*PtrToNode).m_prev = item;
    }

#ifdef INTRUSIVE_LIST_MAINTAIN_COUNT
    m_count++;
#endif
  }
};

}

#endif

