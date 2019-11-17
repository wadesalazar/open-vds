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
#ifndef PYGLOBAL_H_INCLUDED
#define PYGLOBAL_H_INCLUDED

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataAxisDescriptor.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataLayoutDescriptor.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/VolumeData.h>
#include <OpenVDS/VolumeDataAccess.h>

#include "generated_docstrings.h"

#define OPENVDS_DOCSTRING(name) __doc_OpenVDS_ ## name

namespace OpenVDS {

#ifdef OPENVDS_DEVELOPMENT_BUILD

struct VDS;
class IOManager;

#else

struct VDS
{
private:
  VDS();
  ~VDS();
};

class IOManager
{
private:
  IOManager();
  ~IOManager();
};

#endif

}

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
namespace native = OpenVDS;

class PyGlobal
{
public:
  static void initModule(py::module& m);
};

#endif
