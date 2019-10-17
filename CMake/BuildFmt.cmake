function(BuildFmt)
  add_subdirectory(${fmt_SOURCE_DIR} ${PROJECT_BINARY_DIR}/fmt_${fmt_VERSION} EXCLUDE_FROM_ALL)
  set_target_properties(fmt PROPERTIES FOLDER ExternalProjectTargets/fmt)
endfunction()
