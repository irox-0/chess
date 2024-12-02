# C++ Chess Game

A console-based chess game implementation in C++ with AI opponent.

## Features

- Player vs AI gameplay
- Standard chess rules implementation
- Move validation
- Check and checkmate detection
- Castling, en passant, and pawn promotion
- Move history tracking
- Console-based visualization

## Project Structure

```
chess/
├── CMakeLists.txt                    # Main build file
├── src/                              # Source code
│   ├── main.cpp                      # Program entry point
│   ├── game/                         # Game logic
│   ├── board/                        # Board logic
│   ├── pieces/                       # Chess pieces
│   ├── moves/                        # Move logic
│   ├── ai/                           # AI implementation
│   ├── io/                           # Input/Output
│   └── utils/                        # Utilities
├── include/                          # Public headers
├── tests/                            # Unit tests
└── resources/                        # Additional resources
```

## Requirements

- C++17 or higher
- CMake 3.15 or higher
- A C++ compiler (GCC, Clang, or MSVC)

## Building the Project

1. Clone the repository:
```bash
git clone [repository-url]
cd chess
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Generate build files with CMake:
```bash
cmake ..
```

4. Build the project:
```bash
cmake --build .
```

## Running the Game

After building, run the executable:
```bash
./chess
```

## Gameplay Instructions

1. Choose your color (White/Black) at the start
2. Enter moves in algebraic notation (e.g., "e2e4" or "e2 e4")
3. Available commands during game:
   - `move <from> <to>` (e.g., 'move e2 e4')
   - `show moves <pos>` (e.g., 'show moves e2')
   - `history` (shows game move history)
   - `undo` (undo last move)
   - `resign`
   - `quit`

## Testing

To run the tests:
```bash
cd build
ctest
```
