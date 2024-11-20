#pragma once

#include <string>

namespace Chess {

class Position {
public:
    Position(int x, int y);
    Position(const std::string& algebraicNotation);  
    Position() : x(0), y(0) {}  
    
    int getX() const;
    int getY() const;
    
    std::string toAlgebraic() const;
    
    void move(int deltaX, int deltaY);
    Position moved(int deltaX, int deltaY) const;
    
    bool isValid() const;
    
    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;
    bool operator<(const Position& other) const;  
    
    static bool isValidPosition(int x, int y);
    static Position fromAlgebraic(const std::string& algebraic);
    
    static const int BOARD_SIZE = 8;
    
private:
    int x;  
    int y;  
    
    static int fileToX(char file);
    static int rankToY(char rank);
    static char xToFile(int x);
    static char yToRank(int y);
};

} 
