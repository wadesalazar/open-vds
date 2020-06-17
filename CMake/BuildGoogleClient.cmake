function(addGoogleCloudToTarget target)
  configure_file(
    ${google-cloud-cpp_SOURCE_DIR}/google/cloud/internal/build_info.cc.in
    ${CMAKE_BINARY_DIR}/google-cloud-cpp/internal/build_info.cc
  )

  target_sources(${target} PRIVATE
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
    ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/bucket_requests.cc
    ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/resumable_upload_session.cc
    ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/service_account_requests.cc
    ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/sign_blob_requests.cc
    ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/curl_resumable_upload_session.cc
    ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/bucket_access_control.cc
    ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/bucket_metadata.cc
    ${google-cloud-cpp_SOURCE_DIR}/google/cloud/iam_bindings.cc
    ${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/lifecycle_rule.cc
    #${google-cloud-cpp_SOURCE_DIR}/google/cloud/
    #${google-cloud-cpp_SOURCE_DIR}/google/cloud/
    #${google-cloud-cpp_SOURCE_DIR}/google/cloud/
    #${google-cloud-cpp_SOURCE_DIR}/google/cloud/
    #${google-cloud-cpp_SOURCE_DIR}/google/cloud/
    #${google-cloud-cpp_SOURCE_DIR}/google/cloud/
    #${google-cloud-cpp_SOURCE_DIR}/google/cloud/
    ${CMAKE_BINARY_DIR}/google-cloud-cpp/internal/build_info.cc
  )

  if (NOT (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
    set_source_files_properties(${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/openssl_util.cc PROPERTIES COMPILE_FLAGS -Wno-pedantic)
    set_source_files_properties(${google-cloud-cpp_SOURCE_DIR}/google/cloud/storage/internal/bucket_requests.cc PROPERTIES COMPILE_FLAGS -Wno-pedantic)
  endif()

  target_include_directories(${target} PRIVATE
    ${google-cloud-cpp_SOURCE_DIR}
    ${google_nlohmann_SOURCE_DIR}
    )
endfunction()