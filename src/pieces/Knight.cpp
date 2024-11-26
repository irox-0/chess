#include "Knight.hpp"

Knight::Knight(Color color)
    : Piece(color, Type::Knight, Position(0, 0)) {
}

Knight::Knight(Color color, Position position)
    : Piece(color, Type::Knight, position) {
}

std::vector<Position> Knight::getPossibleMoves(const Board* board) const {
    if (!board) {
        return std::vector<Position>();
    }

    std::vector<Position> moves;
    auto possibleMoves = getKnightMoves();

    for (const auto& move : possibleMoves) {
        if (!board->isPositionValid(move)) {
            continue;
        }

        const Square* targetSquare = board->getSquare(move);
        if (!targetSquare->isOccupied() || 
            targetSquare->getPiece()->getColor() != color) {
            moves.push_back(move);
        }
    }

    return moves;
}

std::vector<Position> Knight::getAttackedSquares(const Board* board) const {
    return getPossibleMoves(board);
}

bool Knight::canMoveTo(const Position& target, const Board* board) const {
    if (!board || !target.isValid()) {
        return false;
    }

    if (!isValidKnightMove(target)) {
        return false;
    }

    const Square* targetSquare = board->getSquare(target);
    if (targetSquare->isOccupied()) {
        return targetSquare->getPiece()->getColor() != color;
    }

    return true;
}

char Knight::getSymbol() const {
    return color == Color::White ? 'N' : 'n';
}

Piece* Knight::clone() const {
    return new Knight(*this);
}

bool Knight::isValidKnightMove(const Position& target) const {
    int dx = std::abs(target.getX() - position.getX());
    int dy = std::abs(target.getY() - position.getY());
    return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
}

std::vector<Position> Knight::getKnightMoves() const {
    const std::vector<std::pair<int, int>> knightOffsets = {
        {-2, -1}, {-2, 1},
        {-1, -2}, {-1, 2},
        {1, -2}, {1, 2},
        {2, -1}, {2, 1}
    };

    std::vector<Position> moves;
    for (const auto& [dx, dy] : knightOffsets) {
        moves.push_back(position + Position(dx, dy));
    }

    return moves;
}