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

static uint32_t GetItemSize(VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components)
{
  switch(format)
  {
  default:
    throw std::runtime_error("Illegal format");
  case VolumeDataChannelDescriptor::Format_1Bit:
    return 1;
  case VolumeDataChannelDescriptor::Format_U8:
    return 1 * components;
  case VolumeDataChannelDescriptor::Format_U16:
    return 2 * components;
  case VolumeDataChannelDescriptor::Format_R32:
  case VolumeDataChannelDescriptor::Format_U32:
    return 4 * components;
  case VolumeDataChannelDescriptor::Format_U64:
  case VolumeDataChannelDescriptor::Format_R64:
    return 8 * components;
  }
}

static const char *
GetPythonFormatString(VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components)
{
  switch(format)
  {
  default:
    throw std::runtime_error("Unknown format");
  case VolumeDataChannelDescriptor::Format_U8:
    return "BBBB" + (4 - components);
  case VolumeDataChannelDescriptor::Format_U16:
    return "HHHH" + (4 - components);
  case VolumeDataChannelDescriptor::Format_R32:
    return "ffff" + (4 - components);
  case VolumeDataChannelDescriptor::Format_U32:
    return "IIII" + (4 - components);
  case VolumeDataChannelDescriptor::Format_R64:
    return "dddd" + (4 - components);
  case VolumeDataChannelDescriptor::Format_U64:
    return "QQQQ" + (4 - components);
  }
}

