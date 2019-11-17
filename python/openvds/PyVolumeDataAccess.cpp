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

void 
PyVolumeDataAccess::initModule(py::module& m)
{
#if 0
//AUTOGEN-BEGIN
  // VolumeDataPageAccessor
  py::class_<native::VolumeDataPageAccessor> 
    VolumeDataPageAccessor_(m,"VolumeDataPageAccessor", OPENVDS_DOCSTRING(VolumeDataPageAccessor));

  // VolumeDataAccessManager
  py::class_<native::VolumeDataAccessManager> 
    VolumeDataAccessManager_(m,"VolumeDataAccessManager", OPENVDS_DOCSTRING(VolumeDataAccessManager));

  // VolumeDataAccessor
  py::class_<native::VolumeDataAccessor> 
    VolumeDataAccessor_(m,"VolumeDataAccessor", OPENVDS_DOCSTRING(VolumeDataAccessor));

  VolumeDataAccessor_.def("getManager"                  , static_cast<OpenVDS::VolumeDataAccessManager &(*)()>(&native::GetManager), OPENVDS_DOCSTRING(VolumeDataAccessor_GetManager));
  VolumeDataAccessor_.def("getLayout"                   , static_cast<const OpenVDS::VolumeDataLayout *(*)()>(&native::GetLayout), OPENVDS_DOCSTRING(VolumeDataAccessor_GetLayout));

  // VolumeDataAccessor::IndexOutOfRangeException
  py::class_<native::VolumeDataAccessor::IndexOutOfRangeException> 
    VolumeDataAccessor_IndexOutOfRangeException_(VolumeDataAccessor_,"VolumeDataAccessor::IndexOutOfRangeException", OPENVDS_DOCSTRING(VolumeDataAccessor_IndexOutOfRangeException));

  // VolumeDataAccessManager
  py::class_<native::VolumeDataAccessManager> 
    VolumeDataAccessManager_(m,"VolumeDataAccessManager", OPENVDS_DOCSTRING(VolumeDataAccessManager));

  VolumeDataAccessManager_.def("getVolumeDataLayout"         , static_cast<const OpenVDS::VolumeDataLayout *(*)()>(&native::GetVolumeDataLayout), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetVolumeDataLayout));
  VolumeDataAccessManager_.def("createVolumeDataPageAccessor", static_cast<OpenVDS::VolumeDataPageAccessor *(*)(const OpenVDS::VolumeDataLayout *, OpenVDS::DimensionsND, int, int, int, OpenVDS::VolumeDataAccessManager::AccessMode)>(&native::CreateVolumeDataPageAccessor), OPENVDS_DOCSTRING(VolumeDataAccessManager_CreateVolumeDataPageAccessor));
  VolumeDataAccessManager_.def("destroyVolumeDataPageAccessor", static_cast<void(*)(OpenVDS::VolumeDataPageAccessor *)>(&native::DestroyVolumeDataPageAccessor), OPENVDS_DOCSTRING(VolumeDataAccessManager_DestroyVolumeDataPageAccessor));
  VolumeDataAccessManager_.def("destroyVolumeDataAccessor"   , static_cast<void(*)(OpenVDS::VolumeDataAccessor *)>(&native::DestroyVolumeDataAccessor), OPENVDS_DOCSTRING(VolumeDataAccessManager_DestroyVolumeDataAccessor));
  VolumeDataAccessManager_.def("cloneVolumeDataAccessor"     , static_cast<OpenVDS::VolumeDataAccessor *(*)(const OpenVDS::VolumeDataAccessor &)>(&native::CloneVolumeDataAccessor), OPENVDS_DOCSTRING(VolumeDataAccessManager_CloneVolumeDataAccessor));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessor1Bit", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, bool> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create2DVolumeDataAccessor1Bit), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessor1Bit));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorU8", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, uint8_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create2DVolumeDataAccessorU8), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorU8));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorU16", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, uint16_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create2DVolumeDataAccessorU16), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorU16));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorU32", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, uint32_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create2DVolumeDataAccessorU32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorU32));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorU64", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, uint64_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create2DVolumeDataAccessorU64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorU64));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorR32", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, float> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create2DVolumeDataAccessorR32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorR64", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, double> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create2DVolumeDataAccessorR64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessor1Bit", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, bool> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create3DVolumeDataAccessor1Bit), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessor1Bit));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorU8", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, uint8_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create3DVolumeDataAccessorU8), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorU8));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorU16", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, uint16_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create3DVolumeDataAccessorU16), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorU16));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorU32", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, uint32_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create3DVolumeDataAccessorU32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorU32));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorU64", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, uint64_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create3DVolumeDataAccessorU64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorU64));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorR32", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, float> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create3DVolumeDataAccessorR32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorR64", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, double> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create3DVolumeDataAccessorR64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessor1Bit", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, bool> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create4DVolumeDataAccessor1Bit), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessor1Bit));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorU8", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, uint8_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create4DVolumeDataAccessorU8), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorU8));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorU16", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, uint16_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create4DVolumeDataAccessorU16), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorU16));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorU32", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, uint32_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create4DVolumeDataAccessorU32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorU32));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorU64", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, uint64_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create4DVolumeDataAccessorU64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorU64));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorR32", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, float> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create4DVolumeDataAccessorR32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorR64", static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, double> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::Create4DVolumeDataAccessorR64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create2DInterpolatingVolumeDataAccessorR32", static_cast<VolumeDataReadAccessor<OpenVDS::FloatVector2, float> *(*)(OpenVDS::VolumeDataPageAccessor *, float, OpenVDS::InterpolationMethod)>(&native::Create2DInterpolatingVolumeDataAccessorR32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DInterpolatingVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create2DInterpolatingVolumeDataAccessorR64", static_cast<VolumeDataReadAccessor<OpenVDS::FloatVector2, double> *(*)(OpenVDS::VolumeDataPageAccessor *, float, OpenVDS::InterpolationMethod)>(&native::Create2DInterpolatingVolumeDataAccessorR64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DInterpolatingVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create3DInterpolatingVolumeDataAccessorR32", static_cast<VolumeDataReadAccessor<OpenVDS::FloatVector3, float> *(*)(OpenVDS::VolumeDataPageAccessor *, float, OpenVDS::InterpolationMethod)>(&native::Create3DInterpolatingVolumeDataAccessorR32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DInterpolatingVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create3DInterpolatingVolumeDataAccessorR64", static_cast<VolumeDataReadAccessor<OpenVDS::FloatVector3, double> *(*)(OpenVDS::VolumeDataPageAccessor *, float, OpenVDS::InterpolationMethod)>(&native::Create3DInterpolatingVolumeDataAccessorR64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DInterpolatingVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create4DInterpolatingVolumeDataAccessorR32", static_cast<VolumeDataReadAccessor<OpenVDS::FloatVector4, float> *(*)(OpenVDS::VolumeDataPageAccessor *, float, OpenVDS::InterpolationMethod)>(&native::Create4DInterpolatingVolumeDataAccessorR32), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DInterpolatingVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create4DInterpolatingVolumeDataAccessorR64", static_cast<VolumeDataReadAccessor<OpenVDS::FloatVector4, double> *(*)(OpenVDS::VolumeDataPageAccessor *, float, OpenVDS::InterpolationMethod)>(&native::Create4DInterpolatingVolumeDataAccessorR64), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DInterpolatingVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("requestVolumeSubset"         , static_cast<int64_t(*)(void *, const OpenVDS::VolumeDataLayout *, OpenVDS::DimensionsND, int, int, const int (&)[6], const int (&)[6], VolumeDataChannelDescriptor::Format)>(&native::RequestVolumeSubset), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset));
  VolumeDataAccessManager_.def("requestVolumeSubset"         , static_cast<int64_t(*)(void *, const OpenVDS::VolumeDataLayout *, OpenVDS::DimensionsND, int, int, const int (&)[6], const int (&)[6], VolumeDataChannelDescriptor::Format, float)>(&native::RequestVolumeSubset), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset_2));
  VolumeDataAccessManager_.def("requestProjectedVolumeSubset", static_cast<int64_t(*)(void *, const OpenVDS::VolumeDataLayout *, OpenVDS::DimensionsND, int, int, const int (&)[6], const int (&)[6], const OpenVDS::FloatVector4 &, OpenVDS::DimensionsND, VolumeDataChannelDescriptor::Format, OpenVDS::InterpolationMethod)>(&native::RequestProjectedVolumeSubset), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset));
  VolumeDataAccessManager_.def("requestProjectedVolumeSubset", static_cast<int64_t(*)(void *, const OpenVDS::VolumeDataLayout *, OpenVDS::DimensionsND, int, int, const int (&)[6], const int (&)[6], const OpenVDS::FloatVector4 &, OpenVDS::DimensionsND, VolumeDataChannelDescriptor::Format, OpenVDS::InterpolationMethod, float)>(&native::RequestProjectedVolumeSubset), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset_2));
  VolumeDataAccessManager_.def("requestVolumeSamples"        , static_cast<int64_t(*)(float *, const OpenVDS::VolumeDataLayout *, OpenVDS::DimensionsND, int, int, const float (*)[6], int, OpenVDS::InterpolationMethod)>(&native::RequestVolumeSamples), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples));
  VolumeDataAccessManager_.def("requestVolumeSamples"        , static_cast<int64_t(*)(float *, const OpenVDS::VolumeDataLayout *, OpenVDS::DimensionsND, int, int, const float (*)[6], int, OpenVDS::InterpolationMethod, float)>(&native::RequestVolumeSamples), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples_2));
  VolumeDataAccessManager_.def("requestVolumeTraces"         , static_cast<int64_t(*)(float *, const OpenVDS::VolumeDataLayout *, OpenVDS::DimensionsND, int, int, const float (*)[6], int, OpenVDS::InterpolationMethod, int)>(&native::RequestVolumeTraces), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces));
  VolumeDataAccessManager_.def("requestVolumeTraces"         , static_cast<int64_t(*)(float *, const OpenVDS::VolumeDataLayout *, OpenVDS::DimensionsND, int, int, const float (*)[6], int, OpenVDS::InterpolationMethod, int, float)>(&native::RequestVolumeTraces), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces_2));
  VolumeDataAccessManager_.def("prefetchVolumeChunk"         , static_cast<int64_t(*)(const OpenVDS::VolumeDataLayout *, OpenVDS::DimensionsND, int, int, int64_t)>(&native::PrefetchVolumeChunk), OPENVDS_DOCSTRING(VolumeDataAccessManager_PrefetchVolumeChunk));
  VolumeDataAccessManager_.def("isCompleted"                 , static_cast<bool(*)(int64_t)>(&native::IsCompleted), OPENVDS_DOCSTRING(VolumeDataAccessManager_IsCompleted));
  VolumeDataAccessManager_.def("isCanceled"                  , static_cast<bool(*)(int64_t)>(&native::IsCanceled), OPENVDS_DOCSTRING(VolumeDataAccessManager_IsCanceled));
  VolumeDataAccessManager_.def("waitForCompletion"           , static_cast<bool(*)(int64_t, int)>(&native::WaitForCompletion), OPENVDS_DOCSTRING(VolumeDataAccessManager_WaitForCompletion));
  VolumeDataAccessManager_.def("cancel"                      , static_cast<void(*)(int64_t)>(&native::Cancel), OPENVDS_DOCSTRING(VolumeDataAccessManager_Cancel));
  VolumeDataAccessManager_.def("getCompletionFactor"         , static_cast<float(*)(int64_t)>(&native::GetCompletionFactor), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetCompletionFactor));
  VolumeDataAccessManager_.def("flushUploadQueue"            , static_cast<void(*)()>(&native::FlushUploadQueue), OPENVDS_DOCSTRING(VolumeDataAccessManager_FlushUploadQueue));
  VolumeDataAccessManager_.def("clearUploadErrors"           , static_cast<void(*)()>(&native::ClearUploadErrors), OPENVDS_DOCSTRING(VolumeDataAccessManager_ClearUploadErrors));
  VolumeDataAccessManager_.def("forceClearAllUploadErrors"   , static_cast<void(*)()>(&native::ForceClearAllUploadErrors), OPENVDS_DOCSTRING(VolumeDataAccessManager_ForceClearAllUploadErrors));
  VolumeDataAccessManager_.def("uploadErrorCount"            , static_cast<int32_t(*)()>(&native::UploadErrorCount), OPENVDS_DOCSTRING(VolumeDataAccessManager_UploadErrorCount));
  VolumeDataAccessManager_.def("getCurrentUploadError"       , static_cast<void(*)(const char **, int32_t *, const char **)>(&native::GetCurrentUploadError), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetCurrentUploadError));

  py::enum_<native::VolumeDataAccessManager::AccessMode> 
    VolumeDataAccessManager_AccessMode_(VolumeDataAccessManager_,"AccessMode", OPENVDS_DOCSTRING(VolumeDataAccessManager_AccessMode));

  VolumeDataAccessManager_AccessMode_.value("AccessMode_ReadOnly"         , native::VolumeDataAccessManager::AccessMode::AccessMode_ReadOnly, OPENVDS_DOCSTRING(VolumeDataAccessManager_AccessMode_AccessMode_ReadOnly));
  VolumeDataAccessManager_AccessMode_.value("AccessMode_ReadWrite"        , native::VolumeDataAccessManager::AccessMode::AccessMode_ReadWrite, OPENVDS_DOCSTRING(VolumeDataAccessManager_AccessMode_AccessMode_ReadWrite));
  VolumeDataAccessManager_AccessMode_.value("AccessMode_Create"           , native::VolumeDataAccessManager::AccessMode::AccessMode_Create, OPENVDS_DOCSTRING(VolumeDataAccessManager_AccessMode_AccessMode_Create));

  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, double> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, double> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_2));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, double> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_3));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, float> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_4));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, float> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_5));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, float> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_6));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, uint64_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_7));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, uint64_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_8));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, uint64_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_9));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, uint32_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_10));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, uint32_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_11));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, uint32_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_12));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, uint16_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_13));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, uint16_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_14));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, uint16_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_15));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, uint8_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_16));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, uint8_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_17));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, uint8_t> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_18));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector4, bool> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_19));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector3, bool> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_20));
  m.def("createVolumeDataAccessor"    , static_cast<VolumeDataReadWriteAccessor<OpenVDS::IntVector2, bool> *(*)(OpenVDS::VolumeDataPageAccessor *, float)>(&native::CreateVolumeDataAccessor), OPENVDS_DOCSTRING(CreateVolumeDataAccessor_21));
  m.def("createInterpolatingVolumeDataAccessor", static_cast<VolumeDataReadAccessor<OpenVDS::FloatVector4, double> *(*)(OpenVDS::VolumeDataPageAccessor *, float, OpenVDS::InterpolationMethod)>(&native::CreateInterpolatingVolumeDataAccessor), OPENVDS_DOCSTRING(CreateInterpolatingVolumeDataAccessor));
  m.def("createInterpolatingVolumeDataAccessor", static_cast<VolumeDataReadAccessor<OpenVDS::FloatVector3, double> *(*)(OpenVDS::VolumeDataPageAccessor *, float, OpenVDS::InterpolationMethod)>(&native::CreateInterpolatingVolumeDataAccessor), OPENVDS_DOCSTRING(CreateInterpolatingVolumeDataAccessor_2));
  m.def("createInterpolatingVolumeDataAccessor", static_cast<VolumeDataReadAccessor<OpenVDS::FloatVector2, double> *(*)(OpenVDS::VolumeDataPageAccessor *, float, OpenVDS::InterpolationMethod)>(&native::CreateInterpolatingVolumeDataAccessor), OPENVDS_DOCSTRING(CreateInterpolatingVolumeDataAccessor_3));
  m.def("createInterpolatingVolumeDataAccessor", static_cast<VolumeDataReadAccessor<OpenVDS::FloatVector4, float> *(*)(OpenVDS::VolumeDataPageAccessor *, float, OpenVDS::InterpolationMethod)>(&native::CreateInterpolatingVolumeDataAccessor), OPENVDS_DOCSTRING(CreateInterpolatingVolumeDataAccessor_4));
  m.def("createInterpolatingVolumeDataAccessor", static_cast<VolumeDataReadAccessor<OpenVDS::FloatVector3, float> *(*)(OpenVDS::VolumeDataPageAccessor *, float, OpenVDS::InterpolationMethod)>(&native::CreateInterpolatingVolumeDataAccessor), OPENVDS_DOCSTRING(CreateInterpolatingVolumeDataAccessor_5));
  m.def("createInterpolatingVolumeDataAccessor", static_cast<VolumeDataReadAccessor<OpenVDS::FloatVector2, float> *(*)(OpenVDS::VolumeDataPageAccessor *, float, OpenVDS::InterpolationMethod)>(&native::CreateInterpolatingVolumeDataAccessor), OPENVDS_DOCSTRING(CreateInterpolatingVolumeDataAccessor_6));

  // VolumeDataPage
  py::class_<native::VolumeDataPage> 
    VolumeDataPage_(m,"VolumeDataPage", OPENVDS_DOCSTRING(VolumeDataPage));

  VolumeDataPage_.def("getMinMax"                   , static_cast<void(*)(int (&)[6], int (&)[6])>(&native::GetMinMax), OPENVDS_DOCSTRING(VolumeDataPage_GetMinMax));
  VolumeDataPage_.def("getMinMaxExcludingMargin"    , static_cast<void(*)(int (&)[6], int (&)[6])>(&native::GetMinMaxExcludingMargin), OPENVDS_DOCSTRING(VolumeDataPage_GetMinMaxExcludingMargin));
  VolumeDataPage_.def("getBuffer"                   , static_cast<const void *(*)(int (&)[6])>(&native::GetBuffer), OPENVDS_DOCSTRING(VolumeDataPage_GetBuffer));
  VolumeDataPage_.def("getWritableBuffer"           , static_cast<void *(*)(int (&)[6])>(&native::GetWritableBuffer), OPENVDS_DOCSTRING(VolumeDataPage_GetWritableBuffer));
  VolumeDataPage_.def("updateWrittenRegion"         , static_cast<void(*)(const int (&)[6], const int (&)[6])>(&native::UpdateWrittenRegion), OPENVDS_DOCSTRING(VolumeDataPage_UpdateWrittenRegion));
  VolumeDataPage_.def("release"                     , static_cast<void(*)()>(&native::Release), OPENVDS_DOCSTRING(VolumeDataPage_Release));

  // VolumeDataPageAccessor
  py::class_<native::VolumeDataPageAccessor> 
    VolumeDataPageAccessor_(m,"VolumeDataPageAccessor", OPENVDS_DOCSTRING(VolumeDataPageAccessor));

  VolumeDataPageAccessor_.def("getLayout"                   , static_cast<const OpenVDS::VolumeDataLayout *(*)()>(&native::GetLayout), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLayout_2));
  VolumeDataPageAccessor_.def("getLOD"                      , static_cast<int(*)()>(&native::GetLOD), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLOD));
  VolumeDataPageAccessor_.def("getChannelIndex"             , static_cast<int(*)()>(&native::GetChannelIndex), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelIndex));
  VolumeDataPageAccessor_.def("getChannelDescriptor"        , static_cast<const OpenVDS::VolumeDataChannelDescriptor &(*)()>(&native::GetChannelDescriptor), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelDescriptor));
  VolumeDataPageAccessor_.def("getNumSamples"               , static_cast<void(*)(int (&)[6])>(&native::GetNumSamples), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetNumSamples));
  VolumeDataPageAccessor_.def("getChunkCount"               , static_cast<int64_t(*)()>(&native::GetChunkCount), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkCount));
  VolumeDataPageAccessor_.def("getChunkMinMax"              , static_cast<void(*)(int64_t, int (&)[6], int (&)[6])>(&native::GetChunkMinMax), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkMinMax));
  VolumeDataPageAccessor_.def("getChunkMinMaxExcludingMargin", static_cast<void(*)(int64_t, int (&)[6], int (&)[6])>(&native::GetChunkMinMaxExcludingMargin), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkMinMaxExcludingMargin));
  VolumeDataPageAccessor_.def("getChunkIndex"               , static_cast<int64_t(*)(const int (&)[6])>(&native::GetChunkIndex), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkIndex));
  VolumeDataPageAccessor_.def("addReference"                , static_cast<int(*)()>(&native::AddReference), OPENVDS_DOCSTRING(VolumeDataPageAccessor_AddReference));
  VolumeDataPageAccessor_.def("removeReference"             , static_cast<int(*)()>(&native::RemoveReference), OPENVDS_DOCSTRING(VolumeDataPageAccessor_RemoveReference));
  VolumeDataPageAccessor_.def("getMaxPages"                 , static_cast<int(*)()>(&native::GetMaxPages), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetMaxPages));
  VolumeDataPageAccessor_.def("setMaxPages"                 , static_cast<void(*)(int)>(&native::SetMaxPages), OPENVDS_DOCSTRING(VolumeDataPageAccessor_SetMaxPages));
  VolumeDataPageAccessor_.def("createPage"                  , static_cast<OpenVDS::VolumeDataPage *(*)(int64_t)>(&native::CreatePage), OPENVDS_DOCSTRING(VolumeDataPageAccessor_CreatePage));
  VolumeDataPageAccessor_.def("readPage"                    , static_cast<OpenVDS::VolumeDataPage *(*)(int64_t)>(&native::ReadPage), OPENVDS_DOCSTRING(VolumeDataPageAccessor_ReadPage));
  VolumeDataPageAccessor_.def("readPageAtPosition"          , static_cast<OpenVDS::VolumeDataPage *(*)(const int (&)[6])>(&native::ReadPageAtPosition), OPENVDS_DOCSTRING(VolumeDataPageAccessor_ReadPageAtPosition));
  VolumeDataPageAccessor_.def("commit"                      , static_cast<void(*)()>(&native::Commit), OPENVDS_DOCSTRING(VolumeDataPageAccessor_Commit));

//AUTOGEN-END
#endif
}

