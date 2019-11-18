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

#include "PyVolumeDataLayout.h"

using namespace native;

void 
PyVolumeDataLayout::initModule(py::module& m)
{
//AUTOGEN-BEGIN
  // VolumeDataLayout
  py::class_<VolumeDataLayout, MetadataReadAccess, std::unique_ptr<VolumeDataLayout, py::nodelete>> 
    VolumeDataLayout_(m,"VolumeDataLayout", OPENVDS_DOCSTRING(VolumeDataLayout));

  VolumeDataLayout_.def("getContentsHash"             , static_cast<uint64_t(VolumeDataLayout::*)() const>(&VolumeDataLayout::GetContentsHash), OPENVDS_DOCSTRING(VolumeDataLayout_GetContentsHash));
  VolumeDataLayout_.def("getDimensionality"           , static_cast<int(VolumeDataLayout::*)() const>(&VolumeDataLayout::GetDimensionality), OPENVDS_DOCSTRING(VolumeDataLayout_GetDimensionality));
  VolumeDataLayout_.def("getChannelCount"             , static_cast<int(VolumeDataLayout::*)() const>(&VolumeDataLayout::GetChannelCount), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelCount));
  VolumeDataLayout_.def("isChannelAvailable"          , static_cast<bool(VolumeDataLayout::*)(const char *) const>(&VolumeDataLayout::IsChannelAvailable), OPENVDS_DOCSTRING(VolumeDataLayout_IsChannelAvailable));
  VolumeDataLayout_.def("getChannelIndex"             , static_cast<int(VolumeDataLayout::*)(const char *) const>(&VolumeDataLayout::GetChannelIndex), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelIndex));
  VolumeDataLayout_.def("getChannelDescriptor"        , static_cast<native::VolumeDataChannelDescriptor(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetChannelDescriptor), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelDescriptor));
  VolumeDataLayout_.def("getAxisDescriptor"           , static_cast<native::VolumeDataAxisDescriptor(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetAxisDescriptor), OPENVDS_DOCSTRING(VolumeDataLayout_GetAxisDescriptor));
  VolumeDataLayout_.def("getChannelFormat"            , static_cast<VolumeDataChannelDescriptor::Format(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetChannelFormat), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelFormat));
  VolumeDataLayout_.def("getChannelComponents"        , static_cast<VolumeDataChannelDescriptor::Components(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetChannelComponents), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelComponents));
  VolumeDataLayout_.def("getChannelName"              , static_cast<const char *(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetChannelName), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelName));
  VolumeDataLayout_.def("getChannelUnit"              , static_cast<const char *(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetChannelUnit), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelUnit));
  VolumeDataLayout_.def("getChannelValueRangeMin"     , static_cast<float(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetChannelValueRangeMin), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelValueRangeMin));
  VolumeDataLayout_.def("getChannelValueRangeMax"     , static_cast<float(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetChannelValueRangeMax), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelValueRangeMax));
  VolumeDataLayout_.def("isChannelDiscrete"           , static_cast<bool(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::IsChannelDiscrete), OPENVDS_DOCSTRING(VolumeDataLayout_IsChannelDiscrete));
  VolumeDataLayout_.def("isChannelRenderable"         , static_cast<bool(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::IsChannelRenderable), OPENVDS_DOCSTRING(VolumeDataLayout_IsChannelRenderable));
  VolumeDataLayout_.def("isChannelAllowingLossyCompression", static_cast<bool(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::IsChannelAllowingLossyCompression), OPENVDS_DOCSTRING(VolumeDataLayout_IsChannelAllowingLossyCompression));
  VolumeDataLayout_.def("isChannelUseZipForLosslessCompression", static_cast<bool(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::IsChannelUseZipForLosslessCompression), OPENVDS_DOCSTRING(VolumeDataLayout_IsChannelUseZipForLosslessCompression));
  VolumeDataLayout_.def("getChannelMapping"           , static_cast<native::VolumeDataMapping(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetChannelMapping), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelMapping));
  VolumeDataLayout_.def("getDimensionNumSamples"      , static_cast<int(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetDimensionNumSamples), OPENVDS_DOCSTRING(VolumeDataLayout_GetDimensionNumSamples));
  VolumeDataLayout_.def("getDimensionName"            , static_cast<const char *(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetDimensionName), OPENVDS_DOCSTRING(VolumeDataLayout_GetDimensionName));
  VolumeDataLayout_.def("getDimensionUnit"            , static_cast<const char *(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetDimensionUnit), OPENVDS_DOCSTRING(VolumeDataLayout_GetDimensionUnit));
  VolumeDataLayout_.def("getDimensionMin"             , static_cast<float(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetDimensionMin), OPENVDS_DOCSTRING(VolumeDataLayout_GetDimensionMin));
  VolumeDataLayout_.def("getDimensionMax"             , static_cast<float(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetDimensionMax), OPENVDS_DOCSTRING(VolumeDataLayout_GetDimensionMax));
  VolumeDataLayout_.def("isChannelUseNoValue"         , static_cast<bool(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::IsChannelUseNoValue), OPENVDS_DOCSTRING(VolumeDataLayout_IsChannelUseNoValue));
  VolumeDataLayout_.def("getChannelNoValue"           , static_cast<float(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetChannelNoValue), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelNoValue));
  VolumeDataLayout_.def("getChannelIntegerScale"      , static_cast<float(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetChannelIntegerScale), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelIntegerScale));
  VolumeDataLayout_.def("getChannelIntegerOffset"     , static_cast<float(VolumeDataLayout::*)(int) const>(&VolumeDataLayout::GetChannelIntegerOffset), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelIntegerOffset));

//AUTOGEN-END
}

