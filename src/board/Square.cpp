#include "Square.hpp"
#include "Board.hpp"

Square::Square(Color color) : 
    color(color), 
    position(0, 0),  
    piece(nullptr) {}

Square::Square(Color color, Position position) : 
    color(color), 
    position(position), 
    piece(nullptr) {}

void Square::setPiece(Piece* newPiece) {
    if (piece != nullptr) {
        clear();
    }
    piece = newPiece;
    if (piece != nullptr) {
        piece->setPosition(position);
    }
}

Piece* Square::removePiece() {
    Piece* removedPiece = piece;
    piece = nullptr;
    return removedPiece;
}

void Square::clear() {
    piece = nullptr;
}


bool Square::isAttacked(Piece::Color attackerColor) const {
    extern Board* globalBoard;  
    if (globalBoard) {
        return globalBoard->isPositionAttacked(position, attackerColor);
    }
    return false;
}

bool Square::isDefended(Piece::Color defenderColor) const {
    extern Board* globalBoard;
    if (globalBoard) {
        return globalBoard->isPositionAttacked(position, defenderColor);
    }
    return false;
}


bool Square::isAccessible(Piece::Color pieceColor) const {
    if (!position.isValid()) {
        return false;
    }
    
    return !isOccupied() || (piece && piece->getColor() != pieceColor);
}

bool Square::operator==(const Square& other) const {
    return position == other.position && 
           color == other.color && 
           piece == other.piece;
}

bool Square::operator!=(const Square& other) const {
    return !(*this == other);
}

std::string Square::toString() const {
    std::string result;
    
    result += position.toAlgebraic();
    result += "(";
    
    result += (color == Color::White ? "White" : "Black");
    
    if (piece != nullptr) {
        result += ", Occupied";
    } else {
        result += ", Empty";
    }
    
    result += ")";
    return result;
}