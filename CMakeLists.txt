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
set(AWS_INSTALL "${CMAKE_BINARY_DIR}/aws_install")
set(AWS_INCLUDE_PATH "${AWS_INSTALL}/include")
if (WIN32)
list(APPEND AWS_LIBS "${AWS_INSTALL}/bin/aws-cpp-sdk-core.lib")
list(APPEND AWS_LIBS "${AWS_INSTALL}/bin/aws-cpp-sdk-s3.lib")
list(APPEND AWS_LIBS "winhttp")

list(APPEND AWS_DLLS "${AWS_INSTALL}/bin/aws-c-common.dll")
list(APPEND AWS_DLLS "${AWS_INSTALL}/bin/aws-c-event-stream.dll")
list(APPEND AWS_DLLS "${AWS_INSTALL}/bin/aws-checksums.dll")
list(APPEND AWS_DLLS "${AWS_INSTALL}/bin/aws-cpp-sdk-core.dll")
list(APPEND AWS_DLLS "${AWS_INSTALL}/bin/aws-cpp-sdk-s3.dll")
else()
list(APPEND AWS_LIBS "${AWS_INSTALL}/lib/libaws-cpp-sdk-core.so")
list(APPEND AWS_LIBS "${AWS_INSTALL}/lib/libaws-cpp-sdk-s3.so")
list(APPEND AWS_LIBS "crypto")
list(APPEND AWS_LIBS "curl")
endif()

if (NOT (EXISTS ${AWS_SOURCE_DIR}))
    include(CMake/FetchContentLocal.cmake)
    set(FETCHCONTENT_QUIET OFF)
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
    CMAKE_ARGS "-DBUILD_ONLY=s3" "-DENABLE_TESTING=OFF" "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/aws_install"
    BUILD_BYPRODUCTS ${AWS_LIBS}
    )

add_definitions(-DUSE_IMPORT_EXPORT)

set(GTEST_VERSION 1.8.1)
set(GTEST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/3rdparty/gtest-${GTEST_VERSION})
if (NOT (EXISTS ${GTEST_SOURCE_DIR}))
    FetchContent_Declare( gtest-download
        URL https://github.com/google/googletest/archive/release-${GTEST_VERSION}.tar.gz
        URL_MD5  2e6fbeb6a91310a16efe181886c59596
        SOURCE_DIR ${GTEST_SOURCE_DIR}
        )
    FetchContent_Populate(gtest-download)
endif()

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
