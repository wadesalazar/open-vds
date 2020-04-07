function(BuildCppRestSdk)
  if (WIN32)
    get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (${_isMultiConfig})
      set(TOOLSET_VERSION ${MSVC_TOOLSET_VERSION_LOCAL})
    endif()
    list(APPEND CPPRESTSDK_LIBS_LIST_RELEASE "lib/cpprest${TOOLSET_VERSION}_2_10.lib")
    list(APPEND CPPRESTSDK_DLLS_LIST_RELEASE "bin/cpprest${TOOLSET_VERSION}_2_10.dll")

    list(APPEND CPPRESTSDK_LIBS_LIST_DEBUG "lib/cpprest${TOOLSET_VERSION}_2_10d.lib")
    list(APPEND CPPRESTSDK_DLLS_LIST_DEBUG "bin/cpprest${TOOLSET_VERSION}_2_10d.dll")
  elseif (APPLE)
    list(APPEND CPPRESTSDK_LIBS_LIST_RELEASE "lib/libcpprest.dylib.2.10")
    list(APPEND CPPRESTSDK_LIBS_LIST_RELEASE "lib/libcpprest.dylib")
  else()
    list(APPEND CPPRESTSDK_LIBS_LIST_RELEASE "lib${LIBSUFFIX}/libcpprest.so.2.10")
    list(APPEND CPPRESTSDK_LIBS_LIST_RELEASE "lib${LIBSUFFIX}/libcpprest.so")
  endif()
  if (BOOST_ROOT)
    set(BOOST_FLAGS ";-DBOOST_ROOT=${BOOST_ROOT};")
  elseif(BOOST_INCLUDEDIR)
    set(BOOST_FLAGS ";-DBOOST_INCLUDEDIR=${BOOST_INCLUDEDIR};-DBOOST_LIBRARYDIR=${BOOST_LIBRARYDIR};")
  endif()
  BuildExternal(cpp-rest-api ${cpprestapi_VERSION} "" ${cpprestapi_SOURCE_DIR} "${CPPRESTSDK_LIBS_LIST_RELEASE}" "${CPPRESTSDK_DLLS_LIST_RELEASE}" "${CPPRESTSDK_LIBS_LIST_DEBUG}" "${CPPRESTSDK_DLLS_LIST_DEBUG}" "-DCPPREST_EXCLUDE_WEBSOCKETS=ON;-DCPPREST_EXCLUDE_COMPRESSION=ON;-DCPPREST_EXCLUDE_BROTLI=ON;-DWERROR=OFF;-DBUILD_TESTS=OFF;-DBUILD_SAMPLES=OFF${BOOST_FLAGS}")
  set(cpp-rest-api_INSTALL_INT_CONFIG ${cpp-rest-api_INSTALL_INT_CONFIG} PARENT_SCOPE)
  #add_definitions(-DUSE_IMPORT_EXPORT)

endfunction()
