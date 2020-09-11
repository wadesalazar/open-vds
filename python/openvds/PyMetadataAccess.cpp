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

#include "PyMetadataAccess.h"

using namespace native;

namespace internal
{

typedef int Int;
typedef float Float;
typedef double Double;
typedef std::string String;

template<typename T>
struct MetadataTraits;

template<> struct MetadataTraits<Int>           { static native::MetadataType datatype() { return MetadataType::Int; } };
template<> struct MetadataTraits<IntVector2>    { static native::MetadataType datatype() { return MetadataType::IntVector2; } };
template<> struct MetadataTraits<IntVector3>    { static native::MetadataType datatype() { return MetadataType::IntVector3; } };
template<> struct MetadataTraits<IntVector4>    { static native::MetadataType datatype() { return MetadataType::IntVector4; } };
template<> struct MetadataTraits<Float>         { static native::MetadataType datatype() { return MetadataType::Float; } };
template<> struct MetadataTraits<FloatVector2>  { static native::MetadataType datatype() { return MetadataType::FloatVector2; } };
template<> struct MetadataTraits<FloatVector3>  { static native::MetadataType datatype() { return MetadataType::FloatVector3; } };
template<> struct MetadataTraits<FloatVector4>  { static native::MetadataType datatype() { return MetadataType::FloatVector4; } };
template<> struct MetadataTraits<Double>        { static native::MetadataType datatype() { return MetadataType::Double; } };
template<> struct MetadataTraits<DoubleVector2> { static native::MetadataType datatype() { return MetadataType::DoubleVector2; } };
template<> struct MetadataTraits<DoubleVector3> { static native::MetadataType datatype() { return MetadataType::DoubleVector3; } };
template<> struct MetadataTraits<DoubleVector4> { static native::MetadataType datatype() { return MetadataType::DoubleVector4; } };
template<> struct MetadataTraits<String>        { static native::MetadataType datatype() { return MetadataType::String; } };
template<> struct MetadataTraits<BLOB>          { static native::MetadataType datatype() { return MetadataType::BLOB; } };

class PyMetadataVariant
{
public:
  PyMetadataVariant() : m_IntVector4Value(0, 0, 0, 0), m_Type(MetadataType::Int)
  {
  }

  template<typename T>
  void
  set(const T& value)
  {
    auto self = reinterpret_cast<T*>(this);
    *self = value;
    m_Type = MetadataTraits<T>::datatype();
  }

  void
  set(const String& value)
  {
    m_StringValue = value;
    m_Type = MetadataType::String;
  }

  union
  {
    IntVector4          m_IntVector4Value;
    FloatVector4        m_FloatVector4Value;             
    DoubleVector4       m_DoubleVector4Value;      
    BLOB                m_BLOBValue;       
  };
  String                m_StringValue;
  native::MetadataType  m_Type;
};

}

