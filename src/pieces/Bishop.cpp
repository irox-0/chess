#include "Bishop.hpp"

Bishop::Bishop(Color color)
    : Piece(color, Type::Bishop, Position(0, 0)) {
}

Bishop::Bishop(Color color, Position position)
    : Piece(color, Type::Bishop, position) {
}

std::vector<Position> Bishop::getPossibleMoves(const Board* board) const {
    if (!board) {
        return std::vector<Position>();
    }

    return getDiagonalMoves(board);
}

std::vector<Position> Bishop::getAttackedSquares(const Board* board) const {
    return getPossibleMoves(board);
}

bool Bishop::canMoveTo(const Position& target, const Board* board) const {
    if (!board || !target.isValid()) {
        return false;
    }

    if (!isValidBishopMove(target)) {
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

char Bishop::getSymbol() const {
    return color == Color::White ? 'B' : 'b';
}

Piece* Bishop::clone() const {
    return new Bishop(*this);
}

bool Bishop::isValidBishopMove(const Position& target) const {
    int dx = std::abs(target.getX() - position.getX());
    int dy = std::abs(target.getY() - position.getY());
    return dx == dy && dx > 0;
}