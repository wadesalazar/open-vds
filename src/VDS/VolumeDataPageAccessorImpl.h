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

namespace OpenVDS
{
class VolumeDataPageImpl;
class VolumeDataLayer;
class VolumeDataAccessManagerImpl;
struct DataBlock;

class VolumeDataPageAccessorImpl : public VolumeDataPageAccessor
{
private:
  VolumeDataAccessManagerImpl *m_accessManager;
  VolumeDataLayer *m_layer;
  int m_pagesFound;
  int m_pagesRead;
  int m_pagesWritten;
  int m_maxPages;
  int m_references;
  bool m_isReadWrite;
  bool m_isCommitInProgress;
  std::list<VolumeDataPageImpl *> m_pages;
  std::condition_variable m_pageReadCondition;
  std::condition_variable m_commitFinishedCondition;

  public:
  std::mutex m_pagesMutex;
  IntrusiveListNode<VolumeDataPageAccessorImpl> m_volumeDataPageAccessorListNode;

private:
  void limitPageListSize(int maxPages, std::unique_lock<std::mutex> &pageListMutexLock);

public:
  VolumeDataPageAccessorImpl(VolumeDataAccessManagerImpl *acccessManager, VolumeDataLayer* layer, int maxPages, bool isReadWrite);

  VolumeDataLayout const* getLayout() const override;
  VolumeDataLayer const * getLayer() const { return m_layer; }

  int   getLOD() const override;
  int   getChannelIndex() const override;
  VolumeDataChannelDescriptor const& getChannelDescriptor() const override;
  void  getNumSamples(int(&numSamples)[Dimensionality_Max]) const override;

  int64_t getChunkCount() const override;
  void  getChunkMinMax(int64_t chunk, int(&min)[Dimensionality_Max], int(&max)[Dimensionality_Max]) const override;
  void  getChunkMinMaxExcludingMargin(int64_t iChunk, int(&minExcludingMargin)[Dimensionality_Max], int(&maxExcludingMargin)[Dimensionality_Max]) const override;
  int64_t getChunkIndex(const int(&position)[Dimensionality_Max]) const override;

  int   addReference() override;
  int   removeReference() override;

  VolumeDataPage* prepareReadPage(int64_t chunk, bool *needToCallReadPreparePage);
  bool readPreparedPaged(VolumeDataPage *page);

  int   getMaxPages() override;
  void  setMaxPages(int maxPages) override;

  VolumeDataPage *createPage(int64_t chunk) override;
  VolumeDataPage *readPage(int64_t chunk) override;
 
  int64_t requestWritePage(int64_t chunk, const DataBlock &dataBlock, const std::vector<uint8_t> &data);

  void  commit() override;

  bool isReadWrite() const { return m_isReadWrite; }

  VolumeDataAccessManagerImpl *getManager() const { return m_accessManager; }
};

}
#endif //VOLUMEDATAPAGEACCESSORIMPL_H
