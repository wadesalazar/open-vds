include(GoogleTest)
set(BUILD_GMOCK OFF CACHE BOOL "We dont need mock" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Dont install" FORCE)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(${gtest_SOURCE_DIR} ${PROJECT_BINARY_DIR}/gtest)
set_target_properties(gtest PROPERTIES
  FOLDER ExternalProjectTargets/GoogleTest)
set_target_properties(gtest_main PROPERTIES
  FOLDER ExternalProjectTargets/GoogleTest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)

get_property(include_3rdparty      GLOBAL PROPERTY OPENVDS_INCLUDE_LIBRARIES)

function(add_test_executable name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} PRIVATE openvds_objects gtest gtest_main)
  target_include_directories(${name} SYSTEM PRIVATE ${include_3rdparty})
  target_compile_definitions(${name} PRIVATE openvds_EXPORTS)
  copyDllForTarget(${name})
  gtest_discover_tests(${name})
  set_target_properties(${name} PROPERTIES FOLDER tests)
  get_target_property(fmt_INCLUDE fmt::fmt INTERFACE_INCLUDE_DIRECTORIES)
  get_target_property(json_INCLUDE jsoncpp_lib_static INTERFACE_INCLUDE_DIRECTORIES)
  target_include_directories(${name} PRIVATE ${fmt_INCLUDE} ${json_INCLUDE} ${PROJECT_SOURCE_DIR}/src/OpenVDS)
  target_compile_definitions(${name} PRIVATE -DTEST_DATA_PATH="${PROJECT_SOURCE_DIR}/tests/VDS")
  target_compile_definitions(${name} PRIVATE -DTEST_AWS_REGION="${TEST_AWS_REGION}")
  target_compile_definitions(${name} PRIVATE -DTEST_AWS_BUCKET="${TEST_AWS_BUCKET}")
  target_compile_definitions(${name} PRIVATE -DTEST_AWS_OBJECTID="${TEST_AWS_OBJECTID}")
  string(REPLACE ";" "\\\\;" TEST_AZURE_CONNECTION_ESCAPED "${TEST_AZURE_CONNECTION}")
  target_compile_definitions(${name} PRIVATE -DTEST_AZURE_CONNECTION="${TEST_AZURE_CONNECTION_ESCAPED}")
endfunction()

add_test_executable(io_tests
  io/filetest.cpp
  io/InMemoryIo.cpp
  io/AzureIo.cpp
  )

add_test_executable(io_performance_test
  io/IoPerformance.cpp
  )

add_test_executable(io_vds_roundtrip_test
  io/IoVDSRoundtrip.cpp
)

add_test_executable(io_error_handling_tests
  io/IoErrorHandling.cpp
  )

add_test_executable(jsoncpp_tests
  jsoncpp/jsoncpp_parse.cpp
  jsoncpp/jsoncpp_serialize.cpp)

add_test_executable(fmt_tests
  fmt/basic_fmt.cpp)

add_test_executable(segy_tests
  SEG-Y/SEGYScanTest.cpp
  ../src/SEGYUtils/SEGY.cpp
  ../src/SEGYUtils/SEGYFileInfo.cpp
  ../src/SEGYUtils/SEGYUtils/SEGYFileInfo.h)
target_compile_definitions(segy_tests PRIVATE -DTEST_DATA_PATH="${TEST_DATA_PATH}")
target_include_directories(segy_tests PRIVATE ../src/SEGYUtils)

add_test_executable(vds_integration_tests
  VDS/ParseVDSJsonTest.cpp
  VDS/DeserializeVolumeDataTest.cpp
  VDS/RequestVolumeCleanupThread.cpp
  )

add_test_executable(openvds_integration_tests
  OpenVDS/OpenVDSIntegrationTest.cpp
  OpenVDS/DataAccess.cpp
  OpenVDS/RequestVolumeSamples.cpp
  OpenVDS/RequestVolumeTraces.cpp
  OpenVDS/RequestVolumeSubsetFormat.cpp
  OpenVDS/DataAccessorClone.cpp
  OpenVDS/WaitForCompletion.cpp
  OpenVDS/RequestCancellation.cpp
  )

add_test_executable(multithreaded_requests
  OpenVDS/MultiThreadedRequests.cpp
)

add_test_executable(openvds_mixed_request
  OpenVDS/RequestVolumeSubsetAndPageAccessor.cpp
  )
if (BUILD_PYTHON)
  add_subdirectory(python)
endif()

