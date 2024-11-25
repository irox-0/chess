set(TEST_SOURCES
    test_rook.cpp
)

add_executable(chess_tests ${TEST_SOURCES})

target_include_directories(chess_tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${GTEST_INCLUDE_DIR}
)

target_link_directories(chess_tests
    PRIVATE
    ${GTEST_LIBRARY_DIR}
)

target_link_libraries(chess_tests
    PRIVATE
    chess_lib
    gtest
    gtest_main
)

add_test(NAME chess_tests COMMAND chess_tests)