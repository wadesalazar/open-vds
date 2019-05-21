cmake_minimum_required(VERSION 3.10.2)
project(Open-VDS)

set (CMAKE_CXX_STANDARD 11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads)

if (UNIX)
  add_definitions(-Wreturn-type)
endif()

include(CMake/Fetch3rdParty.cmake)
Fetch3rdParty()

include(CMake/BuildAWSSDK.cmake)
BuildAWSSDK()

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
