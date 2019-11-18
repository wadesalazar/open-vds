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

#include "PyVolumeDataAccess.h"

using namespace native;

void 
PyVolumeDataAccess::initModule(py::module& m)
{
#if 1
//AUTOGEN-BEGIN
  // VolumeDataAccessor
  py::class_<VolumeDataAccessor, std::unique_ptr<VolumeDataAccessor, py::nodelete>> 
    VolumeDataAccessor_(m,"VolumeDataAccessor", OPENVDS_DOCSTRING(VolumeDataAccessor));

  VolumeDataAccessor_.def("getManager"                  , static_cast<native::VolumeDataAccessManager &(VolumeDataAccessor::*)()>(&VolumeDataAccessor::GetManager), OPENVDS_DOCSTRING(VolumeDataAccessor_GetManager));
  VolumeDataAccessor_.def("getLayout"                   , static_cast<const native::VolumeDataLayout *(VolumeDataAccessor::*)()>(&VolumeDataAccessor::GetLayout), OPENVDS_DOCSTRING(VolumeDataAccessor_GetLayout));

  // VolumeDataAccessor::IndexOutOfRangeException
  py::class_<VolumeDataAccessor::IndexOutOfRangeException> 
    VolumeDataAccessor_IndexOutOfRangeException_(VolumeDataAccessor_,"VolumeDataAccessor::IndexOutOfRangeException", OPENVDS_DOCSTRING(VolumeDataAccessor_IndexOutOfRangeException));

  // VolumeDataAccessManager
  py::class_<VolumeDataAccessManager, std::unique_ptr<VolumeDataAccessManager, py::nodelete>> 
    VolumeDataAccessManager_(m,"VolumeDataAccessManager", OPENVDS_DOCSTRING(VolumeDataAccessManager));

  VolumeDataAccessManager_.def("getVolumeDataLayout"         , static_cast<const native::VolumeDataLayout *(VolumeDataAccessManager::*)() const>(&VolumeDataAccessManager::GetVolumeDataLayout), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetVolumeDataLayout));
  VolumeDataAccessManager_.def("createVolumeDataPageAccessor", static_cast<native::VolumeDataPageAccessor *(VolumeDataAccessManager::*)(const native::VolumeDataLayout *, native::DimensionsND, int, int, int, native::VolumeDataAccessManager::AccessMode)>(&VolumeDataAccessManager::CreateVolumeDataPageAccessor), OPENVDS_DOCSTRING(VolumeDataAccessManager_CreateVolumeDataPageAccessor));
  VolumeDataAccessManager_.def("destroyVolumeDataPageAccessor", static_cast<void(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *)>(&VolumeDataAccessManager::DestroyVolumeDataPageAccessor), OPENVDS_DOCSTRING(VolumeDataAccessManager_DestroyVolumeDataPageAccessor));
  VolumeDataAccessManager_.def("destroyVolumeDataAccessor"   , static_cast<void(VolumeDataAccessManager::*)(native::VolumeDataAccessor *)>(&VolumeDataAccessManager::DestroyVolumeDataAccessor), OPENVDS_DOCSTRING(VolumeDataAccessManager_DestroyVolumeDataAccessor));
  VolumeDataAccessManager_.def("cloneVolumeDataAccessor"     , static_cast<native::VolumeDataAccessor *(VolumeDataAccessManager::*)(const native::VolumeDataAccessor &)>(&VolumeDataAccessManager::CloneVolumeDataAccessor), OPENVDS_DOCSTRING(VolumeDataAccessManager_CloneVolumeDataAccessor));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessor1Bit", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, bool> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessor1Bit), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessor1Bit));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorU8", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, uint8_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessorU8), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorU8));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorU16", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, uint16_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessorU16), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorU16));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorU32", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, uint32_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessorU32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorU32));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorU64", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, uint64_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessorU64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorU64));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorR32", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, float> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessorR32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorR64", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, double> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessorR64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessor1Bit", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, bool> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessor1Bit), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessor1Bit));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorU8", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, uint8_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessorU8), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorU8));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorU16", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, uint16_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessorU16), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorU16));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorU32", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, uint32_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessorU32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorU32));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorU64", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, uint64_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessorU64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorU64));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorR32", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, float> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessorR32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorR64", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, double> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessorR64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessor1Bit", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, bool> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessor1Bit), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessor1Bit));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorU8", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, uint8_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessorU8), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorU8));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorU16", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, uint16_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessorU16), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorU16));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorU32", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, uint32_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessorU32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorU32));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorU64", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, uint64_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessorU64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorU64));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorR32", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, float> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessorR32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorR64", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, double> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessorR64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create2DInterpolatingVolumeDataAccessorR32", static_cast<VolumeDataReadAccessor<native::FloatVector2, float> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float, native::InterpolationMethod)>(&VolumeDataAccessManager::Create2DInterpolatingVolumeDataAccessorR32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DInterpolatingVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create2DInterpolatingVolumeDataAccessorR64", static_cast<VolumeDataReadAccessor<native::FloatVector2, double> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float, native::InterpolationMethod)>(&VolumeDataAccessManager::Create2DInterpolatingVolumeDataAccessorR64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DInterpolatingVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create3DInterpolatingVolumeDataAccessorR32", static_cast<VolumeDataReadAccessor<native::FloatVector3, float> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float, native::InterpolationMethod)>(&VolumeDataAccessManager::Create3DInterpolatingVolumeDataAccessorR32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DInterpolatingVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create3DInterpolatingVolumeDataAccessorR64", static_cast<VolumeDataReadAccessor<native::FloatVector3, double> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float, native::InterpolationMethod)>(&VolumeDataAccessManager::Create3DInterpolatingVolumeDataAccessorR64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DInterpolatingVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create4DInterpolatingVolumeDataAccessorR32", static_cast<VolumeDataReadAccessor<native::FloatVector4, float> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float, native::InterpolationMethod)>(&VolumeDataAccessManager::Create4DInterpolatingVolumeDataAccessorR32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DInterpolatingVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create4DInterpolatingVolumeDataAccessorR64", static_cast<VolumeDataReadAccessor<native::FloatVector4, double> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float, native::InterpolationMethod)>(&VolumeDataAccessManager::Create4DInterpolatingVolumeDataAccessorR64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DInterpolatingVolumeDataAccessorR64));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestVolumeSubset"         , static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], VolumeDataChannelDescriptor::Format)>(&VolumeDataAccessManager::RequestVolumeSubset), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestVolumeSubset"         , static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], VolumeDataChannelDescriptor::Format, float)>(&VolumeDataAccessManager::RequestVolumeSubset), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset_2));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestProjectedVolumeSubset", static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], const native::FloatVector4 &, native::DimensionsND, VolumeDataChannelDescriptor::Format, native::InterpolationMethod)>(&VolumeDataAccessManager::RequestProjectedVolumeSubset), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestProjectedVolumeSubset", static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], const native::FloatVector4 &, native::DimensionsND, VolumeDataChannelDescriptor::Format, native::InterpolationMethod, float)>(&VolumeDataAccessManager::RequestProjectedVolumeSubset), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset_2));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestVolumeSamples"        , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod)>(&VolumeDataAccessManager::RequestVolumeSamples), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestVolumeSamples"        , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod, float)>(&VolumeDataAccessManager::RequestVolumeSamples), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples_2));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestVolumeTraces"         , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod, int)>(&VolumeDataAccessManager::RequestVolumeTraces), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestVolumeTraces"         , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod, int, float)>(&VolumeDataAccessManager::RequestVolumeTraces), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces_2));
  VolumeDataAccessManager_.def("prefetchVolumeChunk"         , static_cast<int64_t(VolumeDataAccessManager::*)(const native::VolumeDataLayout *, native::DimensionsND, int, int, int64_t)>(&VolumeDataAccessManager::PrefetchVolumeChunk), OPENVDS_DOCSTRING(VolumeDataAccessManager_PrefetchVolumeChunk));
  VolumeDataAccessManager_.def("isCompleted"                 , static_cast<bool(VolumeDataAccessManager::*)(int64_t)>(&VolumeDataAccessManager::IsCompleted), OPENVDS_DOCSTRING(VolumeDataAccessManager_IsCompleted));
  VolumeDataAccessManager_.def("isCanceled"                  , static_cast<bool(VolumeDataAccessManager::*)(int64_t)>(&VolumeDataAccessManager::IsCanceled), OPENVDS_DOCSTRING(VolumeDataAccessManager_IsCanceled));
  VolumeDataAccessManager_.def("waitForCompletion"           , static_cast<bool(VolumeDataAccessManager::*)(int64_t, int)>(&VolumeDataAccessManager::WaitForCompletion), OPENVDS_DOCSTRING(VolumeDataAccessManager_WaitForCompletion));
  VolumeDataAccessManager_.def("cancel"                      , static_cast<void(VolumeDataAccessManager::*)(int64_t)>(&VolumeDataAccessManager::Cancel), OPENVDS_DOCSTRING(VolumeDataAccessManager_Cancel));
  VolumeDataAccessManager_.def("getCompletionFactor"         , static_cast<float(VolumeDataAccessManager::*)(int64_t)>(&VolumeDataAccessManager::GetCompletionFactor), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetCompletionFactor));
  VolumeDataAccessManager_.def("flushUploadQueue"            , static_cast<void(VolumeDataAccessManager::*)()>(&VolumeDataAccessManager::FlushUploadQueue), OPENVDS_DOCSTRING(VolumeDataAccessManager_FlushUploadQueue));
  VolumeDataAccessManager_.def("clearUploadErrors"           , static_cast<void(VolumeDataAccessManager::*)()>(&VolumeDataAccessManager::ClearUploadErrors), OPENVDS_DOCSTRING(VolumeDataAccessManager_ClearUploadErrors));
  VolumeDataAccessManager_.def("forceClearAllUploadErrors"   , static_cast<void(VolumeDataAccessManager::*)()>(&VolumeDataAccessManager::ForceClearAllUploadErrors), OPENVDS_DOCSTRING(VolumeDataAccessManager_ForceClearAllUploadErrors));
  VolumeDataAccessManager_.def("uploadErrorCount"            , static_cast<int32_t(VolumeDataAccessManager::*)()>(&VolumeDataAccessManager::UploadErrorCount), OPENVDS_DOCSTRING(VolumeDataAccessManager_UploadErrorCount));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("getCurrentUploadError"       , static_cast<void(VolumeDataAccessManager::*)(const char **, int32_t *, const char **)>(&VolumeDataAccessManager::GetCurrentUploadError), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetCurrentUploadError));

  py::enum_<VolumeDataAccessManager::AccessMode> 
    VolumeDataAccessManager_AccessMode_(VolumeDataAccessManager_,"AccessMode", OPENVDS_DOCSTRING(VolumeDataAccessManager_AccessMode));

  VolumeDataAccessManager_AccessMode_.value("AccessMode_ReadOnly"         , VolumeDataAccessManager::AccessMode::AccessMode_ReadOnly, OPENVDS_DOCSTRING(VolumeDataAccessManager_AccessMode_AccessMode_ReadOnly));
  VolumeDataAccessManager_AccessMode_.value("AccessMode_ReadWrite"        , VolumeDataAccessManager::AccessMode::AccessMode_ReadWrite, OPENVDS_DOCSTRING(VolumeDataAccessManager_AccessMode_AccessMode_ReadWrite));
  VolumeDataAccessManager_AccessMode_.value("AccessMode_Create"           , VolumeDataAccessManager::AccessMode::AccessMode_Create, OPENVDS_DOCSTRING(VolumeDataAccessManager_AccessMode_AccessMode_Create));

  // VolumeDataPage
  py::class_<VolumeDataPage, std::unique_ptr<VolumeDataPage, py::nodelete>> 
    VolumeDataPage_(m,"VolumeDataPage", OPENVDS_DOCSTRING(VolumeDataPage));