template<typename INDEX_TYPE, typename T>
static void
RegisterVolumeDataReadAccessor(py::module& m, const char* name)
{
  typedef typename VectorAdapter<typename INDEX_TYPE::element_type, INDEX_TYPE::element_count>::AdaptedType AdaptedIndexType;
  typedef VolumeDataReadAccessor<INDEX_TYPE, T>           AccessorType;
  typedef std::tuple<AdaptedIndexType, AdaptedIndexType>  IndexRegionType;

  py::class_<AccessorType, std::unique_ptr<AccessorType, py::nodelete>> 
    VolumeDataAccessor_(m, name, "A class that provides random read access to the voxel values of a VDS");

  VolumeDataAccessor_.def_property_readonly("regionCount", &AccessorType::RegionCount);
  VolumeDataAccessor_.def("region", [](AccessorType* self, int64_t region)
    {
      auto r = self->Region(region);
      IndexRegionType result = std::make_tuple((AdaptedIndexType)r.Min, (AdaptedIndexType)r.Max);
      return result;
    }, OPENVDS_DOCSTRING(IVolumeDataRegions_Region)
  );
  VolumeDataAccessor_.def("regionFromIndex", [](AccessorType* self, AdaptedIndexType index)
    {
      return self->RegionFromIndex(index);
    }, OPENVDS_DOCSTRING(IVolumeDataRegions_RegionFromIndex)
  );

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
    VolumeDataAccessor_(m, name, "A class that provides random read/write access to the voxel values of a VDS");

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
//AUTOGEN-BEGIN
  py::enum_<VDSProduceStatus> 
    VDSProduceStatus_(m,"VDSProduceStatus", OPENVDS_DOCSTRING(VDSProduceStatus));

  VDSProduceStatus_.value("Normal"                      , VDSProduceStatus::Normal                , OPENVDS_DOCSTRING(VDSProduceStatus_Normal));
  VDSProduceStatus_.value("Remapped"                    , VDSProduceStatus::Remapped              , OPENVDS_DOCSTRING(VDSProduceStatus_Remapped));
  VDSProduceStatus_.value("Unavailable"                 , VDSProduceStatus::Unavailable           , OPENVDS_DOCSTRING(VDSProduceStatus_Unavailable));

  // IVolumeDataAccessor
  py::class_<IVolumeDataAccessor, std::unique_ptr<IVolumeDataAccessor, py::nodelete>> 
    IVolumeDataAccessor_(m,"IVolumeDataAccessor", OPENVDS_DOCSTRING(IVolumeDataAccessor));

  IVolumeDataAccessor_.def("getManager"                  , static_cast<native::IVolumeDataAccessor::Manager &(IVolumeDataAccessor::*)()>(&IVolumeDataAccessor::GetManager), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(IVolumeDataAccessor_GetManager));
  IVolumeDataAccessor_.def_property_readonly("manager", &IVolumeDataAccessor::GetManager, OPENVDS_DOCSTRING(IVolumeDataAccessor_GetManager));
  IVolumeDataAccessor_.def("getLayout"                   , static_cast<const native::VolumeDataLayout *(IVolumeDataAccessor::*)()>(&IVolumeDataAccessor::GetLayout), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(IVolumeDataAccessor_GetLayout));
  IVolumeDataAccessor_.def_property_readonly("layout", &IVolumeDataAccessor::GetLayout, OPENVDS_DOCSTRING(IVolumeDataAccessor_GetLayout));

  // IVolumeDataAccessor::Manager
  py::class_<IVolumeDataAccessor::Manager, std::unique_ptr<IVolumeDataAccessor::Manager, py::nodelete>> 
    IVolumeDataAccessor_Manager_(IVolumeDataAccessor_,"IVolumeDataAccessor::Manager", OPENVDS_DOCSTRING(IVolumeDataAccessor_Manager));

  IVolumeDataAccessor_Manager_.def("destroyVolumeDataAccessor"   , static_cast<void(IVolumeDataAccessor::Manager::*)(native::IVolumeDataAccessor *)>(&IVolumeDataAccessor::Manager::DestroyVolumeDataAccessor), py::arg("accessor").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(IVolumeDataAccessor_Manager_DestroyVolumeDataAccessor));
  IVolumeDataAccessor_Manager_.def("cloneVolumeDataAccessor"     , static_cast<native::IVolumeDataAccessor *(IVolumeDataAccessor::Manager::*)(const native::IVolumeDataAccessor &)>(&IVolumeDataAccessor::Manager::CloneVolumeDataAccessor), py::arg("accessor").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(IVolumeDataAccessor_Manager_CloneVolumeDataAccessor));

  // IVolumeDataAccessor::IndexOutOfRangeException
  py::class_<IVolumeDataAccessor::IndexOutOfRangeException> 
    IVolumeDataAccessor_IndexOutOfRangeException_(IVolumeDataAccessor_,"IVolumeDataAccessor::IndexOutOfRangeException", OPENVDS_DOCSTRING(IVolumeDataAccessor_IndexOutOfRangeException));

  // IVolumeDataAccessor::ReadErrorException
  py::class_<IVolumeDataAccessor::ReadErrorException> 
    IVolumeDataAccessor_ReadErrorException_(IVolumeDataAccessor_,"IVolumeDataAccessor::ReadErrorException", OPENVDS_DOCSTRING(IVolumeDataAccessor_ReadErrorException));

  IVolumeDataAccessor_ReadErrorException_.def_readwrite("message"                     , &IVolumeDataAccessor::ReadErrorException::message, OPENVDS_DOCSTRING(IVolumeDataAccessor_ReadErrorException_message));
  IVolumeDataAccessor_ReadErrorException_.def_readwrite("errorCode"                   , &IVolumeDataAccessor::ReadErrorException::errorCode, OPENVDS_DOCSTRING(IVolumeDataAccessor_ReadErrorException_errorCode));

  // VolumeDataPage
  py::class_<VolumeDataPage, std::unique_ptr<VolumeDataPage, py::nodelete>> 
    VolumeDataPage_(m,"VolumeDataPage", OPENVDS_DOCSTRING(VolumeDataPage));

  VolumeDataPage_.def("getVolumeDataPageAccessor"   , static_cast<native::VolumeDataPageAccessor &(VolumeDataPage::*)() const>(&VolumeDataPage::GetVolumeDataPageAccessor), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPage_GetVolumeDataPageAccessor));
  VolumeDataPage_.def_property_readonly("volumeDataPageAccessor", &VolumeDataPage::GetVolumeDataPageAccessor, OPENVDS_DOCSTRING(VolumeDataPage_GetVolumeDataPageAccessor));
  VolumeDataPage_.def("getMinMax"                   , [](VolumeDataPage* self, py::array_t<int>& min, py::array_t<int>& max) { return self->GetMinMax(PyArrayAdapter<int, 6, true>::getArrayChecked(min), PyArrayAdapter<int, 6, true>::getArrayChecked(max)); }, py::arg("min").none(false), py::arg("max").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPage_GetMinMax));
  VolumeDataPage_.def("getMinMaxExcludingMargin"    , [](VolumeDataPage* self, py::array_t<int>& minExcludingMargin, py::array_t<int>& maxExcludingMargin) { return self->GetMinMaxExcludingMargin(PyArrayAdapter<int, 6, true>::getArrayChecked(minExcludingMargin), PyArrayAdapter<int, 6, true>::getArrayChecked(maxExcludingMargin)); }, py::arg("minExcludingMargin").none(false), py::arg("maxExcludingMargin").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPage_GetMinMaxExcludingMargin));
  VolumeDataPage_.def("updateWrittenRegion"         , [](VolumeDataPage* self, const py::array_t<int>& writtenMin, const py::array_t<int>& writtenMax) { return self->UpdateWrittenRegion(PyArrayAdapter<int, 6, false>::getArrayChecked(writtenMin), PyArrayAdapter<int, 6, false>::getArrayChecked(writtenMax)); }, py::arg("writtenMin").none(false), py::arg("writtenMax").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPage_UpdateWrittenRegion));
  VolumeDataPage_.def("release"                     , static_cast<void(VolumeDataPage::*)()>(&VolumeDataPage::Release), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPage_Release));

  // VolumeDataPageAccessor
  py::class_<VolumeDataPageAccessor, std::unique_ptr<VolumeDataPageAccessor, py::nodelete>> 
    VolumeDataPageAccessor_(m,"VolumeDataPageAccessor", OPENVDS_DOCSTRING(VolumeDataPageAccessor));

  VolumeDataPageAccessor_.def("getLayout"                   , static_cast<const native::VolumeDataLayout *(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetLayout), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLayout));
  VolumeDataPageAccessor_.def_property_readonly("layout", &VolumeDataPageAccessor::GetLayout, OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLayout));
  VolumeDataPageAccessor_.def("getLOD"                      , static_cast<int(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetLOD), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLOD));
  VolumeDataPageAccessor_.def_property_readonly("LOD", &VolumeDataPageAccessor::GetLOD, OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetLOD));
  VolumeDataPageAccessor_.def("getChannelIndex"             , static_cast<int(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetChannelIndex), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelIndex));
  VolumeDataPageAccessor_.def_property_readonly("channelIndex", &VolumeDataPageAccessor::GetChannelIndex, OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelIndex));
  VolumeDataPageAccessor_.def("getChannelDescriptor"        , static_cast<const native::VolumeDataChannelDescriptor &(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetChannelDescriptor), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelDescriptor));
  VolumeDataPageAccessor_.def_property_readonly("channelDescriptor", &VolumeDataPageAccessor::GetChannelDescriptor, OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChannelDescriptor));
  VolumeDataPageAccessor_.def("getNumSamples"               , [](VolumeDataPageAccessor* self, py::array_t<int>& numSamples) { return self->GetNumSamples(PyArrayAdapter<int, 6, true>::getArrayChecked(numSamples)); }, py::arg("numSamples").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetNumSamples));
  VolumeDataPageAccessor_.def("getChunkCount"               , static_cast<int64_t(VolumeDataPageAccessor::*)() const>(&VolumeDataPageAccessor::GetChunkCount), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkCount));
  VolumeDataPageAccessor_.def_property_readonly("chunkCount", &VolumeDataPageAccessor::GetChunkCount, OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkCount));
  VolumeDataPageAccessor_.def("getChunkMinMax"              , [](VolumeDataPageAccessor* self, int64_t chunk, py::array_t<int>& min, py::array_t<int>& max) { return self->GetChunkMinMax(chunk, PyArrayAdapter<int, 6, true>::getArrayChecked(min), PyArrayAdapter<int, 6, true>::getArrayChecked(max)); }, py::arg("chunk").none(false), py::arg("min").none(false), py::arg("max").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkMinMax));
  VolumeDataPageAccessor_.def("getChunkMinMaxExcludingMargin", [](VolumeDataPageAccessor* self, int64_t chunk, py::array_t<int>& minExcludingMargin, py::array_t<int>& maxExcludingMargin) { return self->GetChunkMinMaxExcludingMargin(chunk, PyArrayAdapter<int, 6, true>::getArrayChecked(minExcludingMargin), PyArrayAdapter<int, 6, true>::getArrayChecked(maxExcludingMargin)); }, py::arg("chunk").none(false), py::arg("minExcludingMargin").none(false), py::arg("maxExcludingMargin").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkMinMaxExcludingMargin));
  VolumeDataPageAccessor_.def("getChunkIndex"               , [](VolumeDataPageAccessor* self, const py::array_t<int>& position) { return self->GetChunkIndex(PyArrayAdapter<int, 6, false>::getArrayChecked(position)); }, py::arg("position").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetChunkIndex));
  VolumeDataPageAccessor_.def("addReference"                , static_cast<int(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::AddReference), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_AddReference));
  VolumeDataPageAccessor_.def("removeReference"             , static_cast<int(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::RemoveReference), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_RemoveReference));
  VolumeDataPageAccessor_.def("getMaxPages"                 , static_cast<int(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::GetMaxPages), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetMaxPages));
  VolumeDataPageAccessor_.def_property_readonly("maxPages", &VolumeDataPageAccessor::GetMaxPages, OPENVDS_DOCSTRING(VolumeDataPageAccessor_GetMaxPages));
  VolumeDataPageAccessor_.def("setMaxPages"                 , static_cast<void(VolumeDataPageAccessor::*)(int)>(&VolumeDataPageAccessor::SetMaxPages), py::arg("maxPages").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_SetMaxPages));
  VolumeDataPageAccessor_.def("createPage"                  , static_cast<native::VolumeDataPage *(VolumeDataPageAccessor::*)(int64_t)>(&VolumeDataPageAccessor::CreatePage), py::arg("chunkIndex").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_CreatePage));
  VolumeDataPageAccessor_.def("readPage"                    , static_cast<native::VolumeDataPage *(VolumeDataPageAccessor::*)(int64_t)>(&VolumeDataPageAccessor::ReadPage), py::arg("chunkIndex").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_ReadPage));
  VolumeDataPageAccessor_.def("readPageAtPosition"          , [](VolumeDataPageAccessor* self, const py::array_t<int>& position) { return self->ReadPageAtPosition(PyArrayAdapter<int, 6, false>::getArrayChecked(position)); }, py::arg("position").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_ReadPageAtPosition));
  VolumeDataPageAccessor_.def("commit"                      , static_cast<void(VolumeDataPageAccessor::*)()>(&VolumeDataPageAccessor::Commit), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPageAccessor_Commit));

