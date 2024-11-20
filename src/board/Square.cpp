#include "Square.hpp"
#include <sstream>

namespace Chess {

Square::Square() : position(0, 0), piece(nullptr) {}

Square::Square(const Position& pos) : position(pos), piece(nullptr) {}

Square::Square(const Square& other) : position(other.position) {
    if (other.piece) {
        piece = other.piece->clone();
    } else {
        piece = nullptr;
    }
}

Square& Square::operator=(const Square& other) {
    if (this != &other) {
        position = other.position;
        if (other.piece) {
            piece = other.piece->clone();
        } else {
            piece = nullptr;
        }
    }
    return *this;
}

void Square::setPiece(std::shared_ptr<Piece> newPiece) {
    piece = newPiece;
    if (piece) {
        piece->setPosition(position);
    }
}

std::shared_ptr<Piece> Square::getPiece() const {
    return piece;
}

std::shared_ptr<Piece> Square::removePiece() {
    auto removedPiece = piece;
    piece = nullptr;
    return removedPiece;
}

Position Square::getPosition() const {
    return position;
}

void Square::setPosition(const Position& pos) {
    position = pos;
    if (piece) {
        piece->setPosition(pos);
    }
}

bool Square::isOccupied() const {
    return piece != nullptr;
}

bool Square::isEmpty() const {
    return piece == nullptr;
}

bool Square::isLightSquare() const {
    return (position.getX() + position.getY()) % 2 == 0;
}

bool Square::isDarkSquare() const {
    return !isLightSquare();
}

bool Square::operator==(const Square& other) const {
    if (position != other.position) {
        return false;
    }
    
    if ((piece == nullptr) != (other.piece == nullptr)) {
        return false;
    }
    
    if (piece && other.piece) {
        return *piece == *other.piece;
    }
    
    return true;
}

bool Square::operator!=(const Square& other) const {
    return !(*this == other);
}

std::string Square::toString() const {
    std::stringstream ss;
    ss << position.toString();
    if (piece) {
        ss << ": " << piece->getSymbol();
    } else {
        ss << ": empty";
    }
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Square& square) {
    return os << square.toString();
}

} 