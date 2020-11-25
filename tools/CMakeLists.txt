function(setCompilerFlagsForTools target)
  set_target_properties(${target} PROPERTIES FOLDER tools)
  setWarningFlagsForTarget(${target})
  copyDllForTarget(${target})

  addSystemHeadersToTarget(${target} "${jsoncpp_SOURCE_DIR}/include")

  install(TARGETS ${target}
    RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR}
    )
endfunction()

add_subdirectory(SEGYImport)
add_subdirectory(SEGYExport)
add_subdirectory(VDSInfo)
