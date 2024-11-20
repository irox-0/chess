cmake_minimum_required(VERSION 3.14)
project(chess_game VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем исполняемый файл
add_executable(chess_game 
    src/main.cpp
    src/board/Board.cpp
    src/pieces/Piece.cpp
    src/game/Game.cpp
)

# Добавляем директории с заголовочными файлами
target_include_directories(chess_game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
