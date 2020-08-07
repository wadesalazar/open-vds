pybind11_add_module(core MODULE SYSTEM
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

if (MSVC AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
  target_compile_options(core BEFORE PRIVATE "SHELL:/imsvc\ \"${PYBIND11_INCLUDE_DIR}\" /imsvc\ \"${PYTHON_INCLUDE_DIRS}\"")
endif()

set_target_properties(core
  PROPERTIES
  VERSION ${CMAKE_PROJECT_VERSION}
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
setWarningFlagsForTarget(core)

target_link_libraries(core PRIVATE openvds)
set_target_properties(core PROPERTIES INSTALL_RPATH "\\$\{ORIGIN\}/../lib")

install(TARGETS core LIBRARY DESTINATION python/openvds)
install(FILES ${pythonsources} DESTINATION python/openvds)

if (WIN32)
  get_property(runtime_3rdparty_release GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_RELEASE)
  get_property(runtime_3rdparty_debug   GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_DEBUG)
  install(FILES ${runtime_3rdparty_release} CONFIGURATIONS Release RelWithDebInfo MinSizeRel DESTINATION python/openvds)
  install(FILES ${runtime_3rdparty_debug} CONFIGURATIONS Debug DESTINATION python/openvds)
  install(FILES $<TARGET_FILE:openvds> DESTINATION python/openvds)
endif()
