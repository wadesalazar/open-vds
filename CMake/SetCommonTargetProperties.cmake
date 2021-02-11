function(setCommonTargetProperties target)
  setWarningFlagsForTarget(${target})
  setVersionForTarget(${target})
  setLTOForTarget(${target})
  setCompilerFlagsForTarget(${target})
endfunction()

function(setExportedHeadersForTarget target)
  set_target_properties(${target}
    PROPERTIES
    FRAMEWORK ON
    PUBLIC_HEADER "${ARGN}"
  )
endfunction()
