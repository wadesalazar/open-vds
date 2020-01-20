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

template<typename INDEX_TYPE, typename T>
static void
RegisterVolumeDataReadAccessor(py::module& m, const char* name)
{
  typedef typename VectorAdapter<typename INDEX_TYPE::element_type, INDEX_TYPE::element_count>::AdaptedType AdaptedIndexType;
  typedef VolumeDataReadAccessor<INDEX_TYPE, T> AccessorType;

  py::class_<AccessorType, std::unique_ptr<AccessorType, py::nodelete>> 
    VolumeDataAccessor_(m, name, OPENVDS_DOCSTRING(VolumeDataAccessor));

  VolumeDataAccessor_.def("getValue", [](AccessorType* self, AdaptedIndexType index)
    {
      return self->GetValue(index);
    });
}

template<typename INDEX_TYPE, typename T>
static void
RegisterVolumeDataReadWriteAccessor(py::module& m, const char* name)
{
  typedef typename VectorAdapter<typename INDEX_TYPE::element_type, INDEX_TYPE::element_count>::AdaptedType AdaptedIndexType;
  typedef VolumeDataReadWriteAccessor<INDEX_TYPE, T> AccessorType;
  typedef VolumeDataReadAccessor<INDEX_TYPE, T>      BaseType;

  py::class_<AccessorType, BaseType, std::unique_ptr<AccessorType, py::nodelete>> 
    VolumeDataAccessor_(m, name, OPENVDS_DOCSTRING(VolumeDataAccessor));

  VolumeDataAccessor_.def("setValue", [](AccessorType* self, AdaptedIndexType index, T value)
    {
      self->SetValue(index, value);
    });
  VolumeDataAccessor_.def("commit", &AccessorType::Commit);
  VolumeDataAccessor_.def("cancel", &AccessorType::Cancel);
}

