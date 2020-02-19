execute_process(COMMAND ${Python3_EXECUTABLE} -c "import sphinx"  RESULT_VARIABLE sphinx_ret)

if (sphinx_ret EQUAL "0")
  set(Sphinx_FOUND 1)
else()
  set(Sphinx_FOUND 0)
endif()
