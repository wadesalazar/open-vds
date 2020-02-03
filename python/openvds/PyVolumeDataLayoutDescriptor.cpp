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

#include "PyVolumeDataLayoutDescriptor.h"

using namespace native;

void 
PyVolumeDataLayoutDescriptor::initModule(py::module& m)
{
//AUTOGEN-BEGIN
  // VolumeDataLayoutDescriptor
  py::class_<VolumeDataLayoutDescriptor, std::unique_ptr<VolumeDataLayoutDescriptor>> 
    VolumeDataLayoutDescriptor_(m,"VolumeDataLayoutDescriptor", OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor));

  VolumeDataLayoutDescriptor_.def(py::init<                              >(), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_VolumeDataLayoutDescriptor));
  VolumeDataLayoutDescriptor_.def(py::init<native::VolumeDataLayoutDescriptor::BrickSize, int, int, int, native::VolumeDataLayoutDescriptor::LODLevels, native::VolumeDataLayoutDescriptor::Options, int>(), py::arg("brickSize"), py::arg("negativeMargin"), py::arg("positiveMargin"), py::arg("brickSize2DMultiplier"), py::arg("lodLevels"), py::arg("options"), py::arg("fullResolutionDimension"), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_VolumeDataLayoutDescriptor_2));
  VolumeDataLayoutDescriptor_.def("isValid"                     , static_cast<bool(VolumeDataLayoutDescriptor::*)() const>(&VolumeDataLayoutDescriptor::IsValid), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_IsValid));
  VolumeDataLayoutDescriptor_.def("getBrickSize"                , static_cast<native::VolumeDataLayoutDescriptor::BrickSize(VolumeDataLayoutDescriptor::*)() const>(&VolumeDataLayoutDescriptor::GetBrickSize), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_GetBrickSize));
  VolumeDataLayoutDescriptor_.def("getNegativeMargin"           , static_cast<int(VolumeDataLayoutDescriptor::*)() const>(&VolumeDataLayoutDescriptor::GetNegativeMargin), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_GetNegativeMargin));
  VolumeDataLayoutDescriptor_.def("getPositiveMargin"           , static_cast<int(VolumeDataLayoutDescriptor::*)() const>(&VolumeDataLayoutDescriptor::GetPositiveMargin), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_GetPositiveMargin));
  VolumeDataLayoutDescriptor_.def("getBrickSizeMultiplier2D"    , static_cast<int(VolumeDataLayoutDescriptor::*)() const>(&VolumeDataLayoutDescriptor::GetBrickSizeMultiplier2D), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_GetBrickSizeMultiplier2D));
  VolumeDataLayoutDescriptor_.def("getLODLevels"                , static_cast<native::VolumeDataLayoutDescriptor::LODLevels(VolumeDataLayoutDescriptor::*)() const>(&VolumeDataLayoutDescriptor::GetLODLevels), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_GetLODLevels));
  VolumeDataLayoutDescriptor_.def("isCreate2DLODs"              , static_cast<bool(VolumeDataLayoutDescriptor::*)() const>(&VolumeDataLayoutDescriptor::IsCreate2DLODs), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_IsCreate2DLODs));
  VolumeDataLayoutDescriptor_.def("isForceFullResolutionDimension", static_cast<bool(VolumeDataLayoutDescriptor::*)() const>(&VolumeDataLayoutDescriptor::IsForceFullResolutionDimension), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_IsForceFullResolutionDimension));
  VolumeDataLayoutDescriptor_.def("getFullResolutionDimension"  , static_cast<int(VolumeDataLayoutDescriptor::*)() const>(&VolumeDataLayoutDescriptor::GetFullResolutionDimension), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_GetFullResolutionDimension));

  py::enum_<VolumeDataLayoutDescriptor::BrickSize> 
    VolumeDataLayoutDescriptor_BrickSize_(VolumeDataLayoutDescriptor_,"BrickSize", OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize));

  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_32"                , VolumeDataLayoutDescriptor::BrickSize::BrickSize_32, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_32));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_64"                , VolumeDataLayoutDescriptor::BrickSize::BrickSize_64, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_64));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_128"               , VolumeDataLayoutDescriptor::BrickSize::BrickSize_128, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_128));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_256"               , VolumeDataLayoutDescriptor::BrickSize::BrickSize_256, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_256));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_512"               , VolumeDataLayoutDescriptor::BrickSize::BrickSize_512, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_512));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_1024"              , VolumeDataLayoutDescriptor::BrickSize::BrickSize_1024, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_1024));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_2048"              , VolumeDataLayoutDescriptor::BrickSize::BrickSize_2048, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_2048));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_4096"              , VolumeDataLayoutDescriptor::BrickSize::BrickSize_4096, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_4096));

  py::enum_<VolumeDataLayoutDescriptor::LODLevels> 
    VolumeDataLayoutDescriptor_LODLevels_(VolumeDataLayoutDescriptor_,"LODLevels", OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels));

  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_None"              , VolumeDataLayoutDescriptor::LODLevels::LODLevels_None, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_None));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_1"                 , VolumeDataLayoutDescriptor::LODLevels::LODLevels_1, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_1));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_2"                 , VolumeDataLayoutDescriptor::LODLevels::LODLevels_2, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_2));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_3"                 , VolumeDataLayoutDescriptor::LODLevels::LODLevels_3, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_3));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_4"                 , VolumeDataLayoutDescriptor::LODLevels::LODLevels_4, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_4));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_5"                 , VolumeDataLayoutDescriptor::LODLevels::LODLevels_5, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_5));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_6"                 , VolumeDataLayoutDescriptor::LODLevels::LODLevels_6, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_6));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_7"                 , VolumeDataLayoutDescriptor::LODLevels::LODLevels_7, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_7));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_8"                 , VolumeDataLayoutDescriptor::LODLevels::LODLevels_8, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_8));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_9"                 , VolumeDataLayoutDescriptor::LODLevels::LODLevels_9, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_9));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_10"                , VolumeDataLayoutDescriptor::LODLevels::LODLevels_10, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_10));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_11"                , VolumeDataLayoutDescriptor::LODLevels::LODLevels_11, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_11));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_12"                , VolumeDataLayoutDescriptor::LODLevels::LODLevels_12, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_12));

  py::enum_<VolumeDataLayoutDescriptor::Options> 
    VolumeDataLayoutDescriptor_Options_(VolumeDataLayoutDescriptor_,"Options", OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_Options));

  VolumeDataLayoutDescriptor_Options_.value("Options_None"                , VolumeDataLayoutDescriptor::Options::Options_None, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_Options_Options_None));
  VolumeDataLayoutDescriptor_Options_.value("Options_Create2DLODs"        , VolumeDataLayoutDescriptor::Options::Options_Create2DLODs, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_Options_Options_Create2DLODs));
  VolumeDataLayoutDescriptor_Options_.value("Options_ForceFullResolutionDimension", VolumeDataLayoutDescriptor::Options::Options_ForceFullResolutionDimension, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_Options_Options_ForceFullResolutionDimension));

  m.def("operator_bor"                , static_cast<VolumeDataLayoutDescriptor::Options(*)(VolumeDataLayoutDescriptor::Options, VolumeDataLayoutDescriptor::Options)>(&operator|), py::arg("lhs"), py::arg("rhs"), OPENVDS_DOCSTRING(operator_bor));
//AUTOGEN-END
}

