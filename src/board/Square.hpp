#pragma once

#include <memory>
#include "../pieces/Piece.hpp"
#include "../pieces/Position.hpp"

namespace Chess {

class Square {
public:
    Square();
    explicit Square(const Position& pos);
    Square(const Square& other);
    Square& operator=(const Square& other);
    ~Square() = default;

    void setPiece(std::shared_ptr<Piece> piece);
    std::shared_ptr<Piece> getPiece() const;
    std::shared_ptr<Piece> removePiece();
    
    Position getPosition() const;
    void setPosition(const Position& pos);
    
    bool isOccupied() const;
    bool isEmpty() const;
    
    bool isLightSquare() const;  
    bool isDarkSquare() const;   
    bool operator==(const Square& other) const;
    bool operator!=(const Square& other) const;

    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& os, const Square& square);

private:
    Position position;
    std::shared_ptr<Piece> piece;
};

} 
