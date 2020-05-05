add_executable(SEGYExport
  SEGYExport.cpp
  )

target_link_libraries(SEGYExport PUBLIC Threads::Threads openvds segyutils jsoncpp_lib_static fmt::fmt)
set_target_properties(SEGYExport PROPERTIES FOLDER tools)
setWarningFlagsForTarget(SEGYExport)

copyDllForTarget(SEGYExport)

if (${MSVC_TOOLSET_VERSION_LOCAL})
  set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION_LOCAL}")
endif()

install(TARGETS SEGYExport
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR}
)
