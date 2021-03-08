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

#include "PyVolumeDataChannelDescriptor.h"

using namespace native;

void 
PyVolumeDataChannelDescriptor::initModule(py::module& m)
{
//AUTOGEN-BEGIN
  // VolumeDataChannelDescriptor
  py::class_<VolumeDataChannelDescriptor> 
    VolumeDataChannelDescriptor_(m,"VolumeDataChannelDescriptor", OPENVDS_DOCSTRING(VolumeDataChannelDescriptor));

  VolumeDataChannelDescriptor_.def(py::init<                              >(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor));
  VolumeDataChannelDescriptor_.def("getFormat"                   , static_cast<native::VolumeDataChannelDescriptor::Format(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetFormat), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetFormat));
  VolumeDataChannelDescriptor_.def_property_readonly("format", &VolumeDataChannelDescriptor::GetFormat, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetFormat));
  VolumeDataChannelDescriptor_.def("getComponents"               , static_cast<native::VolumeDataChannelDescriptor::Components(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetComponents), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetComponents));
  VolumeDataChannelDescriptor_.def_property_readonly("components", &VolumeDataChannelDescriptor::GetComponents, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetComponents));
  VolumeDataChannelDescriptor_.def("isDiscrete"                  , static_cast<bool(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::IsDiscrete), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsDiscrete));
  VolumeDataChannelDescriptor_.def_property_readonly("discrete", &VolumeDataChannelDescriptor::IsDiscrete, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsDiscrete));
  VolumeDataChannelDescriptor_.def("isRenderable"                , static_cast<bool(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::IsRenderable), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsRenderable));
  VolumeDataChannelDescriptor_.def_property_readonly("renderable", &VolumeDataChannelDescriptor::IsRenderable, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsRenderable));
  VolumeDataChannelDescriptor_.def("isAllowLossyCompression"     , static_cast<bool(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::IsAllowLossyCompression), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsAllowLossyCompression));
  VolumeDataChannelDescriptor_.def_property_readonly("allowLossyCompression", &VolumeDataChannelDescriptor::IsAllowLossyCompression, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsAllowLossyCompression));
  VolumeDataChannelDescriptor_.def("isUseZipForLosslessCompression", static_cast<bool(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::IsUseZipForLosslessCompression), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsUseZipForLosslessCompression));
  VolumeDataChannelDescriptor_.def_property_readonly("useZipForLosslessCompression", &VolumeDataChannelDescriptor::IsUseZipForLosslessCompression, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsUseZipForLosslessCompression));
  VolumeDataChannelDescriptor_.def("getName"                     , static_cast<const char *(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetName), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetName));
  VolumeDataChannelDescriptor_.def_property_readonly("name", &VolumeDataChannelDescriptor::GetName, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetName));
  VolumeDataChannelDescriptor_.def("getUnit"                     , static_cast<const char *(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetUnit), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetUnit));
  VolumeDataChannelDescriptor_.def_property_readonly("unit", &VolumeDataChannelDescriptor::GetUnit, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetUnit));
  VolumeDataChannelDescriptor_.def("getValueRange"               , static_cast<const native::FloatRange &(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetValueRange), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetValueRange));
  VolumeDataChannelDescriptor_.def_property_readonly("valueRange", &VolumeDataChannelDescriptor::GetValueRange, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetValueRange));
  VolumeDataChannelDescriptor_.def("getValueRangeMin"            , static_cast<float(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetValueRangeMin), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetValueRangeMin));
  VolumeDataChannelDescriptor_.def_property_readonly("valueRangeMin", &VolumeDataChannelDescriptor::GetValueRangeMin, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetValueRangeMin));
  VolumeDataChannelDescriptor_.def("getValueRangeMax"            , static_cast<float(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetValueRangeMax), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetValueRangeMax));
  VolumeDataChannelDescriptor_.def_property_readonly("valueRangeMax", &VolumeDataChannelDescriptor::GetValueRangeMax, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetValueRangeMax));
  VolumeDataChannelDescriptor_.def("getMapping"                  , static_cast<native::VolumeDataMapping(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetMapping), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetMapping));
  VolumeDataChannelDescriptor_.def_property_readonly("mapping", &VolumeDataChannelDescriptor::GetMapping, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetMapping));
  VolumeDataChannelDescriptor_.def("getMappedValueCount"         , static_cast<int(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetMappedValueCount), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetMappedValueCount));
  VolumeDataChannelDescriptor_.def_property_readonly("mappedValueCount", &VolumeDataChannelDescriptor::GetMappedValueCount, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetMappedValueCount));
  VolumeDataChannelDescriptor_.def("isUseNoValue"                , static_cast<bool(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::IsUseNoValue), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsUseNoValue));
  VolumeDataChannelDescriptor_.def_property_readonly("useNoValue", &VolumeDataChannelDescriptor::IsUseNoValue, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsUseNoValue));
  VolumeDataChannelDescriptor_.def("getNoValue"                  , static_cast<float(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetNoValue), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetNoValue));
  VolumeDataChannelDescriptor_.def_property_readonly("noValue", &VolumeDataChannelDescriptor::GetNoValue, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetNoValue));
  VolumeDataChannelDescriptor_.def("getIntegerScale"             , static_cast<float(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetIntegerScale), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetIntegerScale));
  VolumeDataChannelDescriptor_.def_property_readonly("integerScale", &VolumeDataChannelDescriptor::GetIntegerScale, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetIntegerScale));
  VolumeDataChannelDescriptor_.def("getIntegerOffset"            , static_cast<float(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetIntegerOffset), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetIntegerOffset));
  VolumeDataChannelDescriptor_.def_property_readonly("integerOffset", &VolumeDataChannelDescriptor::GetIntegerOffset, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetIntegerOffset));

  py::enum_<VolumeDataChannelDescriptor::Flags> 
    VolumeDataChannelDescriptor_Flags_(VolumeDataChannelDescriptor_,"Flags", OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Flags));

  VolumeDataChannelDescriptor_Flags_.value("Default"                     , VolumeDataChannelDescriptor::Flags::Default, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Flags_Default));
  VolumeDataChannelDescriptor_Flags_.value("DiscreteData"                , VolumeDataChannelDescriptor::Flags::DiscreteData, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Flags_DiscreteData));
  VolumeDataChannelDescriptor_Flags_.value("NoLossyCompression"          , VolumeDataChannelDescriptor::Flags::NoLossyCompression, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Flags_NoLossyCompression));
  VolumeDataChannelDescriptor_Flags_.value("NotRenderable"               , VolumeDataChannelDescriptor::Flags::NotRenderable, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Flags_NotRenderable));
  VolumeDataChannelDescriptor_Flags_.value("NoLossyCompressionUseZip"    , VolumeDataChannelDescriptor::Flags::NoLossyCompressionUseZip, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Flags_NoLossyCompressionUseZip));

  py::enum_<VolumeDataChannelDescriptor::Format> 
    VolumeDataChannelDescriptor_Format_(VolumeDataChannelDescriptor_,"Format", OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format));

  VolumeDataChannelDescriptor_Format_.value("Format_Any"                  , VolumeDataChannelDescriptor::Format::Format_Any, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_Any));
  VolumeDataChannelDescriptor_Format_.value("Format_1Bit"                 , VolumeDataChannelDescriptor::Format::Format_1Bit, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_1Bit));
  VolumeDataChannelDescriptor_Format_.value("Format_U8"                   , VolumeDataChannelDescriptor::Format::Format_U8, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_U8));
  VolumeDataChannelDescriptor_Format_.value("Format_U16"                  , VolumeDataChannelDescriptor::Format::Format_U16, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_U16));
  VolumeDataChannelDescriptor_Format_.value("Format_R32"                  , VolumeDataChannelDescriptor::Format::Format_R32, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_R32));
  VolumeDataChannelDescriptor_Format_.value("Format_U32"                  , VolumeDataChannelDescriptor::Format::Format_U32, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_U32));
  VolumeDataChannelDescriptor_Format_.value("Format_R64"                  , VolumeDataChannelDescriptor::Format::Format_R64, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_R64));
  VolumeDataChannelDescriptor_Format_.value("Format_U64"                  , VolumeDataChannelDescriptor::Format::Format_U64, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_U64));

  py::enum_<VolumeDataChannelDescriptor::Components> 
    VolumeDataChannelDescriptor_Components_(VolumeDataChannelDescriptor_,"Components", OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Components));

  VolumeDataChannelDescriptor_Components_.value("Components_1"                , VolumeDataChannelDescriptor::Components::Components_1, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Components_Components_1));
  VolumeDataChannelDescriptor_Components_.value("Components_2"                , VolumeDataChannelDescriptor::Components::Components_2, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Components_Components_2));
  VolumeDataChannelDescriptor_Components_.value("Components_4"                , VolumeDataChannelDescriptor::Components::Components_4, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Components_Components_4));

  m.def("operator_bor"                , static_cast<VolumeDataChannelDescriptor::Flags(*)(VolumeDataChannelDescriptor::Flags, VolumeDataChannelDescriptor::Flags)>(&operator|), py::arg("lhs").none(false), py::arg("rhs").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(operator_bor));
