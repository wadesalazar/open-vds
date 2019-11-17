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

void 
PyVolumeData::initModule(py::module& m)
{
#if 0
//AUTOGEN-BEGIN
  py::enum_<native::InterpolationMethod> 
    InterpolationMethod_(m,"InterpolationMethod", OPENVDS_DOCSTRING(InterpolationMethod));

  py::enum_<native::DimensionsND> 
    DimensionsND_(m,"DimensionsND", OPENVDS_DOCSTRING(DimensionsND));

  py::enum_<native::VolumeDataMapping> 
    VolumeDataMapping_(m,"VolumeDataMapping", OPENVDS_DOCSTRING(VolumeDataMapping));

  py::enum_<native::CompressionMethod> 
    CompressionMethod_(m,"CompressionMethod", OPENVDS_DOCSTRING(CompressionMethod));

  // CompressionInfo
  py::class_<native::CompressionInfo> 
    CompressionInfo_(m,"CompressionInfo", OPENVDS_DOCSTRING(CompressionInfo));

  CompressionInfo_.def(py::init<                              >(), OPENVDS_DOCSTRING(CompressionInfo_CompressionInfo));
  CompressionInfo_.def(py::init<OpenVDS::CompressionMethod, int>(), OPENVDS_DOCSTRING(CompressionInfo_CompressionInfo_2));
  CompressionInfo_.def("getCompressionMethod"        , static_cast<OpenVDS::CompressionMethod(*)()>(&native::GetCompressionMethod), OPENVDS_DOCSTRING(CompressionInfo_GetCompressionMethod));
  CompressionInfo_.def("getAdaptiveLevel"            , static_cast<int(*)()>(&native::GetAdaptiveLevel), OPENVDS_DOCSTRING(CompressionInfo_GetAdaptiveLevel));

  py::enum_<native::Dimensionality> 
    Dimensionality_(m,"Dimensionality", OPENVDS_DOCSTRING(Dimensionality));

//AUTOGEN-END
#endif
}

