#pragma once
#include "Piece.hpp"
#include "board/Board.hpp"
#include <vector>

class Pawn : public Piece {
public:
    Pawn(Color color);
    Pawn(Color color, Position position);
    
    std::vector<Position> getPossibleMoves(const Board* board) const override;
    std::vector<Position> getAttackedSquares(const Board* board) const override;
    bool canMoveTo(const Position& target, const Board* board) const override;
    char getSymbol() const override;
    Piece* clone() const override;

    bool canBePromoted() const;
    bool isEnPassantPossible(const Position& target, const Board* board) const;
    
private:
    std::vector<Position> getForwardMoves(const Board* board) const;
    std::vector<Position> getCaptureMoves(const Board* board) const;
    int getDirection() const;
    int getStartRank() const;
    int getPromotionRank() const;
};