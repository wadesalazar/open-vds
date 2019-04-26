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
    PREFIX ${CMAKE_BINARY_DIR}/aws-sdk-cpp
    INSTALL_DIR ${CMAKE_BINARY_DIR}/aws_install
    GIT_REPOSITORY https://github.com/aws/aws-sdk-cpp.git
    GIT_TAG f85a6a6a37c5535491d0d689b61fc13142b87ce0
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    CMAKE_ARGS "-DBUILD_ONLY=s3" "-DENABLE_TESTING=OFF" "-DBUILD_SHARED_LIBS=OFF" "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/aws_install"

    )

set(AWS_INCLUDE_PATH "${CMAKE_BINARY_DIR}/aws_install/include")
list(APPEND AWS_LIBS "${CMAKE_BINARY_DIR}/aws_install/lib/aws-cpp-sdk-core.lib")
list(APPEND AWS_LIBS "${CMAKE_BINARY_DIR}/aws_install/lib/aws-cpp-sdk-s3.lib")

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
