macro(BuildZlib)
  if (WIN32)
    list(APPEND ZLIB_LIBS_LIST "lib/zlib.lib")
    list(APPEND ZLIB_DLLS_LIST "bin/zlib.dll")
  elseif (APPLE)
    list(APPEND ZLIB_LIBS_LIST "lib/libz.dylib")
    list(APPEND ZLIB_LIBS_LIST "lib/libz.dylib.1")
    list(APPEND ZLIB_LIBS_LIST "lib/libz.dylib.1.2.11")
  else()
    list(APPEND ZLIB_LIBS_LIST "lib/libz.so")
    list(APPEND ZLIB_LIBS_LIST "lib/libz.so.1")
    list(APPEND ZLIB_LIBS_LIST "lib/libz.so.1.2.11")
  endif()

  BuildExternal(zlib ${zlib_VERSION} ${zlib_SOURCE_DIR} "${ZLIB_LIBS_LIST}" "${ZLIB_DLLS_LIST}" "")
endmacro()


