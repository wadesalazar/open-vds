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

#include <list>
#include <mutex>
#include <condition_variable>

namespace OpenVDS
{
class VolumeDataPageImpl;
class VolumeDataLayer;

class VolumeDataPageAccessorImpl : public VolumeDataPageAccessor
{
private:
  VolumeDataLayer *m_layer;
  int m_pagesFound;
  int m_pagesRead;
  int m_pagesWritten;
  int m_currentPages;
  int m_maxPages;
  int m_references;
  bool m_isReadWrite;
  bool m_isCommitInProgress;
  std::list<VolumeDataPageImpl *> m_pages;
  std::mutex m_pagesMutex;
  std::condition_variable m_pageReadCondition;
  std::condition_variable m_commitFinishedCondition;

  void limitPageListSize(int maxPages, std::unique_lock<std::mutex> &pageListMutexLock);
  void commitPage(VolumeDataPage *page, std::unique_lock<std::mutex> &pageListMutexLock);
public:
  VolumeDataPageAccessorImpl(VolumeDataLayer* layer, int maxPages, bool isReadWrite);

  VolumeDataLayout const* getLayout() const override;

  int   getLod() const override;
  int   getChannelIndex() const override;
  VolumeDataChannelDescriptor const& getChannelDescriptor() const override;
  void  getNumSamples(int(&numSamples)[Dimensionality_Max]) const override;

  int64_t getChunkCount() const override;
  void  getChunkMinMax(int64_t chunk, int(&min)[Dimensionality_Max], int(&max)[Dimensionality_Max]) const override;
  void  getChunkMinMaxExcludingMargin(int64_t iChunk, int(&minExcludingMargin)[Dimensionality_Max], int(&maxExcludingMargin)[Dimensionality_Max]) const override;
  int64_t getChunkIndex(const int(&position)[Dimensionality_Max]) const override;

  int   addReference() override;
  int   removeReference() override;

  VolumeDataPage* readPageAtPosition(const int(&anPosition)[Dimensionality_Max]) override;

  void  commit() override;
};

}
#endif //VOLUMEDATAPAGEACCESSORIMPL_H