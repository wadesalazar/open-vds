add_executable(getting-started main.cpp)
target_link_libraries(getting-started openvds)
set_target_properties(getting-started PROPERTIES FOLDER examples)

target_compile_definitions(getting-started PRIVATE -DTEST_AWS_REGION="${TEST_AWS_REGION}")
target_compile_definitions(getting-started PRIVATE -DTEST_AWS_BUCKET="${TEST_AWS_BUCKET}")
target_compile_definitions(getting-started PRIVATE -DTEST_AWS_OBJECTID="${TEST_AWS_OBJECTID}")

copyDllForTarget(getting-started)
