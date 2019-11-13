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

#ifndef VOLUMEDATAPAGEIMPL_H
#define VOLUMEDATAPAGEIMPL_H

#include <OpenVDS/VolumeDataAccess.h>

#include "DataBlock.h"

#include <mutex>
#include <vector>

namespace OpenVDS
{
class VolumeDataLayer;
class VolumeDataPageAccessorImpl;
class VolumeDataPageImpl : public VolumeDataPage
{
private:
  VolumeDataPageAccessorImpl * m_volumeDataPageAccessor;

  int64_t m_chunk;

  DataBlock m_dataBlock;
  int32_t m_pitch[Dimensionality_Max];
  std::vector<uint8_t> m_blob;

  int32_t m_pins;

  bool    m_isReadWrite;
  bool    m_isDirty;
  bool    m_requestPrepared;

  int32_t m_writtenMin[Dimensionality_Max];
  int32_t m_writtenMax[Dimensionality_Max];

  int64_t m_copiedToChunkIndexes[26];

  int32_t m_chunksCopiedTo;

public:
  VolumeDataPageImpl(VolumeDataPageAccessorImpl *volumeDataPageAccessor, int64_t chunk);
  VolumeDataPageImpl(VolumeDataPageImpl const &) = delete;

  int64_t GetChunkIndex() const { return m_chunk; }
  const DataBlock &GetDataBlock() const { return m_dataBlock;}

  // All these methods require the caller to hold a lock
  bool          IsPinned();
  void          Pin();
  void          UnPin();

  bool          IsEmpty();
  bool          IsDirty();
  bool          IsWritten();
  void          MakeDirty();

  void          SetBufferData(const DataBlock& dataBlock, int32_t(&pitch)[Dimensionality_Max], std::vector<uint8_t>&& blob);
  void          WriteBack(VolumeDataLayer *volumeDataLayer, std::unique_lock<std::mutex> &pageListMutexLock);
  void *        GetBufferInternal(int (&anPitch)[Dimensionality_Max], bool isReadWrite);
  void *        GetRawBufferInternal() { return m_blob.data(); }
  bool          IsCopyMarginNeeded(VolumeDataPageImpl *targetPage);
  void          CopyMargin(VolumeDataPageImpl *targetPage);

  void          SetRequestPrepared(bool prepared) { m_requestPrepared = prepared; }

  // Implementation of Hue::HueSpaceLib::VolumeDataPage interface, these methods aquire a lock (except the GetMinMax methods which don't need to)
  void  GetMinMax(int (&min)[Dimensionality_Max], int (&max)[Dimensionality_Max]) const override;
  void  GetMinMaxExcludingMargin(int (&minExcludingMargin)[Dimensionality_Max], int (&maxExcludingMargin)[Dimensionality_Max]) const override;
  const void *GetBuffer(int (&pitch)[Dimensionality_Max]) override; // Getting the buffer will block if the page is currently being read from the VolumeDataCache
  void *GetWritableBuffer(int (&pitch)[Dimensionality_Max]) override;
  void  UpdateWrittenRegion(const int (&writtenMin)[Dimensionality_Max], const int (&writtenMax)[Dimensionality_Max]) override;
  void  Release() override;
};
}
#endif //VOLUMEDATAPAGEIMPL_H
