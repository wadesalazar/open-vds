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

#include "PyVolumeDataAccessManager.h"

using namespace native;

template<typename INDEX_TYPE>
static void
RegisterVolumeDataRegions(py::module& m, const char* name)
{
  typedef typename VectorAdapter<typename INDEX_TYPE::element_type, INDEX_TYPE::element_count>::AdaptedType AdaptedIndexType;
  typedef VolumeDataRegions<INDEX_TYPE>                   RegionsType;
  typedef std::tuple<AdaptedIndexType, AdaptedIndexType>  IndexRegionType;

  py::class_<RegionsType, std::unique_ptr<RegionsType, py::nodelete>> 
    VolumeDataRegions_(m, name, OPENVDS_DOCSTRING(VolumeDataRegions));

  VolumeDataRegions_.def_property_readonly("regionCount", &RegionsType::RegionCount);
  VolumeDataRegions_.def("region", [](RegionsType* self, int64_t region)
    {
      auto r = self->Region(region);
      IndexRegionType result = std::make_tuple((AdaptedIndexType)r.Min, (AdaptedIndexType)r.Max);
      return result;
    }, OPENVDS_DOCSTRING(VolumeDataRegions_Region)
  );
  VolumeDataRegions_.def("regionFromIndex", [](RegionsType* self, AdaptedIndexType index)
    {
      return self->RegionFromIndex(index);
    }, OPENVDS_DOCSTRING(VolumeDataRegions_RegionFromIndex)
  );
}

template<typename INDEX_TYPE>
static void
RegisterVolumeDataAccessorWithRegions(py::module& m, const char* name)
{
  typedef typename VectorAdapter<typename INDEX_TYPE::element_type, INDEX_TYPE::element_count>::AdaptedType AdaptedIndexType;
  typedef VolumeDataRegions<INDEX_TYPE>                   RegionsType;
  typedef VolumeDataAccessorWithRegions<INDEX_TYPE>       AccessorWithRegionsType;
  typedef std::tuple<AdaptedIndexType, AdaptedIndexType>  IndexRegionType;

  py::class_<AccessorWithRegionsType, RegionsType, VolumeDataAccessor, std::unique_ptr<AccessorWithRegionsType, py::nodelete>> 
    VolumeDataAccessorWithRegions_(m, name, OPENVDS_DOCSTRING(VolumeDataAccessorWithRegions));

  VolumeDataAccessorWithRegions_.def_property_readonly("currentRegion", [](AccessorWithRegionsType* self)
    {
      auto r = self->CurrentRegion();
      IndexRegionType result = std::make_tuple((AdaptedIndexType)r.Min, (AdaptedIndexType)r.Max);
      return result;
    }, OPENVDS_DOCSTRING(VolumeDataAccessorWithRegions_CurrentRegion)
  );
}

