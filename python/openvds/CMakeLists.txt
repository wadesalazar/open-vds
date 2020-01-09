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

SET(pythonsources
    ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
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

install(TARGETS core LIBRARY DESTINATION python/openvds)
install(FILES ${pythonsources} DESTINATION python/openvds)