namespace pybind11 { namespace detail {
    template <> struct type_caster<internal::PyMetadataVariant> {
    public:
        /**
         * This macro establishes the name 'inty' in
         * function signatures and declares a local variable
         * 'value' of type inty
         */
        PYBIND11_TYPE_CASTER(internal::PyMetadataVariant, _("PyMetadataVariant"));

        /**
         * Conversion part 1 (Python->C++): convert a PyObject into a inty
         * instance or return false upon failure. The second argument
         * indicates whether implicit conversions should be applied.
         */
        bool load(handle src, bool) {
            ///* Extract PyObject from handle */
            //PyObject *source = src.ptr();
            ///* Try converting into a Python integer value */
            //PyObject *tmp = PyNumber_Long(source);
            //if (!tmp)
            //    return false;
            ///* Now try to convert into a C++ int */
            //value.long_value = PyLong_AsLong(tmp);
            //Py_DECREF(tmp);
            ///* Ensure return code was OK (to avoid out-of-range errors etc) */
            //return !(value.long_value == -1 && !PyErr_Occurred());
          return false;
        }

        /**
         * Conversion part 2 (C++ -> Python): convert an inty instance into
         * a Python object. The second and third arguments are used to
         * indicate the return value policy and parent object (for
         * ``return_value_policy::reference_internal``) and are generally
         * ignored by implicit casters.
         */
        static handle cast(internal::PyMetadataVariant src, return_value_policy /* policy */, handle /* parent */) {
          switch (src.m_Type)
          {
          case MetadataType::Int:
            return Py_BuildValue("i",     src.m_IntVector4Value[0]);
          case MetadataType::IntVector2:
            return Py_BuildValue("ii",    src.m_IntVector4Value[0], src.m_IntVector4Value[1]);
          case MetadataType::IntVector3:
            return Py_BuildValue("iii",   src.m_IntVector4Value[0], src.m_IntVector4Value[1], src.m_IntVector4Value[2]);
          case MetadataType::IntVector4:
            return Py_BuildValue("iiii",  src.m_IntVector4Value[0], src.m_IntVector4Value[1], src.m_IntVector4Value[2], src.m_IntVector4Value[3]);
          case MetadataType::Float:
            return Py_BuildValue("f",     src.m_FloatVector4Value[0]);
          case MetadataType::FloatVector2:
            return Py_BuildValue("ff",    src.m_FloatVector4Value[0], src.m_FloatVector4Value[1]);
          case MetadataType::FloatVector3:
            return Py_BuildValue("fff",   src.m_FloatVector4Value[0], src.m_FloatVector4Value[1], src.m_FloatVector4Value[2]);
          case MetadataType::FloatVector4:
            return Py_BuildValue("ffff",  src.m_FloatVector4Value[0], src.m_FloatVector4Value[1], src.m_FloatVector4Value[2], src.m_FloatVector4Value[3]);
          case MetadataType::Double:
            return Py_BuildValue("f",     src.m_DoubleVector4Value[0]);
          case MetadataType::DoubleVector2:
            return Py_BuildValue("ff",    src.m_DoubleVector4Value[0], src.m_DoubleVector4Value[1]);
          case MetadataType::DoubleVector3:
            return Py_BuildValue("fff",   src.m_DoubleVector4Value[0], src.m_DoubleVector4Value[1], src.m_DoubleVector4Value[2]);
          case MetadataType::DoubleVector4:
            return Py_BuildValue("ffff",  src.m_DoubleVector4Value[0], src.m_DoubleVector4Value[1], src.m_DoubleVector4Value[2], src.m_DoubleVector4Value[3]);
          case MetadataType::String:
            return PyUnicode_FromString(src.m_StringValue.c_str());
          case MetadataType::BLOB:
            return PyByteArray_FromStringAndSize((const char*)src.m_BLOBValue.m_Data, src.m_BLOBValue.m_Size);
          default:
            Py_RETURN_NONE;

          }
        }
    };
}} // namespace pybind11::detail

internal::PyMetadataVariant
PyMetadataAccess::GetMetadata(native::MetadataReadAccess* self, const char* category, const char* name, native::MetadataType type)
{
  internal::PyMetadataVariant variant;
  BLOB blob;
  switch (type)
  {
  case MetadataType::Int          : variant.set(self->GetMetadataInt          (category, name)) ; break;
  case MetadataType::IntVector2   : variant.set(self->GetMetadataIntVector2   (category, name)) ; break;
  case MetadataType::IntVector3   : variant.set(self->GetMetadataIntVector3   (category, name)) ; break;
  case MetadataType::IntVector4   : variant.set(self->GetMetadataIntVector4   (category, name)) ; break;
  case MetadataType::Float        : variant.set(self->GetMetadataFloat        (category, name)) ; break;
  case MetadataType::FloatVector2 : variant.set(self->GetMetadataFloatVector2 (category, name)) ; break;
  case MetadataType::FloatVector3 : variant.set(self->GetMetadataFloatVector3 (category, name)) ; break;
  case MetadataType::FloatVector4 : variant.set(self->GetMetadataFloatVector4 (category, name)) ; break;
  case MetadataType::Double       : variant.set(self->GetMetadataDouble       (category, name)) ; break;
  case MetadataType::DoubleVector2: variant.set(self->GetMetadataDoubleVector2(category, name)) ; break;
  case MetadataType::DoubleVector3: variant.set(self->GetMetadataDoubleVector3(category, name)) ; break;
  case MetadataType::DoubleVector4: variant.set(self->GetMetadataDoubleVector4(category, name)) ; break;
  case MetadataType::String       : 
    variant.set(internal::String(self->GetMetadataString(category, name))); 
    break;
  case MetadataType::BLOB         : 
    self->GetMetadataBLOB(category, name, (const void**)&blob.m_Data, &blob.m_Size);
    variant.set(blob); 
    break;
  default:
    break;
  }
  return variant;
}

