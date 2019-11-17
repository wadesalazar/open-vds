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

void 
PyVolumeDataChannelDescriptor::initModule(py::module& m)
{
#if 0
//AUTOGEN-BEGIN
  // VolumeDataChannelDescriptor
  py::class_<native::VolumeDataChannelDescriptor> 
    VolumeDataChannelDescriptor_(m,"VolumeDataChannelDescriptor", OPENVDS_DOCSTRING(VolumeDataChannelDescriptor));

  VolumeDataChannelDescriptor_.def(py::init<                              >(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor));
  VolumeDataChannelDescriptor_.def(py::init<OpenVDS::VolumeDataChannelDescriptor::Format, OpenVDS::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_2));
  VolumeDataChannelDescriptor_.def(py::init<OpenVDS::VolumeDataChannelDescriptor::Format, OpenVDS::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, OpenVDS::VolumeDataMapping>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_3));
  VolumeDataChannelDescriptor_.def(py::init<OpenVDS::VolumeDataChannelDescriptor::Format, OpenVDS::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, enum Flags>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_4));
  VolumeDataChannelDescriptor_.def(py::init<OpenVDS::VolumeDataChannelDescriptor::Format, OpenVDS::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, OpenVDS::VolumeDataMapping, enum Flags>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_5));
  VolumeDataChannelDescriptor_.def(py::init<OpenVDS::VolumeDataChannelDescriptor::Format, OpenVDS::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, OpenVDS::VolumeDataMapping, int, enum Flags, float, float>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_6));
  VolumeDataChannelDescriptor_.def(py::init<OpenVDS::VolumeDataChannelDescriptor::Format, OpenVDS::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, float>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_7));
  VolumeDataChannelDescriptor_.def(py::init<OpenVDS::VolumeDataChannelDescriptor::Format, OpenVDS::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, float, OpenVDS::VolumeDataMapping, enum Flags>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_8));
  VolumeDataChannelDescriptor_.def(py::init<OpenVDS::VolumeDataChannelDescriptor::Format, OpenVDS::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, OpenVDS::VolumeDataMapping, int, enum Flags, float, float, float>(), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_9));
  VolumeDataChannelDescriptor_.def("getFormat"                   , static_cast<OpenVDS::VolumeDataChannelDescriptor::Format(*)()>(&native::GetFormat), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetFormat));
  VolumeDataChannelDescriptor_.def("getComponents"               , static_cast<OpenVDS::VolumeDataChannelDescriptor::Components(*)()>(&native::GetComponents), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetComponents));
  VolumeDataChannelDescriptor_.def("isDiscrete"                  , static_cast<bool(*)()>(&native::IsDiscrete), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsDiscrete));
  VolumeDataChannelDescriptor_.def("isRenderable"                , static_cast<bool(*)()>(&native::IsRenderable), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsRenderable));
  VolumeDataChannelDescriptor_.def("isAllowLossyCompression"     , static_cast<bool(*)()>(&native::IsAllowLossyCompression), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsAllowLossyCompression));
  VolumeDataChannelDescriptor_.def("isUseZipForLosslessCompression", static_cast<bool(*)()>(&native::IsUseZipForLosslessCompression), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsUseZipForLosslessCompression));
  VolumeDataChannelDescriptor_.def("getName"                     , static_cast<const char *(*)()>(&native::GetName), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetName));
  VolumeDataChannelDescriptor_.def("getUnit"                     , static_cast<const char *(*)()>(&native::GetUnit), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetUnit));
  VolumeDataChannelDescriptor_.def("getValueRange"               , static_cast<const OpenVDS::FloatRange &(*)()>(&native::GetValueRange), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetValueRange));
  VolumeDataChannelDescriptor_.def("getValueRangeMin"            , static_cast<float(*)()>(&native::GetValueRangeMin), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetValueRangeMin));
  VolumeDataChannelDescriptor_.def("getValueRangeMax"            , static_cast<float(*)()>(&native::GetValueRangeMax), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetValueRangeMax));
  VolumeDataChannelDescriptor_.def("getMapping"                  , static_cast<OpenVDS::VolumeDataMapping(*)()>(&native::GetMapping), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetMapping));
  VolumeDataChannelDescriptor_.def("getMappedValueCount"         , static_cast<int(*)()>(&native::GetMappedValueCount), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetMappedValueCount));
  VolumeDataChannelDescriptor_.def("isUseNoValue"                , static_cast<bool(*)()>(&native::IsUseNoValue), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_IsUseNoValue));
  VolumeDataChannelDescriptor_.def("getNoValue"                  , static_cast<float(*)()>(&native::GetNoValue), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetNoValue));
  VolumeDataChannelDescriptor_.def("getIntegerScale"             , static_cast<float(*)()>(&native::GetIntegerScale), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetIntegerScale));
  VolumeDataChannelDescriptor_.def("getIntegerOffset"            , static_cast<float(*)()>(&native::GetIntegerOffset), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_GetIntegerOffset));
  VolumeDataChannelDescriptor_.def("traceMappedVolumeDataChannelDescriptor", static_cast<OpenVDS::VolumeDataChannelDescriptor(*)(OpenVDS::VolumeDataChannelDescriptor::Format, OpenVDS::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, int, enum Flags)>(&native::TraceMappedVolumeDataChannelDescriptor), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_TraceMappedVolumeDataChannelDescriptor));
  VolumeDataChannelDescriptor_.def("traceMappedVolumeDataChannelDescriptor", static_cast<OpenVDS::VolumeDataChannelDescriptor(*)(OpenVDS::VolumeDataChannelDescriptor::Format, OpenVDS::VolumeDataChannelDescriptor::Components, const char *, const char *, float, float, int, enum Flags, float)>(&native::TraceMappedVolumeDataChannelDescriptor), OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_TraceMappedVolumeDataChannelDescriptor_2));

  py::enum_<native::VolumeDataChannelDescriptor::Flags> 
    VolumeDataChannelDescriptor_Flags_(VolumeDataChannelDescriptor_,"Flags", OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Flags));

  VolumeDataChannelDescriptor_Flags_.value("Default"                     , native::VolumeDataChannelDescriptor::Flags::Default, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Flags_Default));
  VolumeDataChannelDescriptor_Flags_.value("DiscreteData"                , native::VolumeDataChannelDescriptor::Flags::DiscreteData, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Flags_DiscreteData));
  VolumeDataChannelDescriptor_Flags_.value("NoLossyCompression"          , native::VolumeDataChannelDescriptor::Flags::NoLossyCompression, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Flags_NoLossyCompression));
  VolumeDataChannelDescriptor_Flags_.value("NotRenderable"               , native::VolumeDataChannelDescriptor::Flags::NotRenderable, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Flags_NotRenderable));
  VolumeDataChannelDescriptor_Flags_.value("NoLossyCompressionUseZip"    , native::VolumeDataChannelDescriptor::Flags::NoLossyCompressionUseZip, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Flags_NoLossyCompressionUseZip));

  py::enum_<native::VolumeDataChannelDescriptor::Format> 
    VolumeDataChannelDescriptor_Format_(VolumeDataChannelDescriptor_,"Format", OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format));

  VolumeDataChannelDescriptor_Format_.value("Format_Any"                  , native::VolumeDataChannelDescriptor::Format::Format_Any, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_Any));
  VolumeDataChannelDescriptor_Format_.value("Format_1Bit"                 , native::VolumeDataChannelDescriptor::Format::Format_1Bit, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_1Bit));
  VolumeDataChannelDescriptor_Format_.value("Format_U8"                   , native::VolumeDataChannelDescriptor::Format::Format_U8, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_U8));
  VolumeDataChannelDescriptor_Format_.value("Format_U16"                  , native::VolumeDataChannelDescriptor::Format::Format_U16, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_U16));
  VolumeDataChannelDescriptor_Format_.value("Format_R32"                  , native::VolumeDataChannelDescriptor::Format::Format_R32, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_R32));
  VolumeDataChannelDescriptor_Format_.value("Format_U32"                  , native::VolumeDataChannelDescriptor::Format::Format_U32, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_U32));
  VolumeDataChannelDescriptor_Format_.value("Format_R64"                  , native::VolumeDataChannelDescriptor::Format::Format_R64, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_R64));
  VolumeDataChannelDescriptor_Format_.value("Format_U64"                  , native::VolumeDataChannelDescriptor::Format::Format_U64, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Format_Format_U64));

  py::enum_<native::VolumeDataChannelDescriptor::Components> 
    VolumeDataChannelDescriptor_Components_(VolumeDataChannelDescriptor_,"Components", OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Components));

  VolumeDataChannelDescriptor_Components_.value("Components_1"                , native::VolumeDataChannelDescriptor::Components::Components_1, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Components_Components_1));
  VolumeDataChannelDescriptor_Components_.value("Components_2"                , native::VolumeDataChannelDescriptor::Components::Components_2, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Components_Components_2));
  VolumeDataChannelDescriptor_Components_.value("Components_4"                , native::VolumeDataChannelDescriptor::Components::Components_4, OPENVDS_DOCSTRING(VolumeDataChannelDescriptor_Components_Components_4));

  m.def("operator|"                   , static_cast<VolumeDataChannelDescriptor::Flags(*)(VolumeDataChannelDescriptor::Flags, VolumeDataChannelDescriptor::Flags)>(&native::operator|), OPENVDS_DOCSTRING(operator|));
//AUTOGEN-END
#endif
}

