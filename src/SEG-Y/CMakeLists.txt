include (${PROJECT_SOURCE_DIR}/CMake/CopyDllForTarget.cmake)

add_executable(SEGYScan
    SEGY.h
    SEGY.cpp
    SEGYFileInfo.h
    SEGYFileInfo.cpp
    SEGYScan.cpp)

add_executable(SEGYUpload
    SEGY.h
    SEGY.cpp
    SEGYFileInfo.h
  	SEGYFileInfo.cpp
    SEGYUpload.cpp
    )
	
target_include_directories(SEGYScan PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(SEGYScan PUBLIC Threads::Threads openvds jsoncpp_lib_static fmt::fmt)

copyDllForTarget(SEGYScan)

target_include_directories(SEGYUpload PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(SEGYUpload PUBLIC Threads::Threads openvds jsoncpp_lib_static fmt::fmt)

copyDllForTarget(SEGYUpload)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/Dist/OpenVDS CACHE STRING "" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if (${MSVC_TOOLSET_VERSION})
    set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION}")
endif()

install(TARGETS SEGYScan SEGYUpload
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR}
)
