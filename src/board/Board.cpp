#include "Board.hpp"
#include <stdexcept>

Board::Board() {
    setupEmptyBoard();
}

Board::Board(const std::string& fen) {
    setupEmptyBoard();
    setupFromFEN(fen);
}

Board::Board(const Board& other) : squares(BOARD_SIZE, std::vector<Square>(BOARD_SIZE)) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            squares[i][j] = other.squares[i][j];
        }
    }
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        squares = other.squares;
    }
    return *this;
}

void Board::setupEmptyBoard() {
    squares.resize(BOARD_SIZE, std::vector<Square>(BOARD_SIZE));
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            Square::Color squareColor = (i + j) % 2 == 0 ? Square::Color::White : Square::Color::Black;
            squares[i][j] = Square(squareColor, Position(i, j));
        }
    }
}

Square* Board::getSquare(const Position& pos) {
    if (!isPositionValid(pos)) {
        return nullptr;
    }
    return &squares[pos.getX()][pos.getY()];
}

const Square* Board::getSquare(const Position& pos) const {
    if (!isPositionValid(pos)) {
        return nullptr;
    }
    return &squares[pos.getX()][pos.getY()];
}

Square* Board::getSquare(int x, int y) {
    return getSquare(Position(x, y));
}

const Square* Board::getSquare(int x, int y) const {
    return getSquare(Position(x, y));
}

bool Board::placePiece(Piece* piece, const Position& pos) {
    if (!piece || !isPositionValid(pos)) {
        return false;
    }
    Square* square = getSquare(pos);
    if (square->isOccupied()) {
        return false;
    }
    square->setPiece(piece);
    return true;
}

Piece* Board::removePiece(const Position& pos) {
    if (!isPositionValid(pos)) {
        return nullptr;
    }
    Square* square = getSquare(pos);
    return square->removePiece();
}

bool Board::movePiece(const Position& from, const Position& to) {
    if (!isPositionValid(from) || !isPositionValid(to)) {
        return false;
    }
    
    Square* fromSquare = getSquare(from);
    Square* toSquare = getSquare(to);
    
    if (!fromSquare->isOccupied()) {
        return false;
    }
    
    Piece* piece = fromSquare->getPiece();
    
    if (piece->getType() == Piece::Type::Pawn) {
        int deltaY = std::abs(to.getY() - from.getY());
        if (deltaY == 2) {
            int direction = piece->getColor() == Piece::Color::White ? -1 : 1;
            setEnPassantPosition(Position(to.getX(), to.getY() + direction));
        } else {
            clearEnPassantPosition();
        }
    } else {
        clearEnPassantPosition();
    }
    
    fromSquare->removePiece();
    toSquare->setPiece(piece);
    
    return true;
}

bool Board::isPositionValid(const Position& pos) const {
    return pos.getX() >= 0 && pos.getX() < BOARD_SIZE && 
           pos.getY() >= 0 && pos.getY() < BOARD_SIZE;
}

bool Board::isPositionAttacked(const Position& pos, Piece::Color attackerColor) const {
    return isSquareAttackedByPawn(pos, attackerColor) ||
           isSquareAttackedByKnight(pos, attackerColor) ||
           isSquareAttackedByBishop(pos, attackerColor) ||
           isSquareAttackedByRook(pos, attackerColor) ||
           isSquareAttackedByQueen(pos, attackerColor) ||
           isSquareAttackedByKing(pos, attackerColor);
}

bool Board::isSquareAttackedByPawn(const Position& pos, Piece::Color attackerColor) const {
    int direction = (attackerColor == Piece::Color::White) ? 1 : -1;
    Position left(pos.getX() - 1, pos.getY() + direction);
    Position right(pos.getX() + 1, pos.getY() + direction);
    
    for (const Position& attackPos : {left, right}) {
        if (!isPositionValid(attackPos)) continue;
        
        const Square* square = getSquare(attackPos);
        if (square->isOccupied()) {
            Piece* piece = square->getPiece();
            if (piece->getColor() == attackerColor && 
                piece->getType() == Piece::Type::Pawn) {
                return true;
            }
        }
    }
    return false;
}

bool Board::isSquareAttackedByKnight(const Position& pos, Piece::Color attackerColor) const {
    const std::vector<Position> knightMoves = {
        Position(-2, -1), Position(-2, 1),
        Position(-1, -2), Position(-1, 2),
        Position(1, -2), Position(1, 2),
        Position(2, -1), Position(2, 1)
    };
    
    for (const Position& offset : knightMoves) {
        Position attackPos = pos + offset;
        if (!isPositionValid(attackPos)) continue;
        
        const Square* square = getSquare(attackPos);
        if (square->isOccupied()) {
            Piece* piece = square->getPiece();
            if (piece->getColor() == attackerColor && 
                piece->getType() == Piece::Type::Knight) {
                return true;
            }
        }
    }
    return false;
}

bool Board::isSquareAttackedByBishop(const Position& pos, Piece::Color attackerColor) const {
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };
    
    for (const auto& [dx, dy] : directions) {
        Position current = pos;
        while (true) {
            current = current + Position(dx, dy);
            if (!isPositionValid(current)) break;
            
            const Square* square = getSquare(current);
            if (square->isOccupied()) {
                Piece* piece = square->getPiece();
                if (piece->getColor() == attackerColor && 
                    piece->getType() == Piece::Type::Bishop) {
                    return true;
                }
                break;
            }
        }
    }
    return false;
}

bool Board::isSquareAttackedByRook(const Position& pos, Piece::Color attackerColor) const {
    const std::vector<std::pair<int, int>> directions = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0}
    };
    
    for (const auto& [dx, dy] : directions) {
        Position current = pos;
        while (true) {
            current = current + Position(dx, dy);
            if (!isPositionValid(current)) break;
            
            const Square* square = getSquare(current);
            if (square->isOccupied()) {
                Piece* piece = square->getPiece();
                if (piece->getColor() == attackerColor && 
                    piece->getType() == Piece::Type::Rook) {
                    return true;
                }
                break;
            }
        }
    }
    return false;
}

bool Board::isSquareAttackedByQueen(const Position& pos, Piece::Color attackerColor) const {
    const std::vector<std::pair<int, int>> directions = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0},
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };
    
    for (const auto& [dx, dy] : directions) {
        Position current = pos;
        while (true) {
            current = current + Position(dx, dy);
            if (!isPositionValid(current)) break;
            
            const Square* square = getSquare(current);
            if (square->isOccupied()) {
                Piece* piece = square->getPiece();
                if (piece->getColor() == attackerColor && 
                    piece->getType() == Piece::Type::Queen) {
                    return true;
                }
                break;
            }
        }
    }
    return false;
}

bool Board::isSquareAttackedByKing(const Position& pos, Piece::Color attackerColor) const {
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };
    
    for (const auto& [dx, dy] : directions) {
        Position attackPos = pos + Position(dx, dy);
        if (!isPositionValid(attackPos)) continue;
        
        const Square* square = getSquare(attackPos);
        if (square->isOccupied()) {
            Piece* piece = square->getPiece();
            if (piece->getColor() == attackerColor && 
                piece->getType() == Piece::Type::King) {
                return true;
            }
        }
    }
    return false;
}