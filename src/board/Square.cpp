#include "Square.hpp"

Square::Square() : 
    color(Color::White),
    position(0, 0),
    piece(nullptr) {
}

Square::Square(const Color color) :
    color(color),
    position(0, 0),
    piece(nullptr) {
}

Square::Square(const Color color, const Position position) :
    color(color),
    position(position),
    piece(nullptr) {
}

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
    result += piece != nullptr ? ", Occupied" : ", Empty";
    result += ")";
    return result;
}