template<typename INDEX_TYPE, typename T>
static void
RegisterVolumeDataReadAccessor(py::module& m, const char* name)
{
  typedef typename VectorAdapter<typename INDEX_TYPE::element_type, INDEX_TYPE::element_count>::AdaptedType AdaptedIndexType;
  typedef VolumeDataAccessorWithRegions<INDEX_TYPE>       AccessorWithRegionsType;
  typedef VolumeDataReadAccessor<INDEX_TYPE, T>           AccessorType;

  py::class_<AccessorType, AccessorWithRegionsType, std::unique_ptr<AccessorType, py::nodelete>> 
    VolumeDataAccessor_(m, name, OPENVDS_DOCSTRING(VolumeDataAccessor));

  VolumeDataAccessor_.def("getValue", [](AccessorType* self, AdaptedIndexType index)
    {
      return self->GetValue(index);
    }
  );
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
PyVolumeDataAccessManager::initModule(py::module& m)
{
//AUTOGEN-BEGIN
  py::enum_<VDSProduceStatus> 
    VDSProduceStatus_(m,"VDSProduceStatus", OPENVDS_DOCSTRING(VDSProduceStatus));

  VDSProduceStatus_.value("Normal"                      , VDSProduceStatus::Normal                , OPENVDS_DOCSTRING(VDSProduceStatus_Normal));
  VDSProduceStatus_.value("Remapped"                    , VDSProduceStatus::Remapped              , OPENVDS_DOCSTRING(VDSProduceStatus_Remapped));
  VDSProduceStatus_.value("Unavailable"                 , VDSProduceStatus::Unavailable           , OPENVDS_DOCSTRING(VDSProduceStatus_Unavailable));

  // IVolumeDataAccessor
  py::class_<IVolumeDataAccessor, std::unique_ptr<IVolumeDataAccessor, py::nodelete>> 
    IVolumeDataAccessor_(m,"IVolumeDataAccessor", OPENVDS_DOCSTRING(IVolumeDataAccessor));

  IVolumeDataAccessor_.def("getManager"                  , static_cast<native::IVolumeDataAccessor::Manager &(IVolumeDataAccessor::*)()>(&IVolumeDataAccessor::GetManager), OPENVDS_DOCSTRING(IVolumeDataAccessor_GetManager));
  IVolumeDataAccessor_.def_property_readonly("manager", &IVolumeDataAccessor::GetManager, OPENVDS_DOCSTRING(IVolumeDataAccessor_GetManager));
  IVolumeDataAccessor_.def("getLayout"                   , static_cast<const native::VolumeDataLayout *(IVolumeDataAccessor::*)()>(&IVolumeDataAccessor::GetLayout), OPENVDS_DOCSTRING(IVolumeDataAccessor_GetLayout));
  IVolumeDataAccessor_.def_property_readonly("layout", &IVolumeDataAccessor::GetLayout, OPENVDS_DOCSTRING(IVolumeDataAccessor_GetLayout));

  // IVolumeDataAccessor::Manager
  py::class_<IVolumeDataAccessor::Manager, std::unique_ptr<IVolumeDataAccessor::Manager, py::nodelete>> 
    IVolumeDataAccessor_Manager_(IVolumeDataAccessor_,"IVolumeDataAccessor::Manager", OPENVDS_DOCSTRING(IVolumeDataAccessor_Manager));

  IVolumeDataAccessor_Manager_.def("destroyVolumeDataAccessor"   , static_cast<void(IVolumeDataAccessor::Manager::*)(native::IVolumeDataAccessor *)>(&IVolumeDataAccessor::Manager::DestroyVolumeDataAccessor), py::arg("accessor").none(false), OPENVDS_DOCSTRING(IVolumeDataAccessor_Manager_DestroyVolumeDataAccessor));
  IVolumeDataAccessor_Manager_.def("cloneVolumeDataAccessor"     , static_cast<native::IVolumeDataAccessor *(IVolumeDataAccessor::Manager::*)(const native::IVolumeDataAccessor &)>(&IVolumeDataAccessor::Manager::CloneVolumeDataAccessor), py::arg("accessor").none(false), OPENVDS_DOCSTRING(IVolumeDataAccessor_Manager_CloneVolumeDataAccessor));

  // IVolumeDataAccessor::IndexOutOfRangeException
  py::class_<IVolumeDataAccessor::IndexOutOfRangeException, std::unique_ptr<IVolumeDataAccessor::IndexOutOfRangeException>> 
    IVolumeDataAccessor_IndexOutOfRangeException_(IVolumeDataAccessor_,"IVolumeDataAccessor::IndexOutOfRangeException", OPENVDS_DOCSTRING(IVolumeDataAccessor_IndexOutOfRangeException));

  // IVolumeDataAccessor::ReadErrorException
  py::class_<IVolumeDataAccessor::ReadErrorException, std::unique_ptr<IVolumeDataAccessor::ReadErrorException>> 
    IVolumeDataAccessor_ReadErrorException_(IVolumeDataAccessor_,"IVolumeDataAccessor::ReadErrorException", OPENVDS_DOCSTRING(IVolumeDataAccessor_ReadErrorException));

  IVolumeDataAccessor_ReadErrorException_.def_readwrite("message"                     , &IVolumeDataAccessor::ReadErrorException::message, OPENVDS_DOCSTRING(IVolumeDataAccessor_ReadErrorException_message));
  IVolumeDataAccessor_ReadErrorException_.def_readwrite("errorCode"                   , &IVolumeDataAccessor::ReadErrorException::errorCode, OPENVDS_DOCSTRING(IVolumeDataAccessor_ReadErrorException_errorCode));

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
  VolumeDataPageAccessor_.def_property_readonly("layout", &VolumeDataPageAccessor::GetLayout, OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLayout));
  VolumeDataPageAccessor_.def("getLOD"                      , static_cast<int(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetLOD), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLOD));
  VolumeDataPageAccessor_.def_property_readonly("LOD", &VolumeDataPageAccessor::GetLOD, OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLOD));
  VolumeDataPageAccessor_.def("getChannelIndex"             , static_cast<int(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetChannelIndex), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelIndex));
  VolumeDataPageAccessor_.def_property_readonly("channelIndex", &VolumeDataPageAccessor::GetChannelIndex, OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelIndex));
  VolumeDataPageAccessor_.def("getChannelDescriptor"        , static_cast<const native::VolumeDataChannelDescriptor &(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetChannelDescriptor), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelDescriptor));
  VolumeDataPageAccessor_.def_property_readonly("channelDescriptor", &VolumeDataPageAccessor::GetChannelDescriptor, OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelDescriptor));
  VolumeDataPageAccessor_.def("getNumSamples"               , [](VolumeDataPageAccessor* self, py::array_t<int>& numSamples) { return self->GetNumSamples(PyArrayAdapter<int, 6, true>::getArrayChecked(numSamples)); }, py::arg("numSamples"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetNumSamples));
  VolumeDataPageAccessor_.def("getChunkCount"               , static_cast<int64_t(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetChunkCount), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkCount));
  VolumeDataPageAccessor_.def_property_readonly("chunkCount", &VolumeDataPageAccessor::GetChunkCount, OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkCount));
  VolumeDataPageAccessor_.def("getChunkMinMax"              , [](VolumeDataPageAccessor* self, int64_t chunk, py::array_t<int>& min, py::array_t<int>& max) { return self->GetChunkMinMax(chunk, PyArrayAdapter<int, 6, true>::getArrayChecked(min), PyArrayAdapter<int, 6, true>::getArrayChecked(max)); }, py::arg("chunk"), py::arg("min"), py::arg("max"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkMinMax));
  VolumeDataPageAccessor_.def("getChunkMinMaxExcludingMargin", [](VolumeDataPageAccessor* self, int64_t chunk, py::array_t<int>& minExcludingMargin, py::array_t<int>& maxExcludingMargin) { return self->GetChunkMinMaxExcludingMargin(chunk, PyArrayAdapter<int, 6, true>::getArrayChecked(minExcludingMargin), PyArrayAdapter<int, 6, true>::getArrayChecked(maxExcludingMargin)); }, py::arg("chunk"), py::arg("minExcludingMargin"), py::arg("maxExcludingMargin"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkMinMaxExcludingMargin));
  VolumeDataPageAccessor_.def("getChunkIndex"               , [](VolumeDataPageAccessor* self, const py::array_t<int>& position) { return self->GetChunkIndex(PyArrayAdapter<int, 6, false>::getArrayChecked(position)); }, py::arg("position"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkIndex));
  VolumeDataPageAccessor_.def("addReference"                , static_cast<int(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::AddReference), OPENVDS_DOCSTRING(VolumeDataPageAccessor_AddReference));
  VolumeDataPageAccessor_.def("removeReference"             , static_cast<int(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::RemoveReference), OPENVDS_DOCSTRING(VolumeDataPageAccessor_RemoveReference));
  VolumeDataPageAccessor_.def("getMaxPages"                 , static_cast<int(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::GetMaxPages), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetMaxPages));
  VolumeDataPageAccessor_.def_property_readonly("maxPages", &VolumeDataPageAccessor::GetMaxPages, OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetMaxPages));
  VolumeDataPageAccessor_.def("setMaxPages"                 , static_cast<void(VolumeDataPageAccessor::*)(int)>(&VolumeDataPageAccessor::SetMaxPages), py::arg("maxPages").none(false), OPENVDS_DOCSTRING(VolumeDataPageAccessor_SetMaxPages));
  VolumeDataPageAccessor_.def("createPage"                  , static_cast<native::VolumeDataPage *(VolumeDataPageAccessor::*)(int64_t)>(&VolumeDataPageAccessor::CreatePage), py::arg("chunkIndex").none(false), OPENVDS_DOCSTRING(VolumeDataPageAccessor_CreatePage));
  VolumeDataPageAccessor_.def("readPage"                    , static_cast<native::VolumeDataPage *(VolumeDataPageAccessor::*)(int64_t)>(&VolumeDataPageAccessor::ReadPage), py::arg("chunkIndex").none(false), OPENVDS_DOCSTRING(VolumeDataPageAccessor_ReadPage));
  VolumeDataPageAccessor_.def("readPageAtPosition"          , [](VolumeDataPageAccessor* self, const py::array_t<int>& position) { return self->ReadPageAtPosition(PyArrayAdapter<int, 6, false>::getArrayChecked(position)); }, py::arg("position"), OPENVDS_DOCSTRING(VolumeDataPageAccessor_ReadPageAtPosition));
  VolumeDataPageAccessor_.def("commit"                      , static_cast<void(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::Commit), OPENVDS_DOCSTRING(VolumeDataPageAccessor_Commit));

