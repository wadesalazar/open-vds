include (${PROJECT_SOURCE_DIR}/CMake/CopyDllForTarget.cmake)

set(SOURCE_FILES
    SEGYFileInfo.cpp
    SEGYScan.cpp)

set(PRIVATE_HEADER_FILES
    SEGYFileInfo.h)

add_executable(SEGYScan
    ${PRIVATE_HEADER_FILES}
    ${SOURCE_FILES}
    )

target_include_directories(SEGYScan PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(SEGYScan PUBLIC Threads::Threads openvds jsoncpp_lib_static)

copyDllForTarget(SEGYScan)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/Dist/OpenVDS CACHE STRING "" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if (${MSVC_TOOLSET_VERSION})
    set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION}")
endif()

install(TARGETS SEGYScan
    RUNTIME
        DESTINATION bin${LIB_TOOLSET_DIR}
)
