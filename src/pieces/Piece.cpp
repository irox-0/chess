#include "Piece.hpp"
#include "../board/Board.hpp"
#include <memory>

namespace Chess {

Piece::Piece(PieceType type, Color color, Position position)
    : type(type)
    , color(color)
    , position(position)
    , moved(false)
{}

PieceType Piece::getType() const {
    return type;
}

Color Piece::getColor() const {
    return color;
}

Position Piece::getPosition() const {
    return position;
}

bool Piece::hasMoved() const {
    return moved;
}

void Piece::setPosition(const Position& newPosition) {
    position = newPosition;
    moved = true;
}

void Piece::setMoved(bool moved) {
    this->moved = moved;
}

bool Piece::isEnemy(const Piece* other) const {
    return other && color != other->getColor();
}

bool Piece::isSameColor(const Piece* other) const {
    return other && color == other->getColor();
}

bool Piece::isPathClear(const Position& target, const Board& board) const {
    int dx = target.getX() - position.getX();
    int dy = target.getY() - position.getY();
    
    // Determine step direction
    int stepX = (dx != 0) ? dx / std::abs(dx) : 0;
    int stepY = (dy != 0) ? dy / std::abs(dy) : 0;
    
    Position current = position;
    current.move(stepX, stepY);  // Start with first step
    
    // Check all squares between current position and target
    while (current != target) {
        if (board.getPiece(current) != nullptr) {
            return false;  // Path is blocked
        }
        current.move(stepX, stepY);
    }
    
    return true;
}

std::vector<Position> Piece::generateStraightMoves(const Board& board) const {
    std::vector<Position> moves;
    const int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};  // Up, Right, Down, Left
    
    for (const auto& dir : directions) {
        Position current = position;
        while (true) {
            current.move(dir[0], dir[1]);
            
            if (!current.isValid()) {
                break;  // Out of board
            }
            
            std::shared_ptr<Chess::Piece> pieceAtPosition = board.getPiece(current);
            if (pieceAtPosition) {
                if (isEnemy(pieceAtPosition.get())) {
                    moves.push_back(current);  // Can capture enemy piece
                }
                break;  // Can't move through pieces
            }
            
            moves.push_back(current);
        }
    }
    
    return moves;
}

std::vector<Position> Piece::generateDiagonalMoves(const Board& board) const {
    std::vector<Position> moves;
    const int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};  // Diagonal directions
    
    for (const auto& dir : directions) {
        Position current = position;
        while (true) {
            current.move(dir[0], dir[1]);
            
            if (!current.isValid()) {
                break;  // Out of board
            }
            
            std::shared_ptr<Chess::Piece> pieceAtPosition = board.getPiece(current);
            if (pieceAtPosition) {
                if (isEnemy(pieceAtPosition.get())) {
                    moves.push_back(current);  // Can capture enemy piece
                }
                break;  // Can't move through pieces
            }
            
            moves.push_back(current);
        }
    }
    
    return moves;
}

} // namespace Chess


