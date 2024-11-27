#include "Rook.hpp"

Rook::Rook(Color color)
    : Piece(color, Type::Rook, Position(0, 0)) { 
}

Rook::Rook(Color color, Position position)
    : Piece(color, Type::Rook, position) {
}

std::vector<Position> Rook::getPossibleMoves(const Board* board) const {
    std::vector<Position> moves;
    if (!board) return moves;

    auto pinInfo = checkIfPinned(board);
    
    if (pinInfo.isPinned) {
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
        
        return moves;
    }

    const std::vector<std::pair<int, int>> directions = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0}  // вверх, вниз, вправо, влево
    };

    for (const auto& [dx, dy] : directions) {
        Position current = position;
        while (true) {
            current = current + Position(dx, dy);
            if (!board->isPositionValid(current)) break;
            
            const Square* targetSquare = board->getSquare(current);
            if (!targetSquare->isOccupied()) {
                Board tempBoard(*board);
                tempBoard.movePiece(position, current);
                if (!tempBoard.isCheck(color)) {
                    moves.push_back(current);
                }
            } else {
                if (targetSquare->getPiece()->getColor() != color) {
                    Board tempBoard(*board);
                    tempBoard.movePiece(position, current);
                    if (!tempBoard.isCheck(color)) {
                        moves.push_back(current);
                    }
                }
                break;
            }
        }
    }

    return moves;
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
    return !targetSquare->isOccupied() || 
           targetSquare->getPiece()->getColor() != color;
}

char Rook::getSymbol() const {
    return color == Color::White ? 'R' : 'r';
}

Piece* Rook::clone() const {
    return new Rook(*this);
}

bool Rook::isValidRookMove(const Position& target) const {
    int dx = std::abs(target.getX() - position.getX());
    int dy = std::abs(target.getY() - position.getY());
    
    return (dx == 0 && dy > 0) || (dx > 0 && dy == 0);
}