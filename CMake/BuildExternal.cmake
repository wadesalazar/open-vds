function(GetRootInstallDir var name version)
  set(${var} "${PROJECT_BINARY_DIR}/${name}_${version}_install" PARENT_SCOPE)
endfunction()

function(BuildExternal name version source_dir install_libs_release runtime_libs_release install_libs_debug runtime_libs_debug cmake_args)

  GetRootInstallDir(INSTALL_INT ${name} ${version})

  get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if (${_isMultiConfig}) ##we don't need build byproducts for visual studio or xcode
    set(INSTALL_INT_CONFIG "${INSTALL_INT}/$<CONFIG>")
  else()
    set(CMAKE_BUILD_TYPE_ARG "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE};")
    set(INSTALL_INT_CONFIG "${INSTALL_INT}/${CMAKE_BUILD_TYPE}")
    foreach (LIB IN LISTS install_libs_release)
      list(APPEND BUILDBYPRODUCTS "${INSTALL_INT_CONFIG}/${LIB}")
    endforeach()
    foreach (LIB IN LISTS install_libs_debug)
      list(APPEND BUILDBYPRODUCTS "${INSTALL_INT_CONFIG}/${LIB}")
    endforeach()
  endif()

  if (NOT install_libs_debug)
    list(APPEND install_libs_debug ${install_libs_release})
  endif()

  if (NOT runtime_libs_debug)
    list(APPEND runtime_libs_debug ${runtime_libs_release})
  endif()

  set(${name}_INSTALL_INT_CONFIG "${INSTALL_INT_CONFIG}" PARENT_SCOPE)

  foreach (LIB IN LISTS install_libs_release)
    set_property(GLOBAL APPEND PROPERTY OPENVDS_LINK_LIBRARIES_RELEASE "${INSTALL_INT_CONFIG}/${LIB}")
  endforeach()
  foreach (LIB IN LISTS install_libs_debug)
    set_property(GLOBAL APPEND PROPERTY OPENVDS_LINK_LIBRARIES_DEBUG "${INSTALL_INT_CONFIG}/${LIB}")
  endforeach()

  foreach (LIB IN LISTS runtime_libs_release)
    set_property(GLOBAL APPEND PROPERTY OPENVDS_RUNTIME_LIBS_RELEASE "${INSTALL_INT_CONFIG}/${LIB}")
  endforeach()
  foreach (LIB IN LISTS runtime_libs_debug)
    set_property(GLOBAL APPEND PROPERTY OPENVDS_RUNTIME_LIBS_DEBUG "${INSTALL_INT_CONFIG}/${LIB}")
  endforeach()

  set_property(GLOBAL APPEND PROPERTY OPENVDS_INCLUDE_LIBRARIES "${INSTALL_INT_CONFIG}/include")

  if (cmake_args)
    set(cmake_arg_complete "${cmake_args};")
  endif()
  set(cmake_arg_complete "${cmake_arg_complete}${CMAKE_BUILD_TYPE_ARG}")
  set(cmake_arg_complete "${cmake_arg_complete}-DCMAKE_INSTALL_PREFIX=${INSTALL_INT_CONFIG}")
  include(ExternalProject)
  ExternalProject_Add(${name}
    PREFIX ${PROJECT_BINARY_DIR}/${name}_${version}
    SOURCE_DIR ${source_dir}
    BUILD_IN_SOURCE OFF
    CMAKE_ARGS ${cmake_arg_complete}
    BUILD_BYPRODUCTS ${BUILDBYPRODUCTS})
endfunction()

