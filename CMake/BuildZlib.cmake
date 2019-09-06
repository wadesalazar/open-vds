macro(BuildZlib)
  if (WIN32)
    list(APPEND ZLIB_LIBS_LIST "lib/zlib.lib")
    list(APPEND ZLIB_DLLS_LIST "bin/zlib.dll")
  elseif (APPLE)
    list(APPEND ZLIB_LIBS_LIST "lib/zlib.dylib")
  else()
    list(APPEND ZLIB_LIBS_LIST "lib/zlib.so")
  endif()

  BuildExternal(zlib ${zlib_VERSION} ${zlib_SOURCE_DIR} "${ZLIB_LIBS_LIST}" "${ZLIB_DLLS_LIST}" "")
endmacro()


