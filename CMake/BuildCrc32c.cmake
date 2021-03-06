function(BuildCrc32c)
  if (WIN32)
    list(APPEND CRC32C_LIBS_LIST "lib/crc32c.lib")
  elseif (APPLE)
  else()
    list(APPEND CRC32C_LIBS_LIST "lib${LIBSUFFIX}/libcrc32c.a")
  endif()

  list(APPEND CRC32C_CMAKE_ARGS "-DCMAKE_POSITION_INDEPENDENT_CODE=ON")
  list(APPEND CRC32C_CMAKE_ARGS "-DBUILD_SHARED_LIBS=OFF")
  list(APPEND CRC32C_CMAKE_ARGS "-DCRC32C_BUILD_TESTS=OFF")
  list(APPEND CRC32C_CMAKE_ARGS "-DCRC32C_BUILD_BENCHMARKS=OFF")
  list(APPEND CRC32C_CMAKE_ARGS "-DCRC32C_USE_GLOG=OFF")
  BuildExternal(crc32c ${crc32c_VERSION} "" ${crc32c_SOURCE_DIR} "${CRC32C_LIBS_LIST}" "" "" "" "${CRC32C_CMAKE_ARGS}")
  set(crc32c_INSTALL_INT_CONFIG ${crc32c_INSTALL_INT_CONFIG} PARENT_SCOPE)
endfunction()
