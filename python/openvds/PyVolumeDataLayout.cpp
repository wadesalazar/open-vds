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

void 
PyVolumeDataLayout::initModule(py::module& m)
{
#if 0
//AUTOGEN-BEGIN
  // VolumeDataLayout
  py::class_<native::VolumeDataLayout, native::MetadataReadAccess> 
    VolumeDataLayout_(m,"VolumeDataLayout", OPENVDS_DOCSTRING(VolumeDataLayout));

  VolumeDataLayout_.def("getContentsHash"             , static_cast<uint64_t(*)()>(&native::GetContentsHash), OPENVDS_DOCSTRING(VolumeDataLayout_GetContentsHash));
  VolumeDataLayout_.def("getDimensionality"           , static_cast<int(*)()>(&native::GetDimensionality), OPENVDS_DOCSTRING(VolumeDataLayout_GetDimensionality));
  VolumeDataLayout_.def("getChannelCount"             , static_cast<int(*)()>(&native::GetChannelCount), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelCount));
  VolumeDataLayout_.def("isChannelAvailable"          , static_cast<bool(*)(const char *)>(&native::IsChannelAvailable), OPENVDS_DOCSTRING(VolumeDataLayout_IsChannelAvailable));
  VolumeDataLayout_.def("getChannelIndex"             , static_cast<int(*)(const char *)>(&native::GetChannelIndex), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelIndex));
  VolumeDataLayout_.def("getChannelDescriptor"        , static_cast<OpenVDS::VolumeDataChannelDescriptor(*)(int)>(&native::GetChannelDescriptor), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelDescriptor));
  VolumeDataLayout_.def("getAxisDescriptor"           , static_cast<OpenVDS::VolumeDataAxisDescriptor(*)(int)>(&native::GetAxisDescriptor), OPENVDS_DOCSTRING(VolumeDataLayout_GetAxisDescriptor));
  VolumeDataLayout_.def("getChannelFormat"            , static_cast<VolumeDataChannelDescriptor::Format(*)(int)>(&native::GetChannelFormat), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelFormat));
  VolumeDataLayout_.def("getChannelComponents"        , static_cast<VolumeDataChannelDescriptor::Components(*)(int)>(&native::GetChannelComponents), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelComponents));
  VolumeDataLayout_.def("getChannelName"              , static_cast<const char *(*)(int)>(&native::GetChannelName), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelName));
  VolumeDataLayout_.def("getChannelUnit"              , static_cast<const char *(*)(int)>(&native::GetChannelUnit), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelUnit));
  VolumeDataLayout_.def("getChannelValueRangeMin"     , static_cast<float(*)(int)>(&native::GetChannelValueRangeMin), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelValueRangeMin));
  VolumeDataLayout_.def("getChannelValueRangeMax"     , static_cast<float(*)(int)>(&native::GetChannelValueRangeMax), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelValueRangeMax));
  VolumeDataLayout_.def("isChannelDiscrete"           , static_cast<bool(*)(int)>(&native::IsChannelDiscrete), OPENVDS_DOCSTRING(VolumeDataLayout_IsChannelDiscrete));
  VolumeDataLayout_.def("isChannelRenderable"         , static_cast<bool(*)(int)>(&native::IsChannelRenderable), OPENVDS_DOCSTRING(VolumeDataLayout_IsChannelRenderable));
  VolumeDataLayout_.def("isChannelAllowingLossyCompression", static_cast<bool(*)(int)>(&native::IsChannelAllowingLossyCompression), OPENVDS_DOCSTRING(VolumeDataLayout_IsChannelAllowingLossyCompression));
  VolumeDataLayout_.def("isChannelUseZipForLosslessCompression", static_cast<bool(*)(int)>(&native::IsChannelUseZipForLosslessCompression), OPENVDS_DOCSTRING(VolumeDataLayout_IsChannelUseZipForLosslessCompression));
  VolumeDataLayout_.def("getChannelMapping"           , static_cast<OpenVDS::VolumeDataMapping(*)(int)>(&native::GetChannelMapping), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelMapping));
  VolumeDataLayout_.def("getDimensionNumSamples"      , static_cast<int(*)(int)>(&native::GetDimensionNumSamples), OPENVDS_DOCSTRING(VolumeDataLayout_GetDimensionNumSamples));
  VolumeDataLayout_.def("getDimensionName"            , static_cast<const char *(*)(int)>(&native::GetDimensionName), OPENVDS_DOCSTRING(VolumeDataLayout_GetDimensionName));
  VolumeDataLayout_.def("getDimensionUnit"            , static_cast<const char *(*)(int)>(&native::GetDimensionUnit), OPENVDS_DOCSTRING(VolumeDataLayout_GetDimensionUnit));
  VolumeDataLayout_.def("getDimensionMin"             , static_cast<float(*)(int)>(&native::GetDimensionMin), OPENVDS_DOCSTRING(VolumeDataLayout_GetDimensionMin));
  VolumeDataLayout_.def("getDimensionMax"             , static_cast<float(*)(int)>(&native::GetDimensionMax), OPENVDS_DOCSTRING(VolumeDataLayout_GetDimensionMax));
  VolumeDataLayout_.def("isChannelUseNoValue"         , static_cast<bool(*)(int)>(&native::IsChannelUseNoValue), OPENVDS_DOCSTRING(VolumeDataLayout_IsChannelUseNoValue));
  VolumeDataLayout_.def("getChannelNoValue"           , static_cast<float(*)(int)>(&native::GetChannelNoValue), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelNoValue));
  VolumeDataLayout_.def("getChannelIntegerScale"      , static_cast<float(*)(int)>(&native::GetChannelIntegerScale), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelIntegerScale));
  VolumeDataLayout_.def("getChannelIntegerOffset"     , static_cast<float(*)(int)>(&native::GetChannelIntegerOffset), OPENVDS_DOCSTRING(VolumeDataLayout_GetChannelIntegerOffset));

//AUTOGEN-END
#endif
}

