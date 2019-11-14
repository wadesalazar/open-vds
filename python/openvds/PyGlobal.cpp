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
  py::class_<native::OpenOptions> openOptions(m, "OpenOptions");
  openOptions.def_readwrite("connectionType", &native::OpenOptions::connectionType);

  py::enum_<native::OpenOptions::ConnectionType>(openOptions, "ConnectionType", OPENVDS_DOCSTRING(OpenOptions_ConnectionType))
    .value("AWS",   native::OpenOptions::AWS,   OPENVDS_DOCSTRING(OpenOptions_ConnectionType_AWS))
    .value("Azure", native::OpenOptions::Azure, OPENVDS_DOCSTRING(OpenOptions_ConnectionType_Azure))
    .value("File",  native::OpenOptions::File,  OPENVDS_DOCSTRING(OpenOptions_ConnectionType_File))
    .export_values();

  py::class_<native::AWSOpenOptions, native::OpenOptions>(m, "AWSOpenOptions")
    .def(py::init<>(), OPENVDS_DOCSTRING(AWSOpenOptions_AWSOpenOptions))
    .def(py::init<const std::string&, const std::string&, const std::string&>(), OPENVDS_DOCSTRING(AWSOpenOptions_AWSOpenOptions_2))
    .def_readwrite("bucket", &native::AWSOpenOptions::Bucket)
    .def_readwrite("key",    &native::AWSOpenOptions::Key)
    .def_readwrite("region", &native::AWSOpenOptions::Region);

  py::class_<native::Error> error(m, "Error");
  error
    .def(py::init<>())
    .def_readwrite("code",    &native::Error::Code)
    .def_readwrite("string",  &native::Error::String);
  error.def("__repr__", [](native::Error const& self){ char tmp[256]; _itoa_s(self.Code, tmp, 16); return std::string("Error(code=") + tmp + ", string='" + self.String + "')"; });

  py::enum_<Access>(m, "Access")
    .value("Read",      Access::Read)
    .value("ReadWrite", Access::Write);

  // These are opaque pointers, so they must not be destructed from pybind11 code
  py::class_<VDSHandle, std::unique_ptr<VDSHandle, py::nodelete>>(m, "VDSHandle");
  py::class_<IOManager, std::unique_ptr<IOManager, py::nodelete>>(m, "IOManager");

//OPENVDS_EXPORT VDSHANDLE Open(const OpenOptions& options, Error& error);
  m.def("open", static_cast<VDSHANDLE (*)(const OpenOptions& options, native::Error& error)>(&native::Open), OPENVDS_DOCSTRING(Open));

//OPENVDS_EXPORT VDSHANDLE Open(IOManager*ioManager, Error &error);
  m.def("open", static_cast<VDSHANDLE (*)(IOMANAGERHANDLE ioManager, Error &error)>(&native::Open), OPENVDS_DOCSTRING(Open_2));

//OPENVDS_EXPORT VDSHANDLE Create(const OpenOptions& options, VolumeDataLayoutDescriptor const& layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const& axisDescriptors, std::vector<VolumeDataChannelDescriptor> const& channelDescriptors, MetadataContainer const& metadataContainer, Error& error);
  m.def("create", static_cast<VDSHANDLE (*)(const OpenOptions& options, VolumeDataLayoutDescriptor const& layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const& axisDescriptors, std::vector<VolumeDataChannelDescriptor> const& channelDescriptors, MetadataContainer const& metadataContainer, Error& error)>(&native::Create), OPENVDS_DOCSTRING(Create));

//OPENVDS_EXPORT VDSHANDLE Create(IOManager* ioManager, VolumeDataLayoutDescriptor const &layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const &axisDescriptors, std::vector<VolumeDataChannelDescriptor> const &channelDescriptors, MetadataContainer const &metadataContainer, Error &error);
  m.def("create", static_cast<VDSHANDLE (*)(IOManager* ioManager, VolumeDataLayoutDescriptor const &layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const &axisDescriptors, std::vector<VolumeDataChannelDescriptor> const &channelDescriptors, MetadataContainer const &metadataContainer, Error &error)>(&native::Create), OPENVDS_DOCSTRING(Create_2));

//OPENVDS_EXPORT void Close(VDSHandle *handle);
  m.def("close", &native::Close, OPENVDS_DOCSTRING(Close));

//OPENVDS_EXPORT VolumeDataLayout *GetLayout(VDSHandle *handle);
  m.def("getLayout", &native::GetLayout, OPENVDS_DOCSTRING(GetLayout));

//OPENVDS_EXPORT VolumeDataAccessManager *GetDataAccessManager(VDSHandle *handle);
  m.def("getDataAccessManager", &native::GetDataAccessManager, OPENVDS_DOCSTRING(GetDataAccessManager));

}

