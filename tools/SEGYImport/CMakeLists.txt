add_executable(SEGYImport
  SEGYImport.cpp
  )

target_link_libraries(SEGYImport PRIVATE Threads::Threads openvds segyutils jsoncpp_lib_static fmt::fmt)
set_target_properties(SEGYImport PROPERTIES FOLDER tools)

copyDllForTarget(SEGYImport)

if (${MSVC_TOOLSET_VERSION_LOCAL})
  set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION_LOCAL}")
endif()

install(TARGETS SEGYImport
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR}
)
