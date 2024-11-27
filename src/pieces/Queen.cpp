#include "Queen.hpp"

Queen::Queen(Color color)
    : Piece(color, Type::Queen, Position(0, 0)) {
}

Queen::Queen(Color color, Position position)
    : Piece(color, Type::Queen, position) {
}

std::vector<Position> Queen::getPossibleMoves(const Board* board) const {
    std::vector<Position> moves;
    if (!board) return moves;

    auto straightMoves = getStraightMoves(board);
    auto diagonalMoves = getDiagonalMoves(board);
    
    auto checkMove = [board, this](const Position& move) {
        Board tempBoard(*board);
        tempBoard.movePiece(position, move);
        return !tempBoard.isCheck(color);
    };
    
    for (const auto& move : straightMoves) {
        if (checkMove(move)) {
            moves.push_back(move);
        }
    }
    
    for (const auto& move : diagonalMoves) {
        if (checkMove(move)) {
            moves.push_back(move);
        }
    }
    
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
    return !targetSquare->isOccupied() || 
           targetSquare->getPiece()->getColor() != color;
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