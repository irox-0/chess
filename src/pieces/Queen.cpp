#include "Queen.hpp"

Queen::Queen(Color color)
    : Piece(color, Type::Queen, Position(0, 0)) {
}

Queen::Queen(Color color, Position position)
    : Piece(color, Type::Queen, position) {
}

std::vector<Position> Queen::getPossibleMoves(const Board* board) const {
    if (!board) {
        return std::vector<Position>();
    }

    std::vector<Position> moves;
    
    auto straightMoves = getStraightMoves(board);
    moves.insert(moves.end(), straightMoves.begin(), straightMoves.end());
    
    auto diagonalMoves = getDiagonalMoves(board);
    moves.insert(moves.end(), diagonalMoves.begin(), diagonalMoves.end());
    
    return moves;
}

std::vector<Position> Queen::getAttackedSquares(const Board* board) const {
    return getPossibleMoves(board);
}

bool Queen::canMoveTo(const Position& target, const Board* board) const {
    if (!board || !target.isValid()) {
        return false;
    }

    if (!isValidQueenMove(target)) {
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

char Queen::getSymbol() const {
    return color == Color::White ? 'Q' : 'q';
}

Piece* Queen::clone() const {
    return new Queen(*this);
}

bool Queen::isValidQueenMove(const Position& target) const {
    int dx = std::abs(target.getX() - position.getX());
    int dy = std::abs(target.getY() - position.getY());
    
    bool straightMove = (dx == 0 && dy > 0) || (dx > 0 && dy == 0);
    bool diagonalMove = dx == dy && dx > 0;
    
    return straightMove || diagonalMove;
}