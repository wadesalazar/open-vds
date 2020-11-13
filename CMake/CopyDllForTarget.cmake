function(copyDllForTarget target)
    if (WIN32)
      get_target_property(is_imported openvds::openvds IMPORTED)
      if (is_imported)
        get_target_property(openvds_location_debug openvds::openvds IMPORTED_LOCATION_DEBUG)
        get_target_property(openvds_location_relwithdebinfo openvds::openvds IMPORTED_LOCATION_RELWITHDEBINFO)
        get_target_property(openvds_location_release openvds::openvds IMPORTED_LOCATION_RELEASE)

        if (openvds_location_relwithdebinfo)
          set(optimized_location_file ${openvds_location_relwithdebinfo})
        elseif(openvds_location_release)
          set(optimized_location_file ${openvds_location_release})
        endif()

        if (optimized_location_file)
            get_filename_component(optimized_location ${optimized_location_file} DIRECTORY)
        endif()
        get_filename_component(debug_location ${openvds_location_debug} DIRECTORY)

        file(GLOB runtime_release "${optimized_location}/*.dll")
        file(GLOB runtime_debug "${debug_location}/*.dll")
      else()
        add_custom_command(OUTPUT "${target}_copy_vds"
          COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:openvds> $<TARGET_FILE:segyutils> $<TARGET_FILE:sdapi> $<TARGET_FILE_DIR:${target}>
          DEPENDS openvds)
        set_property(SOURCE "${target}_copy_vds"
                     PROPERTY SYMBOLIC ON
                    )
        target_sources(${target} PRIVATE ${target}_copy_vds)
        get_property(runtime_release GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_RELEASE)
        get_property(runtime_debug   GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_DEBUG)
      endif()

      foreach(file ${runtime_release})
        add_custom_command(TARGET ${target}
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<NOT:$<CONFIG:Debug>>:${file}> ${CMAKE_SOURCE_DIR}/README.md $<TARGET_FILE_DIR:${target}>)
      endforeach()
      foreach(file ${runtime_debug})
        add_custom_command(TARGET ${target}
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${file}> ${CMAKE_SOURCE_DIR}/README.md $<TARGET_FILE_DIR:${target}>)
      endforeach()
    endif()
endfunction()
