#pragma once
#include "board/Board.hpp"
#include "moves/Move.hpp"
#include "game/GameState.hpp"
#include <random>
#include <vector>
#include <map>

class AI {
public:
    AI();
    ~AI() = default;

    Move getMove(const Board* board, Piece::Color color) const;
    void setSeed(unsigned int seed) const;

private:
    static const std::map<Piece::Type, int> PIECE_VALUES;
    static const int PAWN_POSITION_BONUS[8][8];
    static const int KNIGHT_POSITION_BONUS[8][8];
    
    const int maxDepth = 3;
    mutable std::mt19937 rng;

    Piece* selectRandomPiece(const std::vector<Piece*>& pieces) const;
    Move selectRandomMove(const std::vector<Move>& moves) const;
    bool handleSpecialCases(const Board* board, Piece::Color color, Move& selectedMove) const;
    bool isCriticalPosition(const Board* board, Piece::Color color) const;
    Piece* findPieceWithMoves(const Board* board, Piece::Color color) const;

    int negamax(Board* board, int depth, int alpha, int beta, Piece::Color color) const;
    int evaluatePosition(const Board* board, Piece::Color color) const;
    int evaluateMaterial(const Board* board, Piece::Color color) const;
    int evaluatePositionalAdvantage(const Board* board, Piece::Color color) const;
    int evaluatePawnPosition(const Position& pos, Piece::Color color) const;
    int evaluateKnightPosition(const Position& pos, Piece::Color color) const;
    int evaluateKingSafety(const Board* board, Piece::Color color) const;
    int evaluateKingPawnShield(const Board* board, const Position& kingPos, Piece::Color color) const;
    int evaluateOpenFilesNearKing(const Board* board, const Position& kingPos, Piece::Color color) const;
    int evaluateCenterControl(const Board* board, Piece::Color color) const;
    int evaluateDevelopment(const Board* board, Piece::Color color) const;
};