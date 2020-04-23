#ifndef HUEBULKDATASTOREFILETYPES_H
#define HUEBULKDATASTOREFILETYPES_H
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

#define DATASTORE_FILETYPE(a, b, c, d) ((a & 0xff) | (b & 0xff) << 8  | (c & 0xff) << 16 | (d & 0xff) << 24)

enum DataStoreFileType
{
  FILETYPE_VDS_LAYER   = DATASTORE_FILETYPE('V', 'D', 'S', ' '),
  FILETYPE_SHAPE       = DATASTORE_FILETYPE('S', 'H', 'P', 'E'),
  FILETYPE_PROPERTY    = DATASTORE_FILETYPE('P', 'R', 'O', 'P'),
  FILETYPE_HUE_OBJECT  = DATASTORE_FILETYPE('O', 'B', 'J', ' '),
  FILETYPE_JSON_OBJECT = DATASTORE_FILETYPE('J', 'S', 'O', 'N')
};

struct VDSLayerMetadata
{
  enum CompressionMethod
  {
    COMPRESSIONMETHOD_NONE,
    COMPRESSIONMETHOD_WAVELET,
    COMPRESSIONMETHOD_RLE,
    COMPRESSIONMETHOD_ZIP,
    COMPRESSIONMETHOD_WAVELET_NORMALIZE_BLOCK,
    COMPRESSIONMETHOD_WAVELET_LOSSLESS,
    COMPRESSIONMETHOD_WAVELET_NORMALIZE_BLOCK_LOSSLESS
  };

  int32_t  m_compressionMethod;
  float    m_compressionTolerance;
  int32_t  m_validChunkCount;
};

struct VDSLayerMetadataWaveletAdaptive : public VDSLayerMetadata
{
  enum
  {
    WAVELET_ADAPTIVE_LEVELS = 16
  };

  int32_t   m_reserved;
  int64_t   m_uncompressedSize;
  int64_t   m_adaptiveLevelSizes[WAVELET_ADAPTIVE_LEVELS];
};

struct VDSChunkMetadata
{
  uint64_t  m_hash;
};

struct VDSWaveletAdaptiveLevelsChunkMetadata : public VDSChunkMetadata
{
  enum
  {
    WAVELET_ADAPTIVE_LEVELS = VDSLayerMetadataWaveletAdaptive::WAVELET_ADAPTIVE_LEVELS
  };

  uint8_t  m_levels[WAVELET_ADAPTIVE_LEVELS];
};

#endif //HUEBULKDATASTOREFILETYPES_H
