add_library(chess_lib
    board/Board.cpp
    board/Square.cpp
    pieces/Piece.cpp
    pieces/Position.cpp
    pieces/Pawn.cpp
    pieces/Knight.cpp
    pieces/Bishop.cpp
    pieces/Rook.cpp
    pieces/Queen.cpp
    pieces/King.cpp
    moves/Move.cpp
    moves/MoveGenerator.cpp
    game/Game.cpp
    game/GameState.cpp
    io/Console.cpp
    io/NotationParser.cpp
    utils/GameLogger.cpp
)

target_include_directories(chess_lib
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(chess_game
    main.cpp
)

target_link_libraries(chess_game
    PRIVATE
        chess_lib
)