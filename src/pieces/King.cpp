#include "King.hpp"
#include <algorithm>

King::King(Color color)
    : Piece(color, Type::King, Position(0, 0)) {
}

King::King(Color color, Position position)
    : Piece(color, Type::King, position) {
}

std::vector<Position> King::getPossibleMoves(const Board* board) const {
    if (!board) {
        return std::vector<Position>();
    }

    std::vector<Position> moves;
    const std::vector<std::pair<int, int>> kingOffsets = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };

    Piece::Color opponentColor = (color == Color::White) ? Color::Black : Color::White;

    for (const auto& [dx, dy] : kingOffsets) {
        Position newPos = position + Position(dx, dy);
        if (!board->isPositionValid(newPos)) continue;

        const Square* targetSquare = board->getSquare(newPos);
        
        // Проверяем, атакована ли позиция противником
        if (!board->isPositionAttacked(newPos, opponentColor)) {
            // Если клетка пустая или на ней фигура противника
            if (!targetSquare->isOccupied() || 
                targetSquare->getPiece()->getColor() != color) {
                
                // Если на клетке фигура противника, проверяем, защищена ли она
                if (targetSquare->isOccupied() && 
                    targetSquare->getPiece()->getColor() == opponentColor) {
                    // Проверяем, защищена ли фигура
                    if (!board->isPositionDefended(newPos, opponentColor)) {
                        moves.push_back(newPos);
                    }
                } else {
                    moves.push_back(newPos);
                }
            }
        }
    }

    // Проверка возможности рокировки
    if (!moved && !board->isCheck(color)) {
        if (canCastleKingside(board)) {
            moves.push_back(Position(position.getX() + 2, position.getY()));
        }
        if (canCastleQueenside(board)) {
            moves.push_back(Position(position.getX() - 2, position.getY()));
        }
    }

    return moves;
}

std::vector<Position> King::getAttackedSquares(const Board* board) const {
    if (!board) {
        return std::vector<Position>();
    }

    std::vector<Position> moves;
    const std::vector<std::pair<int, int>> kingOffsets = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };
    
    for (const auto& [dx, dy] : kingOffsets) {
        Position newPos = position + Position(dx, dy);
        if (board->isPositionValid(newPos)) {
            moves.push_back(newPos);
        }
    }
    
    return moves;
}

bool King::canMoveTo(const Position& target, const Board* board) const {
    if (!board || !target.isValid()) {
        return false;
    }

    int dx = std::abs(target.getX() - position.getX());
    int dy = std::abs(target.getY() - position.getY());
    
    if (dx > 2 || dy > 1) return false;
    if (dx == 2 && dy != 0) return false;
    
    if (dx == 2 && !moved && !board->isCheck(color)) {
        int rookX = target.getX() > position.getX() ? 7 : 0;
        Position rookPos(rookX, position.getY());
        const Square* rookSquare = board->getSquare(rookPos);
        
        if (rookSquare->isOccupied() && 
            rookSquare->getPiece()->getType() == Type::Rook &&
            !rookSquare->getPiece()->hasMoved()) {
            return true;
        }
        return false;
    }

    const Square* targetSquare = board->getSquare(target);

    if (targetSquare->isOccupied() && targetSquare->getPiece()->getColor() == color) {
        return false;
    }

    Piece::Color opponentColor = (color == Color::White) ? Color::Black : Color::White;
    
    if (board->isPositionAttacked(target, opponentColor)) {
        if (targetSquare->isOccupied() && targetSquare->getPiece()->getColor() == opponentColor) {
            if (board->isPositionDefended(target, opponentColor)) {
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}

char King::getSymbol() const {
    return color == Color::White ? 'K' : 'k';
}

Piece* King::clone() const {
    return new King(*this);
}

bool King::isValidKingMove(const Position& target) const {
    int dx = std::abs(target.getX() - position.getX());
    int dy = std::abs(target.getY() - position.getY());
    return dx <= 1 && dy <= 1 && (dx > 0 || dy > 0);
}

std::vector<Position> King::getCastlingMoves(const Board* board) const {
    std::vector<Position> moves;
    if (moved || board->isCheck(color)) return moves;

    int baseRank = (color == Color::White) ? 0 : 7;

    if (canCastleKingside(board)) {
        moves.push_back(Position(6, baseRank));
    }

    if (canCastleQueenside(board)) {
        moves.push_back(Position(2, baseRank));
    }

    return moves;
}

bool King::canCastleKingside(const Board* board) const {
    int baseRank = (color == Color::White) ? 0 : 7;
    Position rookPos(7, baseRank);
    
    const Square* rookSquare = board->getSquare(rookPos);
    if (!rookSquare->isOccupied() || 
        rookSquare->getPiece()->getType() != Type::Rook ||
        rookSquare->getPiece()->hasMoved()) {
        return false;
    }

    // Проверяем путь
    for (int x = position.getX() + 1; x < rookPos.getX(); x++) {
        Position pos(x, baseRank);
        if (board->getSquare(pos)->isOccupied()) return false;
        if (board->isPositionAttacked(pos, color == Color::White ? Color::Black : Color::White)) {
            return false;
        }
    }

    return true;
}

bool King::canCastleQueenside(const Board* board) const {
    int baseRank = (color == Color::White) ? 0 : 7;
    Position rookPos(0, baseRank);
    
    const Square* rookSquare = board->getSquare(rookPos);
    if (!rookSquare->isOccupied() || 
        rookSquare->getPiece()->getType() != Type::Rook ||
        rookSquare->getPiece()->hasMoved()) {
        return false;
    }

    for (int x = position.getX() - 1; x > rookPos.getX(); x--) {
        Position pos(x, baseRank);
        if (board->getSquare(pos)->isOccupied()) return false;
        if (board->isPositionAttacked(pos, color == Color::White ? Color::Black : Color::White)) {
            if (x == 1) continue; 
            return false;
        }
    }

    return true;
}