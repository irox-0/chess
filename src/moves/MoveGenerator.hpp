#pragma once
#include <vector>
#include <memory>
#include "../board/Board.hpp"
#include "../pieces/Piece.hpp"
#include "Move.hpp"

namespace Chess {

class MoveGenerator {
public:
    explicit MoveGenerator(const Board& board);

    std::vector<Move> generateAllMoves(Color color) const;
    std::vector<Move> generateLegalMoves(Color color) const;
    std::vector<Move> generateLegalMoves(const Position& pos) const;
    
    std::vector<Move> generatePawnMoves(const Position& pos) const;
    std::vector<Move> generateKnightMoves(const Position& pos) const;
    std::vector<Move> generateBishopMoves(const Position& pos) const;
    std::vector<Move> generateRookMoves(const Position& pos) const;
    std::vector<Move> generateQueenMoves(const Position& pos) const;
    std::vector<Move> generateKingMoves(const Position& pos) const;
    
    std::vector<Move> generateCastlingMoves(Color color) const;
    std::vector<Move> generateEnPassantMoves(const Position& pos) const;
    std::vector<Move> generatePawnPromotionMoves(const Position& pos) const;
    
    bool isLegalMove(const Move& move) const;
    bool leavesKingInCheck(const Move& move, Color color) const;
    bool isKingInCheck(Color color) const;
    
    bool canPieceMoveToSquare(const Position& from, const Position& to) const;
    bool isSquareAttacked(const Position& pos, Color attackingColor) const;
    std::vector<Position> getAttackingSquares(const Position& pos) const;

private:
    const Board& board;
    
    void addMove(std::vector<Move>& moves, const Position& from, const Position& to, 
                Move::Type type = Move::Type::Normal) const;
    void addPawnMoves(std::vector<Move>& moves, const Position& from) const;
    void addSlidingMoves(std::vector<Move>& moves, const Position& from, 
                        const std::vector<std::pair<int, int>>& directions) const;
    void addKingMoves(std::vector<Move>& moves, const Position& from) const;
    void addKnightMoves(std::vector<Move>& moves, const Position& from) const;
    
    bool canCastleKingSide(Color color) const;
    bool canCastleQueenSide(Color color) const;
    bool isPawnPromotion(const Position& from, const Position& to) const;
    bool isEnPassantPossible(const Position& from, const Position& to) const;
    
    bool isPathClear(const Position& from, const Position& to) const;
    bool areSquaresBetweenEmpty(const Position& from, const Position& to) const;
    
    static std::vector<Position> getPositionsInDirection(const Position& start, int rowDir, int colDir);
    static bool isWithinBoard(const Position& pos);
};

} 