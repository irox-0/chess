#include "Bishop.hpp"

Bishop::Bishop(Color color)
    : Piece(color, Type::Bishop, Position(0, 0)) {
}

Bishop::Bishop(Color color, Position position)
    : Piece(color, Type::Bishop, position) {
}

std::vector<Position> Bishop::getPossibleMoves(const Board* board) const {
    std::vector<Position> moves;
    if (!board) return moves;

    auto pinInfo = checkIfPinned(board);
    
    if (pinInfo.isPinned) {
        if (std::abs(pinInfo.pinDirection.getX()) == std::abs(pinInfo.pinDirection.getY())) {
            Position current = position;
            while (true) {
                current = current + pinInfo.pinDirection;
                if (!board->isPositionValid(current)) break;
                
                const Square* targetSquare = board->getSquare(current);
                if (!targetSquare->isOccupied()) {
                    moves.push_back(current);
                } else {
                    if (targetSquare->getPiece()->getColor() != color) {
                        moves.push_back(current);
                    }
                    break;
                }
            }
            
            current = position;
            while (true) {
                current = current - pinInfo.pinDirection;
                if (!board->isPositionValid(current)) break;
                
                const Square* targetSquare = board->getSquare(current);
                if (!targetSquare->isOccupied()) {
                    moves.push_back(current);
                } else {
                    if (targetSquare->getPiece()->getColor() != color) {
                        moves.push_back(current);
                    }
                    break;
                }
            }
        }
        return moves;
    }

    const std::vector<std::pair<int, int>> directions = {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    Board tempBoard(*board);
    for (const auto& [dx, dy] : directions) {
        Position current = position;
        while (true) {
            current = current + Position(dx, dy);
            if (!board->isPositionValid(current)) break;

            const Square* targetSquare = board->getSquare(current);
            tempBoard = *board;
            tempBoard.movePiece(position, current);
            if (!tempBoard.isCheck(color == Color::White ? Color::Black : Color::White)) {
                if (!targetSquare->isOccupied()) {
                    moves.push_back(current);
                } else {
                    if (targetSquare->getPiece()->getColor() != color) {
                        moves.push_back(current);
                    }
                    break;
                }
            }
            if (targetSquare->isOccupied()) break;
        }
    }

    return moves;
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
    return !targetSquare->isOccupied() || 
           targetSquare->getPiece()->getColor() != color;
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