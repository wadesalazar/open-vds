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

  // MetadataKey
  py::class_<MetadataKey> 
    MetadataKey_(m,"MetadataKey", OPENVDS_DOCSTRING(MetadataKey));

  MetadataKey_.def_readwrite("type"                        , &MetadataKey::type             , OPENVDS_DOCSTRING(MetadataKey_type));
  MetadataKey_.def_readwrite("category"                    , &MetadataKey::category         , OPENVDS_DOCSTRING(MetadataKey_category));
  MetadataKey_.def_readwrite("name"                        , &MetadataKey::name             , OPENVDS_DOCSTRING(MetadataKey_name));

  m.def("operator_eq"                 , static_cast<bool(*)(const native::MetadataKey &, const native::MetadataKey &)>(&operator==), OPENVDS_DOCSTRING(operator_eq));

  // MetadataReadAccess
  py::class_<MetadataReadAccess, std::unique_ptr<MetadataReadAccess, py::nodelete>> 
    MetadataReadAccess_(m,"MetadataReadAccess", OPENVDS_DOCSTRING(MetadataReadAccess));

  MetadataReadAccess_.def("isMetadataIntAvailable"      , static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataIntAvailable), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataIntAvailable));
  MetadataReadAccess_.def("isMetadataIntVector2Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataIntVector2Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataIntVector2Available));
  MetadataReadAccess_.def("isMetadataIntVector3Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataIntVector3Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataIntVector3Available));
  MetadataReadAccess_.def("isMetadataIntVector4Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataIntVector4Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataIntVector4Available));
  MetadataReadAccess_.def("isMetadataFloatAvailable"    , static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataFloatAvailable), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataFloatAvailable));
  MetadataReadAccess_.def("isMetadataFloatVector2Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataFloatVector2Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataFloatVector2Available));
  MetadataReadAccess_.def("isMetadataFloatVector3Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataFloatVector3Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataFloatVector3Available));
  MetadataReadAccess_.def("isMetadataFloatVector4Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataFloatVector4Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataFloatVector4Available));
  MetadataReadAccess_.def("isMetadataDoubleAvailable"   , static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataDoubleAvailable), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataDoubleAvailable));
  MetadataReadAccess_.def("isMetadataDoubleVector2Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataDoubleVector2Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataDoubleVector2Available));
  MetadataReadAccess_.def("isMetadataDoubleVector3Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataDoubleVector3Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataDoubleVector3Available));
  MetadataReadAccess_.def("isMetadataDoubleVector4Available", static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataDoubleVector4Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataDoubleVector4Available));
  MetadataReadAccess_.def("isMetadataStringAvailable"   , static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataStringAvailable), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataStringAvailable));
  MetadataReadAccess_.def("isMetadataBLOBAvailable"     , static_cast<bool(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::IsMetadataBLOBAvailable), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataBLOBAvailable));
  MetadataReadAccess_.def("getMetadataInt"              , static_cast<int(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataInt), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataInt));
  MetadataReadAccess_.def("getMetadataIntVector2"       , static_cast<native::IntVector2(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataIntVector2), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataIntVector2));
  MetadataReadAccess_.def("getMetadataIntVector3"       , static_cast<native::IntVector3(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataIntVector3), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataIntVector3));
  MetadataReadAccess_.def("getMetadataIntVector4"       , static_cast<native::IntVector4(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataIntVector4), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataIntVector4));
  MetadataReadAccess_.def("getMetadataFloat"            , static_cast<float(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataFloat), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataFloat));
  MetadataReadAccess_.def("getMetadataFloatVector2"     , static_cast<native::FloatVector2(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataFloatVector2), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataFloatVector2));
  MetadataReadAccess_.def("getMetadataFloatVector3"     , static_cast<native::FloatVector3(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataFloatVector3), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataFloatVector3));
  MetadataReadAccess_.def("getMetadataFloatVector4"     , static_cast<native::FloatVector4(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataFloatVector4), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataFloatVector4));
  MetadataReadAccess_.def("getMetadataDouble"           , static_cast<double(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataDouble), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataDouble));
  MetadataReadAccess_.def("getMetadataDoubleVector2"    , static_cast<native::DoubleVector2(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataDoubleVector2), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataDoubleVector2));
  MetadataReadAccess_.def("getMetadataDoubleVector3"    , static_cast<native::DoubleVector3(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataDoubleVector3), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataDoubleVector3));
  MetadataReadAccess_.def("getMetadataDoubleVector4"    , static_cast<native::DoubleVector4(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataDoubleVector4), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataDoubleVector4));
  MetadataReadAccess_.def("getMetadataString"           , static_cast<const char *(MetadataReadAccess::*)(const char *, const char *) const>(&MetadataReadAccess::GetMetadataString), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataString));

  // MetadataWriteAccess
  py::class_<MetadataWriteAccess, std::unique_ptr<MetadataWriteAccess, py::nodelete>> 
    MetadataWriteAccess_(m,"MetadataWriteAccess", OPENVDS_DOCSTRING(MetadataWriteAccess));

  MetadataWriteAccess_.def("setMetadataInt"              , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, int)>(&MetadataWriteAccess::SetMetadataInt), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataInt));
  MetadataWriteAccess_.def("setMetadataIntVector2"       , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, native::IntVector2)>(&MetadataWriteAccess::SetMetadataIntVector2), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataIntVector2));
  MetadataWriteAccess_.def("setMetadataIntVector3"       , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, native::IntVector3)>(&MetadataWriteAccess::SetMetadataIntVector3), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataIntVector3));
  MetadataWriteAccess_.def("setMetadataIntVector4"       , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, native::IntVector4)>(&MetadataWriteAccess::SetMetadataIntVector4), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataIntVector4));
  MetadataWriteAccess_.def("setMetadataFloat"            , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, float)>(&MetadataWriteAccess::SetMetadataFloat), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataFloat));
  MetadataWriteAccess_.def("setMetadataFloatVector2"     , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, native::FloatVector2)>(&MetadataWriteAccess::SetMetadataFloatVector2), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataFloatVector2));
  MetadataWriteAccess_.def("setMetadataFloatVector3"     , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, native::FloatVector3)>(&MetadataWriteAccess::SetMetadataFloatVector3), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataFloatVector3));
  MetadataWriteAccess_.def("setMetadataFloatVector4"     , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, native::FloatVector4)>(&MetadataWriteAccess::SetMetadataFloatVector4), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataFloatVector4));
  MetadataWriteAccess_.def("setMetadataDouble"           , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, double)>(&MetadataWriteAccess::SetMetadataDouble), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataDouble));
  MetadataWriteAccess_.def("setMetadataDoubleVector2"    , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, native::DoubleVector2)>(&MetadataWriteAccess::SetMetadataDoubleVector2), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataDoubleVector2));
  MetadataWriteAccess_.def("setMetadataDoubleVector3"    , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, native::DoubleVector3)>(&MetadataWriteAccess::SetMetadataDoubleVector3), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataDoubleVector3));
  MetadataWriteAccess_.def("setMetadataDoubleVector4"    , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, native::DoubleVector4)>(&MetadataWriteAccess::SetMetadataDoubleVector4), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataDoubleVector4));
  MetadataWriteAccess_.def("setMetadataString"           , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, const std::string &)>(&MetadataWriteAccess::SetMetadataString), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataString));
  MetadataWriteAccess_.def("copyMetadata"                , static_cast<void(MetadataWriteAccess::*)(const char *, const native::MetadataReadAccess *)>(&MetadataWriteAccess::CopyMetadata), OPENVDS_DOCSTRING(MetadataWriteAccess_CopyMetadata));
  MetadataWriteAccess_.def("clearMetadata"               , static_cast<void(MetadataWriteAccess::*)(const char *, const char *)>(&MetadataWriteAccess::ClearMetadata), OPENVDS_DOCSTRING(MetadataWriteAccess_ClearMetadata));
  MetadataWriteAccess_.def("clearMetadata"               , static_cast<void(MetadataWriteAccess::*)(const char *)>(&MetadataWriteAccess::ClearMetadata), OPENVDS_DOCSTRING(MetadataWriteAccess_ClearMetadata_2));

  // MetadataContainer
  py::class_<MetadataContainer, MetadataReadAccess> 
    MetadataContainer_(m,"MetadataContainer", OPENVDS_DOCSTRING(MetadataContainer));

  MetadataContainer_.def("isMetadataIntAvailable"      , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataIntAvailable), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntAvailable));
  MetadataContainer_.def("isMetadataIntVector2Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataIntVector2Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntVector2Available));
  MetadataContainer_.def("isMetadataIntVector3Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataIntVector3Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntVector3Available));
  MetadataContainer_.def("isMetadataIntVector4Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataIntVector4Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntVector4Available));
  MetadataContainer_.def("isMetadataFloatAvailable"    , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataFloatAvailable), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatAvailable));
  MetadataContainer_.def("isMetadataFloatVector2Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataFloatVector2Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatVector2Available));
  MetadataContainer_.def("isMetadataFloatVector3Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataFloatVector3Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatVector3Available));
  MetadataContainer_.def("isMetadataFloatVector4Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataFloatVector4Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatVector4Available));
  MetadataContainer_.def("isMetadataDoubleAvailable"   , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataDoubleAvailable), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleAvailable));
  MetadataContainer_.def("isMetadataDoubleVector2Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataDoubleVector2Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleVector2Available));
  MetadataContainer_.def("isMetadataDoubleVector3Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataDoubleVector3Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleVector3Available));
  MetadataContainer_.def("isMetadataDoubleVector4Available", static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataDoubleVector4Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleVector4Available));
  MetadataContainer_.def("isMetadataStringAvailable"   , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataStringAvailable), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataStringAvailable));
  MetadataContainer_.def("isMetadataBLOBAvailable"     , static_cast<bool(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::IsMetadataBLOBAvailable), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataBLOBAvailable));
  MetadataContainer_.def("getMetadataInt"              , static_cast<int(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataInt), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataInt));
  MetadataContainer_.def("getMetadataIntVector2"       , static_cast<native::IntVector2(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataIntVector2), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataIntVector2));
  MetadataContainer_.def("getMetadataIntVector3"       , static_cast<native::IntVector3(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataIntVector3), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataIntVector3));
  MetadataContainer_.def("getMetadataIntVector4"       , static_cast<native::IntVector4(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataIntVector4), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataIntVector4));
  MetadataContainer_.def("getMetadataFloat"            , static_cast<float(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataFloat), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloat));
  MetadataContainer_.def("getMetadataFloatVector2"     , static_cast<native::FloatVector2(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataFloatVector2), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloatVector2));
  MetadataContainer_.def("getMetadataFloatVector3"     , static_cast<native::FloatVector3(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataFloatVector3), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloatVector3));
  MetadataContainer_.def("getMetadataFloatVector4"     , static_cast<native::FloatVector4(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataFloatVector4), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloatVector4));
  MetadataContainer_.def("getMetadataDouble"           , static_cast<double(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataDouble), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDouble));
  MetadataContainer_.def("getMetadataDoubleVector2"    , static_cast<native::DoubleVector2(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataDoubleVector2), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDoubleVector2));
  MetadataContainer_.def("getMetadataDoubleVector3"    , static_cast<native::DoubleVector3(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataDoubleVector3), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDoubleVector3));
  MetadataContainer_.def("getMetadataDoubleVector4"    , static_cast<native::DoubleVector4(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataDoubleVector4), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDoubleVector4));
  MetadataContainer_.def("getMetadataString"           , static_cast<const char *(MetadataContainer::*)(const char *, const char *) const>(&MetadataContainer::GetMetadataString), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataString));
