include(UseJava)

set(OPENVDS_JAVA_VERSION 0.0.6)

#set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.8" "-target" "1.8")

set(GENERATED_JNI_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpp/generated)
#file(REMOVE_RECURSE ${GENERATED_JNI_HEADERS_DIR})
file(MAKE_DIRECTORY ${GENERATED_JNI_HEADERS_DIR})

set(JAVA_SOURCE_FILES
  java/src/org/opengroup/openvds/AWSOpenOptions.java
  java/src/org/opengroup/openvds/AzureOpenOptions.java
  java/src/org/opengroup/openvds/AzurePresignedOpenOptions.java
  java/src/org/opengroup/openvds/B.java
  java/src/org/opengroup/openvds/BufferUtils.java
  java/src/org/opengroup/openvds/Cleaner.java
  java/src/org/opengroup/openvds/CompressionMethod.java
  java/src/org/opengroup/openvds/Dimensionality.java
  java/src/org/opengroup/openvds/DimensionsND.java
  java/src/org/opengroup/openvds/experimental/VariousJavaTests.java
  java/src/org/opengroup/openvds/IndexRegionFloat.java
  java/src/org/opengroup/openvds/IndexRegionLong.java
  java/src/org/opengroup/openvds/InMemoryOpenOptions.java
  java/src/org/opengroup/openvds/InterpolationMethod.java
  java/src/org/opengroup/openvds/JniPointer.java
  java/src/org/opengroup/openvds/JniPointerWithoutDeletion.java
  java/src/org/opengroup/openvds/MemoryVdsGenerator.java
  java/src/org/opengroup/openvds/MetadataKey.java
  java/src/org/opengroup/openvds/MetadataReadAccess.java
  java/src/org/opengroup/openvds/MetadataType.java
  java/src/org/opengroup/openvds/NDBox.java
  java/src/org/opengroup/openvds/NDPos.java
  java/src/org/opengroup/openvds/OpenOptions.java
  java/src/org/opengroup/openvds/OpenVDS.java
  java/src/org/opengroup/openvds/QuantizingValueConverter_FloatToByte.java
  java/src/org/opengroup/openvds/VdsHandle.java
  java/src/org/opengroup/openvds/VDSProduceStatus.java
  java/src/org/opengroup/openvds/VolumeDataAccessManager.java
  java/src/org/opengroup/openvds/VolumeDataAccessor.java
  java/src/org/opengroup/openvds/VolumeDataAxisDescriptor.java
  java/src/org/opengroup/openvds/VolumeDataChannelDescriptor.java
  java/src/org/opengroup/openvds/VolumeDataLayout.java
  java/src/org/opengroup/openvds/VolumeDataLayoutDescriptor.java
  java/src/org/opengroup/openvds/VolumeDataMapping.java
  java/src/org/opengroup/openvds/VolumeDataPage.java
  java/src/org/opengroup/openvds/VolumeDataPageAccessor.java

  java/demo/AWSSliceDumpDemo.java
  java/demo/OpenVdsDemo.java

  #java/experimental/OpenVdsDemoWithArrayIntoBuffers.java
  #java/experimental/OpenVdsDemoWithBuffers.java
  #java/experimental/OpenVdsDemoWithSubset.java
  #java/experimental/OpenVdsDemoWithSubsetAndBytes.java
  #java/experimental/TestVariousAllocationAndTransmissionMethods.java
)


# Java compilation
# Enable JNI header generation for java >=8
add_jar(openvds-java
        SOURCES ${JAVA_SOURCE_FILES}
        VERSION ${OPENVDS_JAVA_VERSION}
        GENERATE_NATIVE_HEADERS openvds-java-generated-headers DESTINATION ${GENERATED_JNI_HEADERS_DIR}
        ENTRY_POINT OpenVdsDemo)

get_property(BUILT_JAR_FILE TARGET openvds-java PROPERTY JAR_FILE)

create_javadoc(openvds-java-javadoc
  FILES ${JAVA_SOURCE_FILES}
  WINDOWTITLE \"OpenVDS-Java\"
  DOCTITLE \"<h1>OpenVDS-Java</h1>\"
  AUTHOR TRUE
  USE TRUE
  VERSION TRUE
  )


set(JAVA_TEST_SOURCES_FILES
  java/test/org/opengroup/openvds/AmazonS3Test.java
  java/test/org/opengroup/openvds/MemoryVdsGeneratorTest.java
  )

add_jar(openvds-java-test
        SOURCES ${JAVA_TEST_SOURCES_FILES}
        VERSION ${OPENVDS_JAVA_VERSION}
        INCLUDE_JARS
        ${testng_SOURCE_FILE}
        ${BUILT_JAR_FILE}
)

add_library(openvds-javacpp
  cpp/src/Common.cpp
  cpp/src/CommonJni.cpp
  cpp/src/JniPointer.cpp
  cpp/src/MemoryVdsGenerator.cpp
  cpp/src/MetadataReadAccess.cpp
  cpp/src/OpenVDS.cpp
  cpp/src/QuantizingValueConverter_FloatToByte.cpp
  cpp/src/VariousJavaTests.cpp
  cpp/src/VdsHandle.cpp
  cpp/src/VolumeDataAccessManager.cpp
  cpp/src/VolumeDataAccessor.cpp
  cpp/src/VolumeDataLayout.cpp
  cpp/src/VolumeDataPage.cpp
  cpp/src/VolumeDataPageAccessor.cpp
  )

set_target_properties(openvds-javacpp PROPERTIES FOLDER java)
set_target_properties(openvds-javacpp PROPERTIES VERSION ${OPENVDS_JAVA_VERSION})
copyDllForTarget(openvds-javacpp)
target_include_directories(openvds-javacpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cpp/include)
target_include_directories(openvds-javacpp PUBLIC ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
target_link_libraries(openvds-javacpp PRIVATE openvds openvds-java-generated-headers)

# Install jar and natives
if (${MSVC_TOOLSET_VERSION_LOCAL})
  set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION_LOCAL}")
endif()
install(TARGETS openvds-javacpp
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR}
  LIBRARY 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_TOOLSET_DIR}
  ARCHIVE 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_TOOLSET_DIR}
)

if (WIN32)
   install(FILES ${BUILT_JAR_FILE} DESTINATION java)
else()
  install_jar(openvds-java DESTINATION java)
endif()
