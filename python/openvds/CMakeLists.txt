cmake_minimum_required(VERSION 3.5.0)
project(openvds-python-extension LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_C_VISIBILITY_PRESET   "hidden")

find_package(PythonExtensions REQUIRED)

find_package(Threads REQUIRED)
if(ENABLE_OPENMP)
  find_package(OpenMP REQUIRED)
endif()
find_package(ZLIB REQUIRED)
find_package(openvds REQUIRED)

if ("${CMAKE_GENERATOR}" MATCHES "Visual Studio")
	# Set up debugging environment for "core" project in Visual Studio
    set(PYTHONEXE "${python3_EXE}")
    set(BINDIR "${openvds_DIR}/SEG-Y") # A bit of a hack to get all the dependent dlls
	set(PYTHONPATH "${package_DIR}")
    configure_file("../core.vcxproj.user.txt" "${CMAKE_CURRENT_BINARY_DIR}/core.vcxproj.user" @ONLY)
endif()	
configure_file("../aws_defs.txt" "${CMAKE_CURRENT_SOURCE_DIR}/test/aws_defs.py" @ONLY)

add_library(core MODULE 
    core.cpp
    PyGlobal.cpp
    PyGlobal.h
	PyVolumeDataLayout.cpp
	PyVolumeDataLayout.h
	PyVolumeDataAccessManager.cpp
	PyVolumeDataAccessManager.h
	PyVolumeDataPageAccessor.cpp
	PyVolumeDataPageAccessor.h
	PyVolumeDataAxisDescriptor.cpp
	PyVolumeDataAxisDescriptor.h
  )
target_include_directories(core
    PRIVATE
        ${PYBIND11_INCLUDE_DIRS}
)
python_extension_module(core)
target_link_libraries(core openvds::openvds)

if (MSVC)
    target_compile_options(core
        BEFORE
        PRIVATE
            /EHsc
    )
    string(REPLACE "/" "\\" _package_dir ${CMAKE_CURRENT_SOURCE_DIR})
	add_custom_command(
	  TARGET core
	  POST_BUILD
	  COMMAND copy $(TargetPath) ${_package_dir}
	)
endif ()

install(TARGETS core LIBRARY DESTINATION openvds)
