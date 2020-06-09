#This file depends on being in open-vds/CMake directory
set(Fetch3rdPartyDir ${CMAKE_CURRENT_LIST_DIR})

macro(Fetch3rdParty_File name dir_prefix version ext url url_hash)
  get_filename_component(thirdParty "${Fetch3rdPartyDir}/../3rdparty/${dir_prefix}" ABSOLUTE)
  file(MAKE_DIRECTORY ${thirdParty})
  set(SRC_FILE ${thirdParty}/${name}-${version}.${ext})
  set(${name}_SOURCE_FILE ${SRC_FILE} PARENT_SCOPE)
  set(${name}_VERSION ${version} PARENT_SCOPE)
  if (NOT (EXISTS ${SRC_FILE}))
    file(DOWNLOAD ${url}
      ${SRC_FILE}
      SHOW_PROGRESS
      EXPECTED_HASH ${url_hash}
      )
  endif()
endmacro()

macro(Fetch3rdParty_Package name version url url_hash)
    get_filename_component(thirdParty "${Fetch3rdPartyDir}/../3rdparty" ABSOLUTE)
    set(SRC_DIR ${thirdParty}/${name}-${version})
    set(${name}_SOURCE_DIR ${SRC_DIR} PARENT_SCOPE)
    set(${name}_VERSION ${version} PARENT_SCOPE)
  if (NOT (EXISTS ${SRC_DIR}))
    FetchContent_Populate(${name}
      URL ${url}
      URL_HASH ${url_hash}
      SOURCE_DIR ${SRC_DIR}
      SUBBUILD_DIR ${thirdParty}/CMakeArtifacts/${name}-sub-${version}
      BINARY_DIR ${thirdParty}/CMakeArtifacts/${name}-${version})
  endif()
endmacro()


function(Fetch3rdParty)
  include(CMake/FetchContentLocal.cmake)
  set(FETCHCONTENT_QUIET OFF)

  Fetch3rdParty_Package(aws-cpp-sdk       1.7.282    https://codeload.github.com/aws/aws-sdk-cpp/tar.gz/1.7.282                           SHA256=e325497a93614b0980df8c294192150e81dc2fe55269b3a6945ee8a381247da4)
  Fetch3rdParty_Package(gtest             1.10.0     https://github.com/google/googletest/archive/release-1.10.0.tar.gz                   SHA256=9dc9157a9a1551ec7a7e43daea9a694a0bb5fb8bec81235d8a1e6ef64c716dcb)
  Fetch3rdParty_Package(jsoncpp           1.8.4      https://github.com/open-source-parsers/jsoncpp/archive/1.8.4.tar.gz                  MD5=fa47a3ab6b381869b6a5f20811198662)
  Fetch3rdParty_Package(fmt               6.0.0      https://github.com/fmtlib/fmt/archive/6.0.0.tar.gz                                   SHA256=f1907a58d5e86e6c382e51441d92ad9e23aea63827ba47fd647eacc0d3a16c78)
  Fetch3rdParty_Package(cpprestapi        2.10.14    https://github.com/microsoft/cpprestsdk/archive/v2.10.14.tar.gz                      SHA256=f2628b248f714d7bbd6a536553bc3782602c68ca1b129017985dd70cc3515278)
  Fetch3rdParty_Package(azure-storage-cpp 7.0.0      https://github.com/Azure/azure-storage-cpp/archive/v7.0.0.tar.gz                     SHA256=2129511dcf40383de5ac643abf68cad4907b5cdbfe471c75ba264a83e3078dc7)
  Fetch3rdParty_Package(pybind11          2.4.3      https://github.com/pybind/pybind11/archive/v2.4.3.tar.gz                             SHA256=1eed57bc6863190e35637290f97a20c81cfe4d9090ac0a24f3bbf08f265eb71d)
  Fetch3rdParty_Package(curl              7.68.0     https://github.com/curl/curl/releases/download/curl-7_68_0/curl-7.68.0.tar.gz        SHA256=1dd7604e418b0b9a9077f62f763f6684c1b092a7bc17e3f354b8ad5c964d7358)
  Fetch3rdParty_Package(libuv             1.34.1     https://github.com/libuv/libuv/archive/v1.34.1.tar.gz                                SHA256=e3e0105c9b26e181e0547607cb6893462beb0c652674c3795766b2e5555288b3)
  Fetch3rdParty_Package(zlib              1.2.11     http://zlib.net/zlib-1.2.11.tar.gz                                                   SHA256=c3e5e9fdd5004dcb542feda5ee4f0ff0744628baf8ed2dd5d66f8ca1197cb1a1)
  Fetch3rdParty_Package(absl              20200225.2 https://codeload.github.com/abseil/abseil-cpp/tar.gz/20200225.2                      SHA256=f41868f7a938605c92936230081175d1eae87f6ea2c248f41077c8f88316f111)
  Fetch3rdParty_Package(crc32c            1.1.1      https://codeload.github.com/google/crc32c/tar.gz/1.1.1                               SHA256=a6533f45b1670b5d59b38a514d82b09c6fb70cc1050467220216335e873074e8)
  Fetch3rdParty_Package(google-cloud-cpp  1.14.0     https://codeload.github.com/googleapis/google-cloud-cpp/tar.gz/v1.14.0               SHA256=839b2d4dcb36a671734dac6b30ea8c298bbeaafcf7a45ee4a7d7aa5986b16569)
  Fetch3rdParty_File(testng  java         6.14.3 jar https://repo1.maven.org/maven2/org/testng/testng/6.14.3/testng-6.14.3.jar            MD5=9f17a8f9e99165e148c42b21f4b63d7c) 
  Fetch3rdParty_File(jcommander java      1.72 jar   https://repo1.maven.org/maven2/com/beust/jcommander/1.72/jcommander-1.72.jar         MD5=9fde6bc0ba1032eceb7267fd1ad1657b) 
  
endfunction()

