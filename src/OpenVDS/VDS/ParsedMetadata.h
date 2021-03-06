/****************************************************************************
** Copyright 2020 The Open Group
** Copyright 2020 Bluware, Inc.
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

#ifndef PARSEDMETADATA_H
#define PARSEDMETADATA_H

#include <vector>
#include <stdint.h>

#include <fmt/format.h>

#ifdef WAVELET_ADAPTIVE_LEVELS
#error "Don't include WaveletTypes.h before this file!"
#endif

namespace OpenVDS
{
  enum
  {
    WAVELET_ADAPTIVE_LEVELS = 16
  };

  struct ParsedMetadata
  {
    ParsedMetadata() :
      m_chunkHash(0),
      m_chunkSize(0)
    {}

    uint64_t
      m_chunkHash;
  
    int32_t
      m_chunkSize;

    std::vector<uint8_t>
      m_adaptiveLevels;

    std::vector<unsigned char>
    CreateChunkMetadata() const
    {
      int
        metadataLength = m_adaptiveLevels.empty() ? 8 : 24;

      std::vector<unsigned char>
        metadata(metadataLength);

      memcpy(metadata.data(), &m_chunkHash, 8);

      if (metadataLength == 24)
      {
        memcpy(metadata.data() + 8, m_adaptiveLevels.data(), 16);
      }

      return metadata;
    }
  };

  inline ParsedMetadata ParseMetadata(unsigned char const* metadata, int metadataByteSize, Error &error)
  {
    ParsedMetadata parsedMetadata;

    if (metadataByteSize == 4 + 24)
    {
      parsedMetadata.m_chunkSize = *reinterpret_cast<int32_t const*>(metadata);

      parsedMetadata.m_chunkHash = *reinterpret_cast<uint64_t const*>(metadata + 4);

      parsedMetadata.m_adaptiveLevels.resize(WAVELET_ADAPTIVE_LEVELS);

      memcpy(parsedMetadata.m_adaptiveLevels.data(), metadata + 4 + 8, WAVELET_ADAPTIVE_LEVELS);
    }
    else if (metadataByteSize == 8)
    {
      parsedMetadata.m_chunkHash = *reinterpret_cast<uint64_t const*>(metadata);
    }
    else
    {
      error.string = fmt::format("Unsupported chunkMetadataByteSize: {}.", metadataByteSize);
      error.code = -1;
    }

    return parsedMetadata;
  }


}
#endif
