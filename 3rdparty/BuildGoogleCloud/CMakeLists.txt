configure_file(
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/internal/build_info.cc.in
  ${CMAKE_BINARY_DIR}/google-cloud-cpp/internal/build_info.cc
)

add_library(google_cloud_ovds STATIC 
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/oauth2/anonymous_credentials.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/oauth2/authorized_user_credentials.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/oauth2/compute_engine_credentials.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/oauth2/credentials.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/oauth2/google_application_default_credentials_file.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/oauth2/google_credentials.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/oauth2/refreshing_credentials_wrapper.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/oauth2/service_account_credentials.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/status.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/openssl_util.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/internal/throw_delegate.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/curl_handle.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/curl_download_request.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/http_response.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/internal/getenv.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/curl_handle_factory.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/curl_request.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/curl_client.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/curl_request_builder.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/compute_engine_util.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/internal/filesystem.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/version.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/version.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/internal/compiler_info.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/binary_data_as_debug_string.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/curl_wrappers.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/log.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/internal/strerror.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/well_known_parameters.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/internal/random.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/bucket_acl_requests.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/bucket_requests.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/iam_policy.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/default_object_acl_requests.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/well_known_headers.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/object_acl_requests.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/hmac_key_requests.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/object_requests.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/sha256_hash.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/metadata_parser.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/object_access_control.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/object_metadata.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/internal/format_time_point.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/access_control_common.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/internal/parse_rfc3339.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/hmac_key_metadata.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/notification_requests.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/notification_metadata.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/hashing_options.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/resumable_upload_session.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/service_account_requests.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/sign_blob_requests.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/curl_resumable_upload_session.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/bucket_access_control.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/bucket_metadata.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/iam_bindings.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/lifecycle_rule.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/client.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/client_options.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/hash_validator.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/object_streambuf.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/logging_client.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/policy_document_request.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/retry_client.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/signed_url_requests.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/object_stream.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/hash_validator_impl.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/iam_policy.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/empty_response.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/service_account.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/logging_resumable_upload_session.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/idempotency_policy.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/internal/backoff_policy.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/retry_object_read_source.cc
  ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/retry_resumable_upload_session.cc
  ${CMAKE_BINARY_DIR}/google-cloud-cpp/internal/build_info.cc
)

if (MSVC AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
  target_sources(google_cloud_ovds PRIVATE ${google-cloud-cpp_SOURCE_DIR}/google/cloud/terminate_handler.cc)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set_source_files_properties(${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/object_streambuf.cc PROPERTIES COMPILE_FLAGS /WX-)
else()
  set_source_files_properties(${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/openssl_util.cc PROPERTIES COMPILE_FLAGS -Wno-pedantic)
  set_source_files_properties(${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/bucket_requests.cc PROPERTIES COMPILE_FLAGS -Wno-pedantic)
endif()

target_include_directories(google_cloud_ovds PUBLIC
  ${google-cloud-cpp_SOURCE_DIR}
  ${google_nlohmann_SOURCE_DIR}
  ${absl_SOURCE_DIR}
  )

add_dependencies(google_cloud_ovds crc32c)
if (WIN32)
  add_dependencies(google_cloud_ovds libressl)
endif()

get_property(include_3rdparty      GLOBAL PROPERTY OPENVDS_INCLUDE_LIBRARIES)
addSystemHeadersToTarget(google_cloud_ovds "${include_3rdparty}")

if (BUILD_CURL)
  add_dependencies(google_cloud_ovds curl)
else()
  find_package(CURL REQUIRED)
  target_link_libraries(google_cloud_ovds PRIVATE CURL::libcurl)
endif()
