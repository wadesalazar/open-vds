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

#include "VolumeDataAccessManagerImpl.h"

#include "VDS.h"
#include "VolumeDataPageAccessorImpl.h"
#include <OpenVDS/ValueConversion.h>
#include <OpenVDS/VolumeSampler.h>
#include "ParseVDSJson.h"
#include "VolumeDataStore.h"
#include "VolumeDataHash.h"
#include "VolumeDataLayoutImpl.h"

#include <algorithm>
#include <inttypes.h>
#include <assert.h>
#include <atomic>
#include <fmt/format.h>

namespace OpenVDS
{

static VolumeDataLayer *GetVolumeDataLayer(VolumeDataLayoutImpl const *layout, DimensionsND dimension, int channel, int lod, bool isAllowFailure)
{
  if(!layout)
  {
    throw std::runtime_error("Volume data layout is NULL, this is usually because the VDS setup is invalid");
    return nullptr;
  }

  if(channel > layout->GetChannelCount())
  {
    throw std::runtime_error("Specified channel doesn't exist");
    return nullptr;
  }

  VolumeDataLayer *layer = layout->GetBaseLayer(DimensionGroupUtil::GetDimensionGroupFromDimensionsND(dimension), channel);

  if(!layer && !isAllowFailure)
  {
    throw std::runtime_error("Specified dimension group doesn't exist");
    return nullptr;
  }

  while(layer && layer->GetLOD() < lod)
  {
    layer = layer->GetParentLayer();
  }

  if((!layer || layer->GetLayerType() == VolumeDataLayer::Virtual) && !isAllowFailure)
  {
    throw std::runtime_error("Specified LOD doesn't exist");
  }

  assert(layer || isAllowFailure);
  return (layer && layer->GetLayerType() != VolumeDataLayer::Virtual) ? layer : nullptr;
}

VolumeDataAccessManagerImpl::VolumeDataAccessManagerImpl(VDS &vds)
  : m_vds(vds)
  , m_requestProcessor(*this)
{
}

VolumeDataAccessManagerImpl::~VolumeDataAccessManagerImpl()
{
}

VolumeDataLayoutImpl const* VolumeDataAccessManagerImpl::GetVolumeDataLayout() const
{
  return m_vds.volumeDataLayout.get();
}

VolumeDataStore * VolumeDataAccessManagerImpl::GetVolumeDataStore()
{
  return m_vds.volumeDataStore.get();
}

VDSProduceStatus VolumeDataAccessManagerImpl::GetVDSProduceStatus(VolumeDataLayout const *volumeDataLayout, DimensionsND dimensionsND, int lod, int channel) const
{
  VolumeDataLayer *layer = GetVolumeDataLayer(static_cast<VolumeDataLayoutImpl const *>(volumeDataLayout), dimensionsND, channel, lod, true);

  if(layer && layer->GetProduceStatus() == VolumeDataLayer::ProduceStatus_Normal)
  {
    return VDSProduceStatus::Normal;
  }
  else if(layer && layer->GetProduceStatus() == VolumeDataLayer::ProduceStatus_Remapped)
  {
    return VDSProduceStatus::Remapped;
  }
  else
  {
    return VDSProduceStatus::Unavailable;
  }
}

VolumeDataPageAccessor* VolumeDataAccessManagerImpl::CreateVolumeDataPageAccessor(VolumeDataLayout const* volumeDataLayout, DimensionsND dimensionsND, int lod, int channel, int maxPages, AccessMode accessMode)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  VolumeDataLayer *layer = GetVolumeDataLayer(static_cast<VolumeDataLayoutImpl const *>(volumeDataLayout), dimensionsND, channel, lod, true);
  if (!layer)
    return nullptr;

  if(accessMode == VolumeDataAccessManager::AccessMode_Create)
  {
    layer->SetProduceStatus(VolumeDataLayer::ProduceStatus_Normal);
    bool success = GetVolumeDataStore()->AddLayer(layer);
    if(!success)
    {
      throw std::runtime_error("Failed to create layer");
    }
  }
  else if (layer->GetProduceStatus() == VolumeDataLayer::ProduceStatus_Unavailable)
  {
    throw std::runtime_error("The accessed dimension group or channel is unavailable (check produce status on VDS before accessing data)");
  }

  VolumeDataPageAccessorImpl *accessor = new VolumeDataPageAccessorImpl(this, layer, maxPages, accessMode != VolumeDataAccessManager::AccessMode_ReadOnly);
  m_volumeDataPageAccessorList.InsertLast(accessor);
  return accessor;
}

void  VolumeDataAccessManagerImpl::DestroyVolumeDataPageAccessor(VolumeDataPageAccessor* volumeDataPageAccessor)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  VolumeDataPageAccessorImpl *pageAccessorImpl = static_cast<VolumeDataPageAccessorImpl *>(volumeDataPageAccessor);
  m_volumeDataPageAccessorList.Remove(pageAccessorImpl);
  delete  pageAccessorImpl;
}

bool VolumeDataAccessManagerImpl::IsCompleted(int64_t requestID)
{
  return m_requestProcessor.IsCompleted(requestID);
}
bool VolumeDataAccessManagerImpl::IsCanceled(int64_t requestID)
{
  return m_requestProcessor.IsCanceled(requestID);
}
bool VolumeDataAccessManagerImpl::WaitForCompletion(int64_t requestID, int millisecondsBeforeTimeout)
{
  return m_requestProcessor.WaitForCompletion(requestID, millisecondsBeforeTimeout);
}
void VolumeDataAccessManagerImpl::Cancel(int64_t requestID)
{
  m_requestProcessor.Cancel(requestID);
}
float VolumeDataAccessManagerImpl::GetCompletionFactor(int64_t requestID)
{
  return m_requestProcessor.GetCompletionFactor(requestID);
}

void VolumeDataAccessManagerImpl::FlushUploadQueue()
{
  GetVolumeDataStore()->Flush();
}

void VolumeDataAccessManagerImpl::ClearUploadErrors()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_uploadErrors.erase(m_uploadErrors.begin(), m_uploadErrors.begin() + m_currentErrorIndex);
  m_currentErrorIndex = 0;
}

void VolumeDataAccessManagerImpl::ForceClearAllUploadErrors()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_uploadErrors.clear();
  m_currentErrorIndex = 0;
}


int32_t VolumeDataAccessManagerImpl::UploadErrorCount()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  return int32_t(m_uploadErrors.size() - m_currentErrorIndex);
}

void VolumeDataAccessManagerImpl::GetCurrentUploadError(const char** objectId, int32_t* errorCode, const char** errorString)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_currentErrorIndex >= m_uploadErrors.size())
  {
    if (objectId)
      *objectId = nullptr;
    if (errorCode)
      *errorCode = 0;
    if (errorString)
      *errorString = nullptr;
    return;
  }

  const auto &error = m_uploadErrors[m_currentErrorIndex];
  m_currentErrorIndex++;
  if (objectId)
    *objectId = error->urlObject.c_str();
  if (errorCode)
    *errorCode = error->error.code;
  if (errorString)
    *errorString = error->error.string.c_str();
}

void VolumeDataAccessManagerImpl::GetCurrentDownloadError(int* errorCode, const char** errorString)
{
  *errorCode = m_currentDownloadError.code;
  *errorString = m_currentDownloadError.string.c_str();
}

}