//AUTOGEN-END

// IMPLEMENTED :   VolumeDataAccessManager_.def("createVolumeDataPageAccessor", static_cast<native::VolumeDataPageAccessor *(VolumeDataAccessManager::*)(const native::VolumeDataLayout *, native::DimensionsND, int, int, int, native::VolumeDataAccessManager::AccessMode, int)>(&VolumeDataAccessManager::CreateVolumeDataPageAccessor), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("maxPages").none(false), py::arg("accessMode").none(false), py::arg("chunkMetadataPageSize").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_CreateVolumeDataPageAccessor));
  VolumeDataAccessManager_.def("createVolumeDataPageAccessor", static_cast<native::VolumeDataPageAccessor *(VolumeDataAccessManager::*)(const native::VolumeDataLayout *, native::DimensionsND, int, int, int, native::VolumeDataAccessManager::AccessMode, int)>(&VolumeDataAccessManager::CreateVolumeDataPageAccessor), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("maxPages").none(false), py::arg("accessMode").none(false), py::arg("chunkMetadataPageSize").none(false) = 1024, OPENVDS_DOCSTRING(VolumeDataAccessManager_CreateVolumeDataPageAccessor));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestVolumeSubset"         , static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], VolumeDataChannelDescriptor::Format)>(&VolumeDataAccessManager::RequestVolumeSubset), py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("minVoxelCoordinates").none(false), py::arg("maxVoxelCoordinates").none(false), py::arg("format").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset));
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
  , py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("minVoxelCoordinates").none(false), py::arg("maxVoxelCoordinates").none(false), py::arg("format").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestVolumeSubset"         , static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], VolumeDataChannelDescriptor::Format, float)>(&VolumeDataAccessManager::RequestVolumeSubset), py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lOD").none(false), py::arg("channel").none(false), py::arg("minVoxelCoordinates").none(false), py::arg("maxVoxelCoordinates").none(false), py::arg("format").none(false), py::arg("replacementNoValue").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset_2));
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
  , py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lOD").none(false), py::arg("channel").none(false), py::arg("minVoxelCoordinates").none(false), py::arg("maxVoxelCoordinates").none(false), py::arg("format").none(false), py::arg("replacementNoValue").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSubset));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestProjectedVolumeSubset", static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], const native::FloatVector4 &, native::DimensionsND, VolumeDataChannelDescriptor::Format, native::InterpolationMethod)>(&VolumeDataAccessManager::RequestProjectedVolumeSubset), py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("minVoxelCoordinates").none(false), py::arg("maxVoxelCoordinates").none(false), py::arg("voxelPlane").none(false), py::arg("projectedDimensions").none(false), py::arg("format").none(false), py::arg("interpolationMethod").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset));
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
  , py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("minVoxelCoordinates").none(false), py::arg("maxVoxelCoordinates").none(false), py::arg("voxelPlane").none(false), py::arg("projectedDimensions").none(false), py::arg("format").none(false), py::arg("interpolationMethod").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset));
