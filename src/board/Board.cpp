#include "Board.hpp"
#include <sstream>
#include <stdexcept>
#include "../pieces/Pawn.hpp"
#include "../pieces/Rook.hpp"
#include "../pieces/Knight.hpp"
#include "../pieces/Bishop.hpp"
#include "../pieces/Queen.hpp"
#include "../pieces/King.hpp"

namespace Chess {

Board::Board() {
    initializeSquares();
}

Board::Board(const Board& other) {
    copyBoard(other);
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        copyBoard(other);
    }
    return *this;
}

Board::~Board() = default;

void Board::initialize() {
    clear();
    
    placePiece(std::make_shared<Rook>(Color::WHITE), Position(0, 0));
    placePiece(std::make_shared<Knight>(Color::WHITE), Position(0, 1));
    placePiece(std::make_shared<Bishop>(Color::WHITE), Position(0, 2));
    placePiece(std::make_shared<Queen>(Color::WHITE), Position(0, 3));
    placePiece(std::make_shared<King>(Color::WHITE), Position(0, 4));
    placePiece(std::make_shared<Bishop>(Color::WHITE), Position(0, 5));
    placePiece(std::make_shared<Knight>(Color::WHITE), Position(0, 6));
    placePiece(std::make_shared<Rook>(Color::WHITE), Position(0, 7));
    
    for (int i = 0; i < BOARD_SIZE; ++i) {
        placePiece(std::make_shared<Pawn>(Color::WHITE), Position(1, i));
    }
    
    placePiece(std::make_shared<Rook>(Color::BLACK), Position(7, 0));
    placePiece(std::make_shared<Knight>(Color::BLACK), Position(7, 1));
    placePiece(std::make_shared<Bishop>(Color::BLACK), Position(7, 2));
    placePiece(std::make_shared<Queen>(Color::BLACK), Position(7, 3));
    placePiece(std::make_shared<King>(Color::BLACK), Position(7, 4));
    placePiece(std::make_shared<Bishop>(Color::BLACK), Position(7, 5));
    placePiece(std::make_shared<Knight>(Color::BLACK), Position(7, 6));
    placePiece(std::make_shared<Rook>(Color::BLACK), Position(7, 7));
    
    for (int i = 0; i < BOARD_SIZE; ++i) {
        placePiece(std::make_shared<Pawn>(Color::BLACK), Position(6, i));
    }
}

void Board::clear() {
    for (auto& row : squares) {
        for (auto& square : row) {
            square.removePiece();
        }
    }
}

bool Board::placePiece(std::shared_ptr<Piece> piece, const Position& pos) {
    if (!isValidPosition(pos) || !piece) {
        return false;
    }
    
    squares[pos.getX()][pos.getY()].setPiece(piece);
    piece->setPosition(pos);
    return true;
}

std::shared_ptr<Piece> Board::removePiece(const Position& pos) {
    if (!isValidPosition(pos)) {
        return nullptr;
    }
    
    return squares[pos.getX()][pos.getY()].removePiece();
}

std::shared_ptr<Piece> Board::getPiece(const Position& pos) const {
    if (!isValidPosition(pos)) {
        return nullptr;
    }
    
    return squares[pos.getX()][pos.getY()].getPiece();
}

bool Board::isValidPosition(const Position& pos) const {
    return isWithinBounds(pos.getX(), pos.getY());
}

bool Board::isEmpty(const Position& pos) const {
    return isValidPosition(pos) && !squares[pos.getX()][pos.getY()].isOccupied();
}

bool Board::isOccupied(const Position& pos) const {
    return isValidPosition(pos) && squares[pos.getX()][pos.getY()].isOccupied();
}

std::vector<Position> Board::getAllPiecePositions() const {
    std::vector<Position> positions;
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (squares[row][col].isOccupied()) {
                positions.emplace_back(row, col);
            }
        }
    }
    return positions;
}

std::vector<Position> Board::getPiecePositions(Color color) const {
    std::vector<Position> positions;
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            auto piece = squares[row][col].getPiece();
            if (piece && piece->getColor() == color) {
                positions.emplace_back(row, col);
            }
        }
    }
    return positions;
}

bool Board::isSquareUnderAttack(const Position& pos, Color attackingColor) const {
    auto positions = getPiecePositions(attackingColor);
    for (const auto& attackerPos : positions) {
        auto piece = getPiece(attackerPos);
        if (piece && piece->canAttack(*this, attackerPos)) {
            return true;
        }
    }
    return false;
}

std::vector<Position> Board::getAttackingSquares(const Position& pos) const {
    std::vector<Position> attackingSquares;
    auto piece = getPiece(pos);
    if (!piece) {
        return attackingSquares;
    }

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            Position targetPos(row, col);
            if (piece->canAttack(*this, targetPos)) {
                attackingSquares.push_back(targetPos);
            }
        }
    }
    return attackingSquares;
}

void Board::initializeSquares() {
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            squares[row][col] = Square(Position(row, col));
        }
    }
}

bool Board::isWithinBounds(int row, int col) const {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

void Board::copyBoard(const Board& other) {
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            auto piece = other.squares[row][col].getPiece();
            if (piece) {
                squares[row][col].setPiece(piece->clone());
            } else {
                squares[row][col].removePiece();
            }
        }
    }
}

std::string Board::toString() const {
    std::stringstream ss;
    ss << "  a b c d e f g h\n";
    for (int row = BOARD_SIZE - 1; row >= 0; --row) {
        ss << (row + 1) << " ";
        for (int col = 0; col < BOARD_SIZE; ++col) {
            auto piece = squares[row][col].getPiece();
            if (piece) {
                ss << piece->getSymbol() << " ";
            } else {
                ss << ". ";
            }
        }
        ss << (row + 1) << "\n";
    }
    ss << "  a b c d e f g h";
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
    return os << board.toString();
}

std::string Board::getFEN() const {
    std::stringstream ss;
    for (int row = BOARD_SIZE - 1; row >= 0; --row) {
        int emptyCount = 0;
        for (int col = 0; col < BOARD_SIZE; ++col) {
            auto piece = squares[row][col].getPiece();
            if (piece) {
                if (emptyCount > 0) {
                    ss << emptyCount;
                    emptyCount = 0;
                }
                ss << piece->getFENSymbol();
            } else {
                emptyCount++;
            }
        }
        if (emptyCount > 0) {
            ss << emptyCount;
        }
        if (row > 0) {
            ss << '/';
        }
    }
    return ss.str();
}

void Board::setFromFEN(const std::string& fen) {
    clear();
    
    int row = 7;
    int col = 0;
    
    for (char c : fen) {
        if (c == '/') {
            row--;
            col = 0;
        } else if (std::isdigit(c)) {
            col += c - '0';
        } else {
            Color color = std::isupper(c) ? Color::WHITE : Color::BLACK;
            char pieceChar = std::tolower(c);
            
            std::shared_ptr<Piece> piece;
            switch (pieceChar) {
                case 'p': piece = std::make_shared<Pawn>(color); break;
                case 'r': piece = std::make_shared<Rook>(color); break;
                case 'n': piece = std::make_shared<Knight>(color); break;
                case 'b': piece = std::make_shared<Bishop>(color); break;
                case 'q': piece = std::make_shared<Queen>(color); break;
                case 'k': piece = std::make_shared<King>(color); break;
                default: throw std::invalid_argument("Invalid FEN string");
            }
            
            placePiece(piece, Position(row, col));
            col++;
        }
    }
}

} // namespace Chess