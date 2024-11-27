#include "Board.hpp"
#include "pieces/Piece.hpp"
#include "pieces/Pawn.hpp"
#include "pieces/Rook.hpp"
#include "pieces/Knight.hpp"
#include "pieces/Bishop.hpp"
#include "pieces/Queen.hpp"
#include "pieces/King.hpp"
#include <stdexcept>
#include <sstream>
#include <cctype>

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
            if (other.squares[i][j].isOccupied()) {
                Piece* originalPiece = other.squares[i][j].getPiece();
                squares[i][j].setPiece(originalPiece->clone());
            }
        }
    }
    enPassantPosition = other.enPassantPosition;
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        clear();
        
        squares.resize(BOARD_SIZE, std::vector<Square>(BOARD_SIZE));
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                squares[i][j] = other.squares[i][j];
                if (other.squares[i][j].isOccupied()) {
                    Piece* originalPiece = other.squares[i][j].getPiece();
                    squares[i][j].setPiece(originalPiece->clone());
                }
            }
        }
        enPassantPosition = other.enPassantPosition;
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
    clearEnPassantPosition();
}

void Board::clear() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (squares[i][j].isOccupied()) {
                delete squares[i][j].removePiece();
            }
        }
    }
    clearEnPassantPosition();
}

void Board::initialize() {
    clear();
    
    placePiece(new Rook(Piece::Color::White), Position(0, 0));
    placePiece(new Knight(Piece::Color::White), Position(1, 0));
    placePiece(new Bishop(Piece::Color::White), Position(2, 0));
    placePiece(new Queen(Piece::Color::White), Position(3, 0));
    placePiece(new King(Piece::Color::White), Position(4, 0));
    placePiece(new Bishop(Piece::Color::White), Position(5, 0));
    placePiece(new Knight(Piece::Color::White), Position(6, 0));
    placePiece(new Rook(Piece::Color::White), Position(7, 0));
    
    for (int i = 0; i < BOARD_SIZE; ++i) {
        placePiece(new Pawn(Piece::Color::White), Position(i, 1));
    }
    
    placePiece(new Rook(Piece::Color::Black), Position(0, 7));
    placePiece(new Knight(Piece::Color::Black), Position(1, 7));
    placePiece(new Bishop(Piece::Color::Black), Position(2, 7));
    placePiece(new Queen(Piece::Color::Black), Position(3, 7));
    placePiece(new King(Piece::Color::Black), Position(4, 7));
    placePiece(new Bishop(Piece::Color::Black), Position(5, 7));
    placePiece(new Knight(Piece::Color::Black), Position(6, 7));
    placePiece(new Rook(Piece::Color::Black), Position(7, 7));
    
    for (int i = 0; i < BOARD_SIZE; ++i) {
        placePiece(new Pawn(Piece::Color::Black), Position(i, 6));
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
    if (!piece || !isPositionValid(pos)) return false;
    
    Square* square = getSquare(pos);
    if (!square || square->isOccupied()) return false;
    
    bool wasMoved = piece->hasMoved(); 
    piece->setPosition(pos);
    piece->setMoved(wasMoved); 
    square->setPiece(piece);
    return true;
}

Piece* Board::removePiece(const Position& pos) {
    if (!isPositionValid(pos)) {
        return nullptr;
    }
    return getSquare(pos)->removePiece();
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
    piece->setMoved(true);
    
    if (piece->getType() == Piece::Type::Pawn) {
        int deltaY = std::abs(to.getY() - from.getY());
        if (deltaY == 2) {
            int direction = piece->getColor() == Piece::Color::White ? -1 : 1;
            setEnPassantPosition(Position(to.getX(), to.getY() + direction));
        } else {
            clearEnPassantPosition();
            
            if (std::abs(to.getX() - from.getX()) == 1 && !toSquare->isOccupied()) {
                Position capturedPawnPos(to.getX(), from.getY());
                removePiece(capturedPawnPos);
            }
        }
    } else {
        clearEnPassantPosition();
    }
    
    if (toSquare->isOccupied()) {
        delete toSquare->removePiece();
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

bool Board::isPositionDefended(const Position& pos, Piece::Color defenderColor) const {
    return isPositionAttacked(pos, defenderColor);
}

bool Board::isCheck(Piece::Color color) const {
    Piece* king = getKing(color);
    if (!king) return false;
    
    return isPositionAttacked(king->getPosition(), 
        color == Piece::Color::White ? Piece::Color::Black : Piece::Color::White);
}

bool Board::isCheckmate(Piece::Color color) const {
    if (!isCheck(color)) return false;
    
    auto pieces = getPieces(color);
    for (Piece* piece : pieces) {
        auto moves = piece->getPossibleMoves(this);
        for (const Position& move : moves) {
            Board tempBoard(*this);
            if (tempBoard.movePiece(piece->getPosition(), move) && !tempBoard.isCheck(color)) {
                return false;
            }
        }
    }
    return true;
}

bool Board::isStalemate(Piece::Color color) const {
    if (isCheck(color)) return false;
    
    auto pieces = getPieces(color);
    for (Piece* piece : pieces) {
        auto moves = piece->getPossibleMoves(this);
        for (const Position& move : moves) {
            Board tempBoard(*this);
            if (tempBoard.movePiece(piece->getPosition(), move) && !tempBoard.isCheck(color)) {
                return false;
            }
        }
    }
    return true;
}

std::vector<Piece*> Board::getPieces(Piece::Color color) const {
    std::vector<Piece*> pieces;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            const Square* square = getSquare(i, j);
            if (square->isOccupied() && square->getPiece()->getColor() == color) {
                pieces.push_back(square->getPiece());
            }
        }
    }
    return pieces;
}

Piece* Board::getKing(Piece::Color color) const {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            const Square* square = getSquare(i, j);
            if (square->isOccupied()) {
                Piece* piece = square->getPiece();
                if (piece->getColor() == color && piece->getType() == Piece::Type::King) {
                    return piece;
                }
            }
        }
    }
    return nullptr;
}

