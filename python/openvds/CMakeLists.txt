pybind11_add_module(core MODULE
    core.cpp
    PyGlobal.cpp
    PyGlobal.h
    PyGlobalMetadataCommon.cpp
    PyGlobalMetadataCommon.h
    PyKnownMetadata.cpp
    PyKnownMetadata.h
    PyMetadata.cpp
    PyMetadata.h
    PyRange.cpp
    PyRange.h
    PyVector.cpp
    PyVector.h
    PyVolumeData.cpp
    PyVolumeData.h
    PyVolumeDataAccess.cpp
    PyVolumeDataAccess.h
    PyVolumeDataAxisDescriptor.cpp
    PyVolumeDataAxisDescriptor.h
    PyVolumeDataChannelDescriptor.cpp
    PyVolumeDataChannelDescriptor.h
    PyVolumeDataLayout.cpp
    PyVolumeDataLayout.h
    PyVolumeDataLayoutDescriptor.cpp
    PyVolumeDataLayoutDescriptor.h
    PyVolumeSampler.cpp
    PyVolumeSampler.h
)

set_target_properties(core
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/..
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/..
  VS_DEBUGGER_COMMAND "${Python3_EXECUTABLE}"
  VS_DEBUGGER_ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/..;$ENV{PYTHONPATH}"
  )


SET(pythonsources
    ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
    ${CMAKE_CURRENT_SOURCE_DIR}/api.py
    ${CMAKE_CURRENT_SOURCE_DIR}/volumedataaccess.py
    ${CMAKE_CURRENT_SOURCE_DIR}/volumedataaccessors.py
)

add_custom_command(TARGET core
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${pythonsources} $<TARGET_FILE_DIR:core>)

copyDllForTarget(core)

target_include_directories(core
    PRIVATE
        ${PYBIND11_INCLUDE_DIRS}
)

target_link_libraries(core PRIVATE openvds)

get_property(runtime_3rdparty_release GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_RELEASE)
get_property(runtime_3rdparty_debug   GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_DEBUG)

install(TARGETS core LIBRARY DESTINATION python/openvds)
install(FILES ${pythonsources} DESTINATION python/openvds)
install(FILES ${runtime_3rdparty_release} CONFIGURATIONS Release RelWithDebInfo MinSizeRel DESTINATION python/openvds)
install(FILES ${runtime_3rdparty_debug} CONFIGURATIONS Debug DESTINATION python/openvds)
install(FILES $<TARGET_FILE:openvds> DESTINATION python/openvds)
