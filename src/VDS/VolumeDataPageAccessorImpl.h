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

#include "VolumeDataAccess.h"

#include <list>
#include <mutex>

namespace OpenVDS
{
class VolumeDataPageImpl;
class VolumeDataLayer;

class VolumeDataPageAccessorImpl : public VolumeDataPageAccessor
{
private:
  VolumeDataLayer *m_layer;
  int m_maxPages;
  int m_references;
  bool m_isReadWrite;
  std::list<VolumeDataPageImpl *> m_pages;
  std::mutex m_pagesMutex;
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