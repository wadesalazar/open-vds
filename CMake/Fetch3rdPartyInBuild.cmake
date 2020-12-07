#This file depends on being in open-vds/CMake directory
set(Fetch3rdPartyDirInternal "${CMAKE_CURRENT_LIST_DIR}")

macro(Fetch3rdParty_File name dir_prefix version ext url url_hash)
  if (OPENVDS_3RD_PARTY_DIR)
    set(Fetch3rdPartyDir "${OPENVDS_3RD_PARTY_DIR}")
  else()
    set(Fetch3rdPartyDir "${Fetch3rdPartyDirInternal}/../3rdparty")
  endif()
  get_filename_component(thirdParty "${Fetch3rdPartyDir}/${dir_prefix}" ABSOLUTE)
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

macro(Fetch3rdParty_FileTarget name dir_prefix dest version url url_hash)
  if (OPENVDS_3RD_PARTY_DIR)
    set(Fetch3rdPartyDir "${OPENVDS_3RD_PARTY_DIR}")
  else()
    set(Fetch3rdPartyDir "${Fetch3rdPartyDirInternal}/../3rdparty")
  endif()
  get_filename_component(thirdParty "${Fetch3rdPartyDir}/${name}-${version}/${dir_prefix}" ABSOLUTE)
  file(MAKE_DIRECTORY ${thirdParty})
  set(DEST_FILE ${thirdParty}/${dest})
  set(${name}_SOURCE_FILE ${DEST_FILE} PARENT_SCOPE)
  set(${name}_SOURCE_DIR  ${Fetch3rdPartyDir}/${name}-${version} PARENT_SCOPE)
  set(${name}_VERSION ${version} PARENT_SCOPE)
  if (NOT (EXISTS ${DEST_FILE}))
    file(DOWNLOAD ${url}
      ${DEST_FILE}
      SHOW_PROGRESS
      EXPECTED_HASH ${url_hash}
      )
  endif()

endmacro()

macro(Fetch3rdParty_Package name version url url_hash)
  if (OPENVDS_3RD_PARTY_DIR)
    set(Fetch3rdPartyDir "${OPENVDS_3RD_PARTY_DIR}")
  else()
    set(Fetch3rdPartyDir "${Fetch3rdPartyDirInternal}/../3rdparty")
  endif()
    get_filename_component(thirdParty "${Fetch3rdPartyDir}" ABSOLUTE)
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

  Fetch3rdParty_Package(aws-cpp-sdk       1.8.96     https://codeload.github.com/aws/aws-sdk-cpp/tar.gz/1.8.96                            SHA256=b03abd6384c54562b6365bb8579f2d58ff11817ade28e1229d6969f48bbf92fe)
  Fetch3rdParty_Package(gtest             1.10.0     https://github.com/google/googletest/archive/release-1.10.0.tar.gz                   SHA256=9dc9157a9a1551ec7a7e43daea9a694a0bb5fb8bec81235d8a1e6ef64c716dcb)
  Fetch3rdParty_Package(jsoncpp           1.8.4      https://github.com/open-source-parsers/jsoncpp/archive/1.8.4.tar.gz                  MD5=fa47a3ab6b381869b6a5f20811198662)
  Fetch3rdParty_Package(fmt               6.0.0      https://github.com/fmtlib/fmt/archive/6.0.0.tar.gz                                   SHA256=f1907a58d5e86e6c382e51441d92ad9e23aea63827ba47fd647eacc0d3a16c78)
  Fetch3rdParty_Package(cpprestapi        2.10.16    https://github.com/microsoft/cpprestsdk/archive/v2.10.16.tar.gz                      SHA256=3d75e17c7d79131320438f2a15331f7ca6281c38c0e2daa27f051e290eeb8681)
  Fetch3rdParty_Package(azure-storage-cpp 7.5.0      https://github.com/Azure/azure-storage-cpp/archive/v7.5.0.tar.gz                     SHA256=446a821d115949f6511b7eb01e6a0e4f014b17bfeba0f3dc33a51750a9d5eca5)
  Fetch3rdParty_Package(pybind11          2.4.3      https://github.com/pybind/pybind11/archive/v2.4.3.tar.gz                             SHA256=1eed57bc6863190e35637290f97a20c81cfe4d9090ac0a24f3bbf08f265eb71d)
  Fetch3rdParty_Package(curl              7.73.0     https://github.com/curl/curl/releases/download/curl-7_73_0/curl-7.73.0.tar.gz        SHA256=ba98332752257b47b9dea6d8c0ad25ec1745c20424f1dd3ff2c99ab59e97cf91)
  Fetch3rdParty_Package(libuv             1.34.1     https://github.com/libuv/libuv/archive/v1.34.1.tar.gz                                SHA256=e3e0105c9b26e181e0547607cb6893462beb0c652674c3795766b2e5555288b3)
  Fetch3rdParty_Package(zlib              1.2.11     http://zlib.net/zlib-1.2.11.tar.gz                                                   SHA256=c3e5e9fdd5004dcb542feda5ee4f0ff0744628baf8ed2dd5d66f8ca1197cb1a1)
  Fetch3rdParty_Package(libressl          3.3.0      https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-3.3.0.tar.gz                   SHA256=728a5edc63ccb418e9166bed11d4b43e9629ff1c4d42a39b649347a20416fad6)
  Fetch3rdParty_Package(absl              20200923.2 https://codeload.github.com/abseil/abseil-cpp/tar.gz/20200923.2                      SHA256=bf3f13b13a0095d926b25640e060f7e13881bd8a792705dd9e161f3c2b9aa976)
  Fetch3rdParty_Package(crc32c            1.1.1      https://codeload.github.com/google/crc32c/tar.gz/1.1.1                               SHA256=a6533f45b1670b5d59b38a514d82b09c6fb70cc1050467220216335e873074e8)
  Fetch3rdParty_Package(google-cloud-cpp  1.21.0     https://codeload.github.com/googleapis/google-cloud-cpp/tar.gz/v1.21.0               SHA256=14bf9bf97431b890e0ae5dca8f8904841d4883b8596a7108a42f5700ae58d711)
  Fetch3rdParty_File(testng  java         6.14.3 jar https://repo1.maven.org/maven2/org/testng/testng/6.14.3/testng-6.14.3.jar            MD5=9f17a8f9e99165e148c42b21f4b63d7c)
  Fetch3rdParty_File(jcommander java      1.72 jar   https://repo1.maven.org/maven2/com/beust/jcommander/1.72/jcommander-1.72.jar         MD5=9fde6bc0ba1032eceb7267fd1ad1657b)
  Fetch3rdParty_FileTarget(google_nlohmann nlohmann json.hpp 3.9.1  https://raw.githubusercontent.com/nlohmann/json/v3.9.1/single_include/nlohmann/json.hpp MD5=5eabadfb8cf8fe1bf0811535c65f027f)

endfunction()

