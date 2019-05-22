include (${PROJECT_SOURCE_DIR}/CMake/CopyDllForTarget.cmake)

include(GoogleTest)
set(BUILD_GMOCK OFF CACHE BOOL "We dont need mock" FORCE)
set(gtest_force_shared_crt ON)
set(INSTALL_GTEST OFF CACHE BOOL "Dont install" FORCE)
add_subdirectory(${gtest_SOURCE_DIR} ${PROJECT_BINARY_DIR}/gtest)
set_target_properties(gtest PROPERTIES
  FOLDER ExternalProjectTargets/GoogleTest)
set_target_properties(gtest_main PROPERTIES
  FOLDER ExternalProjectTargets/GoogleTest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)

add_executable(io_tests
  io/filetest.cpp
  io/s3test.cpp)
target_link_libraries(io_tests openvds Threads::Threads gtest gtest_main)
copyDllForTarget(io_tests)
gtest_discover_tests(io_tests)
set_target_properties(io_tests PROPERTIES FOLDER tests)

add_executable(jsoncpp_tests
  jsoncpp/jsoncpp_parse.cpp
  jsoncpp/jsoncpp_serialize.cpp)
target_link_libraries(jsoncpp_tests openvds Threads::Threads gtest gtest_main ${jsoncpp_LIBS})
target_include_directories(jsoncpp_tests PRIVATE ${jsoncpp_INCLUDE_PATH})
set_target_properties(jsoncpp_tests PROPERTIES FOLDER tests)
