set(CPPRESTSDK_INCLUDE_DIRECTORIES ${cpp-rest-api_INSTALL_INT_CONFIG}/include)

if(WIN32)
  set(TOOLSET)
  if(CMAKE_VS_PLATFORM_TOOLSET)
    string(REGEX REPLACE "^v" "" TOOLSET "${CMAKE_VS_PLATFORM_TOOLSET}")
  endif()
endif()

set(CPPRESTSDK_IMPL_LIB_RELEASE ${cpp-rest-api_INSTALL_INT_CONFIG}/lib/cpprest${TOOLSET}_2_10.lib)
set(CPPRESTSDK_IMPL_LIB_DEBUG ${cpp-rest-api_INSTALL_INT_CONFIG}/lib/cpprest${TOOLSET}_2_10d.lib)

set(CPPRESTSDK_LOCATION_RELEASE ${cpp-rest-api_INSTALL_INT_CONFIG}/bin/cpprest${TOOLSET}_2_10.dll)
set(CPPRESTSDK_LOCATION_DEBUG ${cpp-rest-api_INSTALL_INT_CONFIG}/bin/cpprest${TOOLSET}_2_10d.dll)

if (NOT TARGET cpprestsdk::cpprest)
  add_library(cpprestsdk::cpprest SHARED IMPORTED)
  set_target_properties(cpprestsdk::cpprest
    PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES      "${CPPRESTSDK_INCLUDE_DIRECTORIES}"
    IMPORTED_IMPLIB_RELEASE            "${CPPRESTSDK_IMPL_LIB_RELEASE}"
    IMPORTED_IMPLIB_DEBUG              "${CPPRESTSDK_IMPL_LIB_DEBUG}"
    IMPORTED_LOCATION_RELEASE          "${CPPRESTSDK_LOCATION_RELEASE}"
    IMPORTED_LOCATION_DEBUG            "${CPPRESTSDK_LOCATION_DEBUG}"
    MAP_IMPORTED_CONFIG_MINSIZEREL     Release
    MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
  )
endif()

set(cpprestsdk_FOUND ON)
