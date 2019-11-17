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

#include "PyGlobal.h"

using namespace native;

void 
PyGlobal::initModule(py::module& m)
{
//  py::class_<native::OpenOptions> openOptions(m, "OpenOptions");
//  openOptions.def_readwrite("connectionType", &native::OpenOptions::connectionType);
//
//  py::enum_<native::OpenOptions::ConnectionType>(openOptions, "ConnectionType", OPENVDS_DOCSTRING(OpenOptions_ConnectionType))
//    .value("AWS",   native::OpenOptions::AWS,   OPENVDS_DOCSTRING(OpenOptions_ConnectionType_AWS))
//    .value("Azure", native::OpenOptions::Azure, OPENVDS_DOCSTRING(OpenOptions_ConnectionType_Azure))
//    .value("File",  native::OpenOptions::File,  OPENVDS_DOCSTRING(OpenOptions_ConnectionType_File))
//    .export_values();
//
//  py::class_<native::AWSOpenOptions, native::OpenOptions>(m, "AWSOpenOptions")
//    .def(py::init<>(), OPENVDS_DOCSTRING(AWSOpenOptions_AWSOpenOptions))
//    .def(py::init<const std::string&, const std::string&, const std::string&>(), OPENVDS_DOCSTRING(AWSOpenOptions_AWSOpenOptions_2))
//    .def_readwrite("bucket", &native::AWSOpenOptions::Bucket)
//    .def_readwrite("key",    &native::AWSOpenOptions::Key)
//    .def_readwrite("region", &native::AWSOpenOptions::Region);
//
//  py::class_<native::Error> error(m, "Error");
//  error
//    .def(py::init<>())
//    .def_readwrite("code",    &native::Error::Code)
//    .def_readwrite("string",  &native::Error::String);
//
  // These are opaque pointers, so they must not be destructed from pybind11 code
  py::class_<VDSHandle, std::unique_ptr<VDSHandle, py::nodelete>>(m, "VDSHandle");
  py::class_<IOManager, std::unique_ptr<IOManager, py::nodelete>>(m, "IOManager");

//AUTOGEN-BEGIN
  // OpenOptions
  py::class_<native::OpenOptions> 
    OpenOptions_(m,"OpenOptions", OPENVDS_DOCSTRING(OpenOptions));

  OpenOptions_.def_readwrite("connectionType"              , &native::OpenOptions::connectionType   , OPENVDS_DOCSTRING(OpenOptions_connectionType));

  py::enum_<native::OpenOptions::ConnectionType> 
    OpenOptions_ConnectionType_(OpenOptions_,"ConnectionType", OPENVDS_DOCSTRING(OpenOptions_ConnectionType));

  OpenOptions_ConnectionType_.value("AWS"                         , native::OpenOptions::ConnectionType::AWS        , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_AWS));
  OpenOptions_ConnectionType_.value("Azure"                       , native::OpenOptions::ConnectionType::Azure      , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_Azure));
  OpenOptions_ConnectionType_.value("File"                        , native::OpenOptions::ConnectionType::File       , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_File));

  // AWSOpenOptions
  py::class_<native::AWSOpenOptions, native::OpenOptions> 
    AWSOpenOptions_(m,"AWSOpenOptions", OPENVDS_DOCSTRING(AWSOpenOptions));

  AWSOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(AWSOpenOptions_AWSOpenOptions));
  AWSOpenOptions_.def(py::init<const std::string &, const std::string &, const std::string &>(), OPENVDS_DOCSTRING(AWSOpenOptions_AWSOpenOptions_2));
  AWSOpenOptions_.def_readwrite("bucket"                      , &native::AWSOpenOptions::Bucket        , OPENVDS_DOCSTRING(AWSOpenOptions_Bucket));
  AWSOpenOptions_.def_readwrite("key"                         , &native::AWSOpenOptions::Key           , OPENVDS_DOCSTRING(AWSOpenOptions_Key));
  AWSOpenOptions_.def_readwrite("region"                      , &native::AWSOpenOptions::Region        , OPENVDS_DOCSTRING(AWSOpenOptions_Region));

  // Error
  py::class_<native::Error> 
    Error_(m,"Error", OPENVDS_DOCSTRING(Error));

  Error_.def_readwrite("code"                        , &native::Error::Code                   , OPENVDS_DOCSTRING(Error_Code));
  Error_.def_readwrite("string"                      , &native::Error::String                 , OPENVDS_DOCSTRING(Error_String));

  m.def("open"                        , static_cast<OpenVDS::VDSHandle(*)(const OpenVDS::OpenOptions &, OpenVDS::Error &)>(&native::Open), OPENVDS_DOCSTRING(Open));
  m.def("open"                        , static_cast<OpenVDS::VDSHandle(*)(OpenVDS::IOManager *, OpenVDS::Error &)>(&native::Open), OPENVDS_DOCSTRING(Open_2));
  m.def("create"                      , static_cast<OpenVDS::VDSHandle(*)(const OpenVDS::OpenOptions &, const OpenVDS::VolumeDataLayoutDescriptor &, const std::vector<VolumeDataAxisDescriptor> &, const std::vector<VolumeDataChannelDescriptor> &, const OpenVDS::MetadataContainer &, OpenVDS::Error &)>(&native::Create), OPENVDS_DOCSTRING(Create));
  m.def("create"                      , static_cast<OpenVDS::VDSHandle(*)(OpenVDS::IOManager *, const OpenVDS::VolumeDataLayoutDescriptor &, const std::vector<VolumeDataAxisDescriptor> &, const std::vector<VolumeDataChannelDescriptor> &, const OpenVDS::MetadataContainer &, OpenVDS::Error &)>(&native::Create), OPENVDS_DOCSTRING(Create_2));
  m.def("close"                       , static_cast<void(*)(OpenVDS::VDSHandle)>(&native::Close), OPENVDS_DOCSTRING(Close));
  m.def("getLayout"                   , static_cast<OpenVDS::VolumeDataLayout *(*)(OpenVDS::VDSHandle)>(&native::GetLayout), OPENVDS_DOCSTRING(GetLayout));
  m.def("getDataAccessManager"        , static_cast<OpenVDS::VolumeDataAccessManager *(*)(OpenVDS::VDSHandle)>(&native::GetDataAccessManager), OPENVDS_DOCSTRING(GetDataAccessManager));
//AUTOGEN-END
  Error_.def("__repr__", [](native::Error const& self){ char tmp[256]; _itoa_s(self.Code, tmp, 16); return std::string("Error(code=") + tmp + ", string='" + self.String + "')"; });
}