// AUTOGENERATE FAIL :   VolumeDataPage_.def("getMinMax"                   , static_cast<void(VolumeDataPage::*)(int (&)[6], int (&)[6]) const>(&VolumeDataPage::GetMinMax), OPENVDS_DOCSTRING(VolumeDataPage_GetMinMax));
// AUTOGENERATE FAIL :   VolumeDataPage_.def("getMinMaxExcludingMargin"    , static_cast<void(VolumeDataPage::*)(int (&)[6], int (&)[6]) const>(&VolumeDataPage::GetMinMaxExcludingMargin), OPENVDS_DOCSTRING(VolumeDataPage_GetMinMaxExcludingMargin));
// AUTOGENERATE FAIL :   VolumeDataPage_.def("getBuffer"                   , static_cast<const void *(VolumeDataPage::*)(int (&)[6])>(&VolumeDataPage::GetBuffer), OPENVDS_DOCSTRING(VolumeDataPage_GetBuffer));
// AUTOGENERATE FAIL :   VolumeDataPage_.def("getWritableBuffer"           , static_cast<void *(VolumeDataPage::*)(int (&)[6])>(&VolumeDataPage::GetWritableBuffer), OPENVDS_DOCSTRING(VolumeDataPage_GetWritableBuffer));
// AUTOGENERATE FAIL :   VolumeDataPage_.def("updateWrittenRegion"         , static_cast<void(VolumeDataPage::*)(const int (&)[6], const int (&)[6])>(&VolumeDataPage::UpdateWrittenRegion), OPENVDS_DOCSTRING(VolumeDataPage_UpdateWrittenRegion));
  VolumeDataPage_.def("release"                     , static_cast<void(VolumeDataPage::*)()>(&VolumeDataPage::Release), OPENVDS_DOCSTRING(VolumeDataPage_Release));

  // VolumeDataPageAccessor
  py::class_<VolumeDataPageAccessor, std::unique_ptr<VolumeDataPageAccessor, py::nodelete>> 
    VolumeDataPageAccessor_(m,"VolumeDataPageAccessor", OPENVDS_DOCSTRING(VolumeDataPageAccessor));

  VolumeDataPageAccessor_.def("getLayout"                   , static_cast<const native::VolumeDataLayout *(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetLayout), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLayout));
  VolumeDataPageAccessor_.def("getLOD"                      , static_cast<int(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetLOD), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLOD));
  VolumeDataPageAccessor_.def("getChannelIndex"             , static_cast<int(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetChannelIndex), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelIndex));
  VolumeDataPageAccessor_.def("getChannelDescriptor"        , static_cast<const native::VolumeDataChannelDescriptor &(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetChannelDescriptor), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelDescriptor));
