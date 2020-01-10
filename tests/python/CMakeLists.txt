configure_file("aws_defs.txt" "${CMAKE_CURRENT_BINARY_DIR}/aws_defs.py" @ONLY)

function(add_python_test scope test_file)
  if (UNIX)
    set(seperator ":")
  else ()
    set(seperator ";")
  endif()

  string(REGEX REPLACE "\.py$" "" test_name ${test_file})

  add_test(NAME "python.${scope}.${test_name}"
    COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=$<TARGET_FILE_DIR:core>/..${seperator}${CMAKE_CURRENT_BINARY_DIR}"
      ${PYTHON_EXECUTABLE} ${test_file}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
endfunction()

add_python_test("general" "basictest.py")
add_python_test("general" "fixedarraytest.py")