//AUTOGEN-END
#if 0
    /** \rst
        Creates ``memoryview`` from static buffer.

        This method is meant for providing a ``memoryview`` for C/C++ buffer not
        managed by Python. The caller is responsible for managing the lifetime
        of ``ptr`` and ``format``, which MUST outlive the memoryview constructed
        here.

        See also: Python C API documentation for `PyMemoryView_FromBuffer`_.

        .. _PyMemoryView_FromBuffer: https://docs.python.org/c-api/memoryview.html#c.PyMemoryView_FromBuffer

        :param ptr: Pointer to the buffer.
        :param itemsize: Byte size of an element.
        :param format: Pointer to the null-terminated format string. For
            homogeneous Buffers, this should be set to
            ``format_descriptor<T>::value``.
        :param shape: Shape of the tensor (1 entry per dimension).
        :param strides: Number of bytes between adjacent entries (for each
            per dimension).
        :param readonly: Flag to indicate if the underlying storage may be
            written to.
     \endrst */
    static memoryview from_buffer(
        void *ptr, ssize_t itemsize, const char *format,
        detail::any_container<ssize_t> shape,
        detail::any_container<ssize_t> strides, bool readonly = false);
#endif|
// IMPLEMENTED :     VolumeDataPage_.def("getBuffer"                   , [](VolumeDataPage* self, py::array_t<int>& pitch) { return self->GetBuffer(PyArrayAdapter<int, 6, true>::getArrayChecked(pitch)); }, py::arg("pitch").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPage_GetBuffer));
     VolumeDataPage_.def("getBuffer"                   , [](VolumeDataPage* self)
       {
         int voxelMin[VolumeDataLayout::Dimensionality_Max];
         int voxelMax[VolumeDataLayout::Dimensionality_Max];
         self->GetMinMax(voxelMin, voxelMax);

         int pitch[VolumeDataLayout::Dimensionality_Max];
         const void *buffer = self->GetBuffer(pitch);

         auto channelDescriptor = self->GetVolumeDataPageAccessor().GetChannelDescriptor();
         int itemsize = GetItemSize(channelDescriptor.GetFormat(), channelDescriptor.GetComponents());
         const char *format = GetPythonFormatString(channelDescriptor.GetFormat(), channelDescriptor.GetComponents());

         int dimensionality = self->GetVolumeDataPageAccessor().GetLayout()->GetDimensionality();
         std::vector<int> shape(dimensionality);
         std::vector<int> strides(dimensionality);
         for(int dimension = 0; dimension < dimensionality; dimension++)
         {
           shape[dimensionality - 1 - dimension] = voxelMax[dimension] - voxelMin[dimension];
           strides[dimensionality - 1 - dimension] = pitch[dimension] * itemsize;
         }

         return py::memoryview::from_buffer(buffer, itemsize, format, shape, strides);
       }, OPENVDS_DOCSTRING(VolumeDataPage_GetBuffer));

