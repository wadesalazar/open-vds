add_executable(getting-started main.cpp)
target_link_libraries(getting-started openvds::openvds)
set_target_properties(getting-started PROPERTIES FOLDER examples)

target_compile_definitions(getting-started PRIVATE -DTEST_URL="${TEST_URL}")
string(REPLACE ";" "\\\\;" TEST_CONNECTION_ESCAPED "${TEST_CONNECTION}")
target_compile_definitions(getting-started PRIVATE -DTEST_CONNECTION="${TEST_CONNECTION_ESCAPED}")

setWarningFlagsForTarget(getting-started)
copyDllForTarget(getting-started)
