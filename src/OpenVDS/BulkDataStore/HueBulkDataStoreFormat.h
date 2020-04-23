#ifndef HUEBULKDATASTOREFORMAT_H
#define HUEBULKDATASTOREFORMAT_H
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

struct DataStoreHeader
{
  char     m_magic[12]; // = "HueDataStore"
  int32_t  m_version; // (major << 16) + minor
  int64_t  m_fileTableOffset;
  int32_t  m_fileTableCount;
  int32_t  m_fileNameLength;
};

struct FileHeader
{
  int64_t  m_headPageDirectoryOffset;
  int32_t  m_headChunkCount;
  int32_t  m_headRevisionNumber;
  int32_t  m_indexPageEntryCount;
  int32_t  m_fileType;
  int32_t  m_chunkMetadataLength;
  int32_t  m_fileMetadataLength;
};

struct PageDirectory
{
  int64_t  m_previousPageDirectoryOffset;
  int32_t  m_previousChunkCount;
  int32_t  m_previousRevisionNumber;
 };

struct IndexEntry
{
  int64_t  m_offset;
  int32_t  m_length;
  int32_t  m_reserved; 
};

#endif //HUEBULKDATASTOREFORMAT_H
