include (${PROJECT_SOURCE_DIR}/CMake/CopyDllForTarget.cmake)
#mark_as_advanced(
#    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
#    gmock gmock_main gtest gtest_main gmock_build_tests gtest_build_samples gtest_build_tests
#    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
#)

set(gtest_force_shared_crt ON)
add_subdirectory(${GTEST_SOURCE_DIR} ${PROJECT_BINARY_DIR}/gtest)
set_target_properties(gmock PROPERTIES
    FOLDER ExternalProjectTargets)
set_target_properties(gmock_main PROPERTIES
    FOLDER ExternalProjectTargets)
set_target_properties(gtest PROPERTIES
    FOLDER ExternalProjectTargets)
set_target_properties(gtest_main PROPERTIES
    FOLDER ExternalProjectTargets)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
include(GoogleTest)

add_executable(io_tests
    io/filetest.cpp
    io/s3test.cpp
    )
target_link_libraries(io_tests openvds Threads::Threads gtest gtest_main)
copyDllForTarget(io_tests)
gtest_discover_tests(io_tests)
