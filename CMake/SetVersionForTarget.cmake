function(setVersionForTarget target)
  set_target_properties(${target}
    PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}
    SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR}
  )
endfunction()
