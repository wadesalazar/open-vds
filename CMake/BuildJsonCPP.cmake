macro(BuildJsonCpp)
  if (WIN32)
    list(APPEND JSONCPP_LIBS_NAMES "lib/jsoncpp.lib")
  else()
    list(APPEND JSONCPP_LIBS_NAMES "lib/libjsoncpp.a")
  endif()
  BuildExternal(jsoncpp ${jsoncpp_VERSION} ${jsoncpp_SOURCE_DIR} ${JSONCPP_LIBS_NAMES} "" "-DJSONCPP_WITH_TESTS=OFF;-DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF;-DJSONCPP_WITH_PKGCONFIG_SUPPORT=OFF")
endmacro()

