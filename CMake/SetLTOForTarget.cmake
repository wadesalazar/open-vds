function(setLTOForTarget target)
  set_target_properties(${target}
    PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
  )
endfunction()