//AUTOGEN-END

// IMPLEMENTED :   VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float>(), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_2));
  VolumeDataChannelDescriptor_.def(py::init([](VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, std::string name, std::string unit, float valueRangeMin, float valueRangeMax)
    {
      return VolumeDataChannelDescriptor(format, components, PyDescriptorStringContainer::Add(name), PyDescriptorStringContainer::Add(unit), valueRangeMin, valueRangeMax);
    }), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_2));
// IMPLEMENTED :   VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, native::VolumeDataMapping>(), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("mapping").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_3));
  VolumeDataChannelDescriptor_.def(py::init([](VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, std::string name, std::string unit, float valueRangeMin, float valueRangeMax, VolumeDataMapping mapping)
    {
      return VolumeDataChannelDescriptor(format, components, PyDescriptorStringContainer::Add(name), PyDescriptorStringContainer::Add(unit), valueRangeMin, valueRangeMax, mapping);
    }), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("mapping").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_3));
// IMPLEMENTED :   VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, VolumeDataChannelDescriptor::Flags>(), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("flags").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_4));
  VolumeDataChannelDescriptor_.def(py::init([](VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, std::string name, std::string unit, float valueRangeMin, float valueRangeMax, enum VolumeDataChannelDescriptor::Flags flags)
    {
      return VolumeDataChannelDescriptor(format, components, PyDescriptorStringContainer::Add(name), PyDescriptorStringContainer::Add(unit), valueRangeMin, valueRangeMax, flags);
    }), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("flags").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_4));
