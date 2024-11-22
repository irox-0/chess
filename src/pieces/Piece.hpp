#pragma once
#include "Position.hpp"
#include "Board.hpp"
#include <vector>
#include <memory>

class Piece {
public:
    enum class Color {
        White,
        Black
    };

    enum class Type {
        Pawn,
        Knight,
        Bishop,
        Rook,
        Queen,
        King
    };

    Piece(Color color, Type type, Position position);
    virtual ~Piece() = default;

    Color getColor() const { return color; }
    Type getType() const { return type; }
    Position getPosition() const { return position; }
    bool hasMoved() const { return moved; }
    int getValue() const { return value; }

    void setPosition(const Position& newPosition);
    void setMoved(bool hasMoved) { moved = hasMoved; }

    virtual std::vector<Position> getPossibleMoves(const Board* board) const = 0;
    virtual std::vector<Position> getAttackedSquares(const Board* board) const = 0;
    virtual bool canMoveTo(const Position& target, const Board* board) const = 0;
    virtual char getSymbol() const = 0;
    virtual Piece* clone() const = 0;

    bool isValidMove(const Position& target, const Board* board) const;
    bool threatens(const Position& target, const Board* board) const;
    std::string toString() const;

protected:
    std::vector<Position> getStraightMoves(const Board* board) const;
    std::vector<Position> getDiagonalMoves(const Board* board) const;
    bool isPathClear(const Position& target, const Board* board) const;
    bool isSquareAccessible(const Position& target, const Board* board) const;

    Color color;
    Type type;
    Position position;
    bool moved;
    int value;
};