void 
PyMetadataAccess::initModule(py::module& m)
{
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

//AUTOGEN-BEGIN
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
  MetadataReadAccess_.def("getMetadataKeys"             , static_cast<native::MetadataKeyRange(MetadataReadAccess::*)() const>(&MetadataReadAccess::GetMetadataKeys), OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataKeys));
  MetadataReadAccess_.def_property_readonly("metadataKeys", &MetadataReadAccess::GetMetadataKeys, OPENVDS_DOCSTRING(MetadataReadAccess_GetMetadataKeys));

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
  MetadataWriteAccess_.def("setMetadataString"           , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, const char *)>(&MetadataWriteAccess::SetMetadataString), py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataString));
  MetadataWriteAccess_.def("setMetadataString"           , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, const std::string &)>(&MetadataWriteAccess::SetMetadataString), py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataString_2));
// AUTOGENERATE FAIL :   MetadataWriteAccess_.def("setMetadataBLOB"             , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, const void *, size_t)>(&MetadataWriteAccess::SetMetadataBLOB), py::arg("category"), py::arg("name"), py::arg("data"), py::arg("size"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataBLOB));
  MetadataWriteAccess_.def("copyMetadata"                , static_cast<void(MetadataWriteAccess::*)(const char *, const native::MetadataReadAccess *)>(&MetadataWriteAccess::CopyMetadata), py::arg("category"), py::arg("metadataReadAccess"), OPENVDS_DOCSTRING(MetadataWriteAccess_CopyMetadata));
  MetadataWriteAccess_.def("clearMetadata"               , static_cast<void(MetadataWriteAccess::*)(const char *, const char *)>(&MetadataWriteAccess::ClearMetadata), py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataWriteAccess_ClearMetadata));
  MetadataWriteAccess_.def("clearMetadata"               , static_cast<void(MetadataWriteAccess::*)(const char *)>(&MetadataWriteAccess::ClearMetadata), py::arg("category"), OPENVDS_DOCSTRING(MetadataWriteAccess_ClearMetadata_2));

//AUTOGEN-END
  // IMPLEMENTED :   MetadataWriteAccess_.def("setMetadataBLOB"             , static_cast<void(MetadataWriteAccess::*)(const char *, const char *, const void *, size_t)>(&MetadataWriteAccess::SetMetadataBLOB), py::arg("category"), py::arg("name"), py::arg("data"), py::arg("size"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataBLOB));
  MetadataWriteAccess_.def("setMetadataBLOB", [](MetadataWriteAccess* self, const char * category, const char * name, py::buffer data)
    {
      py::buffer_info buffer = data.request();
      size_t size = buffer.size * buffer.itemsize;
      self->SetMetadataBLOB(category, name, buffer.ptr, size);
    },
    py::arg("category"), py::arg("name"), py::arg("value"), OPENVDS_DOCSTRING(MetadataWriteAccess_SetMetadataBLOB));

  // IMPLEMENTED :   MetadataContainer_.def("getMetadataBLOB"             , static_cast<void(MetadataContainer::*)(const char *, const char *, const void **, size_t *) const>(&MetadataContainer::GetMetadataBLOB), py::arg("category"), py::arg("name"), py::arg("data"), py::arg("size"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataBLOB));
  MetadataReadAccess_.def("getMetadataBLOB", [](MetadataReadAccess* self, const char* category, const char* name) 
    {
      BLOB blob;
      self->GetMetadataBLOB(category, name, (void const**)&blob.m_Data, &blob.m_Size);
      return blob;
    },
    py::arg("category"), py::arg("name"), OPENVDS_DOCSTRING(MetadataContainer_GetMetadataBLOB));

  MetadataReadAccess_.def("getMetadata", [](MetadataReadAccess* self, native::MetadataKey const& key)
    {
      return GetMetadata(self, key.GetCategory(), key.GetName(), key.GetType());
    }, py::arg("key"));
  MetadataReadAccess_.def("getMetadata", [](MetadataReadAccess* self, const char* category, const char* name, native::MetadataType type)
    {
      return GetMetadata(self, category, name, type);
    }, py::arg("category"), py::arg("name"), py::arg("type"));
}

