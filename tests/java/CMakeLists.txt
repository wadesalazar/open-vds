
if (UNIX)
  set(separator ":")
else ()
  set(separator ";")
endif()

get_property(BUILT_JAR_FILE TARGET openvds-java PROPERTY JAR_FILE)
get_property(BUILT_JAR_TEST_FILE TARGET openvds-java-test PROPERTY JAR_FILE)

add_test(NAME "Java.Java.AllJavaTests"
  COMMAND ${CMAKE_COMMAND} -E env "LD_LIBRARY_PATH=$<TARGET_FILE_DIR:openvds-javacpp>"
    ${Java_JAVA_EXECUTABLE} -cp "${testng_SOURCE_FILE}${separator}${jcommander_SOURCE_FILE}${separator}${BUILT_JAR_FILE}${separator}${BUILT_JAR_TEST_FILE}" org.testng.TestNG -testjar ${BUILT_JAR_TEST_FILE} -log 2
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.."
  )