// IMPLEMENTED :   VolumeDataAccessManager_.def("requestProjectedVolumeSubset", static_cast<int64_t(VolumeDataAccessManager::*)(void *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const int (&)[6], const int (&)[6], const native::FloatVector4 &, native::DimensionsND, VolumeDataChannelDescriptor::Format, native::InterpolationMethod, float)>(&VolumeDataAccessManager::RequestProjectedVolumeSubset), py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("minVoxelCoordinates").none(false), py::arg("maxVoxelCoordinates").none(false), py::arg("voxelPlane").none(false), py::arg("projectedDimensions").none(false), py::arg("format").none(false), py::arg("interpolationMethod").none(false), py::arg("replacementNoValue").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset_2));
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
  , py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("minVoxelCoordinates").none(false), py::arg("maxVoxelCoordinates").none(false), py::arg("voxelPlane").none(false), py::arg("projectedDimensions").none(false), py::arg("format").none(false), py::arg("interpolationMethod").none(false), py::arg("replacementNoValue").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestProjectedVolumeSubset_2));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestVolumeSamples"        , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod)>(&VolumeDataAccessManager::RequestVolumeSamples), py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("samplePositions").none(false), py::arg("sampleCount").none(false), py::arg("interpolationMethod").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples));
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
  , py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("samplePositions").none(false), py::arg("interpolationMethod").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestVolumeSamples"        , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod, float)>(&VolumeDataAccessManager::RequestVolumeSamples), py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("samplePositions").none(false), py::arg("sampleCount").none(false), py::arg("interpolationMethod").none(false), py::arg("replacementNoValue").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples_2));
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
  , py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("samplePositions").none(false), py::arg("interpolationMethod").none(false), py::arg("replacementNoValue").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeSamples_2));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestVolumeTraces"         , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod, int)>(&VolumeDataAccessManager::RequestVolumeTraces), py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("tracePositions").none(false), py::arg("traceCount").none(false), py::arg("interpolationMethod").none(false), py::arg("traceDimension").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces));
  VolumeDataAccessManager_.def("requestVolumeTraces"         , [] (VolumeDataAccessManager* self, py::buffer buf, const native::VolumeDataLayout *layout, native::DimensionsND dimensions, int lod, int channel, py::array_t<float>& tracePositions, native::InterpolationMethod interpolationMethod, int traceDimension)
    {
      int traceCount = 0;
      auto& traceCoordinates = PyArrayAdapter<float, Dimensionality_Max, false>::getArrayChecked(tracePositions, &traceCount);
      int64_t bufferSize = self->GetVolumeTracesBufferSize(layout, traceCount, traceDimension, lod);
      py::buffer_info info = buf.request(true);
      if (info.size * info.itemsize < bufferSize)
      {
        throw std::runtime_error("Insufficient buffer");
      }
      return self->RequestVolumeTraces((float*)info.ptr, layout, dimensions, lod, channel, &traceCoordinates, traceCount, interpolationMethod, traceDimension);
    }
  , py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("tracePositions").none(false), py::arg("interpolationMethod").none(false), py::arg("traceDimension").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces));

