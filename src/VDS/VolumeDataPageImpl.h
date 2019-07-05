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

#include "VolumeDataPageAccessorImpl.h"

namespace OpenVDS
{
class VolumeDataPageImpl : public VolumeDataPage
{
private:
  const VolumeDataPageAccessor * m_volumeDataPageAccessor;

  int64_t m_chunk;

  std::vector<uint8_t> m_blob;

  int32_t _nPins;

  void *m_buffer;

  bool _isDirty;

  int32_t m_pitch[Dimensionality_Max];

  int32_t m_writtenMin[Dimensionality_Max];
  int32_t m_writtenMax[Dimensionality_Max];

  int64_t m_copiedToChunkIndexes[26];

  int32_t m_chunksCopiedTo;

public:
  VolumeDataPageImpl(VolumeDataPageImpl const &) = delete;
  VolumeDataPageImpl(VolumeDataPageAccessor *volumeDataPageAccessor, int64_t chunk);
  ~VolumeDataPageImpl();

  int64_t getChunkIndex() const { return m_chunk; }

  // All these methods require the caller to hold a lock
  bool          IsPinned();
  void          Pin();
  void          UnPin();

  bool          IsEmpty();
  bool          IsDirty();
  bool          IsWritten();
  void          MakeDirty();

  void          SetBufferData(std::vector<uint8_t> const &blob, const int (&pitch)[Dimensionality_Max]);
  //void          WriteBack(VolumeDataLayer *volumeDataLayer, HueMutexLock_c &cPageListMutexLock);
  void *        GetBufferInternal(int (&anPitch)[Dimensionality_Max], bool isReadWrite);
  bool          IsCopyMarginNeeded(VolumeDataPage *targetPage);
  void          CopyMargin(VolumeDataPage *targetPage);

  // Implementation of Hue::HueSpaceLib::VolumeDataPage interface, these methods aquire a lock (except the GetMinMax methods which don't need to)
  void  getMinMax(int (&min)[Dimensionality_Max], int (&max)[Dimensionality_Max]) const override;
  void  getMinMaxExcludingMargin(int (&minExcludingMargin)[Dimensionality_Max], int (&maxExcludingMargin)[Dimensionality_Max]) const override;
  const void *getBuffer(int (&pitch)[Dimensionality_Max]) override; // Getting the buffer will block if the page is currently being read from the VolumeDataCache
  void *getWritableBuffer(int (&pitch)[Dimensionality_Max]) override;
  void  updateWrittenRegion(const int (&writtenMin)[Dimensionality_Max], const int (&writtenMax)[Dimensionality_Max]) override;
  void  release() override;
};
}
#endif //VOLUMEDATAPAGEIMPL_H