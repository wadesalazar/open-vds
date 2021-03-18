function(setCompilerFlagsForTools target)
  set_target_properties(${target} PROPERTIES FOLDER tools)
  setWarningFlagsForTarget(${target})
  copyDllForTarget(${target})

  addSystemHeadersToTarget(${target} "${jsoncpp_SOURCE_DIR}/include")

  install(TARGETS ${target}
    RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR}
    )

  set_target_properties(${target} PROPERTIES INSTALL_RPATH "$ORIGIN/../lib${LIBSUFFIX}")
  target_compile_definitions(${target} PRIVATE PROJECT_NAME="${CMAKE_PROJECT_NAME}" PROJECT_VERSION="${PROJECT_VERSION}")
endfunction()

add_subdirectory(SEGYImport)
add_subdirectory(SEGYExport)
add_subdirectory(VDSInfo)