// IMPLEMENTED :   VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, native::VolumeDataMapping, VolumeDataChannelDescriptor::Flags>(), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("mapping").none(false), py::arg("flags").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_5));
  VolumeDataChannelDescriptor_.def(py::init([](VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, std::string name, std::string unit, float valueRangeMin, float valueRangeMax, VolumeDataMapping mapping, enum VolumeDataChannelDescriptor::Flags flags)
    {
      return VolumeDataChannelDescriptor(format, components, PyDescriptorStringContainer::Add(name), PyDescriptorStringContainer::Add(unit), valueRangeMin, valueRangeMax, mapping, flags);
    }), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("mapping").none(false), py::arg("flags").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_5));
// IMPLEMENTED :   VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, native::VolumeDataMapping, int, VolumeDataChannelDescriptor::Flags, float, float>(), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("mapping").none(false), py::arg("mappedValueCount").none(false), py::arg("flags").none(false), py::arg("integerScale").none(false), py::arg("integerOffset").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_6));
  VolumeDataChannelDescriptor_.def(py::init([](VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, std::string name, std::string unit, float valueRangeMin, float valueRangeMax, VolumeDataMapping mapping, int mappedValueCount, enum VolumeDataChannelDescriptor::Flags flags, float integerScale, float integerOffset)
    {
      return VolumeDataChannelDescriptor(format, components, PyDescriptorStringContainer::Add(name), PyDescriptorStringContainer::Add(unit), valueRangeMin, valueRangeMax, mapping, mappedValueCount, flags, integerScale, integerOffset);
    }), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("mapping").none(false), py::arg("mappedValueCount").none(false), py::arg("flags").none(false), py::arg("integerScale").none(false), py::arg("integerOffset").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_6));
// IMPLEMENTED :   VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, float>(), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("noValue").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_7));
  VolumeDataChannelDescriptor_.def(py::init([](VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, std::string name, std::string unit, float valueRangeMin, float valueRangeMax, float noValue)
    {
      return VolumeDataChannelDescriptor(format, components, PyDescriptorStringContainer::Add(name), PyDescriptorStringContainer::Add(unit), valueRangeMin, valueRangeMax, noValue);
    }), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("noValue").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_7));
