set(SOURCES
    main.cpp
    game/Game.cpp
    game/GameState.cpp
    board/Board.cpp
    board/Square.cpp
    pieces/Piece.cpp
    pieces/Pawn.cpp
    pieces/Knight.cpp
    pieces/Bishop.cpp
    pieces/Rook.cpp
    pieces/Queen.cpp
    pieces/King.cpp
    pieces/Position.cpp
    moves/Move.cpp
    moves/MoveGenerator.cpp
    ai/AI.cpp
    io/Console.cpp
    io/NotationParser.cpp
    utils/GameLogger.cpp
)

add_library(chess_lib
    ${SOURCES}
)

target_include_directories(chess_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(chess_game main.cpp)

target_link_libraries(chess_game
    PRIVATE
    chess_lib
)