cmake_minimum_required(VERSION 3.10.2)
project(Open-VDS)

set (TEST_DATA_PATH "" CACHE PATH "Test data path")
set (CMAKE_CXX_STANDARD 11)

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

enable_testing()
add_subdirectory(tests)
