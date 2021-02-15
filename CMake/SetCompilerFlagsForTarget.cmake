function(setCompilerFlagsForTarget target)
  if (MSVC)
    if (MSVC_VERSION GREATER_EQUAL 1910 AND NOT OpenMP_CXX_FOUND)
      target_compile_options(${target} PRIVATE /permissive-)
    endif()
    if(MSVC_VERSION GREATER_EQUAL 1915)
      target_compile_options(${target} PRIVATE $<$<NOT:$<CONFIG:Release>>:/JMC>) 
    endif()
    target_compile_options(${target} PRIVATE /MP)
  else()
    set_target_properties(${target} PROPERTIES LINK_FLAGS_RELEASE -s)
  endif()
  
  if (NOT (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
    target_compile_options(${target} PRIVATE -Werror=return-type)
  endif()
endfunction()