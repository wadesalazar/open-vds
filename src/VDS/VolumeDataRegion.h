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

#ifndef VOLUMEDATAREGION_H
#define VOLUMEDATAREGION_H

#include "VolumeDataLayer.h"

#include <vector>
namespace OpenVDS
{
class VolumeDataRegion
{
private:
  friend class VolumeDataLayer;

  const VolumeDataLayer *m_volumeDataLayer;

  int64_t  m_chunksInRegion;

  IndexArray m_chunkMin;
  IndexArray m_chunkMax;

  int64_t m_modulo[Dimensionality_Max];
  int64_t m_layerModulo[Dimensionality_Max];

public:
  int64_t GetNumChunksInRegion() const;
  int64_t GetChunkIndexInRegion(int64_t iChunkInRegion) const;
  void GetChunkInRegion(int64_t iChunkInRegion, VolumeDataChunk *volumeDataChunk) const { *volumeDataChunk = m_volumeDataLayer->GetChunkFromIndex(GetChunkIndexInRegion(iChunkInRegion)); }
  void GetChunksInRegion(std::vector<VolumeDataChunk> *volumeDataChunk, bool isAppend = false) const;
  bool IsChunkInRegion(VolumeDataChunk const &volumeDataChunk) const;

  VolumeDataRegion() : m_volumeDataLayer(nullptr), m_chunksInRegion(0) {};

  VolumeDataRegion(VolumeDataLayer const &cVolumeDataLayer,
                        const IndexArray &anMin,
                        const IndexArray &anMax);

  static VolumeDataRegion VolumeDataRegionOverlappingChunk(VolumeDataLayer const &VolumeDataLayer, VolumeDataChunk const &VolumeDataChunk, const IndexArray &anOffset);
};
}
#endif //VOLUMEDATAREGION_H
