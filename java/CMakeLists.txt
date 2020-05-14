
include(UseJava)

set(OPENVDS_JAVA_VERSION 0.0.6)
set(CPP_LIBRARY_NAME openvdsjava)
set(JAVA_LIBRARY_NAME openvds-java)
set(JAVA_LIBRARY_DOC_NAME openvds-java-javadoc)
set(GENERATED_HEADER_TARGET_NAME generatedHeadersTarget)

set(JAVA_TEST_ENABLED ON)
if(JAVA_TEST_ENABLED)
    set(JAVA_TEST_LIBRARY_NAME openvds-java-test)
endif()

#set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.8" "-target" "1.8")

set(GENERATED_JNI_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpp/generated)
#file(REMOVE_RECURSE ${GENERATED_JNI_HEADERS_DIR})
file(MAKE_DIRECTORY ${GENERATED_JNI_HEADERS_DIR})

set(JAVA_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/java/src)
set(JAVA_DEMO_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/java/demo)
set(JAVA_EXPERIMENTAL_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/java/experimental)
if(JAVA_TEST_ENABLED)
    set(JAVA_TEST_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/java/test)
endif()

file (GLOB_RECURSE JAVA_SOURCES_FILES CONFIGURE_DEPENDS
        ${JAVA_SOURCES_DIR}/*.java
        ${JAVA_DEMO_SOURCES_DIR}/*.java
        #        ${JAVA_EXPERIMENTAL_SOURCES_DIR}/*.java
        )

if(JAVA_TEST_ENABLED)
    file (GLOB_RECURSE JAVA_TEST_SOURCES_FILES CONFIGURE_DEPENDS
            ${JAVA_TEST_SOURCES_DIR}/*.java)
endif()

message(STATUS "Compiling Java files")
foreach(FILE ${JAVA_SOURCES_FILES})
    message(STATUS ${FILE})
endforeach()

if(JAVA_TEST_ENABLED)
    foreach(FILE ${JAVA_TEST_SOURCES_FILES})
        message(STATUS ${FILE})
    endforeach()
endif()

# Java compilation
# Enable JNI header generation for java >=8
if (WIN32)
    add_jar(${JAVA_LIBRARY_NAME}
            SOURCES ${JAVA_SOURCES_FILES}
            GENERATE_NATIVE_HEADERS ${GENERATED_HEADER_TARGET_NAME} DESTINATION ${GENERATED_JNI_HEADERS_DIR}
            ENTRY_POINT OpenVdsDemo)
else()
    add_jar(${JAVA_LIBRARY_NAME}
            SOURCES ${JAVA_SOURCES_FILES}
            VERSION ${OPENVDS_JAVA_VERSION}
            GENERATE_NATIVE_HEADERS ${GENERATED_HEADER_TARGET_NAME} DESTINATION ${GENERATED_JNI_HEADERS_DIR}
            ENTRY_POINT OpenVdsDemo)
endif()

create_javadoc(${JAVA_LIBRARY_DOC_NAME}
        FILES ${JAVA_SOURCES_FILES}
        #        CLASSPATH ${CMAKE_JAVA_INCLUDE_PATH}
        WINDOWTITLE "OpenVDS-Java"
        DOCTITLE "<h1>OpenVDS-Java</h1>"
        AUTHOR TRUE
        USE TRUE
        VERSION TRUE)

if(JAVA_TEST_ENABLED)
    get_property(BUILT_JAR_FILE TARGET ${JAVA_LIBRARY_NAME} PROPERTY JAR_FILE)
    message(STATUS "Found built library :" ${BUILT_JAR_FILE})

    file(DOWNLOAD https://repo1.maven.org/maven2/org/testng/testng/6.14.3/testng-6.14.3.jar
            ${CMAKE_CURRENT_BINARY_DIR}/testng-6.14.3.jar
            SHOW_PROGRESS
            EXPECTED_HASH MD5=9f17a8f9e99165e148c42b21f4b63d7c)
    add_jar(${JAVA_TEST_LIBRARY_NAME}
            SOURCES ${JAVA_TEST_SOURCES_FILES}
            INCLUDE_JARS
            ${CMAKE_CURRENT_BINARY_DIR}/testng-6.14.3.jar
            ${BUILT_JAR_FILE}
            VERSION ${OPENVDS_JAVA_VERSION})
endif()

# C++ compilation
file (GLOB_RECURSE CPP_SOURCES_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/*.cpp)
message(STATUS "Compiling cpp")
foreach(FILE ${CPP_SOURCES_FILES})
    message(STATUS ${FILE})
endforeach()

add_library(${CPP_LIBRARY_NAME} ${CPP_SOURCES_FILES})
set_target_properties(${CPP_LIBRARY_NAME} PROPERTIES FOLDER java)
set_target_properties(${CPP_LIBRARY_NAME} PROPERTIES VERSION ${OPENVDS_JAVA_VERSION})
copyDllForTarget(${CPP_LIBRARY_NAME})
target_include_directories(${CPP_LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cpp/include)
target_include_directories(${CPP_LIBRARY_NAME} PUBLIC ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
target_link_libraries(${CPP_LIBRARY_NAME} PRIVATE openvds ${GENERATED_HEADER_TARGET_NAME})

# Install jar and natives
install(TARGETS ${CPP_LIBRARY_NAME} LIBRARY DESTINATION java)
install_jar(${JAVA_LIBRARY_NAME} DESTINATION java)
if(JAVA_TEST_ENABLED)
    install_jar(${JAVA_TEST_LIBRARY_NAME} DESTINATION java)
endif()
