enable_testing()
include(GoogleTest)

add_executable(chess_tests
    #test_board.cpp
    #test_square.cpp
    #test_piece.cpp
    test_pawn.cpp
    test_rook.cpp
    #test_knight.cpp
    #test_bishop.cpp
    #test_queen.cpp
    #test_king.cpp
    #test_move.cpp
    #test_game.cpp
    test_move_generator.cpp
    test_pieces.cpp
    #test_game_state.cpp
)

target_link_libraries(chess_tests
    PRIVATE
        chess_lib
        GTest::gtest
        GTest::gtest_main
)

target_include_directories(chess_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

gtest_discover_tests(chess_tests)