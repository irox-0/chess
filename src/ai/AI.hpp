#pragma once
#include "board/Board.hpp"
#include "moves/Move.hpp"
#include "game/GameState.hpp"
#include <random>
#include <vector>

class AI {
public:
    AI();
    ~AI() = default;

    Move getMove(const Board* board, Piece::Color color) const;
    
    void setSeed(unsigned int seed) const;

private:
    Piece* selectRandomPiece(const std::vector<Piece*>& pieces) const;
    
    Move selectRandomMove(const std::vector<Move>& moves) const;
    
    bool handleSpecialCases(const Board* board, Piece::Color color, Move& selectedMove) const;
    
    bool isCriticalPosition(const Board* board, Piece::Color color) const;
    
    Piece* findPieceWithMoves(const Board* board, Piece::Color color) const;

    mutable std::mt19937 rng; 
};