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
  Fetch3rdParty_Package(fmt               7.1.3      https://github.com/fmtlib/fmt/archive/7.1.3.tar.gz                                   SHA256=5cae7072042b3043e12d53d50ef404bbb76949dad1de368d7f993a15c8c05ecc)
  Fetch3rdParty_Package(cpprestapi        2.10.16    https://github.com/microsoft/cpprestsdk/archive/v2.10.16.tar.gz                      SHA256=3d75e17c7d79131320438f2a15331f7ca6281c38c0e2daa27f051e290eeb8681)
  Fetch3rdParty_Package(azure-storage-cpp 7.5.0      https://github.com/Azure/azure-storage-cpp/archive/v7.5.0.tar.gz                     SHA256=446a821d115949f6511b7eb01e6a0e4f014b17bfeba0f3dc33a51750a9d5eca5)
  Fetch3rdParty_Package(pybind11          2.6.2      https://github.com/pybind/pybind11/archive/v2.6.2.tar.gz                             SHA256=8ff2fff22df038f5cd02cea8af56622bc67f5b64534f1b83b9f133b8366acff2)
  Fetch3rdParty_Package(curl              7.73.0     https://github.com/curl/curl/releases/download/curl-7_73_0/curl-7.73.0.tar.gz        SHA256=ba98332752257b47b9dea6d8c0ad25ec1745c20424f1dd3ff2c99ab59e97cf91)
  Fetch3rdParty_Package(libuv             1.34.1     https://github.com/libuv/libuv/archive/v1.34.1.tar.gz                                SHA256=e3e0105c9b26e181e0547607cb6893462beb0c652674c3795766b2e5555288b3)
  Fetch3rdParty_Package(zlib              1.2.11     http://zlib.net/zlib-1.2.11.tar.gz                                                   SHA256=c3e5e9fdd5004dcb542feda5ee4f0ff0744628baf8ed2dd5d66f8ca1197cb1a1)
  Fetch3rdParty_Package(libressl          3.3.0      https://cdn.openbsd.org/pub/OpenBSD/LibreSSL/libressl-3.3.0.tar.gz                   SHA256=728a5edc63ccb418e9166bed11d4b43e9629ff1c4d42a39b649347a20416fad6)
  Fetch3rdParty_Package(absl              20200225.2 https://codeload.github.com/abseil/abseil-cpp/tar.gz/20200225.2                      SHA256=f41868f7a938605c92936230081175d1eae87f6ea2c248f41077c8f88316f111)
  Fetch3rdParty_Package(crc32c            1.1.1      https://codeload.github.com/google/crc32c/tar.gz/1.1.1                               SHA256=a6533f45b1670b5d59b38a514d82b09c6fb70cc1050467220216335e873074e8)
  Fetch3rdParty_Package(google-cloud-cpp  1.14.0     https://codeload.github.com/googleapis/google-cloud-cpp/tar.gz/v1.14.0               SHA256=839b2d4dcb36a671734dac6b30ea8c298bbeaafcf7a45ee4a7d7aa5986b16569)
  Fetch3rdParty_Package(dms               c7ba5398   https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/seismic-dms-suite/seismic-store-cpp-lib/-/archive/c7ba5398eca0f131fbd9bf107e3148d5fd1c512c/seismic-store-cpp-lib-master.tar.gz SHA256=c1ab5348e255246e8e8d23f07dba9f6c3cbcd0b5beb5e48a7138ebbefdf79d8d)
  Fetch3rdParty_File(testng  java         6.14.3 jar https://repo1.maven.org/maven2/org/testng/testng/6.14.3/testng-6.14.3.jar            MD5=9f17a8f9e99165e148c42b21f4b63d7c)
  Fetch3rdParty_File(jcommander java      1.72 jar   https://repo1.maven.org/maven2/com/beust/jcommander/1.72/jcommander-1.72.jar         MD5=9fde6bc0ba1032eceb7267fd1ad1657b)
  Fetch3rdParty_FileTarget(google_nlohmann google/cloud/storage/internal nlohmann_json.hpp 3.4.0  https://raw.githubusercontent.com/nlohmann/json/v3.4.0/single_include/nlohmann/json.hpp MD5=27f3760c1d3a0fff7d8a2407d8db8f9d)

endfunction()

