function(setCompilerFlagsForTools target)
  set_target_properties(${target} PROPERTIES FOLDER tools)
  setWarningFlagsForTarget(${target})
  copyDllForTarget(${target})

  if (MSVC AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    target_compile_options(${target} BEFORE PRIVATE "SHELL:/imsvc\ ${jsoncpp_SOURCE_DIR}/include")
  endif()

  if (${MSVC_TOOLSET_VERSION_LOCAL})
    set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION_LOCAL}")
  endif()

  install(TARGETS ${target}
    RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR}
    )
endfunction()

add_subdirectory(SEGYImport)
add_subdirectory(SEGYExport)
add_subdirectory(VDSInfo)
