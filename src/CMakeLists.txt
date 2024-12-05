set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(chess_lib STATIC
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
    utils/GameLogger.cpp
    ai/AI.cpp
    utils/Timer.cpp
)

target_include_directories(chess_lib
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        #${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(chess_game
    main.cpp
)

target_link_libraries(chess_game
    PRIVATE
        chess_lib
)