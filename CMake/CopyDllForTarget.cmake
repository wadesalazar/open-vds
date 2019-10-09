function(copyDllForTarget target)
    if (WIN32)
      add_custom_command(OUTPUT "${target}_copy_vds"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:openvds> $<TARGET_FILE_DIR:${target}>
        DEPENDS openvds
      )
      set_property(SOURCE "${target}_copy_vds"
                   PROPERTY SYMBOLIC ON
                  )
      target_sources(${target} PRIVATE ${target}_copy_vds)

      get_property(runtime_release GLOBAL PROPERTY RUNTIME_LIBS_RELEASE)
      get_property(runtime_debug   GLOBAL PROPERTY RUNTIME_LIBS_DEBUG)
      foreach(file ${runtime_release})
        add_custom_command(TARGET ${target}
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<NOT:$<CONFIG:Debug>>:${file}> ${PROJECT_SOURCE_DIR}/README.md $<TARGET_FILE_DIR:${target}>)
      endforeach()
      foreach(file ${runtime_debug})
        add_custom_command(TARGET ${target}
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${file}> ${PROJECT_SOURCE_DIR}/README.md $<TARGET_FILE_DIR:${target}>)
      endforeach()
    endif()
endfunction()
