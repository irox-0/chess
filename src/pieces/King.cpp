#include "King.hpp"

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
    auto basicMoves = getBasicMoves();
    
    // Filter basic moves
    for (const auto& move : basicMoves) {
        if (!board->isPositionValid(move)) {
            continue;
        }

        const Square* targetSquare = board->getSquare(move);
        if (!targetSquare->isOccupied() || 
            targetSquare->getPiece()->getColor() != color) {
            if (isSquareSafe(move, board)) {
                moves.push_back(move);
            }
        }
    }
    
    auto castlingMoves = getCastlingMoves(board);
    moves.insert(moves.end(), castlingMoves.begin(), castlingMoves.end());
    
    return moves;
}

std::vector<Position> King::getAttackedSquares(const Board* board) const {
    if (!board) {
        return std::vector<Position>();
    }

    std::vector<Position> moves;
    auto basicMoves = getBasicMoves();
    
    for (const auto& move : basicMoves) {
        if (board->isPositionValid(move)) {
            const Square* targetSquare = board->getSquare(move);
            if (!targetSquare->isOccupied() || 
                targetSquare->getPiece()->getColor() != color) {
                moves.push_back(move);
            }
        }
    }
    
    return moves;
}

bool King::canMoveTo(const Position& target, const Board* board) const {
    if (!board || !target.isValid()) {
        return false;
    }

    auto castlingMoves = getCastlingMoves(board);
    for (const auto& move : castlingMoves) {
        if (move == target) {
            return true;
        }
    }

    if (!isValidKingMove(target)) {
        return false;
    }

    const Square* targetSquare = board->getSquare(target);
    if (targetSquare->isOccupied() && 
        targetSquare->getPiece()->getColor() == color) {
        return false;
    }

    return isSquareSafe(target, board);
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

std::vector<Position> King::getBasicMoves() const {
    const std::vector<std::pair<int, int>> kingOffsets = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };

    std::vector<Position> moves;
    for (const auto& [dx, dy] : kingOffsets) {
        moves.push_back(position + Position(dx, dy));
    }

    return moves;
}

std::vector<Position> King::getCastlingMoves(const Board* board) const {
    std::vector<Position> moves;
    
    if (moved || board->isCheck(color)) {
        return moves;
    }

    if (canCastleKingside(board)) {
        moves.push_back(Position(position.getX() + 2, position.getY()));
    }

    if (canCastleQueenside(board)) {
        moves.push_back(Position(position.getX() - 2, position.getY()));
    }

    return moves;
}

bool King::canCastleKingside(const Board* board) const {
    int baseRank = (color == Color::White) ? 0 : 7;
    
    const Square* rookSquare = board->getSquare(7, baseRank);
    if (!rookSquare->isOccupied() || 
        rookSquare->getPiece()->getType() != Type::Rook ||
        rookSquare->getPiece()->hasMoved()) {
        return false;
    }

    for (int x = 5; x <= 6; x++) {
        if (board->getSquare(x, baseRank)->isOccupied()) {
            return false;
        }
        if (!isSquareSafe(Position(x, baseRank), board)) {
            return false;
        }
    }

    return true;
}

bool King::canCastleQueenside(const Board* board) const {
    int baseRank = (color == Color::White) ? 0 : 7;
    
    const Square* rookSquare = board->getSquare(0, baseRank);
    if (!rookSquare->isOccupied() || 
        rookSquare->getPiece()->getType() != Type::Rook ||
        rookSquare->getPiece()->hasMoved()) {
        return false;
    }

    for (int x = 1; x <= 3; x++) {
        if (board->getSquare(x, baseRank)->isOccupied()) {
            return false;
        }
        if (x >= 2 && !isSquareSafe(Position(x, baseRank), board)) {
            return false;
        }
    }

    return true;
}

bool King::isSquareSafe(const Position& pos, const Board* board) const {
    Board tempBoard(*board);
    
    tempBoard.movePiece(position, pos);
    
    return !tempBoard.isCheck(color);
}