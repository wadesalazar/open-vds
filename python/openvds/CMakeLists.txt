set(CMAKE_DEBUG_POSTFIX "")
pybind11_add_module(core MODULE
    core.cpp
    PyGlobal.cpp
    PyGlobal.h
    PyMetadataKey.cpp
    PyMetadataKey.h
    PyKnownMetadata.cpp
    PyKnownMetadata.h
    PyMetadataAccess.cpp
    PyMetadataAccess.h
    PyMetadataContainer.cpp
    PyMetadataContainer.h
    PyVolumeData.cpp
    PyVolumeData.h
    PyVolumeDataAccess.cpp
    PyVolumeDataAccess.h
    PyVolumeDataAccessManager.cpp
    PyVolumeDataAccessManager.h
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
    PyGlobalState.cpp
    PyGlobalState.h
)

if (MSVC AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
  target_compile_options(core BEFORE PRIVATE "SHELL:/imsvc\ \"${PYBIND11_INCLUDE_DIR}\" /imsvc\ \"${PYTHON_INCLUDE_DIRS}\"")
else ()
  target_include_directories(core SYSTEM BEFORE PRIVATE ${PYBIND11_INCLUDE_DIR})
endif()

if(WIN32)
  # In case we have the Python debug library installed it will try to pragma-link with the non-debug library. 
  # We therefore need to add the library dir to the LIBPATH.
  target_link_options(core PRIVATE "/LIBPATH:${Python3_LIBRARY_DIRS}")
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
    ${CMAKE_CURRENT_SOURCE_DIR}/asyncmethods.py
)

add_custom_target(core_py_files ALL
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${pythonsources} $<TARGET_FILE_DIR:core>)
#add_dependencies(core core_py_files)

copyDllForTarget(core)
setWarningFlagsForTarget(core)

target_link_libraries(core PRIVATE openvds)

if (OpenMP_CXX_FOUND)
    target_link_libraries(core PRIVATE OpenMP::OpenMP_CXX)
endif()

set_target_properties(core PROPERTIES INSTALL_RPATH "$ORIGIN/../../lib${LIBSUFFIX}")

set(INSTALL_DIR "python$<$<NOT:$<CONFIG:Release>>:/$<CONFIG>>/openvds")
install(TARGETS core LIBRARY DESTINATION ${INSTALL_DIR})
install(FILES ${pythonsources} DESTINATION ${INSTALL_DIR})

if (WIN32)
  get_property(runtime_3rdparty_release GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_RELEASE)
  get_property(runtime_3rdparty_debug   GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_DEBUG)
  install(FILES ${runtime_3rdparty_release} CONFIGURATIONS Release RelWithDebInfo MinSizeRel DESTINATION ${INSTALL_DIR})
  install(FILES ${runtime_3rdparty_debug} CONFIGURATIONS Debug DESTINATION ${INSTALL_DIR})
  install(FILES $<TARGET_FILE:openvds> $<TARGET_FILE:segyutils> $<TARGET_FILE:sdapi> DESTINATION ${INSTALL_DIR})
endif()
