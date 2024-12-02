#include "Piece.hpp"
#include "board/Board.hpp"
#include "board/Square.hpp"
#include <algorithm>

Piece::Piece(Color color, Type type)
    : color(color)
    , type(type)
    , position(0, 0)  
    , moved(false)
    , value(0)
{
    switch (type) {
        case Type::Pawn:   value = 1; break;
        case Type::Knight: value = 3; break;
        case Type::Bishop: value = 3; break;
        case Type::Rook:   value = 5; break;
        case Type::Queen:  value = 9; break;
        case Type::King:   value = 0; break;
    }
}

Piece::Piece(Color color, Type type, Position position)
    : color(color)
    , type(type)
    , position(position)
    , moved(false)
    , value(0)
{
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
    if (!board) return moves;

    const std::pair<int, int> directions[] = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0}
    };

    for (const auto& move : directions) {
        Position current = position;
        while (true) {
            current = current + Position(move.first, move.second);
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
    if (!board) return moves;

    const std::pair<int, int> directions[] = {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    for (const auto& move : directions) {
        Position current = position;
        while (true) {
            current = current + Position(move.first, move.second);
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
    if (!board) return false;
    
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
    if (!board) {
        return false;
    }

    const Square* square = board->getSquare(target);
    if (!square) return false;

    return !square->isOccupied() || square->getPiece()->getColor() != color;
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

Piece::PinInfo Piece::checkIfPinned(const Board* board) const {
    if (!board) return PinInfo();

    Position kingPos;
    bool kingFound = false;
    
    for (int x = 0; x < 8 && !kingFound; x++) {
        for (int y = 0; y < 8 && !kingFound; y++) {
            const Square* square = board->getSquare(x, y);
            if (square->isOccupied() && 
                square->getPiece()->getType() == Type::King &&
                square->getPiece()->getColor() == color) {
                kingPos = Position(x, y);
                kingFound = true;
            }
        }
    }
    
    if (!kingFound) return PinInfo();

    int dx = position.getX() - kingPos.getX();
    int dy = position.getY() - kingPos.getY();

    if (dx != 0 && dy != 0 && std::abs(dx) != std::abs(dy)) {
        return PinInfo();
    }

    int stepX = dx == 0 ? 0 : dx/std::abs(dx);
    int stepY = dy == 0 ? 0 : dy/std::abs(dy);

    Position current = kingPos + Position(stepX, stepY);
    bool foundOurPiece = false;

    while (current != position) {
        const Square* square = board->getSquare(current);
        if (square->isOccupied()) {
            return PinInfo(); 
        }
        current = current + Position(stepX, stepY);
    }

    current = position + Position(stepX, stepY);
    while (board->isPositionValid(current)) {
        const Square* square = board->getSquare(current);
        if (square->isOccupied()) {
            Piece* piece = square->getPiece();
            if (piece->getColor() != color) {
                bool canPin = false;
                if (dx == 0 || dy == 0) {
                    canPin = (piece->getType() == Type::Rook || 
                             piece->getType() == Type::Queen);
                } else {
                    canPin = (piece->getType() == Type::Bishop || 
                             piece->getType() == Type::Queen);
                }
                if (canPin) {
                    return PinInfo(true, Position(stepX, stepY));
                }
            }
            break;
        }
        current = current + Position(stepX, stepY);
    }
    
    return PinInfo();
}