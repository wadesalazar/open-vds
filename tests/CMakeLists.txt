include (${PROJECT_SOURCE_DIR}/CMake/CopyDllForTarget.cmake)

include(GoogleTest)
set(BUILD_GMOCK OFF CACHE BOOL "We dont need mock" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Dont install" FORCE)
add_subdirectory(${gtest_SOURCE_DIR} ${PROJECT_BINARY_DIR}/gtest)
set_target_properties(gtest PROPERTIES
  FOLDER ExternalProjectTargets/GoogleTest)
set_target_properties(gtest_main PROPERTIES
  FOLDER ExternalProjectTargets/GoogleTest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)

macro(add_test_executable name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} openvds_objects gtest gtest_main)
  target_compile_definitions(${name} PRIVATE openvds_EXPORTS)
  copyDllForTarget(${name})
  gtest_discover_tests(${name})
  set_target_properties(${name} PROPERTIES FOLDER tests)
endmacro()

add_test_executable(io_tests
  io/filetest.cpp)

add_test_executable(jsoncpp_tests
  jsoncpp/jsoncpp_parse.cpp
  jsoncpp/jsoncpp_serialize.cpp)

add_test_executable(segy_tests
  SEG-Y/SEGYScanTest.cpp
  ../src/SEG-Y/SEGY.cpp
  ../src/SEG-Y/SEGYFileInfo.cpp
  ../src/SEG-Y/SEGYFileInfo.h)
target_compile_definitions(segy_tests PRIVATE -DTEST_DATA_PATH="${TEST_DATA_PATH}")
target_include_directories(segy_tests PRIVATE ../src/SEG-Y)

add_test_executable(vds_integration_tests
  VDS/VDSIntegrationTest.cpp
  VDS/ParseVDSJsonTest.cpp
  VDS/DeserializeVolumeDataTest.cpp
  )
target_compile_definitions(vds_integration_tests PRIVATE -DTEST_DATA_PATH="${PROJECT_SOURCE_DIR}/tests/VDS")
target_compile_definitions(vds_integration_tests PRIVATE -DTEST_AWS_REGION="${TEST_AWS_REGION}")
target_compile_definitions(vds_integration_tests PRIVATE -DTEST_AWS_BUCKET="${TEST_AWS_BUCKET}")
target_compile_definitions(vds_integration_tests PRIVATE -DTEST_AWS_OBJECTID="${TEST_AWS_OBJECTID}")
