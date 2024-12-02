#pragma once
#include "pieces/Position.hpp"
#include "pieces/Piece.hpp"
#include <string>

class Square {
public:
    enum class Color {
        White,
        Black
    };

    Square();
    explicit Square(Color color);
    Square(Color color, Position position);

    Color getColor() const { return color; }
    bool isOccupied() const { return piece != nullptr; }
    Position getPosition() const { return position; }
    Piece* getPiece() const { return piece; }

    void setPiece(Piece* newPiece);
    Piece* removePiece();
    void clear();

    bool operator==(const Square& other) const;
    bool operator!=(const Square& other) const;

    std::string toString() const;

private:
    Color color;
    Position position;
    Piece* piece;
};