cmake_minimum_required(VERSION 3.10.2)
project(Open-VDS)

set (CMAKE_CXX_STANDARD 11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads)

if (UNIX)
  add_definitions(-Wreturn-type)
endif()

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
ExternalProject_Add(aws-sdk-cpp
    GIT_REPOSITORY https://github.com/aws/aws-sdk-cpp.git
    GIT_TAG 1.7.90
    CMAKE_ARGS "-DBUILD_ONLY=\"s3\""
    STEP_TARGETS build
    )

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
