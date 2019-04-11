enable_testing()

add_executable(io-filetest io/filetest.cpp)
target_include_directories(io-filetest PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(io-filetest openvds)
add_test(NAME io-filetest COMMAND io-filetest)
