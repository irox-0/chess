#pragma once
#include "Piece.hpp"
#include "board/Board.hpp"

class Knight : public Piece {
public:
    Knight(Color color);
    Knight(Color color, Position position);
    
    std::vector<Position> getPossibleMoves(const Board* board) const override;
    std::vector<Position> getAttackedSquares(const Board* board) const override;
    bool canMoveTo(const Position& target, const Board* board) const override;
    char getSymbol() const override;
    Piece* clone() const override;

private:
    bool isValidKnightMove(const Position& target) const;
    std::vector<Position> getKnightMoves() const;
};