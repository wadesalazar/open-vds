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

#include "PyMetadataContainer.h"

using namespace native;

void 
PyMetadataContainer::initModule(py::module& m)
{
//AUTOGEN-BEGIN
  // MetadataContainer
  py::class_<MetadataContainer, MetadataReadAccess, MetadataWriteAccess, std::unique_ptr<MetadataContainer>> 
    MetadataContainer_(m,"MetadataContainer", OPENVDS_DOCSTRING(MetadataContainer));

  MetadataContainer_.def("isMetadataIntAvailable"      , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataIntAvailable), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntAvailable));
  MetadataContainer_.def("isMetadataIntVector2Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataIntVector2Available), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntVector2Available));
  MetadataContainer_.def("isMetadataIntVector3Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataIntVector3Available), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntVector3Available));
  MetadataContainer_.def("isMetadataIntVector4Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataIntVector4Available), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntVector4Available));
  MetadataContainer_.def("isMetadataFloatAvailable"    , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataFloatAvailable), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatAvailable));
  MetadataContainer_.def("isMetadataFloatVector2Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataFloatVector2Available), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatVector2Available));
  MetadataContainer_.def("isMetadataFloatVector3Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataFloatVector3Available), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatVector3Available));
  MetadataContainer_.def("isMetadataFloatVector4Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataFloatVector4Available), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatVector4Available));
  MetadataContainer_.def("isMetadataDoubleAvailable"   , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataDoubleAvailable), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleAvailable));
  MetadataContainer_.def("isMetadataDoubleVector2Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataDoubleVector2Available), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleVector2Available));
  MetadataContainer_.def("isMetadataDoubleVector3Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataDoubleVector3Available), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleVector3Available));
  MetadataContainer_.def("isMetadataDoubleVector4Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataDoubleVector4Available), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleVector4Available));
  MetadataContainer_.def("isMetadataStringAvailable"   , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataStringAvailable), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataStringAvailable));
  MetadataContainer_.def("isMetadataBLOBAvailable"     , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataBLOBAvailable), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataBLOBAvailable));
  MetadataContainer_.def("getMetadataInt"              , static_cast<int(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataInt), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataInt));
  MetadataContainer_.def("getMetadataIntVector2"       , [](MetadataContainer* self, const char * category, const char * name) { return (IntVector2Adapter::AdaptedType)(self->GetMetadataIntVector2(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataIntVector2));
  MetadataContainer_.def("getMetadataIntVector3"       , [](MetadataContainer* self, const char * category, const char * name) { return (IntVector3Adapter::AdaptedType)(self->GetMetadataIntVector3(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataIntVector3));
  MetadataContainer_.def("getMetadataIntVector4"       , [](MetadataContainer* self, const char * category, const char * name) { return (IntVector4Adapter::AdaptedType)(self->GetMetadataIntVector4(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataIntVector4));
  MetadataContainer_.def("getMetadataFloat"            , static_cast<float(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataFloat), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloat));
  MetadataContainer_.def("getMetadataFloatVector2"     , [](MetadataContainer* self, const char * category, const char * name) { return (FloatVector2Adapter::AdaptedType)(self->GetMetadataFloatVector2(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloatVector2));
  MetadataContainer_.def("getMetadataFloatVector3"     , [](MetadataContainer* self, const char * category, const char * name) { return (FloatVector3Adapter::AdaptedType)(self->GetMetadataFloatVector3(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloatVector3));
  MetadataContainer_.def("getMetadataFloatVector4"     , [](MetadataContainer* self, const char * category, const char * name) { return (FloatVector4Adapter::AdaptedType)(self->GetMetadataFloatVector4(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloatVector4));
  MetadataContainer_.def("getMetadataDouble"           , static_cast<double(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataDouble), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDouble));
  MetadataContainer_.def("getMetadataDoubleVector2"    , [](MetadataContainer* self, const char * category, const char * name) { return (DoubleVector2Adapter::AdaptedType)(self->GetMetadataDoubleVector2(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDoubleVector2));
  MetadataContainer_.def("getMetadataDoubleVector3"    , [](MetadataContainer* self, const char * category, const char * name) { return (DoubleVector3Adapter::AdaptedType)(self->GetMetadataDoubleVector3(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDoubleVector3));
  MetadataContainer_.def("getMetadataDoubleVector4"    , [](MetadataContainer* self, const char * category, const char * name) { return (DoubleVector4Adapter::AdaptedType)(self->GetMetadataDoubleVector4(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDoubleVector4));
  MetadataContainer_.def("getMetadataString"           , static_cast<const char *(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataString), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataString));
  MetadataContainer_.def("setMetadataInt"              , static_cast<void(MetadataContainer::*)(const char *, const char *, int)>(&MetadataContainer::SetMetadataInt), py::arg("category").none(false), py::arg("name").none(false), py::arg("value").none(false), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataInt));
  MetadataContainer_.def("setMetadataIntVector2"       , [](MetadataContainer* self, const char * category, const char * name, IntVector2Adapter::AdaptedType value) { return self->SetMetadataIntVector2(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataIntVector2));
  MetadataContainer_.def("setMetadataIntVector3"       , [](MetadataContainer* self, const char * category, const char * name, IntVector3Adapter::AdaptedType value) { return self->SetMetadataIntVector3(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataIntVector3));
  MetadataContainer_.def("setMetadataIntVector4"       , [](MetadataContainer* self, const char * category, const char * name, IntVector4Adapter::AdaptedType value) { return self->SetMetadataIntVector4(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataIntVector4));
  MetadataContainer_.def("setMetadataFloat"            , static_cast<void(MetadataContainer::*)(const char *, const char *, float)>(&MetadataContainer::SetMetadataFloat), py::arg("category").none(false), py::arg("name").none(false), py::arg("value").none(false), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataFloat));
  MetadataContainer_.def("setMetadataFloatVector2"     , [](MetadataContainer* self, const char * category, const char * name, FloatVector2Adapter::AdaptedType value) { return self->SetMetadataFloatVector2(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataFloatVector2));
  MetadataContainer_.def("setMetadataFloatVector3"     , [](MetadataContainer* self, const char * category, const char * name, FloatVector3Adapter::AdaptedType value) { return self->SetMetadataFloatVector3(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataFloatVector3));
  MetadataContainer_.def("setMetadataFloatVector4"     , [](MetadataContainer* self, const char * category, const char * name, FloatVector4Adapter::AdaptedType value) { return self->SetMetadataFloatVector4(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataFloatVector4));
  MetadataContainer_.def("setMetadataDouble"           , static_cast<void(MetadataContainer::*)(const char *, const char *, double)>(&MetadataContainer::SetMetadataDouble), py::arg("category").none(false), py::arg("name").none(false), py::arg("value").none(false), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataDouble));
  MetadataContainer_.def("setMetadataDoubleVector2"    , [](MetadataContainer* self, const char * category, const char * name, DoubleVector2Adapter::AdaptedType value) { return self->SetMetadataDoubleVector2(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataDoubleVector2));
  MetadataContainer_.def("setMetadataDoubleVector3"    , [](MetadataContainer* self, const char * category, const char * name, DoubleVector3Adapter::AdaptedType value) { return self->SetMetadataDoubleVector3(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataDoubleVector3));
  MetadataContainer_.def("setMetadataDoubleVector4"    , [](MetadataContainer* self, const char * category, const char * name, DoubleVector4Adapter::AdaptedType value) { return self->SetMetadataDoubleVector4(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataDoubleVector4));
  MetadataContainer_.def("setMetadataString"           , static_cast<void(MetadataContainer::*)(const char *, const char *, const char *)>(&MetadataContainer::SetMetadataString), py::arg("category").none(false), py::arg("name").none(false), py::arg("value").none(false), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataString));
// AUTOGENERATE FAIL :   MetadataContainer_.def("setMetadataBLOB"             , static_cast<void(MetadataContainer::*)(const char *, const char *, const void *, size_t)>(&MetadataContainer::SetMetadataBLOB), py::arg("category").none(false), py::arg("name").none(false), py::arg("data").none(false), py::arg("size").none(false), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataBLOB));
  MetadataContainer_.def("copyMetadata"                , static_cast<void(MetadataContainer::*)(const char *, const native::MetadataReadAccess *)>(&MetadataContainer::CopyMetadata), py::arg("category").none(false), py::arg("metadataReadAccess").none(false), OPENVDS_DOCSTRING(MetadataContainer_CopyMetadata));
  MetadataContainer_.def("clearMetadata"               , static_cast<void(MetadataContainer::*)(const char *, const char *)>(&MetadataContainer::ClearMetadata), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_ClearMetadata));
  MetadataContainer_.def("clearMetadata"               , static_cast<void(MetadataContainer::*)(const char *)>(&MetadataContainer::ClearMetadata), py::arg("category").none(false), OPENVDS_DOCSTRING(MetadataContainer_ClearMetadata_2));
// AUTOGENERATE FAIL :   MetadataContainer_.def("getMetadataBLOB"             , static_cast<void(MetadataContainer::*)(const char *, const char *, const void **, size_t *) const>(&MetadataContainer::GetMetadataBLOB), py::arg("category").none(false), py::arg("name").none(false), py::arg("data").none(false), py::arg("size").none(false), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataBLOB));
  MetadataContainer_.def("getMetadataKeys"             , static_cast<native::MetadataKeyRange(MetadataContainer::*)() const>(&MetadataContainer::GetMetadataKeys), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataKeys));
  MetadataContainer_.def_property_readonly("metadataKeys", &MetadataContainer::GetMetadataKeys, OPENVDS_DOCSTRING(MetadataContainer_GetMetadataKeys));

//AUTOGEN-END
  // IMPLEMENTED :   MetadataContainer_.def("setMetadataBLOB"             , static_cast<void(MetadataContainer::*)(const char *, const char *, const void *, size_t)>(&MetadataContainer::SetMetadataBLOB), py::arg("category").none(false), py::arg("name").none(false), py::arg("data").none(false), py::arg("size").none(false), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataBLOB));
  MetadataContainer_.def("setMetadataBLOB", [](MetadataContainer* self, const char* category, const char* name, py::buffer data)
    {
      py::buffer_info buffer = data.request();
      size_t size = buffer.size * buffer.itemsize;
      self->SetMetadataBLOB(category, name, buffer.ptr, size);
    },
    py::arg("category").none(false), py::arg("name").none(false), py::arg("value").none(false), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataBLOB));

  // IMPLEMENTED :   MetadataContainer_.def("getMetadataBLOB"             , static_cast<void(MetadataContainer::*)(const char *, const char *, const void **, size_t *) const>(&MetadataContainer::GetMetadataBLOB), py::arg("category").none(false), py::arg("name").none(false), py::arg("data").none(false), py::arg("size").none(false), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataBLOB));
  MetadataContainer_.def("getMetadataBLOB", [](MetadataContainer* self, const char* category, const char* name) 
    {
      BLOB blob;
      self->GetMetadataBLOB(category, name, (void const**)&blob.m_Data, &blob.m_Size);
      return blob;
    },
    py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataBLOB));

  // Add a default constructor since the constructor of this class is implicit
  MetadataContainer_.def(py::init<>(), R"doc(Default constructor)doc");
}

