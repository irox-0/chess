#pragma once

#include <vector>
#include <memory>
#include "../board/Square.hpp"
#include "Position.hpp"

namespace Chess {

enum class PieceType {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum class Color {
    WHITE,
    BLACK
};

class Piece {
public:
    // Constructor
    Piece(PieceType type, Color color, Position position);
    
    // Virtual destructor for proper inheritance
    virtual ~Piece() = default;

    // Getter methods
    PieceType getType() const;
    Color getColor() const;
    Position getPosition() const;
    bool hasMoved() const;
    
    // Setter methods
    void setPosition(const Position& newPosition);
    void setMoved(bool moved);

    // Pure virtual method for generating possible moves
    virtual std::vector<Position> getPossibleMoves(const class Board& board) const = 0;
    
    // Method to check if a move to target position is valid
    virtual bool isValidMove(const Position& target, const Board& board) const = 0;
    
    // Method to get piece value for AI evaluation
    virtual int getValue() const = 0;
    
    // Utility methods
    bool isEnemy(const Piece* other) const;
    bool isSameColor(const Piece* other) const;
    
    virtual bool canAttack(const Board& board, const Position& target) const = 0;

    // Return piece symbol for board display
    virtual char getSymbol() const = 0;

protected:
    PieceType type;
    Color color;
    Position position;
    bool moved;  // Track if piece has moved (useful for castling and pawn first move)
    
    // Helper methods for move validation
    bool isPathClear(const Position& target, const Board& board) const;
    std::vector<Position> generateStraightMoves(const Board& board) const;
    std::vector<Position> generateDiagonalMoves(const Board& board) const;
};

} // namespace Chess
