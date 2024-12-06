# C++ Chess Game

A console-based chess game implementation in C++ with AI opponent and game saving capabilities.

## Features

- Player vs AI gameplay
- Standard chess rules implementation
- Move validation
- Check and checkmate detection
- Castling, en passant, and pawn promotion
- Move history tracking
- Game timer functionality
- Save and load games
- Console-based visualization

## Project Structure

```
chess/
├── CMakeLists.txt                    # Main build file
├── src/                              # Source code
│   ├── main.cpp                      # Program entry point
│   ├── game/                         # Game logic
│   │   ├── Game.cpp                  # Main game class
│   │   └── GameState.cpp             # Game state tracking
│   ├── board/                        # Board logic
│   │   ├── Board.cpp                 # Chess board implementation
│   │   └── Square.cpp               # Board square representation
│   ├── pieces/                       # Chess pieces
│   │   ├── Piece.cpp                # Base piece class
│   │   ├── Pawn.cpp
│   │   ├── Knight.cpp
│   │   ├── Bishop.cpp
│   │   ├── Rook.cpp
│   │   ├── Queen.cpp
│   │   └── King.cpp
│   ├── moves/                        # Move logic
│   │   ├── Move.cpp                  # Move representation
│   │   └── MoveGenerator.cpp         # Legal move generation
│   ├── ai/                           # AI implementation
│   │   └── AI.cpp                    # AI move decision
│   ├── io/                           # Input/Output
│   │   └── Console.cpp               # Console interface
│   └── utils/                        # Utilities
│       ├── Timer.cpp                 # Game timer
│       └── GameLogger.cpp            # Game saving/loading
├── include/                          # Public headers
├── tests/                            # Unit tests
└── resources/                        # Additional resources
    └── saved_games/                  # Saved game files
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

## Game Features

### Main Menu
1. New Game
2. Load Game from File
3. Exit

### Game Timer
- Set time limit for each player at the start of the game
- Timer countdown during player's turn
- Automatic game over when time runs out

### Save/Load Games
- Save games in standard chess notation
- Load and continue previously saved games
- Games are saved in resources/saved_games directory

### Gameplay Instructions

1. Choose your color (White/Black) at the start
2. Set the timer duration in minutes
3. Enter moves in algebraic notation (e.g., "e2e4" or "e2 e4")
4. Available commands during game:
   - `move <from> <to>` (e.g., 'move e2 e4')
   - `show moves <pos>` (e.g., 'show moves e2')
   - `history` (shows game move history)
   - `undo` (undo last move)
   - `resign`
   - `quit`

### Game End Conditions
- Checkmate
- Stalemate
- Player resignation
- Time out
- Draw offer accepted

## Testing

To run the tests:
```bash
cd build
ctest
```
