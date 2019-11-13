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

  py::enum_<native::OpenOptions::ConnectionType>(openOptions, "ConnectionType")
    .value("AWS", native::OpenOptions::AWS)
    .value("Azure", native::OpenOptions::Azure)
    .value("File", native::OpenOptions::File)
    .export_values();

  py::class_<native::AWSOpenOptions, native::OpenOptions>(m, "AWSOpenOptions")
    .def(py::init<>())
    .def(py::init<const std::string&, const std::string&, const std::string&>())
    .def_readwrite("bucket", &native::AWSOpenOptions::Bucket)
    .def_readwrite("key",    &native::AWSOpenOptions::Key)
    .def_readwrite("region", &native::AWSOpenOptions::Region);

  py::class_<native::Error> error(m, "Error");
  error
    .def(py::init<>())
    .def_readwrite("code",    &native::Error::Code)
    .def_readwrite("string",  &native::Error::String);
  error.def("__repr__", [](native::Error const& self){ char tmp[256]; _itoa_s(self.Code, tmp, 16); return std::string("Error(code=") + tmp + ", string='" + self.String + "')"; });

  enum class AllAccess
  {
    None      = 0,
    Read      = (int)native::Access::Read,
    Write     = (int)native::Access::Write,
    ReadWrite = (int)native::Access::Read | (int)native::Access::Write
  };

  py::enum_<AllAccess> access(m, "Access", py::arithmetic());
  access.def("__or__",[](AllAccess const& lhs, AllAccess const& rhs){ return AllAccess((int)lhs | (int)rhs); });
  access
    .value("NONE",      AllAccess::None)
    .value("Read",      AllAccess::Read)
    .value("Write",     AllAccess::Write)
    .value("ReadWrite", AllAccess::ReadWrite);

  // These are opaque pointers, so they must not be destructed from pybind11 code
  py::class_<VDSHandle, std::unique_ptr<VDSHandle, py::nodelete>>(m, "VDSHandle");
  py::class_<IOManager, std::unique_ptr<IOManager, py::nodelete>>(m, "IOManager");

///// <summary>
///// Open an existing VDS
///// </summary>
///// <param name="options">
///// The options for the connection
///// </param>
///// <param name="error">
///// If an error occured, the error code and message will be written to this output parameter
///// </param>
///// <returns>
///// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
///// </returns>
//OPENVDS_EXPORT VDSHANDLE Open(const OpenOptions& options, Error& error);
  m.def("open", static_cast<VDSHANDLE (*)(const OpenOptions& options, native::Error& error)>(&native::Open));

///// <summary>
///// Open an existing VDS
///// </summary>
///// <param name="ioManager">
///// The IOManager for the connection, it will be deleted automatically when the VDS handle is closed
///// </param>
///// <param name="error">
///// If an error occured, the error code and message will be written to this output parameter
///// </param>
///// <returns>
///// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
///// </returns>
//OPENVDS_EXPORT VDSHANDLE Open(IOManager*ioManager, Error &error);
  m.def("open", static_cast<VDSHANDLE (*)(IOMANAGERHANDLE ioManager, Error &error)>(&native::Open));

///// <summary>
///// Create a new VDS
///// </summary>
///// <param name="options">
///// The options for the connection
///// </param>
///// <param name="error">
///// If an error occured, the error code and message will be written to this output parameter
///// </param>
///// <returns>
///// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
///// </returns>
//OPENVDS_EXPORT VDSHANDLE Create(const OpenOptions& options, VolumeDataLayoutDescriptor const& layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const& axisDescriptors, std::vector<VolumeDataChannelDescriptor> const& channelDescriptors, MetadataContainer const& metadataContainer, Error& error);
  m.def("create", static_cast<VDSHANDLE (*)(const OpenOptions& options, VolumeDataLayoutDescriptor const& layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const& axisDescriptors, std::vector<VolumeDataChannelDescriptor> const& channelDescriptors, MetadataContainer const& metadataContainer, Error& error)>(&native::Create));

///// <summary>
///// Create a new VDS
///// </summary>
///// <param name="ioManager">
///// The IOManager for the connection, it will be deleted automatically when the VDS handle is closed
///// </param>
///// <param name="error">
///// If an error occured, the error code and message will be written to this output parameter
///// </param>
///// <returns>
///// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
///// </returns>
//OPENVDS_EXPORT VDSHANDLE Create(IOManager* ioManager, VolumeDataLayoutDescriptor const &layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const &axisDescriptors, std::vector<VolumeDataChannelDescriptor> const &channelDescriptors, MetadataContainer const &metadataContainer, Error &error);
  m.def("create", static_cast<VDSHANDLE (*)(IOManager* ioManager, VolumeDataLayoutDescriptor const &layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const &axisDescriptors, std::vector<VolumeDataChannelDescriptor> const &channelDescriptors, MetadataContainer const &metadataContainer, Error &error)>(&native::Create));

///// <summary>
///// Close a VDS and free up all associated resources
///// </summary>
///// <param name="handle">
///// The handle to close
///// </param>
//OPENVDS_EXPORT void Close(VDSHandle *handle);
  m.def("close", &native::Close);

//OPENVDS_EXPORT VolumeDataLayout *GetLayout(VDSHandle *handle);
  m.def("getLayout", &native::GetLayout);

//OPENVDS_EXPORT VolumeDataAccessManager *GetDataAccessManager(VDSHandle *handle);
  m.def("getDataAccessManager", &native::GetDataAccessManager);

}