// AUTOGENERATE FAIL :   MetadataContainer_.def("getMetadataBLOB"             , static_cast<void(MetadataContainer::*)(const char *, const char *, const void **, size_t *) const>(&MetadataContainer::GetMetadataBLOB), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataBLOB));
  MetadataContainer_.def_readwrite("intData"                     , &MetadataContainer::intData    , OPENVDS_DOCSTRING(MetadataContainer_intData));
  MetadataContainer_.def_readwrite("intVector2Data"              , &MetadataContainer::intVector2Data, OPENVDS_DOCSTRING(MetadataContainer_intVector2Data));
  MetadataContainer_.def_readwrite("intVector3Data"              , &MetadataContainer::intVector3Data, OPENVDS_DOCSTRING(MetadataContainer_intVector3Data));
  MetadataContainer_.def_readwrite("intVector4Data"              , &MetadataContainer::intVector4Data, OPENVDS_DOCSTRING(MetadataContainer_intVector4Data));
  MetadataContainer_.def_readwrite("floatData"                   , &MetadataContainer::floatData  , OPENVDS_DOCSTRING(MetadataContainer_floatData));
  MetadataContainer_.def_readwrite("floatVector2Data"            , &MetadataContainer::floatVector2Data, OPENVDS_DOCSTRING(MetadataContainer_floatVector2Data));
  MetadataContainer_.def_readwrite("floatVector3Data"            , &MetadataContainer::floatVector3Data, OPENVDS_DOCSTRING(MetadataContainer_floatVector3Data));
  MetadataContainer_.def_readwrite("floatVector4Data"            , &MetadataContainer::floatVector4Data, OPENVDS_DOCSTRING(MetadataContainer_floatVector4Data));
  MetadataContainer_.def_readwrite("doubleData"                  , &MetadataContainer::doubleData , OPENVDS_DOCSTRING(MetadataContainer_doubleData));
  MetadataContainer_.def_readwrite("doubleVector2Data"           , &MetadataContainer::doubleVector2Data, OPENVDS_DOCSTRING(MetadataContainer_doubleVector2Data));
  MetadataContainer_.def_readwrite("doubleVector3Data"           , &MetadataContainer::doubleVector3Data, OPENVDS_DOCSTRING(MetadataContainer_doubleVector3Data));
  MetadataContainer_.def_readwrite("doubleVector4Data"           , &MetadataContainer::doubleVector4Data, OPENVDS_DOCSTRING(MetadataContainer_doubleVector4Data));
  MetadataContainer_.def_readwrite("stringData"                  , &MetadataContainer::stringData , OPENVDS_DOCSTRING(MetadataContainer_stringData));
  MetadataContainer_.def_readwrite("blobData"                    , &MetadataContainer::blobData   , OPENVDS_DOCSTRING(MetadataContainer_blobData));
  MetadataContainer_.def_readwrite("keys"                        , &MetadataContainer::keys       , OPENVDS_DOCSTRING(MetadataContainer_keys));

//AUTOGEN-END
}

