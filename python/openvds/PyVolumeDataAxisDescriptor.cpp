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

using namespace native;

void 
PyVolumeDataAxisDescriptor::initModule(py::module& m)
{
//AUTOGEN-BEGIN
  // VolumeDataAxisDescriptor
  py::class_<VolumeDataAxisDescriptor> 
    VolumeDataAxisDescriptor_(m,"VolumeDataAxisDescriptor", OPENVDS_DOCSTRING(VolumeDataAxisDescriptor));

  VolumeDataAxisDescriptor_.def(py::init<                              >(), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_VolumeDataAxisDescriptor));
  VolumeDataAxisDescriptor_.def("getNumSamples"               , static_cast<int(VolumeDataAxisDescriptor::*)() const>(&VolumeDataAxisDescriptor::GetNumSamples), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetNumSamples));
  VolumeDataAxisDescriptor_.def_property_readonly("numSamples", &VolumeDataAxisDescriptor::GetNumSamples, OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetNumSamples));
  VolumeDataAxisDescriptor_.def("getName"                     , static_cast<const char *(VolumeDataAxisDescriptor::*)() const>(&VolumeDataAxisDescriptor::GetName), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetName));
  VolumeDataAxisDescriptor_.def_property_readonly("name", &VolumeDataAxisDescriptor::GetName, OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetName));
  VolumeDataAxisDescriptor_.def("getUnit"                     , static_cast<const char *(VolumeDataAxisDescriptor::*)() const>(&VolumeDataAxisDescriptor::GetUnit), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetUnit));
  VolumeDataAxisDescriptor_.def_property_readonly("unit", &VolumeDataAxisDescriptor::GetUnit, OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetUnit));
  VolumeDataAxisDescriptor_.def("getCoordinateMin"            , static_cast<float(VolumeDataAxisDescriptor::*)() const>(&VolumeDataAxisDescriptor::GetCoordinateMin), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetCoordinateMin));
  VolumeDataAxisDescriptor_.def_property_readonly("coordinateMin", &VolumeDataAxisDescriptor::GetCoordinateMin, OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetCoordinateMin));
  VolumeDataAxisDescriptor_.def("getCoordinateMax"            , static_cast<float(VolumeDataAxisDescriptor::*)() const>(&VolumeDataAxisDescriptor::GetCoordinateMax), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetCoordinateMax));
  VolumeDataAxisDescriptor_.def_property_readonly("coordinateMax", &VolumeDataAxisDescriptor::GetCoordinateMax, OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetCoordinateMax));
  VolumeDataAxisDescriptor_.def("getCoordinateStep"           , static_cast<float(VolumeDataAxisDescriptor::*)() const>(&VolumeDataAxisDescriptor::GetCoordinateStep), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetCoordinateStep));
  VolumeDataAxisDescriptor_.def_property_readonly("coordinateStep", &VolumeDataAxisDescriptor::GetCoordinateStep, OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_GetCoordinateStep));
  VolumeDataAxisDescriptor_.def("sampleIndexToCoordinate"     , static_cast<float(VolumeDataAxisDescriptor::*)(int)>(&VolumeDataAxisDescriptor::SampleIndexToCoordinate), py::arg("sampleIndex").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_SampleIndexToCoordinate));
  VolumeDataAxisDescriptor_.def("coordinateToSampleIndex"     , static_cast<int(VolumeDataAxisDescriptor::*)(float)>(&VolumeDataAxisDescriptor::CoordinateToSampleIndex), py::arg("coordinate").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_CoordinateToSampleIndex));
  VolumeDataAxisDescriptor_.def("coordinateToSamplePosition"  , static_cast<float(VolumeDataAxisDescriptor::*)(float)>(&VolumeDataAxisDescriptor::CoordinateToSamplePosition), py::arg("coordinate").none(false), py::call_guard<py::gil_scoped_release>(), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_CoordinateToSamplePosition));

//AUTOGEN-END
// IMPLEMENTED :   VolumeDataAxisDescriptor_.def(py::init<int, const char *, const char *, float, float>(), py::arg("numSamples").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("coordinateMin").none(false), py::arg("coordinateMax").none(false), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_VolumeDataAxisDescriptor_2));
  VolumeDataAxisDescriptor_.def(py::init([](int numSamples, std::string name, std::string unit, float coordinateMin, float coordinateMax)
    {
      return VolumeDataAxisDescriptor(numSamples, PyDescriptorStringContainer::Add(name), PyDescriptorStringContainer::Add(unit), coordinateMin, coordinateMax);
    }), py::arg("numSamples").none(false), py::arg("name").none(false), py::arg("unit").none(false), py::arg("coordinateMin").none(false), py::arg("coordinateMax").none(false), OPENVDS_DOCSTRING(VolumeDataAxisDescriptor_VolumeDataAxisDescriptor_2));
}
