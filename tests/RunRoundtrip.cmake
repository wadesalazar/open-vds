function(check_variable variable)
  if (NOT ${variable})
    message(FATAL_ERROR "Missing ${variable} variable")
  endif()
endfunction()

check_variable(SEGYImport)
check_variable(SEGYExport)
check_variable(TEST_URL)
check_variable(TEST_SEGY_FILE)

if (TEST_CONNECTION)
  set(IMPORT_CONNECTION "--url-connection")
  set(EXPORT_CONNECTION "--connection")
endif()

if (TEST_SEGY_HEADER_FILE)
  set(HEADER "--header-format")
endif()


message("Running: ${SEGYImport} --persistentID roundtrip_test --url "${TEST_URL}" ${IMPORT_CONNECTION} ${TEST_CONNECTION} ${HEADER} ${TEST_SEGY_HEADER_FILE} ${TEST_SEGY_FILE}")
execute_process(COMMAND ${SEGYImport} --persistentID roundtrip_test --url "${TEST_URL}" ${IMPORT_CONNECTION} ${TEST_CONNECTION} ${HEADER} ${TEST_SEGY_HEADER_FILE} ${TEST_SEGY_FILE} RESULT_VARIABLE CMD_RESULT)
if (CMD_RESULT)
  message(FATAL_ERROR "Failed to run SEGYImport")
endif()

message("Running: ${SEGYExport} --persistentID roundtrip_test --url "${TEST_URL}" ${EXPORT_CONNECTION} ${TEST_CONNECTION} roundtrip_test.segy")
execute_process(COMMAND ${SEGYExport} --persistentID roundtrip_test --url "${TEST_URL}" ${EXPORT_CONNECTION} ${TEST_CONNECTION} roundtrip_test.segy RESULT_VARIABLE CMD_RESULT)
if (CMD_RESULT)
  message(FATAL_ERROR "Failed to run SEGYExport")
endif()

file(SHA256 ${TEST_SEGY_FILE} INPUT_SHA)
file(SHA256 roundtrip_test.segy OUTPUT_SHA)

if (INPUT_SHA STREQUAL OUTPUT_SHA)
  message("Input segy is equal to output segy. Compared hash is ${INPUT_SHA}")
else()
  message(FATAL_ERROR "The generated SEGY file is not equal to the input SEGY file")
endif()
