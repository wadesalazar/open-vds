set(EXPORTED_HEADER_FILES
    SEGYUtils/DataProvider.h
    SEGYUtils/SEGY.h
    SEGYUtils/SEGYFileInfo.h
    SEGYUtils/TraceDataManager.h
)
add_library(segyutils
    ${EXPORTED_HEADER_FILES}
    SEGY.cpp
    SEGYFileInfo.cpp
    )
setWarningFlagsForTarget(segyutils)
set_target_properties(segyutils
  PROPERTIES
  FRAMEWORK ON
  VERSION ${CMAKE_PROJECT_VERSION}
  PUBLIC_HEADER "${EXPORTED_HEADER_FILES}"
  #OUTPUT_NAME ${PROJECT_NAME}
)

target_compile_definitions(segyutils PRIVATE openvds_EXPORTS)

target_link_libraries(segyutils PUBLIC openvds)

if (ENABLE_RUNPATH_ORIGIN)
set_target_properties(segyutils
  PROPERTIES
  INSTALL_RPATH  "\\$\{ORIGIN\}"
)
endif()

target_include_directories(segyutils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

if (${MSVC_TOOLSET_VERSION_LOCAL})
  set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION_LOCAL}")
endif()

install(TARGETS segyutils 
  EXPORT segyutils-export
  ARCHIVE
    DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_TOOLSET_DIR}
  LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_TOOLSET_DIR}
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR}
  FRAMEWORK
    DESTINATION framework
  PUBLIC_HEADER
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SEGYUtils
  PRIVATE_HEADER
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SEGYUtils/private)

install(
    EXPORT segyutils-export
    NAMESPACE segyutils::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/segyutils/cmake
    FILE segyutils-config.cmake
)

install(FILES ${runtime_3rdparty_release} CONFIGURATIONS Release RelWithDebInfo MinSizeRel DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR})
install(FILES ${runtime_3rdparty_debug} CONFIGURATIONS Debug DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR})

if (UNIX)
  install(FILES ${link_3rdparty_release} CONFIGURATIONS Release RelWithDebInfo MinSizeRel DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_TOOLSET_DIR})
  install(FILES ${link_3rdparty_debug} CONFIGURATIONS Debug DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_TOOLSET_DIR})
endif()

export(
    TARGETS
        segyutils 
    NAMESPACE
        segyutils::
    FILE
        segyutils-config.cmake
)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR})
include (InstallRequiredSystemLibraries)

set(OPENVDS_LIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "")