// IMPLEMENTED :   VolumeDataAccessManager_.def("requestVolumeTraces"         , static_cast<int64_t(VolumeDataAccessManager::*)(float *, const native::VolumeDataLayout *, native::DimensionsND, int, int, const float (*)[6], int, native::InterpolationMethod, int, float)>(&VolumeDataAccessManager::RequestVolumeTraces), py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("tracePositions").none(false), py::arg("traceCount").none(false), py::arg("interpolationMethod").none(false), py::arg("traceDimension").none(false), py::arg("replacementNoValue").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces_2));
  VolumeDataAccessManager_.def("requestVolumeTraces"         , [] (VolumeDataAccessManager* self, py::buffer buf, const native::VolumeDataLayout *layout, native::DimensionsND dimensions, int lod, int channel, py::array_t<float>& tracePositions, native::InterpolationMethod interpolationMethod, int traceDimension, float replacementNoValue)
    {
      int traceCount = 0;
      auto& traceCoordinates = PyArrayAdapter<float, Dimensionality_Max, false>::getArrayChecked(tracePositions, &traceCount);
      int64_t bufferSize = self->GetVolumeTracesBufferSize(layout, traceCount, traceDimension, lod);
      py::buffer_info info = buf.request(true);
      if (info.size * info.itemsize < bufferSize)
      {
        throw std::runtime_error("Insufficient buffer");
      }
      return self->RequestVolumeTraces((float*)info.ptr, layout, dimensions, lod, channel, &traceCoordinates, traceCount, interpolationMethod, traceDimension, replacementNoValue);
    }
  , py::arg("buffer").none(false), py::arg("volumeDataLayout").none(false), py::arg("dimensionsND").none(false), py::arg("lod").none(false), py::arg("channel").none(false), py::arg("tracePositions").none(false), py::arg("interpolationMethod").none(false), py::arg("traceDimension").none(false), py::arg("replacementNoValue").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_RequestVolumeTraces_2));

