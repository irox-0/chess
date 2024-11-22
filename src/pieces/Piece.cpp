#include "Piece.hpp"
#include <algorithm>

Piece::Piece(Color color, Type type, Position position)
    : color(color)
    , type(type)
    , position(position)
    , moved(false) {
    switch (type) {
        case Type::Pawn:   value = 1; break;
        case Type::Knight: value = 3; break;
        case Type::Bishop: value = 3; break;
        case Type::Rook:   value = 5; break;
        case Type::Queen:  value = 9; break;
        case Type::King:   value = 0; break; 
    }
}

void Piece::setPosition(const Position& newPosition) {
    if (position != newPosition) {
        position = newPosition;
        moved = true;
    }
}

bool Piece::isValidMove(const Position& target, const Board* board) const {
    if (!board || !target.isValid()) {
        return false;
    }

    if (position == target) {
        return false;
    }

    if (!isSquareAccessible(target, board)) {
        return false;
    }

    return canMoveTo(target, board);
}

bool Piece::threatens(const Position& target, const Board* board) const {
    if (!board || !target.isValid()) {
        return false;
    }

    std::vector<Position> attackedSquares = getAttackedSquares(board);
    return std::find(attackedSquares.begin(), attackedSquares.end(), target) != attackedSquares.end();
}

std::vector<Position> Piece::getStraightMoves(const Board* board) const {
    std::vector<Position> moves;
    const std::pair<int, int> directions[] = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0}  
    };

    for (const auto& [dx, dy] : directions) {
        Position current = position;
        while (true) {
            current = current + Position(dx, dy);
            if (!current.isValid()) break;

            const Square* square = board->getSquare(current);
            if (!square) break;

            if (square->isOccupied()) {
                if (square->getPiece()->getColor() != color) {
                    moves.push_back(current);
                }
                break;
            }
            moves.push_back(current);
        }
    }
    return moves;
}

std::vector<Position> Piece::getDiagonalMoves(const Board* board) const {
    std::vector<Position> moves;
    const std::pair<int, int> directions[] = {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}  
    };

    for (const auto& [dx, dy] : directions) {
        Position current = position;
        while (true) {
            current = current + Position(dx, dy);
            if (!current.isValid()) break;

            const Square* square = board->getSquare(current);
            if (!square) break;

            if (square->isOccupied()) {
                if (square->getPiece()->getColor() != color) {
                    moves.push_back(current);
                }
                break;
            }
            moves.push_back(current);
        }
    }
    return moves;
}

bool Piece::isPathClear(const Position& target, const Board* board) const {
    int dx = target.getX() - position.getX();
    int dy = target.getY() - position.getY();
    int steps = std::max(std::abs(dx), std::abs(dy));

    if (steps == 0) return true;

    int stepX = dx / steps;
    int stepY = dy / steps;

    Position current = position;
    for (int i = 1; i < steps; ++i) {
        current = current + Position(stepX, stepY);
        const Square* square = board->getSquare(current);
        if (square && square->isOccupied()) {
            return false;
        }
    }
    return true;
}

bool Piece::isSquareAccessible(const Position& target, const Board* board) const {
    const Square* square = board->getSquare(target);
    if (!square) return false;

    if (square->isOccupied()) {
        return square->getPiece()->getColor() != color;
    }
    return true;
}

std::string Piece::toString() const {
    std::string result;
    result += "Piece(";
    result += color == Color::White ? "White" : "Black";
    result += " ";
    
    switch (type) {
        case Type::Pawn:   result += "Pawn";   break;
        case Type::Knight: result += "Knight"; break;
        case Type::Bishop: result += "Bishop"; break;
        case Type::Rook:   result += "Rook";   break;
        case Type::Queen:  result += "Queen";  break;
        case Type::King:   result += "King";   break;
    }
    
    result += " at " + position.toAlgebraic();
    result += moved ? ", moved" : ", not moved";
    result += ")";
    return result;
}