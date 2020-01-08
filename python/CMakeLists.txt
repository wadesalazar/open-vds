cmake_minimum_required(VERSION 3.5.0)
project(openvds-python-extension LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_C_VISIBILITY_PRESET   "hidden")

pybind11_add_module(core MODULE 
    core.cpp
    PyGlobal.cpp
    PyGlobal.h
    PyGlobalMetadataCommon.cpp
    PyGlobalMetadataCommon.h
    PyKnownMetadata.cpp
    PyKnownMetadata.h
    PyMetadata.cpp
    PyMetadata.h
    PyRange.cpp
    PyRange.h
    PyVector.cpp
    PyVector.h
    PyVolumeData.cpp
    PyVolumeData.h
    PyVolumeDataAccess.cpp
    PyVolumeDataAccess.h
    PyVolumeDataAxisDescriptor.cpp
    PyVolumeDataAxisDescriptor.h
    PyVolumeDataChannelDescriptor.cpp
    PyVolumeDataChannelDescriptor.h
    PyVolumeDataLayout.cpp
    PyVolumeDataLayout.h
    PyVolumeDataLayoutDescriptor.cpp
    PyVolumeDataLayoutDescriptor.h
    PyVolumeSampler.cpp
    PyVolumeSampler.h
)

target_include_directories(core
    PRIVATE
        ${PYBIND11_INCLUDE_DIRS}
)

target_link_libraries(core PRIVATE openvds)

install(TARGETS core LIBRARY DESTINATION python)
