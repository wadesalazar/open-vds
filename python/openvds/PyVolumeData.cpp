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

#include "PyVolumeData.h"

using namespace native;

void 
PyVolumeData::initModule(py::module& m)
{
//AUTOGEN-BEGIN
  py::enum_<InterpolationMethod> 
    InterpolationMethod_(m,"InterpolationMethod", OPENVDS_DOCSTRING(InterpolationMethod));

  py::enum_<DimensionsND> 
    DimensionsND_(m,"DimensionsND", OPENVDS_DOCSTRING(DimensionsND));

  py::enum_<VolumeDataMapping> 
    VolumeDataMapping_(m,"VolumeDataMapping", OPENVDS_DOCSTRING(VolumeDataMapping));

  py::enum_<CompressionMethod> 
    CompressionMethod_(m,"CompressionMethod", OPENVDS_DOCSTRING(CompressionMethod));

  // CompressionInfo
  py::class_<CompressionInfo> 
    CompressionInfo_(m,"CompressionInfo", OPENVDS_DOCSTRING(CompressionInfo));

  CompressionInfo_.def(py::init<                              >(), OPENVDS_DOCSTRING(CompressionInfo_CompressionInfo));
  CompressionInfo_.def(py::init<native::CompressionMethod, int>(), OPENVDS_DOCSTRING(CompressionInfo_CompressionInfo_2));
  CompressionInfo_.def("getCompressionMethod"        , static_cast<native::CompressionMethod(CompressionInfo::*)() const>(&CompressionInfo::GetCompressionMethod), OPENVDS_DOCSTRING(CompressionInfo_GetCompressionMethod));
  CompressionInfo_.def("getAdaptiveLevel"            , static_cast<int(CompressionInfo::*)() const>(&CompressionInfo::GetAdaptiveLevel), OPENVDS_DOCSTRING(CompressionInfo_GetAdaptiveLevel));

  py::enum_<Dimensionality> 
    Dimensionality_(m,"Dimensionality", OPENVDS_DOCSTRING(Dimensionality));

  m.def("getLODSize"                  , static_cast<int(*)(int, int, int, bool)>(&GetLODSize), OPENVDS_DOCSTRING(GetLODSize));
  //m.def("readElement"                 , static_cast<bool(*)(const bool *, size_t)>(&ReadElement), OPENVDS_DOCSTRING(ReadElement));
  //m.def("writeElement"                , static_cast<void(*)(bool *, size_t, bool)>(&WriteElement), OPENVDS_DOCSTRING(WriteElement));
  //m.def("pitchScale"                  , static_cast<int(*)()>(&PitchScale), OPENVDS_DOCSTRING(PitchScale));
//AUTOGEN-END
}

