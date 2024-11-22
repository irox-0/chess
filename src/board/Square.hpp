#pragma once
#include "Position.hpp"

class Piece;

class Square {
public:
    enum class Color {
        White,
        Black
    };

    Square(Color color);
    Square(Color color, Position position);
    
    Color getColor() const { return color; }
    bool isOccupied() const { return piece != nullptr; }
    Position getPosition() const { return position; }
    Piece* getPiece() const { return piece; }
    
    void setPiece(Piece* newPiece);
    Piece* removePiece();
    void clear();
    
    bool isAttacked(Piece::Color attackerColor) const;
    bool isDefended(Piece::Color defenderColor) const;
    bool isAccessible(Piece::Color pieceColor) const;
    
    bool operator==(const Square& other) const;
    bool operator!=(const Square& other) const;
    
    std::string toString() const;
    
private:
    Color color;         
    Position position;    
    Piece* piece;        
};