void 
PyVolumeDataAccess::initModule(py::module& m)
{
  // Register accessor types
  RegisterVolumeDataReadAccessor<native::IntVector2, bool>          (m, "Read2DVolumeDataAccessor1Bit");
  RegisterVolumeDataReadAccessor<native::IntVector2, uint8_t>       (m, "Read2DVolumeDataAccessorU8");  
  RegisterVolumeDataReadAccessor<native::IntVector2, uint16_t>      (m, "Read2DVolumeDataAccessorU16"); 
  RegisterVolumeDataReadAccessor<native::IntVector2, uint32_t>      (m, "Read2DVolumeDataAccessorU32"); 
  RegisterVolumeDataReadAccessor<native::IntVector2, uint64_t>      (m, "Read2DVolumeDataAccessorU64"); 
  RegisterVolumeDataReadAccessor<native::IntVector2, float>         (m, "Read2DVolumeDataAccessorR32"); 
  RegisterVolumeDataReadAccessor<native::IntVector2, double>        (m, "Read2DVolumeDataAccessorR64"); 
  RegisterVolumeDataReadAccessor<native::IntVector3, bool>          (m, "Read3DVolumeDataAccessor1Bit");
  RegisterVolumeDataReadAccessor<native::IntVector3, uint8_t>       (m, "Read3DVolumeDataAccessorU8");  
  RegisterVolumeDataReadAccessor<native::IntVector3, uint16_t>      (m, "Read3DVolumeDataAccessorU16"); 
  RegisterVolumeDataReadAccessor<native::IntVector3, uint32_t>      (m, "Read3DVolumeDataAccessorU32"); 
  RegisterVolumeDataReadAccessor<native::IntVector3, uint64_t>      (m, "Read3DVolumeDataAccessorU64"); 
  RegisterVolumeDataReadAccessor<native::IntVector3, float>         (m, "Read3DVolumeDataAccessorR32"); 
  RegisterVolumeDataReadAccessor<native::IntVector3, double>        (m, "Read3DVolumeDataAccessorR64"); 
  RegisterVolumeDataReadAccessor<native::IntVector4, bool>          (m, "Read4DVolumeDataAccessor1Bit");
  RegisterVolumeDataReadAccessor<native::IntVector4, uint8_t>       (m, "Read4DVolumeDataAccessorU8");  
  RegisterVolumeDataReadAccessor<native::IntVector4, uint16_t>      (m, "Read4DVolumeDataAccessorU16"); 
  RegisterVolumeDataReadAccessor<native::IntVector4, uint32_t>      (m, "Read4DVolumeDataAccessorU32"); 
  RegisterVolumeDataReadAccessor<native::IntVector4, uint64_t>      (m, "Read4DVolumeDataAccessorU64");
  RegisterVolumeDataReadAccessor<native::IntVector4, float>         (m, "Read4DVolumeDataAccessorR32");
  RegisterVolumeDataReadAccessor<native::IntVector4, double>        (m, "Read4DVolumeDataAccessorR64");
  RegisterVolumeDataReadAccessor<native::FloatVector2, float>       (m, "Interpolating2DVolumeDataAccessorR32");
  RegisterVolumeDataReadAccessor<native::FloatVector2, double>      (m, "Interpolating2DVolumeDataAccessorR64");
  RegisterVolumeDataReadAccessor<native::FloatVector3, float>       (m, "Interpolating3DVolumeDataAccessorR32");
  RegisterVolumeDataReadAccessor<native::FloatVector3, double>      (m, "Interpolating3DVolumeDataAccessorR64");
  RegisterVolumeDataReadAccessor<native::FloatVector4, float>       (m, "Interpolating4DVolumeDataAccessorR32");
  RegisterVolumeDataReadAccessor<native::FloatVector4, double>      (m, "Interpolating4DVolumeDataAccessorR64");

  RegisterVolumeDataReadWriteAccessor<native::IntVector2, bool>     (m, "ReadWrite2DVolumeDataAccessor1Bit");
  RegisterVolumeDataReadWriteAccessor<native::IntVector2, uint8_t>  (m, "ReadWrite2DVolumeDataAccessorU8");  
  RegisterVolumeDataReadWriteAccessor<native::IntVector2, uint16_t> (m, "ReadWrite2DVolumeDataAccessorU16"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector2, uint32_t> (m, "ReadWrite2DVolumeDataAccessorU32"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector2, uint64_t> (m, "ReadWrite2DVolumeDataAccessorU64"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector2, float>    (m, "ReadWrite2DVolumeDataAccessorR32"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector2, double>   (m, "ReadWrite2DVolumeDataAccessorR64"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, bool>     (m, "ReadWrite3DVolumeDataAccessor1Bit");
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, uint8_t>  (m, "ReadWrite3DVolumeDataAccessorU8");  
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, uint16_t> (m, "ReadWrite3DVolumeDataAccessorU16"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, uint32_t> (m, "ReadWrite3DVolumeDataAccessorU32"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, uint64_t> (m, "ReadWrite3DVolumeDataAccessorU64"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, float>    (m, "ReadWrite3DVolumeDataAccessorR32"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, double>   (m, "ReadWrite3DVolumeDataAccessorR64"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, bool>     (m, "ReadWrite4DVolumeDataAccessor1Bit");
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, uint8_t>  (m, "ReadWrite4DVolumeDataAccessorU8");  
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, uint16_t> (m, "ReadWrite4DVolumeDataAccessorU16"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, uint32_t> (m, "ReadWrite4DVolumeDataAccessorU32"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, uint64_t> (m, "ReadWrite4DVolumeDataAccessorU64");
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, float>    (m, "ReadWrite4DVolumeDataAccessorR32");
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, double>   (m, "ReadWrite4DVolumeDataAccessorR64");

//AUTOGEN-BEGIN
  // VolumeDataAccessor
  py::class_<VolumeDataAccessor, std::unique_ptr<VolumeDataAccessor, py::nodelete>> 
    VolumeDataAccessor_(m,"VolumeDataAccessor", OPENVDS_DOCSTRING(VolumeDataAccessor));

  VolumeDataAccessor_.def("getManager"                  , static_cast<native::VolumeDataAccessManager &(VolumeDataAccessor::*)()>(&VolumeDataAccessor::GetManager), OPENVDS_DOCSTRING(VolumeDataAccessor_GetManager));
  VolumeDataAccessor_.def("getLayout"                   , static_cast<const native::VolumeDataLayout *(VolumeDataAccessor::*)()>(&VolumeDataAccessor::GetLayout), OPENVDS_DOCSTRING(VolumeDataAccessor_GetLayout));
  VolumeDataAccessor_.def("getBase"                     , static_cast<native::VolumeDataAccessorBase *(VolumeDataAccessor::*)()>(&VolumeDataAccessor::GetBase), OPENVDS_DOCSTRING(VolumeDataAccessor_GetBase));

  // VolumeDataAccessor::IndexOutOfRangeException
  py::class_<VolumeDataAccessor::IndexOutOfRangeException> 
    VolumeDataAccessor_IndexOutOfRangeException_(VolumeDataAccessor_,"VolumeDataAccessor::IndexOutOfRangeException", OPENVDS_DOCSTRING(VolumeDataAccessor_IndexOutOfRangeException));

  // VolumeDataAccessManager
  py::class_<VolumeDataAccessManager, std::unique_ptr<VolumeDataAccessManager, py::nodelete>> 
    VolumeDataAccessManager_(m,"VolumeDataAccessManager", OPENVDS_DOCSTRING(VolumeDataAccessManager));

  VolumeDataAccessManager_.def("getVolumeDataLayout"         , static_cast<const native::VolumeDataLayout *(VolumeDataAccessManager::*)() const>(&VolumeDataAccessManager::GetVolumeDataLayout), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetVolumeDataLayout));
  VolumeDataAccessManager_.def("createVolumeDataPageAccessor", static_cast<native::VolumeDataPageAccessor *(VolumeDataAccessManager::*)(const native::VolumeDataLayout *, native::DimensionsND, int, int, int, native::VolumeDataAccessManager::AccessMode)>(&VolumeDataAccessManager::CreateVolumeDataPageAccessor), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("maxPages"), py::arg("accessMode"), OPENVDS_DOCSTRING(VolumeDataAccessManager_CreateVolumeDataPageAccessor));
  VolumeDataAccessManager_.def("destroyVolumeDataPageAccessor", static_cast<void(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *)>(&VolumeDataAccessManager::DestroyVolumeDataPageAccessor), py::arg("volumeDataPageAccessor"), OPENVDS_DOCSTRING(VolumeDataAccessManager_DestroyVolumeDataPageAccessor));
  VolumeDataAccessManager_.def("destroyVolumeDataAccessor"   , static_cast<void(VolumeDataAccessManager::*)(native::VolumeDataAccessor *)>(&VolumeDataAccessManager::DestroyVolumeDataAccessor), py::arg("accessor"), OPENVDS_DOCSTRING(VolumeDataAccessManager_DestroyVolumeDataAccessor));
  VolumeDataAccessManager_.def("cloneVolumeDataAccessor"     , static_cast<native::VolumeDataAccessor *(VolumeDataAccessManager::*)(const native::VolumeDataAccessor &)>(&VolumeDataAccessManager::CloneVolumeDataAccessor), py::arg("accessor"), OPENVDS_DOCSTRING(VolumeDataAccessManager_CloneVolumeDataAccessor));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessor1Bit", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, bool> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessor1Bit), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessor1Bit));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorU8", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, uint8_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessorU8), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorU8));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorU16", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, uint16_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessorU16), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorU16));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorU32", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, uint32_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessorU32), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorU32));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorU64", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, uint64_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessorU64), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorU64));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorR32", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, float> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessorR32), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create2DVolumeDataAccessorR64", static_cast<VolumeDataReadWriteAccessor<native::IntVector2, double> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create2DVolumeDataAccessorR64), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessor1Bit", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, bool> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessor1Bit), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessor1Bit));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorU8", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, uint8_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessorU8), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorU8));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorU16", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, uint16_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessorU16), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorU16));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorU32", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, uint32_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessorU32), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorU32));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorU64", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, uint64_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessorU64), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorU64));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorR32", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, float> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessorR32), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create3DVolumeDataAccessorR64", static_cast<VolumeDataReadWriteAccessor<native::IntVector3, double> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create3DVolumeDataAccessorR64), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessor1Bit", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, bool> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessor1Bit), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessor1Bit));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorU8", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, uint8_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessorU8), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorU8));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorU16", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, uint16_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessorU16), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorU16));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorU32", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, uint32_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessorU32), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorU32));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorU64", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, uint64_t> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessorU64), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorU64));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorR32", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, float> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessorR32), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create4DVolumeDataAccessorR64", static_cast<VolumeDataReadWriteAccessor<native::IntVector4, double> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float)>(&VolumeDataAccessManager::Create4DVolumeDataAccessorR64), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create2DInterpolatingVolumeDataAccessorR32", static_cast<VolumeDataReadAccessor<native::FloatVector2, float> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float, native::InterpolationMethod)>(&VolumeDataAccessManager::Create2DInterpolatingVolumeDataAccessorR32), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), py::arg("interpolationMethod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DInterpolatingVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create2DInterpolatingVolumeDataAccessorR64", static_cast<VolumeDataReadAccessor<native::FloatVector2, double> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float, native::InterpolationMethod)>(&VolumeDataAccessManager::Create2DInterpolatingVolumeDataAccessorR64), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), py::arg("interpolationMethod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create2DInterpolatingVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create3DInterpolatingVolumeDataAccessorR32", static_cast<VolumeDataReadAccessor<native::FloatVector3, float> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float, native::InterpolationMethod)>(&VolumeDataAccessManager::Create3DInterpolatingVolumeDataAccessorR32), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), py::arg("interpolationMethod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DInterpolatingVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create3DInterpolatingVolumeDataAccessorR64", static_cast<VolumeDataReadAccessor<native::FloatVector3, double> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float, native::InterpolationMethod)>(&VolumeDataAccessManager::Create3DInterpolatingVolumeDataAccessorR64), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), py::arg("interpolationMethod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create3DInterpolatingVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("create4DInterpolatingVolumeDataAccessorR32", static_cast<VolumeDataReadAccessor<native::FloatVector4, float> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float, native::InterpolationMethod)>(&VolumeDataAccessManager::Create4DInterpolatingVolumeDataAccessorR32), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), py::arg("interpolationMethod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DInterpolatingVolumeDataAccessorR32));
  VolumeDataAccessManager_.def("create4DInterpolatingVolumeDataAccessorR64", static_cast<VolumeDataReadAccessor<native::FloatVector4, double> *(VolumeDataAccessManager::*)(native::VolumeDataPageAccessor *, float, native::InterpolationMethod)>(&VolumeDataAccessManager::Create4DInterpolatingVolumeDataAccessorR64), py::arg("volumeDataPageAccessor"), py::arg("replacementNoValue"), py::arg("interpolationMethod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Create4DInterpolatingVolumeDataAccessorR64));
  VolumeDataAccessManager_.def("getVolumeSubsetBufferSize"   , [](VolumeDataAccessManager* self, const OpenVDS::VolumeDataLayout * volumeDataLayout, const py::array_t<int>& minVoxelCoordinates, const py::array_t<int>& maxVoxelCoordinates, VolumeDataChannelDescriptor::Format format, int lod) { return self->GetVolumeSubsetBufferSize(volumeDataLayout, PyArrayAdapter<int, 6, false>::getArrayChecked(minVoxelCoordinates), PyArrayAdapter<int, 6, false>::getArrayChecked(maxVoxelCoordinates), format, lod); }, py::arg("volumeDataLayout"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("format"), py::arg("lod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetVolumeSubsetBufferSize));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestVolumeSubset"         , static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], VolumeDataChannelDescriptor::Format)>(&VolumeDataAccessManager::RequestVolumeSubset), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("format"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestVolumeSubset"         , static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], VolumeDataChannelDescriptor::Format, float)>(&VolumeDataAccessManager::RequestVolumeSubset), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lOD"), py::arg("channel"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("format"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset_2));
  VolumeDataAccessManager_.def("getProjectedVolumeSubsetBufferSize", [](VolumeDataAccessManager* self, const OpenVDS::VolumeDataLayout * volumeDataLayout, const py::array_t<int>& minVoxelCoordinates, const py::array_t<int>& maxVoxelCoordinates, OpenVDS::DimensionsND projectedDimensions, VolumeDataChannelDescriptor::Format format, int lod) { return self->GetProjectedVolumeSubsetBufferSize(volumeDataLayout, PyArrayAdapter<int, 6, false>::getArrayChecked(minVoxelCoordinates), PyArrayAdapter<int, 6, false>::getArrayChecked(maxVoxelCoordinates), projectedDimensions, format, lod); }, py::arg("volumeDataLayout"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("projectedDimensions"), py::arg("format"), py::arg("lod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetProjectedVolumeSubsetBufferSize));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestProjectedVolumeSubset", static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], const native::FloatVector4 &, native::DimensionsND, VolumeDataChannelDescriptor::Format, native::InterpolationMethod)>(&VolumeDataAccessManager::RequestProjectedVolumeSubset), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("voxelPlane"), py::arg("projectedDimensions"), py::arg("format"), py::arg("interpolationMethod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestProjectedVolumeSubset", static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], const native::FloatVector4 &, native::DimensionsND, VolumeDataChannelDescriptor::Format, native::InterpolationMethod, float)>(&VolumeDataAccessManager::RequestProjectedVolumeSubset), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("voxelPlane"), py::arg("projectedDimensions"), py::arg("format"), py::arg("interpolationMethod"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset_2));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestVolumeSamples"        , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod)>(&VolumeDataAccessManager::RequestVolumeSamples), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("samplePositions"), py::arg("sampleCount"), py::arg("interpolationMethod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestVolumeSamples"        , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod, float)>(&VolumeDataAccessManager::RequestVolumeSamples), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("samplePositions"), py::arg("sampleCount"), py::arg("interpolationMethod"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples_2));
  VolumeDataAccessManager_.def("getVolumeTracesBufferSize"   , static_cast<int64_t(VolumeDataAccessManager::*)(const native::VolumeDataLayout *, int, int, int)>(&VolumeDataAccessManager::GetVolumeTracesBufferSize), py::arg("volumeDataLayout"), py::arg("traceCount"), py::arg("traceDimension"), py::arg("lod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetVolumeTracesBufferSize));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestVolumeTraces"         , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod, int)>(&VolumeDataAccessManager::RequestVolumeTraces), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("tracePositions"), py::arg("traceCount"), py::arg("interpolationMethod"), py::arg("traceDimension"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("requestVolumeTraces"         , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod, int, float)>(&VolumeDataAccessManager::RequestVolumeTraces), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("tracePositions"), py::arg("traceCount"), py::arg("interpolationMethod"), py::arg("traceDimension"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces_2));
  VolumeDataAccessManager_.def("prefetchVolumeChunk"         , static_cast<int64_t(VolumeDataAccessManager::*)(const native::VolumeDataLayout *, native::DimensionsND, int, int, int64_t)>(&VolumeDataAccessManager::PrefetchVolumeChunk), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("chunk"), OPENVDS_DOCSTRING(VolumeDataAccessManager_PrefetchVolumeChunk));
  VolumeDataAccessManager_.def("isCompleted"                 , static_cast<bool(VolumeDataAccessManager::*)(int64_t)>(&VolumeDataAccessManager::IsCompleted), py::arg("requestID"), OPENVDS_DOCSTRING(VolumeDataAccessManager_IsCompleted));
  VolumeDataAccessManager_.def("isCanceled"                  , static_cast<bool(VolumeDataAccessManager::*)(int64_t)>(&VolumeDataAccessManager::IsCanceled), py::arg("requestID"), OPENVDS_DOCSTRING(VolumeDataAccessManager_IsCanceled));
  VolumeDataAccessManager_.def("waitForCompletion"           , static_cast<bool(VolumeDataAccessManager::*)(int64_t, int)>(&VolumeDataAccessManager::WaitForCompletion), py::arg("requestID"), py::arg("millisecondsBeforeTimeout"), OPENVDS_DOCSTRING(VolumeDataAccessManager_WaitForCompletion));
  VolumeDataAccessManager_.def("cancel"                      , static_cast<void(VolumeDataAccessManager::*)(int64_t)>(&VolumeDataAccessManager::Cancel), py::arg("requestID"), OPENVDS_DOCSTRING(VolumeDataAccessManager_Cancel));
  VolumeDataAccessManager_.def("getCompletionFactor"         , static_cast<float(VolumeDataAccessManager::*)(int64_t)>(&VolumeDataAccessManager::GetCompletionFactor), py::arg("requestID"), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetCompletionFactor));
  VolumeDataAccessManager_.def("flushUploadQueue"            , static_cast<void(VolumeDataAccessManager::*)()>(&VolumeDataAccessManager::FlushUploadQueue), OPENVDS_DOCSTRING(VolumeDataAccessManager_FlushUploadQueue));
  VolumeDataAccessManager_.def("clearUploadErrors"           , static_cast<void(VolumeDataAccessManager::*)()>(&VolumeDataAccessManager::ClearUploadErrors), OPENVDS_DOCSTRING(VolumeDataAccessManager_ClearUploadErrors));
  VolumeDataAccessManager_.def("forceClearAllUploadErrors"   , static_cast<void(VolumeDataAccessManager::*)()>(&VolumeDataAccessManager::ForceClearAllUploadErrors), OPENVDS_DOCSTRING(VolumeDataAccessManager_ForceClearAllUploadErrors));
  VolumeDataAccessManager_.def("uploadErrorCount"            , static_cast<int32_t(VolumeDataAccessManager::*)()>(&VolumeDataAccessManager::UploadErrorCount), OPENVDS_DOCSTRING(VolumeDataAccessManager_UploadErrorCount));
// AUTOGENERATE FAIL :   VolumeDataAccessManager_.def("getCurrentUploadError"       , static_cast<void(VolumeDataAccessManager::*)(const char **, int32_t *, const char **)>(&VolumeDataAccessManager::GetCurrentUploadError), py::arg("objectId"), py::arg("errorCode"), py::arg("errorString"), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetCurrentUploadError));

  py::enum_<VolumeDataAccessManager::AccessMode> 
    VolumeDataAccessManager_AccessMode_(VolumeDataAccessManager_,"AccessMode", OPENVDS_DOCSTRING(VolumeDataAccessManager_AccessMode));

  VolumeDataAccessManager_AccessMode_.value("AccessMode_ReadOnly"         , VolumeDataAccessManager::AccessMode::AccessMode_ReadOnly, OPENVDS_DOCSTRING(VolumeDataAccessManager_AccessMode_AccessMode_ReadOnly));
  VolumeDataAccessManager_AccessMode_.value("AccessMode_ReadWrite"        , VolumeDataAccessManager::AccessMode::AccessMode_ReadWrite, OPENVDS_DOCSTRING(VolumeDataAccessManager_AccessMode_AccessMode_ReadWrite));
  VolumeDataAccessManager_AccessMode_.value("AccessMode_Create"           , VolumeDataAccessManager::AccessMode::AccessMode_Create, OPENVDS_DOCSTRING(VolumeDataAccessManager_AccessMode_AccessMode_Create));

  // VolumeDataPage
  py::class_<VolumeDataPage, std::unique_ptr<VolumeDataPage, py::nodelete>> 
    VolumeDataPage_(m,"VolumeDataPage", OPENVDS_DOCSTRING(VolumeDataPage));

  VolumeDataPage_.def("getMinMax"                   , [](VolumeDataPage* self, py::array_t<int>& min, py::array_t<int>& max) { return self->GetMinMax(PyArrayAdapter<int, 6, true>::getArrayChecked(min), PyArrayAdapter<int, 6, true>::getArrayChecked(max)); }, py::arg("min"), py::arg("max"), OPENVDS_DOCSTRING(VolumeDataPage_GetMinMax));
  VolumeDataPage_.def("getMinMaxExcludingMargin"    , [](VolumeDataPage* self, py::array_t<int>& minExcludingMargin, py::array_t<int>& maxExcludingMargin) { return self->GetMinMaxExcludingMargin(PyArrayAdapter<int, 6, true>::getArrayChecked(minExcludingMargin), PyArrayAdapter<int, 6, true>::getArrayChecked(maxExcludingMargin)); }, py::arg("minExcludingMargin"), py::arg("maxExcludingMargin"), OPENVDS_DOCSTRING(VolumeDataPage_GetMinMaxExcludingMargin));
  VolumeDataPage_.def("getBuffer"                   , [](VolumeDataPage* self, py::array_t<int>& pitch) { return self->GetBuffer(PyArrayAdapter<int, 6, true>::getArrayChecked(pitch)); }, py::arg("pitch"), OPENVDS_DOCSTRING(VolumeDataPage_GetBuffer));
  VolumeDataPage_.def("getWritableBuffer"           , [](VolumeDataPage* self, py::array_t<int>& pitch) { return self->GetWritableBuffer(PyArrayAdapter<int, 6, true>::getArrayChecked(pitch)); }, py::arg("pitch"), OPENVDS_DOCSTRING(VolumeDataPage_GetWritableBuffer));
  VolumeDataPage_.def("updateWrittenRegion"         , [](VolumeDataPage* self, const py::array_t<int>& writtenMin, const py::array_t<int>& writtenMax) { return self->UpdateWrittenRegion(PyArrayAdapter<int, 6, false>::getArrayChecked(writtenMin), PyArrayAdapter<int, 6, false>::getArrayChecked(writtenMax)); }, py::arg("writtenMin"), py::arg("writtenMax"), OPENVDS_DOCSTRING(VolumeDataPage_UpdateWrittenRegion));
  VolumeDataPage_.def("release"                     , static_cast<void(VolumeDataPage::*)()>(&VolumeDataPage::Release), OPENVDS_DOCSTRING(VolumeDataPage_Release));

  // VolumeDataPageAccessor
  py::class_<VolumeDataPageAccessor, std::unique_ptr<VolumeDataPageAccessor, py::nodelete>> 
    VolumeDataPageAccessor_(m,"VolumeDataPageAccessor", OPENVDS_DOCSTRING(VolumeDataPageAccessor));

  VolumeDataPageAccessor_.def("getLayout"                   , static_cast<const native::VolumeDataLayout *(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetLayout), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLayout));
  VolumeDataPageAccessor_.def("getLOD"                      , static_cast<int(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetLOD), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLOD));
  VolumeDataPageAccessor_.def("getChannelIndex"             , static_cast<int(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetChannelIndex), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelIndex));
  VolumeDataPageAccessor_.def("getChannelDescriptor"        , static_cast<const native::VolumeDataChannelDescriptor &(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetChannelDescriptor), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelDescriptor));
  VolumeDataPageAccessor_.def("getNumSamples"               , [](VolumeDataPageAccessor* self, py::array_t<int>& numSamples) { return self->GetNumSamples(PyArrayAdapter<int, 6, true>::getArrayChecked(numSamples)); }, py::arg("numSamples"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetNumSamples));
  VolumeDataPageAccessor_.def("getChunkCount"               , static_cast<int64_t(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetChunkCount), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkCount));
  VolumeDataPageAccessor_.def("getChunkMinMax"              , [](VolumeDataPageAccessor* self, int64_t chunk, py::array_t<int>& min, py::array_t<int>& max) { return self->GetChunkMinMax(chunk, PyArrayAdapter<int, 6, true>::getArrayChecked(min), PyArrayAdapter<int, 6, true>::getArrayChecked(max)); }, py::arg("chunk"), py::arg("min"), py::arg("max"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkMinMax));
  VolumeDataPageAccessor_.def("getChunkMinMaxExcludingMargin", [](VolumeDataPageAccessor* self, int64_t chunk, py::array_t<int>& minExcludingMargin, py::array_t<int>& maxExcludingMargin) { return self->GetChunkMinMaxExcludingMargin(chunk, PyArrayAdapter<int, 6, true>::getArrayChecked(minExcludingMargin), PyArrayAdapter<int, 6, true>::getArrayChecked(maxExcludingMargin)); }, py::arg("chunk"), py::arg("minExcludingMargin"), py::arg("maxExcludingMargin"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkMinMaxExcludingMargin));
  VolumeDataPageAccessor_.def("getChunkIndex"               , [](VolumeDataPageAccessor* self, const py::array_t<int>& position) { return self->GetChunkIndex(PyArrayAdapter<int, 6, false>::getArrayChecked(position)); }, py::arg("position"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkIndex));
  VolumeDataPageAccessor_.def("addReference"                , static_cast<int(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::AddReference), OPENVDS_DOCSTRING(VolumeDataPageAccessor_AddReference));
  VolumeDataPageAccessor_.def("removeReference"             , static_cast<int(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::RemoveReference), OPENVDS_DOCSTRING(VolumeDataPageAccessor_RemoveReference));
  VolumeDataPageAccessor_.def("getMaxPages"                 , static_cast<int(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::GetMaxPages), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetMaxPages));
  VolumeDataPageAccessor_.def("setMaxPages"                 , static_cast<void(VolumeDataPageAccessor::*)(int)>(&VolumeDataPageAccessor::SetMaxPages), py::arg("maxPages"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_SetMaxPages));
  VolumeDataPageAccessor_.def("createPage"                  , static_cast<native::VolumeDataPage *(VolumeDataPageAccessor::*)(int64_t)>(&VolumeDataPageAccessor::CreatePage), py::arg("chunkIndex"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_CreatePage));
  VolumeDataPageAccessor_.def("readPage"                    , static_cast<native::VolumeDataPage *(VolumeDataPageAccessor::*)(int64_t)>(&VolumeDataPageAccessor::ReadPage), py::arg("chunkIndex"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_ReadPage));
  VolumeDataPageAccessor_.def("readPageAtPosition"          , [](VolumeDataPageAccessor* self, const py::array_t<int>& position) { return self->ReadPageAtPosition(PyArrayAdapter<int, 6, false>::getArrayChecked(position)); }, py::arg("position"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_ReadPageAtPosition));
  VolumeDataPageAccessor_.def("commit"                      , static_cast<void(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::Commit), OPENVDS_DOCSTRING(VolumeDataPageAccessor_Commit));

