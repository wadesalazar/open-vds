add_executable(io-filetest io/filetest.cpp)
target_include_directories(io-filetest PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(io-filetest openvds Threads::Threads)
if (UNIX)
  target_link_libraries(io-filetest -pthread)
endif()
add_test(NAME io-filetest COMMAND io-filetest)

add_executable(io-s3test io/s3test.cpp)
target_include_directories(io-s3test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(io-s3test openvds Threads::Threads)
add_test(NAME io-s3test COMMAND io-s3test)
if (WIN32)
  add_custom_command(TARGET io-s3test
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${AWS_DLLS} $<TARGET_FILE_DIR:io-s3test>)
endif()
