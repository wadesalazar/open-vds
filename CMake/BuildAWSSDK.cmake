macro(BuildAWSSDK)
  if (WIN32)
    list(APPEND AWS_LIBS_LIST "bin/aws-cpp-sdk-core.lib")
    list(APPEND AWS_LIBS_LIST "bin/aws-cpp-sdk-s3.lib")

    list(APPEND AWS_DLLS_LIST "bin/aws-c-common.dll")
    list(APPEND AWS_DLLS_LIST "bin/aws-c-event-stream.dll")
    list(APPEND AWS_DLLS_LIST "bin/aws-checksums.dll")
    list(APPEND AWS_DLLS_LIST "bin/aws-cpp-sdk-core.dll")
    list(APPEND AWS_DLLS_LIST "bin/aws-cpp-sdk-s3.dll")
  elseif (APPLE)
    list(APPEND AWS_LIBS_LIST "lib/libaws-cpp-sdk-core.dylib")
    list(APPEND AWS_LIBS_LIST "lib/libaws-cpp-sdk-s3.dylib")
  else()
    list(APPEND AWS_LIBS_LIST "lib/libaws-cpp-sdk-core.so")
    list(APPEND AWS_LIBS_LIST "lib/libaws-cpp-sdk-s3.so")
  endif()

  BuildExternal(aws-cpp-sdk ${aws-cpp-sdk_VERSION} ${aws-cpp-sdk_SOURCE_DIR} "${AWS_LIBS_LIST}" "${AWS_DLLS_LIST}" "" "" "-DBUILD_ONLY=s3;-DENABLE_TESTING=OFF")

  add_definitions(-DUSE_IMPORT_EXPORT)

  if (WIN32)
    list(APPEND aws-cpp-sdk_EXTERNAL_LIBS "winhttp")
  elseif (APPLE)
  else()
    list(APPEND aws-cpp-sdk_EXTERNAL_LIBS "crypto")
    list(APPEND aws-cpp-sdk_EXTERNAL_LIBS "curl")
  endif()
endmacro()
