function(BuildAWSSDK_impl)
  set(AWS_INSTALL_INT "${PROJECT_BINARY_DIR}/aws_install")
  if (WIN32)
    list(APPEND AWS_LIBS_LIST "${AWS_INSTALL_INT}/bin/aws-cpp-sdk-core.lib")
    list(APPEND AWS_LIBS_LIST "${AWS_INSTALL_INT}/bin/aws-cpp-sdk-s3.lib")
    list(APPEND AWS_LIBS_LIST "winhttp")

    list(APPEND AWS_DLLS_LIST "${AWS_INSTALL_INT}/bin/aws-c-common.dll")
    list(APPEND AWS_DLLS_LIST "${AWS_INSTALL_INT}/bin/aws-c-event-stream.dll")
    list(APPEND AWS_DLLS_LIST "${AWS_INSTALL_INT}/bin/aws-checksums.dll")
    list(APPEND AWS_DLLS_LIST "${AWS_INSTALL_INT}/bin/aws-cpp-sdk-core.dll")
    list(APPEND AWS_DLLS_LIST "${AWS_INSTALL_INT}/bin/aws-cpp-sdk-s3.dll")
  else()
    list(APPEND AWS_LIBS_LIST "${AWS_INSTALL_INT}/lib/libaws-cpp-sdk-core.so")
    list(APPEND AWS_LIBS_LIST "${AWS_INSTALL_INT}/lib/libaws-cpp-sdk-s3.so")
    list(APPEND AWS_LIBS_LIST "crypto")
    list(APPEND AWS_LIBS_LIST "curl")
  endif()

  include(ExternalProject)
  ExternalProject_Add(aws-cpp-sdk
    PREFIX ${PROJECT_BINARY_DIR}/aws-cpp-sdk
    SOURCE_DIR ${aws-cpp-sdk_SOURCE_DIR}
    URL ""
    INSTALL_DIR ${AWS_INSTALL_INT}
    CMAKE_ARGS "-DBUILD_ONLY=s3" "-DENABLE_TESTING=OFF" "-DCMAKE_BUILD_TYPE=$<CONFIG>" "-DCMAKE_INSTALL_PREFIX=${AWS_INSTALL_INT}"
    BUILD_BYPRODUCTS ${AWS_LIBS_LIST})

  #parent scope variables cant be used in this file
  set(AWS_LIBS ${AWS_LIBS_LIST} PARENT_SCOPE)
  set(AWS_DLLS ${AWS_DLLS_LIST} PARENT_SCOPE)
  set(AWS_INCLUDE_PATH "${AWS_INSTALL_INT}/include" PARENT_SCOPE)
endfunction()

macro(BuildAWSSDK)
  BuildAWSSDK_impl()
  add_definitions(-DUSE_IMPORT_EXPORT)
endmacro()
