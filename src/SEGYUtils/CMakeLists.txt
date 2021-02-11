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
setCommonTargetProperties(segyutils)
setExportedHeadersForTarget(segyutils ${EXPORTED_HEADER_FILES})
target_compile_definitions(segyutils PRIVATE openvds_EXPORTS)

target_link_libraries(segyutils PUBLIC openvds)

if (ENABLE_RUNPATH_ORIGIN)
set_target_properties(segyutils
  PROPERTIES
  INSTALL_RPATH  "$ORIGIN"
)
endif()

target_include_directories(segyutils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

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
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/segyutils
    FILE segyutils-config.cmake
)

set(RUNTIME_LIBS_DESTINATION $<IF:$<PLATFORM_ID:Windows>,${CMAKE_INSTALL_BINDIR},${CMAKE_INSTALL_LIBDIR}>${LIB_TOOLSET_DIR})
install(FILES ${runtime_3rdparty_release} CONFIGURATIONS Release RelWithDebInfo MinSizeRel DESTINATION ${RUNTIME_LIBS_DESTINATION})
install(FILES ${runtime_3rdparty_debug} CONFIGURATIONS Debug DESTINATION ${RUNTIME_LIBS_DESTINATION})


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