// IMPLEMENTED :     VolumeDataPage_.def("getWritableBuffer"           , [](VolumeDataPage* self, py::array_t<int>& pitch) { return self->GetWritableBuffer(PyArrayAdapter<int, 6, true>::getArrayChecked(pitch)); }, py::arg("pitch").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataPage_GetWritableBuffer));
     VolumeDataPage_.def("getWritableBuffer"           , [](VolumeDataPage* self)
       {
         int voxelMin[VolumeDataLayout::Dimensionality_Max];
         int voxelMax[VolumeDataLayout::Dimensionality_Max];
         self->GetMinMax(voxelMin, voxelMax);

         int pitch[VolumeDataLayout::Dimensionality_Max];
         void *buffer = self->GetWritableBuffer(pitch);

         auto channelDescriptor = self->GetVolumeDataPageAccessor().GetChannelDescriptor();
         int itemsize = GetItemSize(channelDescriptor.GetFormat(), channelDescriptor.GetComponents());
         const char *format = GetPythonFormatString(channelDescriptor.GetFormat(), channelDescriptor.GetComponents());

         int dimensionality = self->GetVolumeDataPageAccessor().GetLayout()->GetDimensionality();
         std::vector<int> shape(dimensionality);
         std::vector<int> strides(dimensionality);
         for(int dimension = 0; dimension < dimensionality; dimension++)
         {
           shape[dimensionality - 1 - dimension] = voxelMax[dimension] - voxelMin[dimension];
           strides[dimensionality - 1 - dimension] = pitch[dimension] * itemsize;
         }

         return py::memoryview::from_buffer(buffer, itemsize, format, shape, strides);
       }, OPENVDS_DOCSTRING(VolumeDataPage_GetWritableBuffer));

  // Register accessor types
  RegisterVolumeDataReadAccessor<native::IntVector2, bool>          (m, "VolumeData2DReadAccessor1Bit");
  RegisterVolumeDataReadAccessor<native::IntVector2, uint8_t>       (m, "VolumeData2DReadAccessorU8");  
  RegisterVolumeDataReadAccessor<native::IntVector2, uint16_t>      (m, "VolumeData2DReadAccessorU16"); 
  RegisterVolumeDataReadAccessor<native::IntVector2, uint32_t>      (m, "VolumeData2DReadAccessorU32"); 
  RegisterVolumeDataReadAccessor<native::IntVector2, uint64_t>      (m, "VolumeData2DReadAccessorU64"); 
  RegisterVolumeDataReadAccessor<native::IntVector2, float>         (m, "VolumeData2DReadAccessorR32"); 
  RegisterVolumeDataReadAccessor<native::IntVector2, double>        (m, "VolumeData2DReadAccessorR64"); 
  RegisterVolumeDataReadAccessor<native::IntVector3, bool>          (m, "VolumeData3DReadAccessor1Bit");
  RegisterVolumeDataReadAccessor<native::IntVector3, uint8_t>       (m, "VolumeData3DReadAccessorU8");  
  RegisterVolumeDataReadAccessor<native::IntVector3, uint16_t>      (m, "VolumeData3DReadAccessorU16"); 
  RegisterVolumeDataReadAccessor<native::IntVector3, uint32_t>      (m, "VolumeData3DReadAccessorU32"); 
  RegisterVolumeDataReadAccessor<native::IntVector3, uint64_t>      (m, "VolumeData3DReadAccessorU64"); 
  RegisterVolumeDataReadAccessor<native::IntVector3, float>         (m, "VolumeData3DReadAccessorR32"); 
  RegisterVolumeDataReadAccessor<native::IntVector3, double>        (m, "VolumeData3DReadAccessorR64"); 
  RegisterVolumeDataReadAccessor<native::IntVector4, bool>          (m, "VolumeData4DReadAccessor1Bit");
  RegisterVolumeDataReadAccessor<native::IntVector4, uint8_t>       (m, "VolumeData4DReadAccessorU8");  
  RegisterVolumeDataReadAccessor<native::IntVector4, uint16_t>      (m, "VolumeData4DReadAccessorU16"); 
  RegisterVolumeDataReadAccessor<native::IntVector4, uint32_t>      (m, "VolumeData4DReadAccessorU32"); 
  RegisterVolumeDataReadAccessor<native::IntVector4, uint64_t>      (m, "VolumeData4DReadAccessorU64");
  RegisterVolumeDataReadAccessor<native::IntVector4, float>         (m, "VolumeData4DReadAccessorR32");
  RegisterVolumeDataReadAccessor<native::IntVector4, double>        (m, "VolumeData4DReadAccessorR64");
  RegisterVolumeDataReadAccessor<native::FloatVector2, float>       (m, "VolumeData2DInterpolatingAccessorR32");
  RegisterVolumeDataReadAccessor<native::FloatVector2, double>      (m, "VolumeData2DInterpolatingAccessorR64");
  RegisterVolumeDataReadAccessor<native::FloatVector3, float>       (m, "VolumeData3DInterpolatingAccessorR32");
  RegisterVolumeDataReadAccessor<native::FloatVector3, double>      (m, "VolumeData3DInterpolatingAccessorR64");
  RegisterVolumeDataReadAccessor<native::FloatVector4, float>       (m, "VolumeData4DInterpolatingAccessorR32");
  RegisterVolumeDataReadAccessor<native::FloatVector4, double>      (m, "VolumeData4DInterpolatingAccessorR64");

  RegisterVolumeDataReadWriteAccessor<native::IntVector2, bool>          (m, "VolumeData2DReadWriteAccessor1Bit");
  RegisterVolumeDataReadWriteAccessor<native::IntVector2, uint8_t>       (m, "VolumeData2DReadWriteAccessorU8");  
  RegisterVolumeDataReadWriteAccessor<native::IntVector2, uint16_t>      (m, "VolumeData2DReadWriteAccessorU16"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector2, uint32_t>      (m, "VolumeData2DReadWriteAccessorU32"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector2, uint64_t>      (m, "VolumeData2DReadWriteAccessorU64"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector2, float>         (m, "VolumeData2DReadWriteAccessorR32"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector2, double>        (m, "VolumeData2DReadWriteAccessorR64"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, bool>          (m, "VolumeData3DReadWriteAccessor1Bit");
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, uint8_t>       (m, "VolumeData3DReadWriteAccessorU8");  
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, uint16_t>      (m, "VolumeData3DReadWriteAccessorU16"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, uint32_t>      (m, "VolumeData3DReadWriteAccessorU32"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, uint64_t>      (m, "VolumeData3DReadWriteAccessorU64"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, float>         (m, "VolumeData3DReadWriteAccessorR32"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector3, double>        (m, "VolumeData3DReadWriteAccessorR64"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, bool>          (m, "VolumeData4DReadWriteAccessor1Bit");
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, uint8_t>       (m, "VolumeData4DReadWriteAccessorU8");  
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, uint16_t>      (m, "VolumeData4DReadWriteAccessorU16"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, uint32_t>      (m, "VolumeData4DReadWriteAccessorU32"); 
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, uint64_t>      (m, "VolumeData4DReadWriteAccessorU64");
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, float>         (m, "VolumeData4DReadWriteAccessorR32");
  RegisterVolumeDataReadWriteAccessor<native::IntVector4, double>        (m, "VolumeData4DReadWriteAccessorR64");
}