// AUTOGENERATE FAIL :   VolumeDataPageAccessor_.def("getNumSamples"               , static_cast<void(VolumeDataPageAccessor::*)(int (&)[6]) const>(&VolumeDataPageAccessor::GetNumSamples), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetNumSamples));
  VolumeDataPageAccessor_.def("getChunkCount"               , static_cast<int64_t(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetChunkCount), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkCount));
// AUTOGENERATE FAIL :   VolumeDataPageAccessor_.def("getChunkMinMax"              , static_cast<void(VolumeDataPageAccessor::*)(int64_t, int (&)[6], int (&)[6]) const>(&VolumeDataPageAccessor::GetChunkMinMax), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkMinMax));
// AUTOGENERATE FAIL :   VolumeDataPageAccessor_.def("getChunkMinMaxExcludingMargin", static_cast<void(VolumeDataPageAccessor::*)(int64_t, int (&)[6], int (&)[6]) const>(&VolumeDataPageAccessor::GetChunkMinMaxExcludingMargin), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkMinMaxExcludingMargin));
// AUTOGENERATE FAIL :   VolumeDataPageAccessor_.def("getChunkIndex"               , static_cast<int64_t(VolumeDataPageAccessor::*)(const int (&)[6]) const>(&VolumeDataPageAccessor::GetChunkIndex), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkIndex));
  VolumeDataPageAccessor_.def("addReference"                , static_cast<int(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::AddReference), OPENVDS_DOCSTRING(VolumeDataPageAccessor_AddReference));
  VolumeDataPageAccessor_.def("removeReference"             , static_cast<int(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::RemoveReference), OPENVDS_DOCSTRING(VolumeDataPageAccessor_RemoveReference));
  VolumeDataPageAccessor_.def("getMaxPages"                 , static_cast<int(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::GetMaxPages), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetMaxPages));
  VolumeDataPageAccessor_.def("setMaxPages"                 , static_cast<void(VolumeDataPageAccessor::*)(int)>(&VolumeDataPageAccessor::SetMaxPages), OPENVDS_DOCSTRING(VolumeDataPageAccessor_SetMaxPages));
  VolumeDataPageAccessor_.def("createPage"                  , static_cast<native::VolumeDataPage *(VolumeDataPageAccessor::*)(int64_t)>(&VolumeDataPageAccessor::CreatePage), OPENVDS_DOCSTRING(VolumeDataPageAccessor_CreatePage));
  VolumeDataPageAccessor_.def("readPage"                    , static_cast<native::VolumeDataPage *(VolumeDataPageAccessor::*)(int64_t)>(&VolumeDataPageAccessor::ReadPage), OPENVDS_DOCSTRING(VolumeDataPageAccessor_ReadPage));
// AUTOGENERATE FAIL :   VolumeDataPageAccessor_.def("readPageAtPosition"          , static_cast<native::VolumeDataPage *(VolumeDataPageAccessor::*)(const int (&)[6])>(&VolumeDataPageAccessor::ReadPageAtPosition), OPENVDS_DOCSTRING(VolumeDataPageAccessor_ReadPageAtPosition));
  VolumeDataPageAccessor_.def("commit"                      , static_cast<void(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::Commit), OPENVDS_DOCSTRING(VolumeDataPageAccessor_Commit));

//AUTOGEN-END
#endif
}

