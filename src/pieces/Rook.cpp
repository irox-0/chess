#include "Rook.hpp"


Rook::Rook(Color color)
    : Piece(color, Type::Rook, Position(0, 0)) { 
}

Rook::Rook(Color color, Position position)
    : Piece(color, Type::Rook, position) {
}

std::vector<Position> Rook::getPossibleMoves(const Board* board) const {
    if (!board) {
        return std::vector<Position>();
    }

    return getStraightMoves(board);
}

std::vector<Position> Rook::getAttackedSquares(const Board* board) const {
    return getPossibleMoves(board);
}

bool Rook::canMoveTo(const Position& target, const Board* board) const {
    if (!board || !target.isValid()) {
        return false;
    }

    if (!isValidRookMove(target)) {
        return false;
    }

    if (!isPathClear(target, board)) {
        return false;
    }

    const Square* targetSquare = board->getSquare(target);
    if (targetSquare->isOccupied()) {
        return targetSquare->getPiece()->getColor() != color;
    }

    return true;
}

char Rook::getSymbol() const {
    return color == Color::White ? 'R' : 'r';
}

Piece* Rook::clone() const {
    return new Rook(*this);
}

bool Rook::isValidRookMove(const Position& target) const {
    return position.getX() == target.getX() || position.getY() == target.getY();
}
