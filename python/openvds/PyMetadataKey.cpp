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

#include "PyMetadataKey.h"

using namespace native;

void 
PyMetadataKey::initModule(py::module& m)
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

  MetadataKey_.def(py::init<                              >(), OPENVDS_DOCSTRING(MetadataKey_MetadataKey));
  MetadataKey_.def(py::init<native::MetadataType, const char *, const char *>(), py::arg("type").none(false), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataKey_MetadataKey_2));
  MetadataKey_.def("getType"                     , static_cast<native::MetadataType(MetadataKey::*)() const>(&MetadataKey::GetType), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(MetadataKey_GetType));
  MetadataKey_.def_property_readonly("type", &MetadataKey::GetType, OPENVDS_DOCSTRING(MetadataKey_GetType));
  MetadataKey_.def("getCategory"                 , static_cast<const char *(MetadataKey::*)() const>(&MetadataKey::GetCategory), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(MetadataKey_GetCategory));
  MetadataKey_.def_property_readonly("category", &MetadataKey::GetCategory, OPENVDS_DOCSTRING(MetadataKey_GetCategory));
  MetadataKey_.def("getName"                     , static_cast<const char *(MetadataKey::*)() const>(&MetadataKey::GetName), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(MetadataKey_GetName));
  MetadataKey_.def_property_readonly("name", &MetadataKey::GetName, OPENVDS_DOCSTRING(MetadataKey_GetName));
  MetadataKey_.def(py::self == py::self);
  MetadataKey_.def(py::self != py::self);

  // MetadataKeyRange
  py::class_<MetadataKeyRange> 
    MetadataKeyRange_(m,"MetadataKeyRange", OPENVDS_DOCSTRING(MetadataKeyRange));

  MetadataKeyRange_.def(py::init<native::MetadataKeyRange::const_iterator, native::MetadataKeyRange::const_iterator>(), py::arg("begin").none(false), py::arg("end").none(false), OPENVDS_DOCSTRING(MetadataKeyRange_MetadataKeyRange));
  MetadataKeyRange_.def("begin"                       , static_cast<native::MetadataKeyRange::const_iterator(MetadataKeyRange::*)() const>(&MetadataKeyRange::begin), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(MetadataKeyRange_begin));
  MetadataKeyRange_.def("end"                         , static_cast<native::MetadataKeyRange::const_iterator(MetadataKeyRange::*)() const>(&MetadataKeyRange::end), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(MetadataKeyRange_end));

//AUTOGEN-END
  MetadataKeyRange_.def("__len__", [](MetadataKeyRange* self) 
    {
      return (Py_ssize_t)(self->end() - self->begin());
    });
  MetadataKeyRange_.def("__iter__", [](MetadataKeyRange* self) 
    {
      return py::make_iterator(self->begin(), self->end());
    });

  MetadataKey_.def("__repr__", [](MetadataKey* self)
    {
      const char* type = "unknown";
      switch(self->GetType())
      {
      case MetadataType::Int          : type = "Int"           ; break;
      case MetadataType::IntVector2   : type = "IntVector2"    ; break;
      case MetadataType::IntVector3   : type = "IntVector3"    ; break;
      case MetadataType::IntVector4   : type = "IntVector4"    ; break;
      case MetadataType::Float        : type = "Float"         ; break;
      case MetadataType::FloatVector2 : type = "FloatVector2"  ; break;
      case MetadataType::FloatVector3 : type = "FloatVector3"  ; break;
      case MetadataType::FloatVector4 : type = "FloatVector4"  ; break;
      case MetadataType::Double       : type = "Double"        ; break;
      case MetadataType::DoubleVector2: type = "DoubleVector2" ; break;
      case MetadataType::DoubleVector3: type = "DoubleVector3" ; break;
      case MetadataType::DoubleVector4: type = "DoubleVector4" ; break;
      case MetadataType::String       : type = "String"        ; break;
      case MetadataType::BLOB         : type = "BLOB"          ; break;
      }
      return std::string("MetadataKey(category='") + self->GetCategory() + "', name='" + self->GetName() + "', type=MetadataType." + type + ")";
    });

// IMPLEMENTED :   MetadataKey_.def(py::init<native::MetadataType, const char *, const char *>(), py::arg("type").none(false), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataKey_MetadataKey_2));
  MetadataKey_.def(py::init([](native::MetadataType type, std::string category, std::string name)
    {
      return MetadataKey(type, PyDescriptorStringContainer::Add(category), PyDescriptorStringContainer::Add(name));
    }), py::arg("type").none(false), py::arg("category").none(false), py::arg("name").none(false), OPENVDS_DOCSTRING(MetadataKey_MetadataKey_2));
}
