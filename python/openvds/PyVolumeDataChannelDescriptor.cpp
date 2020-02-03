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
  py::class_<VolumeDataChannelDescriptor, std::unique_ptr<VolumeDataChannelDescriptor>> 
    VolumeDataChannelDescriptor_(m,"VolumeDataChannelDescriptor", OPENVDS_DOCSTRING(VolumeDataChannelDescriptor));

  VolumeDataChannelDescriptor_.def(py::init<                              >(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor));
  VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float>(), py::arg("format"), py::arg("components"), py::arg("pName"), py::arg("pUnit"), py::arg("valueRangeMin"), py::arg("valueRangeMax"), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_2));
  VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, native::VolumeDataMapping>(), py::arg("format"), py::arg("components"), py::arg("pName"), py::arg("pUnit"), py::arg("valueRangeMin"), py::arg("valueRangeMax"), py::arg("mapping"), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_3));
  VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, VolumeDataChannelDescriptor::Flags>(), py::arg("format"), py::arg("components"), py::arg("pName"), py::arg("pUnit"), py::arg("valueRangeMin"), py::arg("valueRangeMax"), py::arg("flags"), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_4));
  VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, native::VolumeDataMapping, VolumeDataChannelDescriptor::Flags>(), py::arg("format"), py::arg("components"), py::arg("pName"), py::arg("pUnit"), py::arg("valueRangeMin"), py::arg("valueRangeMax"), py::arg("mapping"), py::arg("flags"), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_5));
  VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, native::VolumeDataMapping, int, VolumeDataChannelDescriptor::Flags, float, float>(), py::arg("format"), py::arg("components"), py::arg("pName"), py::arg("pUnit"), py::arg("valueRangeMin"), py::arg("valueRangeMax"), py::arg("mapping"), py::arg("mappedValueCount"), py::arg("flags"), py::arg("integerScale"), py::arg("integerOffset"), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_6));
  VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, float>(), py::arg("format"), py::arg("components"), py::arg("pName"), py::arg("pUnit"), py::arg("valueRangeMin"), py::arg("valueRangeMax"), py::arg("noValue"), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_7));
  VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, float, native::VolumeDataMapping, VolumeDataChannelDescriptor::Flags>(), py::arg("format"), py::arg("components"), py::arg("pName"), py::arg("pUnit"), py::arg("valueRangeMin"), py::arg("valueRangeMax"), py::arg("noValue"), py::arg("mapping"), py::arg("flags"), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_8));
  VolumeDataChannelDescriptor_.def(py::init<native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, native::VolumeDataMapping, int, VolumeDataChannelDescriptor::Flags, float, float, float>(), py::arg("format"), py::arg("components"), py::arg("name"), py::arg("unit"), py::arg("valueRangeMin"), py::arg("valueRangeMax"), py::arg("mapping"), py::arg("mappedValueCount"), py::arg("flags"), py::arg("noValue"), py::arg("integerScale"), py::arg("integerOffset"), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_9));
  VolumeDataChannelDescriptor_.def("getFormat"                   , static_cast<native::VolumeDataChannelDescriptor::Format(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetFormat), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetFormat));
  VolumeDataChannelDescriptor_.def("getComponents"               , static_cast<native::VolumeDataChannelDescriptor::Components(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetComponents), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetComponents));
  VolumeDataChannelDescriptor_.def("isDiscrete"                  , static_cast<bool(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::IsDiscrete), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsDiscrete));
  VolumeDataChannelDescriptor_.def("isRenderable"                , static_cast<bool(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::IsRenderable), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsRenderable));
  VolumeDataChannelDescriptor_.def("isAllowLossyCompression"     , static_cast<bool(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::IsAllowLossyCompression), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsAllowLossyCompression));
  VolumeDataChannelDescriptor_.def("isUseZipForLosslessCompression", static_cast<bool(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::IsUseZipForLosslessCompression), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsUseZipForLosslessCompression));
  VolumeDataChannelDescriptor_.def("getName"                     , static_cast<const char *(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetName), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetName));
  VolumeDataChannelDescriptor_.def("getUnit"                     , static_cast<const char *(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetUnit), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetUnit));
  VolumeDataChannelDescriptor_.def("getValueRange"               , static_cast<const native::FloatRange &(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetValueRange), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetValueRange));
  VolumeDataChannelDescriptor_.def("getValueRangeMin"            , static_cast<float(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetValueRangeMin), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetValueRangeMin));
  VolumeDataChannelDescriptor_.def("getValueRangeMax"            , static_cast<float(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetValueRangeMax), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetValueRangeMax));
  VolumeDataChannelDescriptor_.def("getMapping"                  , static_cast<native::VolumeDataMapping(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetMapping), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetMapping));
  VolumeDataChannelDescriptor_.def("getMappedValueCount"         , static_cast<int(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetMappedValueCount), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetMappedValueCount));
  VolumeDataChannelDescriptor_.def("isUseNoValue"                , static_cast<bool(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::IsUseNoValue), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsUseNoValue));
  VolumeDataChannelDescriptor_.def("getNoValue"                  , static_cast<float(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetNoValue), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetNoValue));
  VolumeDataChannelDescriptor_.def("getIntegerScale"             , static_cast<float(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetIntegerScale), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetIntegerScale));
  VolumeDataChannelDescriptor_.def("getIntegerOffset"            , static_cast<float(VolumeDataChannelDescriptor::*)() const>(&VolumeDataChannelDescriptor::GetIntegerOffset), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetIntegerOffset));
  VolumeDataChannelDescriptor_.def_static("traceMappedVolumeDataChannelDescriptor", static_cast<native::VolumeDataChannelDescriptor(*)(native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, int, VolumeDataChannelDescriptor::Flags)>(&VolumeDataChannelDescriptor::TraceMappedVolumeDataChannelDescriptor), py::arg("format"), py::arg("components"), py::arg("pName"), py::arg("pUnit"), py::arg("valueRangeMin"), py::arg("valueRangeMax"), py::arg("mappedValueCount"), py::arg("flags"), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_TraceMappedVolumeDataChannelDescriptor));
  VolumeDataChannelDescriptor_.def_static("traceMappedVolumeDataChannelDescriptor", static_cast<native::VolumeDataChannelDescriptor(*)(native::VolumeDataChannelDescriptor::Format, native::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, int, VolumeDataChannelDescriptor::Flags, float)>(&VolumeDataChannelDescriptor::TraceMappedVolumeDataChannelDescriptor), py::arg("format"), py::arg("components"), py::arg("pName"), py::arg("pUnit"), py::arg("valueRangeMin"), py::arg("valueRangeMax"), py::arg("mappedValueCount"), py::arg("flags"), py::arg("noValue"), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_TraceMappedVolumeDataChannelDescriptor_2));

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

  m.def("operator_bor"                , static_cast<VolumeDataChannelDescriptor::Flags(*)(VolumeDataChannelDescriptor::Flags, VolumeDataChannelDescriptor::Flags)>(&operator|), py::arg("lhs"), py::arg("rhs"), OPENVDS_DOCSTRING(operator_bor));
//AUTOGEN-END
}

