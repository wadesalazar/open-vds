macro(Fetch3rdParty_Package name version url url_hash)
  set(SRC_DIR ${PROJECT_SOURCE_DIR}/3rdparty/${name}-${version})
  set(${name}_SOURCE_DIR ${SRC_DIR} PARENT_SCOPE)
  set(${name}_VERSION ${version} PARENT_SCOPE)
  FetchContent_Declare(${name}
    URL ${url}
    URL_HASH ${url_hash}
    SOURCE_DIR ${SRC_DIR})
  if (NOT (EXISTS ${SRC_DIR}))
    FetchContent_Populate(${name})
  endif()
endmacro()


function(Fetch3rdParty)
  include(CMake/FetchContentLocal.cmake)
  set(FETCHCONTENT_QUIET OFF)

  Fetch3rdParty_Package(aws-cpp-sdk 1.7.213    https://codeload.github.com/aws/aws-sdk-cpp/tar.gz/1.7.213                                             SHA256=dd0485837ee2b3b46a98f439fb6799e2bb63d9a0038b0de1c2173181630b775c)
  Fetch3rdParty_Package(gtest       1.9.0      https://codeload.github.com/google/googletest/legacy.tar.gz/33a0d4f6d76a0ed6061e612848532cba82d42870   MD5=5226644d616f5a00b436b487bf0a1dde)
  Fetch3rdParty_Package(jsoncpp     1.8.4      https://github.com/open-source-parsers/jsoncpp/archive/1.8.4.tar.gz                                    MD5=fa47a3ab6b381869b6a5f20811198662)
  Fetch3rdParty_Package(fmt         6.0.0      https://github.com/fmtlib/fmt/archive/6.0.0.tar.gz                                                     SHA256=f1907a58d5e86e6c382e51441d92ad9e23aea63827ba47fd647eacc0d3a16c78)
  Fetch3rdParty_Package(cpprestapi  2.10.14   https://github.com/microsoft/cpprestsdk/archive/v2.10.14.tar.gz                                         SHA256=f2628b248f714d7bbd6a536553bc3782602c68ca1b129017985dd70cc3515278)
  Fetch3rdParty_Package(azure-storage-cpp 7.0.0      https://github.com/Azure/azure-storage-cpp/archive/v7.0.0.tar.gz                                 SHA256=2129511dcf40383de5ac643abf68cad4907b5cdbfe471c75ba264a83e3078dc7)

  if (BUILD_ZLIB)
    Fetch3rdParty_Package(zlib      1.2.11     http://zlib.net/zlib-1.2.11.tar.gz                                                                     SHA256=c3e5e9fdd5004dcb542feda5ee4f0ff0744628baf8ed2dd5d66f8ca1197cb1a1)
  endif()
endfunction()

