function(BuildExternal name version source_dir install_libs runtime_libs cmake_args)
  set(INSTALL_INT "${PROJECT_BINARY_DIR}/${name}_${version}_install")
  set(INSTALL_INT_CONFIG "${INSTALL_INT}/$<CONFIG>")


  get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if (${_isMultiConfig}) ##we don't need build byproducts for visual studio or xcode
    set(CMAKE_BUILD_TYPE_ARG "-DCMAKE_BUILD_TYPE=$<CONFIG>")
    set(INSTALL_INT_CONFIG "${INSTALL_INT}/$<CONFIG>")
  else()
    set(CMAKE_BUILD_TYPE_ARG "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
    set(INSTALL_INT_CONFIG "${INSTALL_INT}/${CMAKE_BUILD_TYPE}")
    foreach (LIB IN LISTS install_libs)
      list(APPEND BUILDBYPRODUCTS "${INSTALL_INT_CONFIG}/${LIB}")
    endforeach()
  endif()
  foreach (LIB IN LISTS install_libs)
    list(APPEND LIBS_LIST "${INSTALL_INT_CONFIG}/${LIB}")
  endforeach()

  foreach (LIB IN LISTS runtime_libs)
    list(APPEND RUNTIME_LIST "${INSTALL_INT_CONFIG}/${LIB}")
  endforeach()

  include(ExternalProject)
  ExternalProject_Add(${name}
    PREFIX ${PROJECT_BINARY_DIR}/${name}_${version}
    SOURCE_DIR ${source_dir}
    BUILD_IN_SOURCE OFF
    URL ""
    INSTALL_DIR ${INSTALL_INT}
    CMAKE_ARGS ${cmake_args};${CMAKE_BUILD_TYPE_ARG};-DCMAKE_INSTALL_PREFIX=${INSTALL_INT_CONFIG}
    BUILD_BYPRODUCTS ${BUILDBYPRODUCTS})

  #parent scope variables cant be used in this file
  set("${name}_INCLUDE_PATH" ${INSTALL_INT_CONFIG}/include PARENT_SCOPE)
  set("${name}_LIBS" ${LIBS_LIST} PARENT_SCOPE)
  if (RUNTIME_LIST)
    set("${name}_RUNTIME" ${RUNTIME_LIST} PARENT_SCOPE)
  endif()
endfunction()

