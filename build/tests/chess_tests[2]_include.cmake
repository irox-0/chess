if(EXISTS "D:/cpp/chess1/build/tests/chess_tests[2]_tests.cmake")
  include("D:/cpp/chess1/build/tests/chess_tests[2]_tests.cmake")
else()
  add_test(chess_tests_NOT_BUILT chess_tests_NOT_BUILT)
endif()
