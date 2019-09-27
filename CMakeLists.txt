cmake_minimum_required(VERSION 3.10.2)
project(Open-VDS)

set (TEST_DATA_PATH "" CACHE PATH "Test data path")
set (TEST_AWS_REGION "" CACHE STRING "Test AWS region")
set (TEST_AWS_BUCKET "" CACHE STRING "Test AWS bucket")
set (TEST_AWS_OBJECTID "" CACHE STRING "Test AWS object ID")
option(BUILD_PYTHON "Build python bindings" ON)

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

define_property(GLOBAL PROPERTY RUNTIME_LIBS_RELEASE BRIEF_DOCS "Runtime libraries" FULL_DOCS "Files that will be copied to test folders")
define_property(GLOBAL PROPERTY RUNTIME_LIBS_DEBUG   BRIEF_DOCS "Runtime libraries" FULL_DOCS "Files that will be copied to test folders")
define_property(GLOBAL PROPERTY OPENVDS_LINK_LIBRARIES_RELEASE BRIEF_DOCS "OpenVDS link libraries" FULL_DOCS "Files OpenVDS will link with")
define_property(GLOBAL PROPERTY OPENVDS_LINK_LIBRARIES_DEBUG   BRIEF_DOCS "OpenVDS link libraries" FULL_DOCS "Files OpenVDS will link with")
define_property(GLOBAL PROPERTY OPENVDS_INCLUDE_LIBRARIES BRIEF_DOCS "OpenVDS include libraries" FULL_DOCS "Include paths for 3rdparty")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads)

if (UNIX)
  add_definitions(-Wreturn-type)
  add_compile_options(-fPIC)
endif()

if (WIN32)
  set(DEFAULT_BUILD_ZLIB ON)
else()
  set(DEFAULT_BUILD_ZLIB OFF)
endif()

option(BUILD_ZLIB "Build zlib" ${DEFAULT_BUILD_ZLIB})

include(CMake/Fetch3rdParty.cmake)
Fetch3rdParty()

include(CMake/Build3rdParty.cmake)
build3rdparty()

add_subdirectory(src)
add_subdirectory(python)

enable_testing()
add_subdirectory(tests)
