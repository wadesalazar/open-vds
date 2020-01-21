cmake_minimum_required(VERSION 3.12.0)
project(OpenVDS)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(GNUInstallDirs)

set (TEST_DATA_PATH "" CACHE PATH "Test data path")
set (TEST_AWS_REGION "" CACHE STRING "Test AWS region")
set (TEST_AWS_BUCKET "" CACHE STRING "Test AWS bucket")
set (TEST_AWS_OBJECTID "" CACHE STRING "Test AWS object ID")
set (TEST_AZURE_CONNECTION "" CACHE STRING "Test Azure connection string")

set (OPENVDS_LIBDIR_SUFFIX "" CACHE STRING "Suffix to add to lib directories on linux")
set (OPENVDS_PYTHON_VERSION "" CACHE STRING "Excact Python 3 version to discover")
option(FORCE_NO_LIBDIR_SUFFIX "Force there to be no suffix on lib directories linux")

get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
if ("${LIB64}" STREQUAL "TRUE")
  set(LIBSUFFIX 64)
else()
  set(LIBSUFFIX "")
endif()

set(BUILD_PYTHON_DEFAULT ON)
if (OPENVDS_PYTHON_VERSION)
    find_package(Python3 ${OPENVDS_PYTHON_VERSION} EXACT REQUIRED COMPONENTS Interpreter Development)
else()
    find_package(Python3 COMPONENTS Interpreter Development)
endif()

if (Python3_FOUND)
    message(${Python3_EXECUTABLE})
    set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
else()
  message("Failed to find Python3 interpreter. Disabling Python build")
  set(BUILD_PYTHON_DEFAULT OFF)
endif()

option(BUILD_PYTHON "Build python bindings" ${BUILD_PYTHON_DEFAULT})
option(ENABLE_OPENMP "Enable the OpenMP codepath" ON)
option(BUILD_SHARED_LIBS "Build OpenVDS as a shared library" ON)
option(BUILD_EXAMPLES "Build OpenVDS example projects" ON)
option(BUILD_TESTS "Build OpenVDS tests" ON)
option(BUILD_DOCS  "Build OpenVDS docs" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

define_property(GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_RELEASE BRIEF_DOCS "Runtime libraries" FULL_DOCS "Files that will be copied to test folders")
define_property(GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_DEBUG   BRIEF_DOCS "Runtime libraries" FULL_DOCS "Files that will be copied to test folders")
define_property(GLOBAL PROPERTY OPENVDS_LINK_LIBRARIES_RELEASE BRIEF_DOCS "OpenVDS link libraries" FULL_DOCS "Files OpenVDS will link with")
define_property(GLOBAL PROPERTY OPENVDS_LINK_LIBRARIES_DEBUG   BRIEF_DOCS "OpenVDS link libraries" FULL_DOCS "Files OpenVDS will link with")
define_property(GLOBAL PROPERTY OPENVDS_INCLUDE_LIBRARIES BRIEF_DOCS "OpenVDS include libraries" FULL_DOCS "Include paths for 3rdparty")

if(CMAKE_VS_PLATFORM_TOOLSET)
  string(REGEX REPLACE "^v" "" MSVC_TOOLSET_VERSION_LOCAL "${CMAKE_VS_PLATFORM_TOOLSET}")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads)
if (ENABLE_OPENMP)
    find_package(OpenMP)
endif()
if (UNIX)
  add_compile_options(-Wreturn-type)
elseif (MSVC_VERSION GREATER_EQUAL 1910 AND NOT OpenMP_CXX_FOUND)
  add_compile_options(/permissive-)
endif()
if(MSVC_VERSION GREATER_EQUAL 1915)
  add_compile_options( /JMC )
endif()

if (WIN32)
  set(DEFAULT_BUILD_ZLIB ON)
else()
  set(DEFAULT_BUILD_ZLIB OFF)
endif()

option(BUILD_ZLIB "Build zlib" ${DEFAULT_BUILD_ZLIB})

include(CMake/Fetch3rdPartyInBuild.cmake)
Fetch3rdParty()

include(CMake/Build3rdParty.cmake)
build3rdparty()

add_subdirectory(src)

if (BUILD_PYTHON)
  add_subdirectory(python)
endif ()

if (BUILD_DOCS)
  add_subdirectory(docs)
endif()

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
