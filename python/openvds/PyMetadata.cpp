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

void 
PyMetadata::initModule(py::module& m)
{
#if 0
//AUTOGEN-BEGIN
  py::enum_<native::MetadataType> 
    MetadataType_(m,"MetadataType", OPENVDS_DOCSTRING(MetadataType));

  // MetadataKey
  py::class_<native::MetadataKey> 
    MetadataKey_(m,"MetadataKey", OPENVDS_DOCSTRING(MetadataKey));

  MetadataKey_.def_readwrite("type"                        , &native::MetadataKey::type             , OPENVDS_DOCSTRING(MetadataKey_type));
  MetadataKey_.def_readwrite("category"                    , &native::MetadataKey::category         , OPENVDS_DOCSTRING(MetadataKey_category));
  MetadataKey_.def_readwrite("name"                        , &native::MetadataKey::name             , OPENVDS_DOCSTRING(MetadataKey_name));

  m.def("operator=="                  , static_cast<bool(*)(const OpenVDS::MetadataKey &, const OpenVDS::MetadataKey &)>(&native::operator==), OPENVDS_DOCSTRING(operator==));

  // MetadataReadAccess
  py::class_<native::MetadataReadAccess> 
    MetadataReadAccess_(m,"MetadataReadAccess", OPENVDS_DOCSTRING(MetadataReadAccess));

  MetadataReadAccess_.def("isMetadataIntAvailable"      , static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataIntAvailable), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataIntAvailable));
  MetadataReadAccess_.def("isMetadataIntVector2Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataIntVector2Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataIntVector2Available));
  MetadataReadAccess_.def("isMetadataIntVector3Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataIntVector3Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataIntVector3Available));
  MetadataReadAccess_.def("isMetadataIntVector4Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataIntVector4Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataIntVector4Available));
  MetadataReadAccess_.def("isMetadataFloatAvailable"    , static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataFloatAvailable), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataFloatAvailable));
  MetadataReadAccess_.def("isMetadataFloatVector2Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataFloatVector2Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataFloatVector2Available));
  MetadataReadAccess_.def("isMetadataFloatVector3Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataFloatVector3Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataFloatVector3Available));
  MetadataReadAccess_.def("isMetadataFloatVector4Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataFloatVector4Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataFloatVector4Available));
  MetadataReadAccess_.def("isMetadataDoubleAvailable"   , static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataDoubleAvailable), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataDoubleAvailable));
  MetadataReadAccess_.def("isMetadataDoubleVector2Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataDoubleVector2Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataDoubleVector2Available));
  MetadataReadAccess_.def("isMetadataDoubleVector3Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataDoubleVector3Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataDoubleVector3Available));
  MetadataReadAccess_.def("isMetadataDoubleVector4Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataDoubleVector4Available), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataDoubleVector4Available));
  MetadataReadAccess_.def("isMetadataStringAvailable"   , static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataStringAvailable), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataStringAvailable));
  MetadataReadAccess_.def("isMetadataBLOBAvailable"     , static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataBLOBAvailable), OPENVDS_DOCSTRING(MetadataReadAccess_IsMetadataBLOBAvailable));
  MetadataReadAccess_.def("getMetadataInt"              , static_cast<int(*)(const char *, const char *)>(&native::GetMetadataInt), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataInt));
  MetadataReadAccess_.def("getMetadataIntVector2"       , static_cast<OpenVDS::IntVector2(*)(const char *, const char *)>(&native::GetMetadataIntVector2), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataIntVector2));
  MetadataReadAccess_.def("getMetadataIntVector3"       , static_cast<OpenVDS::IntVector3(*)(const char *, const char *)>(&native::GetMetadataIntVector3), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataIntVector3));
  MetadataReadAccess_.def("getMetadataIntVector4"       , static_cast<OpenVDS::IntVector4(*)(const char *, const char *)>(&native::GetMetadataIntVector4), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataIntVector4));
  MetadataReadAccess_.def("getMetadataFloat"            , static_cast<float(*)(const char *, const char *)>(&native::GetMetadataFloat), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataFloat));
  MetadataReadAccess_.def("getMetadataFloatVector2"     , static_cast<OpenVDS::FloatVector2(*)(const char *, const char *)>(&native::GetMetadataFloatVector2), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataFloatVector2));
  MetadataReadAccess_.def("getMetadataFloatVector3"     , static_cast<OpenVDS::FloatVector3(*)(const char *, const char *)>(&native::GetMetadataFloatVector3), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataFloatVector3));
  MetadataReadAccess_.def("getMetadataFloatVector4"     , static_cast<OpenVDS::FloatVector4(*)(const char *, const char *)>(&native::GetMetadataFloatVector4), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataFloatVector4));
  MetadataReadAccess_.def("getMetadataDouble"           , static_cast<double(*)(const char *, const char *)>(&native::GetMetadataDouble), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataDouble));
  MetadataReadAccess_.def("getMetadataDoubleVector2"    , static_cast<OpenVDS::DoubleVector2(*)(const char *, const char *)>(&native::GetMetadataDoubleVector2), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataDoubleVector2));
  MetadataReadAccess_.def("getMetadataDoubleVector3"    , static_cast<OpenVDS::DoubleVector3(*)(const char *, const char *)>(&native::GetMetadataDoubleVector3), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataDoubleVector3));
  MetadataReadAccess_.def("getMetadataDoubleVector4"    , static_cast<OpenVDS::DoubleVector4(*)(const char *, const char *)>(&native::GetMetadataDoubleVector4), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataDoubleVector4));
  MetadataReadAccess_.def("getMetadataString"           , static_cast<const char *(*)(const char *, const char *)>(&native::GetMetadataString), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataString));

  // MetadataWriteAccess
  py::class_<native::MetadataWriteAccess> 
    MetadataWriteAccess_(m,"MetadataWriteAccess", OPENVDS_DOCSTRING(MetadataWriteAccess));

  MetadataWriteAccess_.def("setMetadataInt"              , static_cast<void(*)(const char *, const char *, int)>(&native::SetMetadataInt), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataInt));
  MetadataWriteAccess_.def("setMetadataIntVector2"       , static_cast<void(*)(const char *, const char *, OpenVDS::IntVector2)>(&native::SetMetadataIntVector2), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataIntVector2));
  MetadataWriteAccess_.def("setMetadataIntVector3"       , static_cast<void(*)(const char *, const char *, OpenVDS::IntVector3)>(&native::SetMetadataIntVector3), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataIntVector3));
  MetadataWriteAccess_.def("setMetadataIntVector4"       , static_cast<void(*)(const char *, const char *, OpenVDS::IntVector4)>(&native::SetMetadataIntVector4), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataIntVector4));
  MetadataWriteAccess_.def("setMetadataFloat"            , static_cast<void(*)(const char *, const char *, float)>(&native::SetMetadataFloat), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataFloat));
  MetadataWriteAccess_.def("setMetadataFloatVector2"     , static_cast<void(*)(const char *, const char *, OpenVDS::FloatVector2)>(&native::SetMetadataFloatVector2), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataFloatVector2));
  MetadataWriteAccess_.def("setMetadataFloatVector3"     , static_cast<void(*)(const char *, const char *, OpenVDS::FloatVector3)>(&native::SetMetadataFloatVector3), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataFloatVector3));
  MetadataWriteAccess_.def("setMetadataFloatVector4"     , static_cast<void(*)(const char *, const char *, OpenVDS::FloatVector4)>(&native::SetMetadataFloatVector4), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataFloatVector4));
  MetadataWriteAccess_.def("setMetadataDouble"           , static_cast<void(*)(const char *, const char *, double)>(&native::SetMetadataDouble), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataDouble));
  MetadataWriteAccess_.def("setMetadataDoubleVector2"    , static_cast<void(*)(const char *, const char *, OpenVDS::DoubleVector2)>(&native::SetMetadataDoubleVector2), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataDoubleVector2));
  MetadataWriteAccess_.def("setMetadataDoubleVector3"    , static_cast<void(*)(const char *, const char *, OpenVDS::DoubleVector3)>(&native::SetMetadataDoubleVector3), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataDoubleVector3));
  MetadataWriteAccess_.def("setMetadataDoubleVector4"    , static_cast<void(*)(const char *, const char *, OpenVDS::DoubleVector4)>(&native::SetMetadataDoubleVector4), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataDoubleVector4));
  MetadataWriteAccess_.def("setMetadataString"           , static_cast<void(*)(const char *, const char *, const std::string &)>(&native::SetMetadataString), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataString_2));
  MetadataWriteAccess_.def("copyMetadata"                , static_cast<void(*)(const char *, const OpenVDS::MetadataReadAccess *)>(&native::CopyMetadata), OPENVDS_DOCSTRING(MetadataWriteAccess_CopyMetadata));
  MetadataWriteAccess_.def("clearMetadata"               , static_cast<void(*)(const char *, const char *)>(&native::ClearMetadata), OPENVDS_DOCSTRING(MetadataWriteAccess_ClearMetadata));
  MetadataWriteAccess_.def("clearMetadata"               , static_cast<void(*)(const char *)>(&native::ClearMetadata), OPENVDS_DOCSTRING(MetadataWriteAccess_ClearMetadata_2));

  // hash
  py::class_<native::hash> 
    hash_(m,"hash", OPENVDS_DOCSTRING(hash));

  hash_.def("operator()"                  , static_cast<std::size_t(*)()(const OpenVDS::MetadataKey &)>(&native::operator()), OPENVDS_DOCSTRING(hash_operator()));

  // MetadataContainer
  py::class_<native::MetadataContainer, native::MetadataReadAccess> 
    MetadataContainer_(m,"MetadataContainer", OPENVDS_DOCSTRING(MetadataContainer));

  MetadataContainer_.def("isMetadataIntAvailable"      , static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataIntAvailable), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntAvailable_2));
  MetadataContainer_.def("isMetadataIntVector2Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataIntVector2Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntVector2Available_2));
  MetadataContainer_.def("isMetadataIntVector3Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataIntVector3Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntVector3Available_2));
  MetadataContainer_.def("isMetadataIntVector4Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataIntVector4Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataIntVector4Available_2));
  MetadataContainer_.def("isMetadataFloatAvailable"    , static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataFloatAvailable), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatAvailable_2));
  MetadataContainer_.def("isMetadataFloatVector2Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataFloatVector2Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatVector2Available_2));
  MetadataContainer_.def("isMetadataFloatVector3Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataFloatVector3Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatVector3Available_2));
  MetadataContainer_.def("isMetadataFloatVector4Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataFloatVector4Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataFloatVector4Available_2));
  MetadataContainer_.def("isMetadataDoubleAvailable"   , static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataDoubleAvailable), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleAvailable_2));
  MetadataContainer_.def("isMetadataDoubleVector2Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataDoubleVector2Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleVector2Available_2));
  MetadataContainer_.def("isMetadataDoubleVector3Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataDoubleVector3Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleVector3Available_2));
  MetadataContainer_.def("isMetadataDoubleVector4Available", static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataDoubleVector4Available), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataDoubleVector4Available_2));
  MetadataContainer_.def("isMetadataStringAvailable"   , static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataStringAvailable), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataStringAvailable_2));
  MetadataContainer_.def("isMetadataBLOBAvailable"     , static_cast<bool(*)(const char *, const char *)>(&native::IsMetadataBLOBAvailable), OPENVDS_DOCSTRING(MetadataContainer_IsMetadataBLOBAvailable_2));
  MetadataContainer_.def("getMetadataInt"              , static_cast<int(*)(const char *, const char *)>(&native::GetMetadataInt), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataInt_2));
  MetadataContainer_.def("getMetadataIntVector2"       , static_cast<OpenVDS::IntVector2(*)(const char *, const char *)>(&native::GetMetadataIntVector2), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataIntVector2_2));
  MetadataContainer_.def("getMetadataIntVector3"       , static_cast<OpenVDS::IntVector3(*)(const char *, const char *)>(&native::GetMetadataIntVector3), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataIntVector3_2));
  MetadataContainer_.def("getMetadataIntVector4"       , static_cast<OpenVDS::IntVector4(*)(const char *, const char *)>(&native::GetMetadataIntVector4), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataIntVector4_2));
  MetadataContainer_.def("getMetadataFloat"            , static_cast<float(*)(const char *, const char *)>(&native::GetMetadataFloat), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloat_2));
  MetadataContainer_.def("getMetadataFloatVector2"     , static_cast<OpenVDS::FloatVector2(*)(const char *, const char *)>(&native::GetMetadataFloatVector2), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloatVector2_2));
  MetadataContainer_.def("getMetadataFloatVector3"     , static_cast<OpenVDS::FloatVector3(*)(const char *, const char *)>(&native::GetMetadataFloatVector3), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloatVector3_2));
  MetadataContainer_.def("getMetadataFloatVector4"     , static_cast<OpenVDS::FloatVector4(*)(const char *, const char *)>(&native::GetMetadataFloatVector4), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataFloatVector4_2));
  MetadataContainer_.def("getMetadataDouble"           , static_cast<double(*)(const char *, const char *)>(&native::GetMetadataDouble), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDouble_2));
  MetadataContainer_.def("getMetadataDoubleVector2"    , static_cast<OpenVDS::DoubleVector2(*)(const char *, const char *)>(&native::GetMetadataDoubleVector2), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDoubleVector2_2));
  MetadataContainer_.def("getMetadataDoubleVector3"    , static_cast<OpenVDS::DoubleVector3(*)(const char *, const char *)>(&native::GetMetadataDoubleVector3), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDoubleVector3_2));
  MetadataContainer_.def("getMetadataDoubleVector4"    , static_cast<OpenVDS::DoubleVector4(*)(const char *, const char *)>(&native::GetMetadataDoubleVector4), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataDoubleVector4_2));
  MetadataContainer_.def("getMetadataString"           , static_cast<const char *(*)(const char *, const char *)>(&native::GetMetadataString), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataString_2));
  MetadataContainer_.def("getMetadataBLOB"             , static_cast<void(*)(const char *, const char *, const void **, size_t *)>(&native::GetMetadataBLOB), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataBLOB_2));
  MetadataContainer_.def_readwrite("intData"                     , &native::MetadataContainer::intData    , OPENVDS_DOCSTRING(MetadataContainer_intData));
  MetadataContainer_.def_readwrite("intVector2Data"              , &native::MetadataContainer::intVector2Data, OPENVDS_DOCSTRING(MetadataContainer_intVector2Data));
  MetadataContainer_.def_readwrite("intVector3Data"              , &native::MetadataContainer::intVector3Data, OPENVDS_DOCSTRING(MetadataContainer_intVector3Data));
  MetadataContainer_.def_readwrite("intVector4Data"              , &native::MetadataContainer::intVector4Data, OPENVDS_DOCSTRING(MetadataContainer_intVector4Data));
  MetadataContainer_.def_readwrite("floatData"                   , &native::MetadataContainer::floatData  , OPENVDS_DOCSTRING(MetadataContainer_floatData));
  MetadataContainer_.def_readwrite("floatVector2Data"            , &native::MetadataContainer::floatVector2Data, OPENVDS_DOCSTRING(MetadataContainer_floatVector2Data));
  MetadataContainer_.def_readwrite("floatVector3Data"            , &native::MetadataContainer::floatVector3Data, OPENVDS_DOCSTRING(MetadataContainer_floatVector3Data));
  MetadataContainer_.def_readwrite("floatVector4Data"            , &native::MetadataContainer::floatVector4Data, OPENVDS_DOCSTRING(MetadataContainer_floatVector4Data));
  MetadataContainer_.def_readwrite("doubleData"                  , &native::MetadataContainer::doubleData , OPENVDS_DOCSTRING(MetadataContainer_doubleData));
  MetadataContainer_.def_readwrite("doubleVector2Data"           , &native::MetadataContainer::doubleVector2Data, OPENVDS_DOCSTRING(MetadataContainer_doubleVector2Data));
  MetadataContainer_.def_readwrite("doubleVector3Data"           , &native::MetadataContainer::doubleVector3Data, OPENVDS_DOCSTRING(MetadataContainer_doubleVector3Data));
  MetadataContainer_.def_readwrite("doubleVector4Data"           , &native::MetadataContainer::doubleVector4Data, OPENVDS_DOCSTRING(MetadataContainer_doubleVector4Data));
  MetadataContainer_.def_readwrite("stringData"                  , &native::MetadataContainer::stringData , OPENVDS_DOCSTRING(MetadataContainer_stringData));
  MetadataContainer_.def_readwrite("blobData"                    , &native::MetadataContainer::blobData   , OPENVDS_DOCSTRING(MetadataContainer_blobData));
  MetadataContainer_.def_readwrite("keys"                        , &native::MetadataContainer::keys       , OPENVDS_DOCSTRING(MetadataContainer_keys));

//AUTOGEN-END
#endif
}

