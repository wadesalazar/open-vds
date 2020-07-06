add_executable(VDSInfo VDSInfo.cpp Base64.cpp)

target_link_libraries(VDSInfo PUBLIC openvds fmt::fmt jsoncpp_lib_static)
set_target_properties(VDSInfo PROPERTIES FOLDER tools)
setWarningFlagsForTarget(VDSInfo)

copyDllForTarget(VDSInfo)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/Dist/OpenVDS CACHE STRING "" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if (${MSVC_TOOLSET_VERSION_LOCAL})
  set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION_LOCAL}")
endif()

install(TARGETS VDSInfo
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR}
)
