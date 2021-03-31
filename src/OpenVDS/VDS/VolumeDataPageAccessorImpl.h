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

#ifndef VOLUMEDATAPAGEACCESSORIMPL_H
#define VOLUMEDATAPAGEACCESSORIMPL_H

#include <OpenVDS/VolumeDataAccess.h>
#include "IntrusiveList.h"

#include <list>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <chrono>

namespace OpenVDS
{
class VolumeDataPageImpl;
class VolumeDataLayer;
class VolumeDataAccessManagerImpl;
struct Error;
struct DataBlock;

class VolumeDataPageAccessorImpl : public VolumeDataPageAccessor
{
private:
  VolumeDataAccessManagerImpl *m_accessManager;
  VolumeDataLayer const *m_layer;
  int m_pagesFound;
  int m_pagesRead;
  int m_pagesWritten;
  int m_maxPages;
  std::atomic_int m_references;
  bool m_isReadWrite;
  bool m_isCommitInProgress;
  std::atomic<std::chrono::time_point<std::chrono::steady_clock>> m_lastUsed;
  std::list<VolumeDataPageImpl *> m_pages;
  std::condition_variable m_pageReadCondition;
  std::condition_variable m_commitFinishedCondition;

  public:
  std::mutex m_pagesMutex;
  IntrusiveListNode<VolumeDataPageAccessorImpl> m_volumeDataPageAccessorListNode;

private:
  void LimitPageListSize(int maxPages, std::unique_lock<std::mutex> &pageListMutexLock);

public:
  VolumeDataPageAccessorImpl(VolumeDataAccessManagerImpl *acccessManager, VolumeDataLayer const* layer, int maxPages, bool IsReadWrite);
  ~VolumeDataPageAccessorImpl();

  VolumeDataLayout const* GetLayout() const override;
  VolumeDataLayer const * GetLayer() const { return m_layer; }

  int   GetLOD() const override;
  int   GetChannelIndex() const override;
  VolumeDataChannelDescriptor const& GetChannelDescriptor() const override;
  void  GetNumSamples(int(&numSamples)[Dimensionality_Max]) const override;

  int64_t GetChunkCount() const override;
  void    GetChunkMinMax(int64_t chunk, int(&min)[Dimensionality_Max], int(&max)[Dimensionality_Max]) const override;
  void    GetChunkMinMaxExcludingMargin(int64_t iChunk, int(&minExcludingMargin)[Dimensionality_Max], int(&maxExcludingMargin)[Dimensionality_Max]) const override;
  int64_t GetChunkIndex(const int(&position)[Dimensionality_Max]) const override;
  int64_t GetMappedChunkIndex(int64_t primaryChannelChunkIndex) const override;
  int64_t GetPrimaryChannelChunkIndex(int64_t chunkIndex) const override;

  int   AddReference() override;
  int   RemoveReference() override;
  int   GetReferenceCount() const { return m_references.load(); }

  VolumeDataPage* PrepareReadPage(int64_t chunk, Error &error);
  bool ReadPreparedPaged(VolumeDataPage *page);
  void CancelPreparedReadPage(VolumeDataPage *page);

  int   GetMaxPages() override;
  void  SetMaxPages(int maxPages) override;

  VolumeDataPage *CreatePage(int64_t chunk) override;
  VolumeDataPage *ReadPage(int64_t chunk) override;
  VolumeDataPage *ReadPageAtPosition(const int (&position)[Dimensionality_Max]) override;
 
  int64_t RequestWritePage(int64_t chunk, const DataBlock &dataBlock, const std::vector<uint8_t> &data);

  void  Commit() override;

  bool IsReadWrite() const { return m_isReadWrite; }

  VolumeDataAccessManagerImpl *GetManager() const { return m_accessManager; }

  void SetLastUsed(std::chrono::time_point<std::chrono::steady_clock> lastUsed) { m_lastUsed = lastUsed; }
  std::chrono::time_point<std::chrono::steady_clock> GetLastUsed() const { return m_lastUsed.load(); }

};

}
#endif //VOLUMEDATAPAGEACCESSORIMPL_H
