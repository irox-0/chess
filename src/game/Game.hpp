#pragma once

#include "../board/Board.hpp"
#include "../moves/Move.hpp"
#include "GameState.hpp"
#include <memory>
#include <vector>

namespace Chess {

enum class GameResult {
    IN_PROGRESS,
    WHITE_WINS,
    BLACK_WINS,
    DRAW,
    STALEMATE
};

class Game {
public:
    Game();
    ~Game() = default;

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    bool makeMove(const Move& move);
    bool undoLastMove();
    std::vector<Move> getValidMoves() const;
    
    bool isWhiteTurn() const;
    GameResult getGameResult() const;
    const Board& getBoard() const;
    bool isCheck() const;
    bool isCheckmate() const;
    bool isStalemate() const;
    
    const std::vector<Move>& getMoveHistory() const;
    
    bool saveGame(const std::string& filename) const;
    bool loadGame(const std::string& filename);
    
    void reset();

private:
    std::unique_ptr<Board> board;
    std::vector<Move> moveHistory;
    GameState currentState;
    
    bool isMoveLegal(const Move& move) const;
    void updateGameState();
    bool isKingInCheck(bool white) const;
    void switchTurn();
};

} 