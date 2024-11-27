#include "Position.hpp"

Position::Position() : x(-1), y(-1) {}

Position::Position(int x, int y) : x(x), y(y) {}

Position::Position(const std::string& algebraicNotation) {
    if (algebraicNotation.length() != 2) {
        x = -1;
        y = -1;
        return;
    }
    
    char file = algebraicNotation[0];
    if (file >= 'a' && file <= 'h') {
        x = file - 'a';
    } else if (file >= 'A' && file <= 'H') {
        x = file - 'A';
    } else {
        x = -1;
        y = -1;
        return;
    }
    
    char rank = algebraicNotation[1];
    if (rank >= '1' && rank <= '8') {
        y = rank - '1';
    } else {
        x = -1;
        y = -1;
        return;
    }
}

bool Position::isValid() const {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

std::string Position::toAlgebraic() const {
    if (!isValid()) {
        return "invalid";
    }
    
    std::string notation;
    notation += static_cast<char>('a' + x);  
    notation += static_cast<char>('1' + y);  
    return notation;
}

bool Position::operator==(const Position& other) const {
    return x == other.x && y == other.y;
}

bool Position::operator!=(const Position& other) const {
    return !(*this == other);
}

Position Position::operator+(const Position& other) const {
    return Position(x + other.x, y + other.y);
}

Position Position::operator-(const Position& other) const {
    return Position(x - other.x, y - other.y);
}