// IMPLEMENTED :   VolumeDataAccessManager_.def("flushUploadQueue"            , static_cast<void(VolumeDataAccessManager::*)(bool)>(&VolumeDataAccessManager::FlushUploadQueue), py::arg("writeUpdatedLayerStatus").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_FlushUploadQueue));
VolumeDataAccessManager_.def("flushUploadQueue"            , static_cast<void(VolumeDataAccessManager::*)(bool)>(&VolumeDataAccessManager::FlushUploadQueue), py::arg("writeUpdatedLayerStatus").none(false) = true, OPENVDS_DOCSTRING(VolumeDataAccessManager_FlushUploadQueue));

// IMPLEMENTED :   VolumeDataAccessManager_.def("getCurrentUploadError"       , static_cast<void(VolumeDataAccessManager::*)(const char **, int32_t *, const char **)>(&VolumeDataAccessManager::GetCurrentUploadError), py::arg("objectId").none(false), py::arg("errorCode").none(false), py::arg("errorString").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetCurrentUploadError));
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

  // IMPLEMENTED :   VolumeDataAccessManager_.def("getCurrentDownloadError"     , static_cast<void(VolumeDataAccessManager::*)(int *, const char **)>(&VolumeDataAccessManager::GetCurrentDownloadError), py::arg("code").none(false), py::arg("errorString").none(false), OPENVDS_DOCSTRING(VolumeDataAccessManager_GetCurrentDownloadError));
  VolumeDataAccessManager_.def("getCurrentDownloadError"       , [](VolumeDataAccessManager* self)
    {
      const char
        *pErrorString = nullptr;

      int32_t
        errorCode = 0;

      self->GetCurrentDownloadError(&errorCode, &pErrorString);
      return std::make_tuple(errorCode, std::string(pErrorString));
    }, OPENVDS_DOCSTRING(VolumeDataAccessManager_GetCurrentDownloadError));

  VolumeDataAccessManager_.attr("Dimensionality_Max") = py::int_(VolumeDataAccessManager::Dimensionality_Max);

  // Register region types
  RegisterVolumeDataRegions<native::IntVector2>      (m, "VolumeDataRegionsIntVector2");
  RegisterVolumeDataRegions<native::IntVector3>      (m, "VolumeDataRegionsIntVector3");
  RegisterVolumeDataRegions<native::IntVector4>      (m, "VolumeDataRegionsIntVector4");
  RegisterVolumeDataRegions<native::FloatVector2>    (m, "VolumeDataRegionsFloatVector2");
  RegisterVolumeDataRegions<native::FloatVector3>    (m, "VolumeDataRegionsFloatVector3");
  RegisterVolumeDataRegions<native::FloatVector4>    (m, "VolumeDataRegionsFloatVector4");

  // Register accessor base types
  RegisterVolumeDataAccessorWithRegions<native::IntVector2>      (m, "VolumeDataAccessorWithRegionsIntVector2");
  RegisterVolumeDataAccessorWithRegions<native::IntVector3>      (m, "VolumeDataAccessorWithRegionsIntVector3");
  RegisterVolumeDataAccessorWithRegions<native::IntVector4>      (m, "VolumeDataAccessorWithRegionsIntVector4");
  RegisterVolumeDataAccessorWithRegions<native::FloatVector2>    (m, "VolumeDataAccessorWithRegionsFloatVector2");
  RegisterVolumeDataAccessorWithRegions<native::FloatVector3>    (m, "VolumeDataAccessorWithRegionsFloatVector3");
  RegisterVolumeDataAccessorWithRegions<native::FloatVector4>    (m, "VolumeDataAccessorWithRegionsFloatVector4");

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

}

