#pragma once
#include "Piece.hpp"
#include "board/Board.hpp"

class King : public Piece {
public:
    King(Color color);
    King(Color color, Position position);
    
    std::vector<Position> getPossibleMoves(const Board* board) const override;
    std::vector<Position> getAttackedSquares(const Board* board) const override;
    bool canMoveTo(const Position& target, const Board* board) const override;
    char getSymbol() const override;
    Piece* clone() const override;

private:
    bool isValidKingMove(const Position& target) const;
    
    std::vector<Position> getBasicMoves() const;
    
    std::vector<Position> getCastlingMoves(const Board* board) const;
    
    bool canCastleKingside(const Board* board) const;
    
    bool canCastleQueenside(const Board* board) const;
    
    bool isSquareSafe(const Position& pos, const Board* board) const;
};