std::vector<Position> Board::getAttackedPositions(Piece::Color attackerColor) const {
    std::vector<Position> positions;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            Position pos(i, j);
            if (isPositionAttacked(pos, attackerColor)) {
                positions.push_back(pos);
            }
        }
    }
    return positions;
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

void Board::setupFromFEN(const std::string& fen) {
    clear();
    
    std::istringstream iss(fen);
    std::string placement;
    iss >> placement;
    
    int rank = 7;
    int file = 0;
    
    for (char c : placement) {
        if (c == '/') {
            rank--;
            file = 0;
            continue;
        }
        
        if (std::isdigit(c)) {
            file += c - '0';
            continue;
        }
        
        Piece* piece = nullptr;
        Piece::Color color = std::isupper(c) ? Piece::Color::White : Piece::Color::Black;
        char pieceChar = std::tolower(c);
        
        switch (pieceChar) {
            case 'p': piece = new Pawn(color); break;
            case 'r': piece = new Rook(color); break;
            case 'n': piece = new Knight(color); break;
            case 'b': piece = new Bishop(color); break;
            case 'q': piece = new Queen(color); break;
            case 'k': piece = new King(color); break;
        }
        
        if (piece) {
            placePiece(piece, Position(file, rank));
        }
        
        file++;
    }
}

std::string Board::toFEN() const {
    std::string fen;
    
    for (int rank = 7; rank >= 0; rank--) {
        int emptyCount = 0;
        
        for (int file = 0; file < BOARD_SIZE; file++) {
            const Square* square = getSquare(file, rank);
            
            if (square->isOccupied()) {
                if (emptyCount > 0) {
                    fen += std::to_string(emptyCount);
                    emptyCount = 0;
                }
                fen += square->getPiece()->getSymbol();
            } else {
                emptyCount++;
            }
        }
        
        if (emptyCount > 0) {
            fen += std::to_string(emptyCount);
        }
        
        if (rank > 0) {
            fen += '/';
        }
    }
    
    return fen;
}

std::string Board::toString() const {
    std::string result;
    for (int rank = BOARD_SIZE - 1; rank >= 0; rank--) {
        result += std::to_string(rank + 1) + " ";
        for (int file = 0; file < BOARD_SIZE; file++) {
            const Square* square = getSquare(file, rank);
            if (square->isOccupied()) {
                result += square->getPiece()->getSymbol();
            } else {
                result += square->getColor() == Square::Color::White ? "." : " ";
            }
            result += " ";
        }
        result += "\n";
    }
    result += "  a b c d e f g h\n";
    return result;
}