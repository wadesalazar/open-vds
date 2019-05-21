macro(Fetch3rdParty_Package name version url md5)
  set(SRC_DIR ${PROJECT_SOURCE_DIR}/3rdparty/${name}-${version})
  set(${name}_SOURCE_DIR ${SRC_DIR} PARENT_SCOPE)
  FetchContent_Declare(${name}
    URL ${url}
    URL_MD5 ${md5}
    SOURCE_DIR ${SRC_DIR})
  if (NOT (EXISTS ${SRC_DIR}))
    FetchContent_Populate(${name})
  endif()
endmacro()


function(Fetch3rdParty)
  include(CMake/FetchContentLocal.cmake)
  set(FETCHCONTENT_QUIET OFF)

  Fetch3rdParty_Package(aws-cpp-sdk 1.7.90     https://codeload.github.com/aws/aws-sdk-cpp/zip/1.7.90               d2b923b1ea11e780365cbec0fc627b7f)
  Fetch3rdParty_Package(gtest       1.8.1      https://github.com/google/googletest/archive/release-1.8.1.tar.gz    2e6fbeb6a91310a16efe181886c59596)
endfunction()

