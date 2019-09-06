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

  Fetch3rdParty_Package(aws-cpp-sdk 1.7.109    https://codeload.github.com/aws/aws-sdk-cpp/tar.gz/1.7.109           MD5=7496040c6a5d3681d1edf356d6ccfbd7)
  Fetch3rdParty_Package(gtest       1.8.1      https://github.com/google/googletest/archive/release-1.8.1.tar.gz    MD5=2e6fbeb6a91310a16efe181886c59596)
  Fetch3rdParty_Package(jsoncpp     1.8.4      https://github.com/open-source-parsers/jsoncpp/archive/1.8.4.tar.gz  MD5=fa47a3ab6b381869b6a5f20811198662)
  Fetch3rdParty_Package(zlib        1.2.11     http://zlib.net/zlib-1.2.11.tar.gz                                   SHA256=c3e5e9fdd5004dcb542feda5ee4f0ff0744628baf8ed2dd5d66f8ca1197cb1a1)
endfunction()

