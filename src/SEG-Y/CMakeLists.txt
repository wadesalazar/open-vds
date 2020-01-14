include (${PROJECT_SOURCE_DIR}/CMake/CopyDllForTarget.cmake)

add_executable(SEGYImport
    SEGY.h
    SEGY.cpp
    SEGYFileInfo.h
  	SEGYFileInfo.cpp
    SEGYImport.cpp
    )

add_executable(SEGYExport
    SEGY.h
    SEGY.cpp
    SEGYExport.cpp
    )

target_include_directories(SEGYImport PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(SEGYImport PUBLIC Threads::Threads openvds jsoncpp_lib_static fmt::fmt)
set_target_properties(SEGYImport PROPERTIES FOLDER tools)

copyDllForTarget(SEGYImport)

target_include_directories(SEGYExport PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(SEGYExport PUBLIC Threads::Threads openvds jsoncpp_lib_static fmt::fmt)
set_target_properties(SEGYExport PROPERTIES FOLDER tools)

copyDllForTarget(SEGYExport)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/Dist/OpenVDS CACHE STRING "" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if (${MSVC_TOOLSET_VERSION_LOCAL})
  set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION_LOCAL}")
endif()

install(TARGETS SEGYImport
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR}
)
