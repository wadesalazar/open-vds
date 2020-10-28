function(BuildLibreSSL)
  if (USE_LIBRESSL)
    get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (${_isMultiConfig})
      set(TOOLSET_VERSION ${MSVC_TOOLSET_VERSION_LOCAL})
    endif()
    list(APPEND LIBRESSL_LIBS_LIST "lib/crypto-46.lib")
    list(APPEND LIBRESSL_LIBS_LIST "lib/tls-20.lib")
    
    list(APPEND LIBRESSL_DLLS_LIST "bin/crypto-46.dll")
    list(APPEND LIBRESSL_DLLS_LIST "bin/tls-20.dll")

    list(APPEND CMAKE_ARGS "-DLIBRESSL_APPS=OFF")
    list(APPEND CMAKE_ARGS "-DLIBRESSL_TESTS=OFF")
    list(APPEND CMAKE_ARGS "-DBUILD_SHARED_LIBS=ON")
    BuildExternal(libressl ${libressl_VERSION} "" ${libressl_SOURCE_DIR} "${LIBRESSL_LIBS_LIST}" "${LIBRESSL_DLLS_LIST}" "" "" "${CMAKE_ARGS}")
  endif()

endfunction()