// IMPLEMENTED :   VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, float, native::VolumeDataMapping, VolumeDataChannelDescriptor::Flags>(), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("noValue").none(false), py::arg("mapping").none(false), py::arg("flags").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_8));
  VolumeDataChannelDescriptor_.def(py::init([](VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, std::string name, std::string unit, float valueRangeMin, float valueRangeMax, float noValue, VolumeDataMapping mapping, enum VolumeDataChannelDescriptor::Flags flags)
    {
      return VolumeDataChannelDescriptor(format, components, PyDescriptorStringContainer::Add(name), PyDescriptorStringContainer::Add(unit), valueRangeMin, valueRangeMax, noValue, mapping, flags);
    }), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("noValue").none(false), py::arg("mapping").none(false), py::arg("flags").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_8));
// IMPLEMENTED :   VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, native::VolumeDataMapping, int, VolumeDataChannelDescriptor::Flags, float, float, float>(), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("mapping").none(false), py::arg("mappedValueCount").none(false), py::arg("flags").none(false), py::arg("noValue").none(false), py::arg("integerScale").none(false), py::arg("integerOffset").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_9));
  VolumeDataChannelDescriptor_.def(py::init([](VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, std::string name, std::string unit, float valueRangeMin, float valueRangeMax, VolumeDataMapping mapping, int mappedValueCount, enum VolumeDataChannelDescriptor::Flags flags, float noValue, float integerScale, float integerOffset)
    {
      return VolumeDataChannelDescriptor(format, components, PyDescriptorStringContainer::Add(name), PyDescriptorStringContainer::Add(unit), valueRangeMin, valueRangeMax, mapping, mappedValueCount, flags, noValue, integerScale, integerOffset);
    }), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("mapping").none(false), py::arg("mappedValueCount").none(false), py::arg("flags").none(false), py::arg("noValue").none(false), py::arg("integerScale").none(false), py::arg("integerOffset").none(false), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_9));
// IMPLEMENTED :   VolumeDataChannelDescriptor_.def_static("traceMappedVolumeDataChannelDescriptor", static_cast<native::VolumeDataChannelDescriptor(*)(native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, int, VolumeDataChannelDescriptor::Flags)>(&VolumeDataChannelDescriptor::TraceMappedVolumeDataChannelDescriptor), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("mappedValueCount").none(false), py::arg("flags").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_TraceMappedVolumeDataChannelDescriptor));
  VolumeDataChannelDescriptor_.def_static("traceMappedVolumeDataChannelDescriptor", [](VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, std::string name, std::string unit, float valueRangeMin, float valueRangeMax, int mappedValueCount, enum VolumeDataChannelDescriptor::VolumeDataChannelDescriptor::Flags flags)
    {
      return VolumeDataChannelDescriptor::TraceMappedVolumeDataChannelDescriptor(format, components, PyDescriptorStringContainer::Add(name), PyDescriptorStringContainer::Add(unit), valueRangeMin, valueRangeMax, mappedValueCount, flags);
    }, py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("mappedValueCount").none(false), py::arg("flags").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_TraceMappedVolumeDataChannelDescriptor));
// IMPLEMENTED :   VolumeDataChannelDescriptor_.def_static("traceMappedVolumeDataChannelDescriptor", static_cast<native::VolumeDataChannelDescriptor(*)(native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, int, VolumeDataChannelDescriptor::Flags, float)>(&VolumeDataChannelDescriptor::TraceMappedVolumeDataChannelDescriptor), py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("mappedValueCount").none(false), py::arg("flags").none(false), py::arg("noValue").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_TraceMappedVolumeDataChannelDescriptor_2));
  VolumeDataChannelDescriptor_.def_static("traceMappedVolumeDataChannelDescriptor", [](VolumeDataChannelDescriptor::Format format, VolumeDataChannelDescriptor::Components components, std::string name, std::string unit, float valueRangeMin, float valueRangeMax, int mappedValueCount, enum VolumeDataChannelDescriptor::Flags flags, float noValue)
    {
      return VolumeDataChannelDescriptor::TraceMappedVolumeDataChannelDescriptor(format, components, PyDescriptorStringContainer::Add(name), PyDescriptorStringContainer::Add(unit), valueRangeMin, valueRangeMax, mappedValueCount, flags, noValue);
    }, py::arg("format").none(false), py::arg("components").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("valueRangeMin").none(false), py::arg("valueRangeMax").none(false), py::arg("mappedValueCount").none(false), py::arg("flags").none(false), py::arg("noValue").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_TraceMappedVolumeDataChannelDescriptor_2));
}
