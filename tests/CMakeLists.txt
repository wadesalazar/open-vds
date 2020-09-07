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

add_executable(TestRootTarget RootTestExecutable.cpp)
set_target_properties(TestRootTarget PROPERTIES FOLDER tests)
copyDllForTarget(TestRootTarget)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)

get_property(include_3rdparty      GLOBAL PROPERTY OPENVDS_INCLUDE_LIBRARIES)

string(REPLACE ";" "\\\\;" TEST_CONNECTION_ESCAPED "${TEST_CONNECTION}")

function(add_test_executable name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} PRIVATE openvds_objects gtest gtest_main)
  addSystemHeadersToTarget(${name} "${include_3rdparty}")
  target_compile_definitions(${name} PRIVATE openvds_EXPORTS)
  add_dependencies(${name} TestRootTarget)
  gtest_discover_tests(${name})
  setWarningFlagsForTarget(${name})
  set_target_properties(${name} PROPERTIES FOLDER tests)
  get_target_property(fmt_INCLUDE fmt::fmt INTERFACE_INCLUDE_DIRECTORIES)
  get_target_property(json_INCLUDE jsoncpp_lib_static INTERFACE_INCLUDE_DIRECTORIES)
  target_include_directories(${name} SYSTEM PRIVATE ${fmt_INCLUDE} ${json_INCLUDE})
  addSystemHeadersToTarget(${name} "${fmt_INCLUDE}")
  addSystemHeadersToTarget(${name} "${json_INCLUDE}")
  target_include_directories(${name} PRIVATE ${PROJECT_SOURCE_DIR}/src/OpenVDS)
  target_compile_definitions(${name} PRIVATE -DTEST_DATA_PATH="${PROJECT_SOURCE_DIR}/tests/VDS")
  target_compile_definitions(${name} PRIVATE -DTEST_URL="${TEST_URL}")
  target_compile_definitions(${name} PRIVATE -DTEST_CONNECTION="${TEST_CONNECTION_ESCAPED}")
endfunction()

add_test_executable(io_tests
  io/filetest.cpp
  io/InMemoryIo.cpp
  io/IoManagerBasic.cpp
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
target_include_directories(segy_tests PRIVATE ../src/SEGYUtils)

add_test_executable(vds_integration_tests
  VDS/ParseVDSJsonTest.cpp
  VDS/DeserializeVolumeDataTest.cpp
  VDS/RequestVolumeCleanupThread.cpp
  VDS/ParseConnectionStringTest.cpp
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
  OpenVDS/VolumeIndexerSymbols.cpp
  OpenVDS/RequestVolumeError.cpp
  )

add_test_executable(multithreaded_requests
  OpenVDS/MultiThreadedRequests.cpp
)

add_test_executable(global_state
  OpenVDS/GlobalStateTest.cpp
)


add_test_executable(openvds_mixed_request
  OpenVDS/RequestVolumeSubsetAndPageAccessor.cpp
  )

if (TEST_SEGY_FILE AND TEST_URL)
  add_test(NAME "tools.SegyRoundtrip"
    COMMAND ${CMAKE_COMMAND}
                -DSEGYImport=$<TARGET_FILE:SEGYImport>
                -DSEGYExport=$<TARGET_FILE:SEGYExport>
                -DTEST_URL=${TEST_URL}
                -DTEST_CONNECTION=${TEST_CONNECTION_ESCAPED}
                -DTEST_SEGY_FILE=${TEST_SEGY_FILE}
                -DTEST_SEGY_HEADER_FILE=${TEST_SEGY_HEADER_FILE}
                -P ${CMAKE_CURRENT_SOURCE_DIR}/RunRoundtrip.cmake
    )
endif()

if (BUILD_PYTHON)
  add_subdirectory(python)
endif()

if (BUILD_JAVA)
  add_custom_command(TARGET TestRootTarget
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:openvds-javacpp> $<TARGET_FILE_DIR:TestRootTarget>
    DEPENDS openvds-javacpp 
    )
  add_subdirectory(java)
endif()