//AUTOGEN-END


// IMPLEMENTED :   VolumeDataAccessManager_.def("requestVolumeSubset"         , static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], VolumeDataChannelDescriptor::Format)>(&VolumeDataAccessManager::RequestVolumeSubset), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("format"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset));
  VolumeDataAccessManager_.def("requestVolumeSubset"         , [] (VolumeDataAccessManager* self, py::buffer buf, const native::VolumeDataLayout *layout, native::DimensionsND dimensions, int lod, int channel, py::array_t<int>& arg1, py::array_t<int>& arg2, VolumeDataChannelDescriptor::Format fmt)
    {
      auto& minVoxelCoordinates = PyArrayAdapter<int, Dimensionality_Max, false>::getArrayChecked(arg1);
      auto& maxVoxelCoordinates = PyArrayAdapter<int, Dimensionality_Max, false>::getArrayChecked(arg2);
      int64_t bufferSize = self->GetVolumeSubsetBufferSize(layout, minVoxelCoordinates, maxVoxelCoordinates, fmt, lod);
      py::buffer_info info = buf.request(true);
      if (info.size * info.itemsize < bufferSize)
      {
        throw std::runtime_error("Insufficient buffer");
      }
      return self->RequestVolumeSubset(info.ptr, layout, dimensions, lod, channel, minVoxelCoordinates, maxVoxelCoordinates, fmt);
    }
  , py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("format"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestVolumeSubset"         , static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], VolumeDataChannelDescriptor::Format, float)>(&VolumeDataAccessManager::RequestVolumeSubset), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lOD"), py::arg("channel"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("format"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset_2));
  VolumeDataAccessManager_.def("requestVolumeSubset"         , [] (VolumeDataAccessManager* self, py::buffer buf, const native::VolumeDataLayout *layout, native::DimensionsND dimensions, int lod, int channel, py::array_t<int>& arg1, py::array_t<int>& arg2, VolumeDataChannelDescriptor::Format fmt, float replacementNoValue)
    {
      auto& minVoxelCoordinates = PyArrayAdapter<int, Dimensionality_Max, false>::getArrayChecked(arg1);
      auto& maxVoxelCoordinates = PyArrayAdapter<int, Dimensionality_Max, false>::getArrayChecked(arg2);
      int64_t bufferSize = self->GetVolumeSubsetBufferSize(layout, minVoxelCoordinates, maxVoxelCoordinates, fmt, lod);
      py::buffer_info info = buf.request(true);
      if (info.size * info.itemsize < bufferSize)
      {
        throw std::runtime_error("Insufficient buffer");
      }
      return self->RequestVolumeSubset(info.ptr, layout, dimensions, lod, channel, minVoxelCoordinates, maxVoxelCoordinates, fmt, replacementNoValue);
    }
  , py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lOD"), py::arg("channel"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("format"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestProjectedVolumeSubset", static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], const native::FloatVector4 &, native::DimensionsND, VolumeDataChannelDescriptor::Format, native::InterpolationMethod)>(&VolumeDataAccessManager::RequestProjectedVolumeSubset), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("voxelPlane"), py::arg("projectedDimensions"), py::arg("format"), py::arg("interpolationMethod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset));
  VolumeDataAccessManager_.def("requestProjectedVolumeSubset"         , [] (VolumeDataAccessManager* self, py::buffer buf, const native::VolumeDataLayout *layout, native::DimensionsND dimensions, int lod, int channel, py::array_t<int>& arg1, py::array_t<int>& arg2, const FloatVector4Adapter::AdaptedType &voxelPlane, native::DimensionsND projectedDimensions,  VolumeDataChannelDescriptor::Format fmt, native::InterpolationMethod interpolationMethod)
    {
      auto& minVoxelCoordinates = PyArrayAdapter<int, Dimensionality_Max, false>::getArrayChecked(arg1);
      auto& maxVoxelCoordinates = PyArrayAdapter<int, Dimensionality_Max, false>::getArrayChecked(arg2);
      int64_t bufferSize = self->GetProjectedVolumeSubsetBufferSize(layout, minVoxelCoordinates, maxVoxelCoordinates, projectedDimensions, fmt, lod);
      py::buffer_info info = buf.request(true);
      if (info.size * info.itemsize < bufferSize)
      {
        throw std::runtime_error("Insufficient buffer");
      }
      return self->RequestProjectedVolumeSubset(info.ptr, layout, dimensions, lod, channel, minVoxelCoordinates, maxVoxelCoordinates, voxelPlane, projectedDimensions, fmt, interpolationMethod);
    }
  , py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("voxelPlane"), py::arg("projectedDimensions"), py::arg("format"), py::arg("interpolationMethod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset));
// IMPLEMENTED :   VolumeDataAccessManager_.def("requestProjectedVolumeSubset", static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], const native::FloatVector4 &, native::DimensionsND, VolumeDataChannelDescriptor::Format, native::InterpolationMethod, float)>(&VolumeDataAccessManager::RequestProjectedVolumeSubset), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("voxelPlane"), py::arg("projectedDimensions"), py::arg("format"), py::arg("interpolationMethod"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset_2));
  VolumeDataAccessManager_.def("requestProjectedVolumeSubset"         , [] (VolumeDataAccessManager* self, py::buffer buf, const native::VolumeDataLayout *layout, native::DimensionsND dimensions, int lod, int channel, py::array_t<int>& arg1, py::array_t<int>& arg2, const FloatVector4Adapter::AdaptedType &voxelPlane, native::DimensionsND projectedDimensions,  VolumeDataChannelDescriptor::Format fmt, native::InterpolationMethod interpolationMethod, float replacementNoValue)
    {
      auto& minVoxelCoordinates = PyArrayAdapter<int, Dimensionality_Max, false>::getArrayChecked(arg1);
      auto& maxVoxelCoordinates = PyArrayAdapter<int, Dimensionality_Max, false>::getArrayChecked(arg2);
      int64_t bufferSize = self->GetProjectedVolumeSubsetBufferSize(layout, minVoxelCoordinates, maxVoxelCoordinates, projectedDimensions, fmt, lod);
      py::buffer_info info = buf.request(true);
      if (info.size * info.itemsize < bufferSize)
      {
        throw std::runtime_error("Insufficient buffer");
      }
      return self->RequestProjectedVolumeSubset(info.ptr, layout, dimensions, lod, channel, minVoxelCoordinates, maxVoxelCoordinates, voxelPlane, projectedDimensions, fmt, interpolationMethod, replacementNoValue);
    }
  , py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("minVoxelCoordinates"), py::arg("maxVoxelCoordinates"), py::arg("voxelPlane"), py::arg("projectedDimensions"), py::arg("format"), py::arg("interpolationMethod"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset_2));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestVolumeSamples"        , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod)>(&VolumeDataAccessManager::RequestVolumeSamples), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("samplePositions"), py::arg("sampleCount"), py::arg("interpolationMethod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples));
  VolumeDataAccessManager_.def("requestVolumeSamples"         , [] (VolumeDataAccessManager* self, py::buffer buf, const native::VolumeDataLayout *layout, native::DimensionsND dimensions, int lod, int channel, py::array_t<float>& sampleCoordinates, native::InterpolationMethod interpolationMethod)
    {
      int sampleCount = 0;
      auto& voxelCoordinates = PyArrayAdapter<float, Dimensionality_Max, false>::getArrayChecked(sampleCoordinates, &sampleCount);
      int64_t bufferSize = sizeof(float) * sampleCount;
      py::buffer_info info = buf.request(true);
      if (info.size * info.itemsize < bufferSize)
      {
        throw std::runtime_error("Insufficient buffer");
      }
      return self->RequestVolumeSamples((float*)info.ptr, layout, dimensions, lod, channel, &voxelCoordinates, sampleCount, interpolationMethod);
    }
  , py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("samplePositions"), py::arg("interpolationMethod"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestVolumeSamples"        , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod, float)>(&VolumeDataAccessManager::RequestVolumeSamples), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("samplePositions"), py::arg("sampleCount"), py::arg("interpolationMethod"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples_2));
  VolumeDataAccessManager_.def("requestVolumeSamples"         , [] (VolumeDataAccessManager* self, py::buffer buf, const native::VolumeDataLayout *layout, native::DimensionsND dimensions, int lod, int channel, py::array_t<float>& sampleCoordinates, native::InterpolationMethod interpolationMethod, float replacementNoValue)
    {
      int sampleCount = 0;
      auto& voxelCoordinates = PyArrayAdapter<float, Dimensionality_Max, false>::getArrayChecked(sampleCoordinates, &sampleCount);
      int64_t bufferSize = sizeof(float) * sampleCount;
      py::buffer_info info = buf.request(true);
      if (info.size * info.itemsize < bufferSize)
      {
        throw std::runtime_error("Insufficient buffer");
      }
      return self->RequestVolumeSamples((float*)info.ptr, layout, dimensions, lod, channel, &voxelCoordinates, sampleCount, interpolationMethod, replacementNoValue);
    }
  , py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("samplePositions"), py::arg("interpolationMethod"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples_2));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestVolumeTraces"         , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod, int)>(&VolumeDataAccessManager::RequestVolumeTraces), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("tracePositions"), py::arg("traceCount"), py::arg("interpolationMethod"), py::arg("traceDimension"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces));
  VolumeDataAccessManager_.def("requestVolumeTraces"         , [] (VolumeDataAccessManager* self, py::buffer buf, const native::VolumeDataLayout *layout, native::DimensionsND dimensions, int lod, int channel, py::array_t<float>& tracePositions, native::InterpolationMethod interpolationMethod, int traceDimension)
    {
      int traceCount = 0;
      auto& traceCoordinates = PyArrayAdapter<float, Dimensionality_Max, false>::getArrayChecked(tracePositions, &traceCount);
      int64_t bufferSize = sizeof(float) * traceCount;
      py::buffer_info info = buf.request(true);
      if (info.size * info.itemsize < bufferSize)
      {
        throw std::runtime_error("Insufficient buffer");
      }
      return self->RequestVolumeTraces((float*)info.ptr, layout, dimensions, lod, channel, &traceCoordinates, traceCount, interpolationMethod, traceDimension);
    }
  , py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("tracePositions"), py::arg("interpolationMethod"), py::arg("traceDimension"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestVolumeTraces"         , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod, int, float)>(&VolumeDataAccessManager::RequestVolumeTraces), py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("tracePositions"), py::arg("traceCount"), py::arg("interpolationMethod"), py::arg("traceDimension"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces_2));
  VolumeDataAccessManager_.def("requestVolumeTraces"         , [] (VolumeDataAccessManager* self, py::buffer buf, const native::VolumeDataLayout *layout, native::DimensionsND dimensions, int lod, int channel, py::array_t<float>& tracePositions, native::InterpolationMethod interpolationMethod, int traceDimension, float replacementNoValue)
    {
      int traceCount = 0;
      auto& traceCoordinates = PyArrayAdapter<float, Dimensionality_Max, false>::getArrayChecked(tracePositions, &traceCount);
      int64_t bufferSize = sizeof(float) * traceCount;
      py::buffer_info info = buf.request(true);
      if (info.size * info.itemsize < bufferSize)
      {
        throw std::runtime_error("Insufficient buffer");
      }
      return self->RequestVolumeTraces((float*)info.ptr, layout, dimensions, lod, channel, &traceCoordinates, traceCount, interpolationMethod, traceDimension, replacementNoValue);
    }
  , py::arg("buffer"), py::arg("volumeDataLayout"), py::arg("dimensionsND"), py::arg("lod"), py::arg("channel"), py::arg("tracePositions"), py::arg("interpolationMethod"), py::arg("traceDimension"), py::arg("replacementNoValue"), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces_2));

// IMPLEMENTED :   VolumeDataAccessManager_.def("getCurrentUploadError"       , static_cast<void(VolumeDataAccessManager::*)(const char **, int32_t *, const char **)>(&VolumeDataAccessManager::GetCurrentUploadError), py::arg("objectId"), py::arg("errorCode"), py::arg("errorString"), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetCurrentUploadError));
  VolumeDataAccessManager_.def("getCurrentUploadError"       , [](VolumeDataAccessManager* self)
    {
      const char
        *pObjectID = nullptr,
        *pErrorString = nullptr;

      int32_t
        errorCode = 0;

      self->GetCurrentUploadError(&pObjectID, &errorCode, &pErrorString);
      return std::make_tuple(std::string(pObjectID), errorCode, std::string(pErrorString));
    }, OPENVDS_DOCSTRING(VolumeDataAccessManager_GetCurrentUploadError));
}

