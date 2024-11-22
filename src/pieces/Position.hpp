#pragma once
#include <string>

class Position {
public:
    Position(int x, int y);
    Position(const std::string& algebraicNotation);  
    int getX() const { return x; }
    int getY() const { return y; }
    void setX(int x) { this->x = x;}
    void setRank(int y) { this->y = y; }
    
    bool isValid() const;  
    std::string toAlgebraic() const;  
    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;
    Position operator+(const Position& other) const;  
    
private:
    int x;  
    int y;  
};