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

#include "PyVolumeSampler.h"

void 
PyVolumeSampler::initModule(py::module& m)
{
#if 0
//AUTOGEN-BEGIN
  m.def("readElement"                 , static_cast<bool(*)(const bool *, size_t)>(&native::ReadElement), OPENVDS_DOCSTRING(ReadElement));
  m.def("pitchScale"                  , static_cast<int(*)()>(&native::PitchScale), OPENVDS_DOCSTRING(PitchScale));

  // InterpolatedRealType
  py::class_<native::InterpolatedRealType> 
    InterpolatedRealType_(m,"InterpolatedRealType", OPENVDS_DOCSTRING(InterpolatedRealType));

  // InterpolatedRealType
  py::class_<native::InterpolatedRealType> 
    InterpolatedRealType_(m,"InterpolatedRealType", OPENVDS_DOCSTRING(InterpolatedRealType));

  // InterpolatedRealType
  py::class_<native::InterpolatedRealType> 
    InterpolatedRealType_(m,"InterpolatedRealType", OPENVDS_DOCSTRING(InterpolatedRealType));

//AUTOGEN-END
#endif
}

