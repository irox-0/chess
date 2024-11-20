#include "Position.hpp"
#include <stdexcept>

namespace Chess {

Position::Position(int x, int y) : x(x), y(y) {
    if (!isValidPosition(x, y)) {
        throw std::invalid_argument("Invalid position coordinates");
    }
}

Position::Position(const std::string& algebraicNotation) {
    if (algebraicNotation.length() != 2) {
        throw std::invalid_argument("Invalid algebraic notation format");
    }
    
    char file = std::tolower(algebraicNotation[0]);
    char rank = algebraicNotation[1];
    
    x = fileToX(file);
    y = rankToY(rank);
    
    if (!isValidPosition(x, y)) {
        throw std::invalid_argument("Invalid algebraic notation position");
    }
}

int Position::getX() const {
    return x;
}

int Position::getY() const {
    return y;
}

std::string Position::toAlgebraic() const {
    std::string algebraic;
    algebraic += xToFile(x);
    algebraic += yToRank(y);
    return algebraic;
}

void Position::move(int deltaX, int deltaY) {
    int newX = x + deltaX;
    int newY = y + deltaY;
    
    if (!isValidPosition(newX, newY)) {
        throw std::invalid_argument("Movement results in invalid position");
    }
    
    x = newX;
    y = newY;
}

Position Position::moved(int deltaX, int deltaY) const {
    int newX = x + deltaX;
    int newY = y + deltaY;
    
    if (!isValidPosition(newX, newY)) {
        throw std::invalid_argument("Movement results in invalid position");
    }
    
    return Position(newX, newY);
}

bool Position::isValid() const {
    return isValidPosition(x, y);
}

bool Position::operator==(const Position& other) const {
    return x == other.x && y == other.y;
}

bool Position::operator!=(const Position& other) const {
    return !(*this == other);
}

bool Position::operator<(const Position& other) const {
    if (y != other.y) {
        return y < other.y;
    }
    return x < other.x;
}

bool Position::isValidPosition(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

Position Position::fromAlgebraic(const std::string& algebraic) {
    return Position(algebraic);
}

int Position::fileToX(char file) {
    if (file < 'a' || file > 'h') {
        throw std::invalid_argument("Invalid file character");
    }
    return file - 'a';
}

int Position::rankToY(char rank) {
    if (rank < '1' || rank > '8') {
        throw std::invalid_argument("Invalid rank character");
    }
    return rank - '1';
}

char Position::xToFile(int x) {
    return static_cast<char>('a' + x);
}

char Position::yToRank(int y) {
    return static_cast<char>('1' + y);
}

} 