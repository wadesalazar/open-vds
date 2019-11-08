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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <OpenVDS/VolumeDataAxisDescriptor.h>

#include "SharedLibrary.h"

using namespace PyOpenVDS;
namespace py = pybind11;

#if PY_MAJOR_VERSION < 2
dfafa
#endif

PYBIND11_MODULE(core, m) {
  SharedLibrary lib;
  lib.loadLibrary("openvds.dll");
  py::class_< OpenVDS::VolumeDataAxisDescriptor >(m, "Axis")
      .def(py::init<>())
      .def(py::init<int, const char*, const char*, float, float>())
      .def_property_readonly("name", &OpenVDS::VolumeDataAxisDescriptor::getName)
  ;
}
