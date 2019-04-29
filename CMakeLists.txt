cmake_minimum_required(VERSION 3.10.2)
project(Open-VDS)

set (CMAKE_CXX_STANDARD 11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads)

if (UNIX)
  add_definitions(-Wreturn-type)
endif()

set(AWS_VERSION 1.7.90)
set(AWS_SOURCE_DIR  ${PROJECT_SOURCE_DIR}/3rdparty/aws-cpp-sdk-${AWS_VERSION})
set(AWS_INSTALL "${CMAKE_BINARY_DIR}/aws_install/")
if (WIN32)
list(APPEND AWS_LIBS "${AWS_INSTALL}/lib/aws-cpp-sdk-core.lib")
list(APPEND AWS_LIBS "${AWS_INSTALL}/lib/aws-cpp-sdk-s3.lib")
else()
list(APPEND AWS_LIBS "${AWS_INSTALL}/lib/libaws-cpp-sdk-core.a")
list(APPEND AWS_LIBS "${AWS_INSTALL}/lib/libaws-cpp-sdk-s3.a")
endif()

if (NOT (EXISTS ${AWS_SOURCE_DIR}))
    include(CMake/FetchContentLocal.cmake)

    FetchContent_Declare( aws-cpp-sdk-download
        URL https://github.com/aws/aws-sdk-cpp/archive/${AWS_VERSION}.zip
        URL_MD5 d2b923b1ea11e780365cbec0fc627b7f
        SOURCE_DIR ${AWS_SOURCE_DIR}
        )
    FetchContent_Populate(aws-cpp-sdk-download)
endif()
include(ExternalProject)
ExternalProject_Add(aws-cpp-sdk
    PREFIX ${CMAKE_BINARY_DIR}/aws-cpp-sdk
    SOURCE_DIR ${AWS_SOURCE_DIR}
    URL ""
    INSTALL_DIR ${CMAKE_BINARY_DIR}/aws_install
    CMAKE_ARGS "-DBUILD_ONLY=s3" "-DENABLE_TESTING=OFF" "-DBUILD_SHARED_LIBS=OFF" "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/aws_install"
    BUILD_BYPRODUCTS ${AWS_LIBS}
    )

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
