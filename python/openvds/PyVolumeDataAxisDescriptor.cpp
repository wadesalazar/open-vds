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

#include "PyVolumeDataAxisDescriptor.h"

void 
PyVolumeDataAxisDescriptor::initModule(py::module& m)
{
#if 0
//AUTOGEN-BEGIN
  // VolumeDataAxisDescriptor
  py::class_<native::VolumeDataAxisDescriptor> 
    VolumeDataAxisDescriptor_(m,"VolumeDataAxisDescriptor", OPENVDS_DOCSTRING(VolumeDataAxisDescriptor));

  VolumeDataAxisDescriptor_.def(py::init<                              >(), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_VolumeDataAxisDescriptor));
  VolumeDataAxisDescriptor_.def(py::init<int, const char *, const char *, float, float>(), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_VolumeDataAxisDescriptor_2));
  VolumeDataAxisDescriptor_.def("getNumSamples"               , static_cast<int(*)()>(&native::GetNumSamples), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetNumSamples));
  VolumeDataAxisDescriptor_.def("getName"                     , static_cast<const char *(*)()>(&native::GetName), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetName));
  VolumeDataAxisDescriptor_.def("getUnit"                     , static_cast<const char *(*)()>(&native::GetUnit), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetUnit));
  VolumeDataAxisDescriptor_.def("getCoordinateMin"            , static_cast<float(*)()>(&native::GetCoordinateMin), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetCoordinateMin));
  VolumeDataAxisDescriptor_.def("getCoordinateMax"            , static_cast<float(*)()>(&native::GetCoordinateMax), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetCoordinateMax));
  VolumeDataAxisDescriptor_.def("getCoordinateStep"           , static_cast<float(*)()>(&native::GetCoordinateStep), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetCoordinateStep));
  VolumeDataAxisDescriptor_.def("sampleIndexToCoordinate"     , static_cast<float(*)(int)>(&native::SampleIndexToCoordinate), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_SampleIndexToCoordinate));
  VolumeDataAxisDescriptor_.def("coordinateToSampleIndex"     , static_cast<int(*)(float)>(&native::CoordinateToSampleIndex), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_CoordinateToSampleIndex));

//AUTOGEN-END
#endif
}

