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

#include "PyMetadata.h"

using namespace native;

void 
PyMetadata::initModule(py::module& m)
{
//AUTOGEN-BEGIN
  py::enum_<MetadataType> 
    MetadataType_(m,"MetadataType", OPENVDS_DOCSTRING(MetadataType));

  MetadataType_.value("Int"                         , MetadataType::Int                       , OPENVDS_DOCSTRING(MetadataType_Int));
  MetadataType_.value("IntVector2"                  , MetadataType::IntVector2                , OPENVDS_DOCSTRING(MetadataType_IntVector2));
  MetadataType_.value("IntVector3"                  , MetadataType::IntVector3                , OPENVDS_DOCSTRING(MetadataType_IntVector3));
  MetadataType_.value("IntVector4"                  , MetadataType::IntVector4                , OPENVDS_DOCSTRING(MetadataType_IntVector4));
  MetadataType_.value("Float"                       , MetadataType::Float                     , OPENVDS_DOCSTRING(MetadataType_Float));
  MetadataType_.value("FloatVector2"                , MetadataType::FloatVector2              , OPENVDS_DOCSTRING(MetadataType_FloatVector2));
  MetadataType_.value("FloatVector3"                , MetadataType::FloatVector3              , OPENVDS_DOCSTRING(MetadataType_FloatVector3));
  MetadataType_.value("FloatVector4"                , MetadataType::FloatVector4              , OPENVDS_DOCSTRING(MetadataType_FloatVector4));
  MetadataType_.value("Double"                      , MetadataType::Double                    , OPENVDS_DOCSTRING(MetadataType_Double));
  MetadataType_.value("DoubleVector2"               , MetadataType::DoubleVector2             , OPENVDS_DOCSTRING(MetadataType_DoubleVector2));
  MetadataType_.value("DoubleVector3"               , MetadataType::DoubleVector3             , OPENVDS_DOCSTRING(MetadataType_DoubleVector3));
  MetadataType_.value("DoubleVector4"               , MetadataType::DoubleVector4             , OPENVDS_DOCSTRING(MetadataType_DoubleVector4));
  MetadataType_.value("String"                      , MetadataType::String                    , OPENVDS_DOCSTRING(MetadataType_String));
  MetadataType_.value("BLOB"                        , MetadataType::BLOB                      , OPENVDS_DOCSTRING(MetadataType_BLOB));

  // MetadataKey
  py::class_<MetadataKey> 
    MetadataKey_(m,"MetadataKey", OPENVDS_DOCSTRING(MetadataKey));

  MetadataKey_.def_readwrite("type"                        , &MetadataKey::type             , OPENVDS_DOCSTRING(MetadataKey_type));
  MetadataKey_.def_readwrite("category"                    , &MetadataKey::category         , OPENVDS_DOCSTRING(MetadataKey_category));
  MetadataKey_.def_readwrite("name"                        , &MetadataKey::name             , OPENVDS_DOCSTRING(MetadataKey_name));

  // MetadataReadAccess
  py::class_<MetadataReadAccess, std::unique_ptr<MetadataReadAccess, py::nodelete>> 
    MetadataReadAccess_(m,"MetadataReadAccess", OPENVDS_DOCSTRING(MetadataReadAccess));

  MetadataReadAccess_.def("isMetadataIntAvailable"      , static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataIntAvailable), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataIntAvailable));
  MetadataReadAccess_.def("isMetadataIntVector2Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataIntVector2Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataIntVector2Available));
  MetadataReadAccess_.def("isMetadataIntVector3Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataIntVector3Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataIntVector3Available));
  MetadataReadAccess_.def("isMetadataIntVector4Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataIntVector4Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataIntVector4Available));
  MetadataReadAccess_.def("isMetadataFloatAvailable"    , static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataFloatAvailable), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataFloatAvailable));
  MetadataReadAccess_.def("isMetadataFloatVector2Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataFloatVector2Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataFloatVector2Available));
  MetadataReadAccess_.def("isMetadataFloatVector3Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataFloatVector3Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataFloatVector3Available));
  MetadataReadAccess_.def("isMetadataFloatVector4Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataFloatVector4Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataFloatVector4Available));
  MetadataReadAccess_.def("isMetadataDoubleAvailable"   , static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataDoubleAvailable), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataDoubleAvailable));
  MetadataReadAccess_.def("isMetadataDoubleVector2Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataDoubleVector2Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataDoubleVector2Available));
  MetadataReadAccess_.def("isMetadataDoubleVector3Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataDoubleVector3Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataDoubleVector3Available));
  MetadataReadAccess_.def("isMetadataDoubleVector4Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataDoubleVector4Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataDoubleVector4Available));
  MetadataReadAccess_.def("isMetadataStringAvailable"   , static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataStringAvailable), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataStringAvailable));
  MetadataReadAccess_.def("isMetadataBLOBAvailable"     , static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataBLOBAvailable), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataBLOBAvailable));
  MetadataReadAccess_.def("getMetadataInt"              , static_cast<int(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataInt), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataInt));
  MetadataReadAccess_.def("getMetadataIntVector2"       , [](MetadataReadAccess* self, const char * category, const char * name) { return (IntVector2Adapter::AdaptedType)(self->GetMetadataIntVector2(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataIntVector2));
  MetadataReadAccess_.def("getMetadataIntVector3"       , [](MetadataReadAccess* self, const char * category, const char * name) { return (IntVector3Adapter::AdaptedType)(self->GetMetadataIntVector3(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataIntVector3));
  MetadataReadAccess_.def("getMetadataIntVector4"       , [](MetadataReadAccess* self, const char * category, const char * name) { return (IntVector4Adapter::AdaptedType)(self->GetMetadataIntVector4(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataIntVector4));
  MetadataReadAccess_.def("getMetadataFloat"            , static_cast<float(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataFloat), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataFloat));
  MetadataReadAccess_.def("getMetadataFloatVector2"     , [](MetadataReadAccess* self, const char * category, const char * name) { return (FloatVector2Adapter::AdaptedType)(self->GetMetadataFloatVector2(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataFloatVector2));
  MetadataReadAccess_.def("getMetadataFloatVector3"     , [](MetadataReadAccess* self, const char * category, const char * name) { return (FloatVector3Adapter::AdaptedType)(self->GetMetadataFloatVector3(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataFloatVector3));
  MetadataReadAccess_.def("getMetadataFloatVector4"     , [](MetadataReadAccess* self, const char * category, const char * name) { return (FloatVector4Adapter::AdaptedType)(self->GetMetadataFloatVector4(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataFloatVector4));
  MetadataReadAccess_.def("getMetadataDouble"           , static_cast<double(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataDouble), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataDouble));
  MetadataReadAccess_.def("getMetadataDoubleVector2"    , [](MetadataReadAccess* self, const char * category, const char * name) { return (DoubleVector2Adapter::AdaptedType)(self->GetMetadataDoubleVector2(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataDoubleVector2));
  MetadataReadAccess_.def("getMetadataDoubleVector3"    , [](MetadataReadAccess* self, const char * category, const char * name) { return (DoubleVector3Adapter::AdaptedType)(self->GetMetadataDoubleVector3(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataDoubleVector3));
  MetadataReadAccess_.def("getMetadataDoubleVector4"    , [](MetadataReadAccess* self, const char * category, const char * name) { return (DoubleVector4Adapter::AdaptedType)(self->GetMetadataDoubleVector4(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataDoubleVector4));
  MetadataReadAccess_.def("getMetadataString"           , static_cast<const char *(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataString), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataString));
  MetadataReadAccess_.def("begin"                       , static_cast<native::MetadataReadAccess::const_iterator(MetadataReadAccess::*)() const>(&MetadataReadAccess::begin), OPENVDS_DOCSTRING(MetadataReadAccess_begin));
  MetadataReadAccess_.def("end"                         , static_cast<native::MetadataReadAccess::const_iterator(MetadataReadAccess::*)() const>(&MetadataReadAccess::end), OPENVDS_DOCSTRING(MetadataReadAccess_end));

  // MetadataWriteAccess
  py::class_<MetadataWriteAccess, std::unique_ptr<MetadataWriteAccess, py::nodelete>> 
    MetadataWriteAccess_(m,"MetadataWriteAccess", OPENVDS_DOCSTRING(MetadataWriteAccess));

  MetadataWriteAccess_.def("setMetadataInt"              , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, int)>(&MetadataWriteAccess::SetMetadataInt), py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataInt));
  MetadataWriteAccess_.def("setMetadataIntVector2"       , [](MetadataWriteAccess* self, const char * category, const char * name, IntVector2Adapter::AdaptedType value) { return self->SetMetadataIntVector2(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataIntVector2));
  MetadataWriteAccess_.def("setMetadataIntVector3"       , [](MetadataWriteAccess* self, const char * category, const char * name, IntVector3Adapter::AdaptedType value) { return self->SetMetadataIntVector3(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataIntVector3));
  MetadataWriteAccess_.def("setMetadataIntVector4"       , [](MetadataWriteAccess* self, const char * category, const char * name, IntVector4Adapter::AdaptedType value) { return self->SetMetadataIntVector4(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataIntVector4));
  MetadataWriteAccess_.def("setMetadataFloat"            , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, float)>(&MetadataWriteAccess::SetMetadataFloat), py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataFloat));
  MetadataWriteAccess_.def("setMetadataFloatVector2"     , [](MetadataWriteAccess* self, const char * category, const char * name, FloatVector2Adapter::AdaptedType value) { return self->SetMetadataFloatVector2(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataFloatVector2));
  MetadataWriteAccess_.def("setMetadataFloatVector3"     , [](MetadataWriteAccess* self, const char * category, const char * name, FloatVector3Adapter::AdaptedType value) { return self->SetMetadataFloatVector3(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataFloatVector3));
  MetadataWriteAccess_.def("setMetadataFloatVector4"     , [](MetadataWriteAccess* self, const char * category, const char * name, FloatVector4Adapter::AdaptedType value) { return self->SetMetadataFloatVector4(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataFloatVector4));
  MetadataWriteAccess_.def("setMetadataDouble"           , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, double)>(&MetadataWriteAccess::SetMetadataDouble), py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataDouble));
  MetadataWriteAccess_.def("setMetadataDoubleVector2"    , [](MetadataWriteAccess* self, const char * category, const char * name, DoubleVector2Adapter::AdaptedType value) { return self->SetMetadataDoubleVector2(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataDoubleVector2));
  MetadataWriteAccess_.def("setMetadataDoubleVector3"    , [](MetadataWriteAccess* self, const char * category, const char * name, DoubleVector3Adapter::AdaptedType value) { return self->SetMetadataDoubleVector3(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataDoubleVector3));
  MetadataWriteAccess_.def("setMetadataDoubleVector4"    , [](MetadataWriteAccess* self, const char * category, const char * name, DoubleVector4Adapter::AdaptedType value) { return self->SetMetadataDoubleVector4(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataDoubleVector4));
  MetadataWriteAccess_.def("setMetadataString"           , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, const std::string &)>(&MetadataWriteAccess::SetMetadataString), py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataString));
  MetadataWriteAccess_.def("copyMetadata"                , static_cast<void(MetadataWriteAccess::*)(const char *, const native::MetadataReadAccess *)>(&MetadataWriteAccess::CopyMetadata), py::arg("category"), py::arg("metadataReadAccess"), OPENVDS_DOCSTRING(MetadataWriteAccess_CopyMetadata));
  MetadataWriteAccess_.def("clearMetadata"               , static_cast<void(MetadataWriteAccess::*)(const char *, const char *)>(&MetadataWriteAccess::ClearMetadata), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataWriteAccess_ClearMetadata));
  MetadataWriteAccess_.def("clearMetadata"               , static_cast<void(MetadataWriteAccess::*)(const char *)>(&MetadataWriteAccess::ClearMetadata), py::arg("category"), OPENVDS_DOCSTRING(MetadataWriteAccess_ClearMetadata_2));

  m.def("operator_eq"                 , static_cast<bool(*)(const native::MetadataKey &, const native::MetadataKey &)>(&operator==), py::arg("a"), py::arg("b"), OPENVDS_DOCSTRING(operator_eq));

  // MetadataContainer
  py::class_<MetadataContainer, MetadataReadAccess, MetadataWriteAccess> 
    MetadataContainer_(m,"MetadataContainer", OPENVDS_DOCSTRING(MetadataContainer));

  MetadataContainer_.def("isMetadataIntAvailable"      , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataIntAvailable), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntAvailable));
  MetadataContainer_.def("isMetadataIntVector2Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataIntVector2Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntVector2Available));
  MetadataContainer_.def("isMetadataIntVector3Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataIntVector3Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntVector3Available));
  MetadataContainer_.def("isMetadataIntVector4Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataIntVector4Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntVector4Available));
  MetadataContainer_.def("isMetadataFloatAvailable"    , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataFloatAvailable), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatAvailable));
  MetadataContainer_.def("isMetadataFloatVector2Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataFloatVector2Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatVector2Available));
  MetadataContainer_.def("isMetadataFloatVector3Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataFloatVector3Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatVector3Available));
  MetadataContainer_.def("isMetadataFloatVector4Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataFloatVector4Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatVector4Available));
  MetadataContainer_.def("isMetadataDoubleAvailable"   , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataDoubleAvailable), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleAvailable));
  MetadataContainer_.def("isMetadataDoubleVector2Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataDoubleVector2Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleVector2Available));
  MetadataContainer_.def("isMetadataDoubleVector3Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataDoubleVector3Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleVector3Available));
  MetadataContainer_.def("isMetadataDoubleVector4Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataDoubleVector4Available), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleVector4Available));
  MetadataContainer_.def("isMetadataStringAvailable"   , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataStringAvailable), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataStringAvailable));
  MetadataContainer_.def("isMetadataBLOBAvailable"     , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataBLOBAvailable), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataBLOBAvailable));
  MetadataContainer_.def("getMetadataInt"              , static_cast<int(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataInt), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataInt));
  MetadataContainer_.def("getMetadataIntVector2"       , [](MetadataContainer* self, const char * category, const char * name) { return (IntVector2Adapter::AdaptedType)(self->GetMetadataIntVector2(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataIntVector2));
  MetadataContainer_.def("getMetadataIntVector3"       , [](MetadataContainer* self, const char * category, const char * name) { return (IntVector3Adapter::AdaptedType)(self->GetMetadataIntVector3(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataIntVector3));
  MetadataContainer_.def("getMetadataIntVector4"       , [](MetadataContainer* self, const char * category, const char * name) { return (IntVector4Adapter::AdaptedType)(self->GetMetadataIntVector4(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataIntVector4));
  MetadataContainer_.def("getMetadataFloat"            , static_cast<float(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataFloat), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloat));
  MetadataContainer_.def("getMetadataFloatVector2"     , [](MetadataContainer* self, const char * category, const char * name) { return (FloatVector2Adapter::AdaptedType)(self->GetMetadataFloatVector2(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloatVector2));
  MetadataContainer_.def("getMetadataFloatVector3"     , [](MetadataContainer* self, const char * category, const char * name) { return (FloatVector3Adapter::AdaptedType)(self->GetMetadataFloatVector3(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloatVector3));
  MetadataContainer_.def("getMetadataFloatVector4"     , [](MetadataContainer* self, const char * category, const char * name) { return (FloatVector4Adapter::AdaptedType)(self->GetMetadataFloatVector4(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloatVector4));
  MetadataContainer_.def("getMetadataDouble"           , static_cast<double(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataDouble), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDouble));
  MetadataContainer_.def("getMetadataDoubleVector2"    , [](MetadataContainer* self, const char * category, const char * name) { return (DoubleVector2Adapter::AdaptedType)(self->GetMetadataDoubleVector2(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDoubleVector2));
  MetadataContainer_.def("getMetadataDoubleVector3"    , [](MetadataContainer* self, const char * category, const char * name) { return (DoubleVector3Adapter::AdaptedType)(self->GetMetadataDoubleVector3(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDoubleVector3));
  MetadataContainer_.def("getMetadataDoubleVector4"    , [](MetadataContainer* self, const char * category, const char * name) { return (DoubleVector4Adapter::AdaptedType)(self->GetMetadataDoubleVector4(category, name)); }, py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDoubleVector4));
  MetadataContainer_.def("getMetadataString"           , static_cast<const char *(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataString), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataString));
  MetadataContainer_.def("setMetadataInt"              , static_cast<void(MetadataContainer::*)(const char *, const char *, int)>(&MetadataContainer::SetMetadataInt), py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataInt));
  MetadataContainer_.def("setMetadataIntVector2"       , [](MetadataContainer* self, const char * category, const char * name, IntVector2Adapter::AdaptedType value) { return self->SetMetadataIntVector2(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataIntVector2));
  MetadataContainer_.def("setMetadataIntVector3"       , [](MetadataContainer* self, const char * category, const char * name, IntVector3Adapter::AdaptedType value) { return self->SetMetadataIntVector3(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataIntVector3));
  MetadataContainer_.def("setMetadataIntVector4"       , [](MetadataContainer* self, const char * category, const char * name, IntVector4Adapter::AdaptedType value) { return self->SetMetadataIntVector4(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataIntVector4));
  MetadataContainer_.def("setMetadataFloat"            , static_cast<void(MetadataContainer::*)(const char *, const char *, float)>(&MetadataContainer::SetMetadataFloat), py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataFloat));
  MetadataContainer_.def("setMetadataFloatVector2"     , [](MetadataContainer* self, const char * category, const char * name, FloatVector2Adapter::AdaptedType value) { return self->SetMetadataFloatVector2(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataFloatVector2));
  MetadataContainer_.def("setMetadataFloatVector3"     , [](MetadataContainer* self, const char * category, const char * name, FloatVector3Adapter::AdaptedType value) { return self->SetMetadataFloatVector3(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataFloatVector3));
  MetadataContainer_.def("setMetadataFloatVector4"     , [](MetadataContainer* self, const char * category, const char * name, FloatVector4Adapter::AdaptedType value) { return self->SetMetadataFloatVector4(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataFloatVector4));
  MetadataContainer_.def("setMetadataDouble"           , static_cast<void(MetadataContainer::*)(const char *, const char *, double)>(&MetadataContainer::SetMetadataDouble), py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataDouble));
  MetadataContainer_.def("setMetadataDoubleVector2"    , [](MetadataContainer* self, const char * category, const char * name, DoubleVector2Adapter::AdaptedType value) { return self->SetMetadataDoubleVector2(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataDoubleVector2));
  MetadataContainer_.def("setMetadataDoubleVector3"    , [](MetadataContainer* self, const char * category, const char * name, DoubleVector3Adapter::AdaptedType value) { return self->SetMetadataDoubleVector3(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataDoubleVector3));
  MetadataContainer_.def("setMetadataDoubleVector4"    , [](MetadataContainer* self, const char * category, const char * name, DoubleVector4Adapter::AdaptedType value) { return self->SetMetadataDoubleVector4(category, name, value); }, py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataContainer_SetMetadataDoubleVector4));
  MetadataContainer_.def("copyMetadata"                , static_cast<void(MetadataContainer::*)(const char *, const native::MetadataReadAccess *)>(&MetadataContainer::CopyMetadata), py::arg("category"), py::arg("metadataReadAccess"), OPENVDS_DOCSTRING(MetadataContainer_CopyMetadata));
  MetadataContainer_.def("clearMetadata"               , static_cast<void(MetadataContainer::*)(const char *, const char *)>(&MetadataContainer::ClearMetadata), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_ClearMetadata));
  MetadataContainer_.def("clearMetadata"               , static_cast<void(MetadataContainer::*)(const char *)>(&MetadataContainer::ClearMetadata), py::arg("category"), OPENVDS_DOCSTRING(MetadataContainer_ClearMetadata_2));
// AUTOGENERATE FAIL :   MetadataContainer_.def("getMetadataBLOB"             , static_cast<void(MetadataContainer::*)(const char *, const char *, const void **, size_t *) const>(&MetadataContainer::GetMetadataBLOB), py::arg("category"), py::arg("name"), py::arg("data"), py::arg("size"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataBLOB));
  MetadataContainer_.def("begin"                       , static_cast<native::MetadataReadAccess::const_iterator(MetadataContainer::*)() const>(&MetadataContainer::begin), OPENVDS_DOCSTRING(MetadataContainer_begin));
  MetadataContainer_.def("end"                         , static_cast<native::MetadataReadAccess::const_iterator(MetadataContainer::*)() const>(&MetadataContainer::end), OPENVDS_DOCSTRING(MetadataContainer_end));

//AUTOGEN-END

  py::class_<BLOB> BLOB_(m, "BLOB", py::buffer_protocol());
  BLOB_.def_buffer([](BLOB& blob)
    {
      return py::buffer_info(
        blob.m_Data,
        sizeof(uint8_t),
        py::format_descriptor<uint8_t>::format(),
        1,
        { blob.m_Size },
        { sizeof(uint8_t) }
      );
    });

  // IMPLEMENTED :   MetadataContainer_.def("getMetadataBLOB"             , static_cast<void(MetadataContainer::*)(const char *, const char *, const void **, size_t *) const>(&MetadataContainer::GetMetadataBLOB), py::arg("category"), py::arg("name"), py::arg("data"), py::arg("size"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataBLOB));
  MetadataContainer_.def("getMetadataBLOB", [](MetadataContainer* self, const char* category, const char* name) 
    {
      BLOB blob;
      self->GetMetadataBLOB(category, name, (void const**)&blob.m_Data, &blob.m_Size);
      return blob;
    },
    py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataBLOB));
}

