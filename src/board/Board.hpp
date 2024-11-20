#pragma once

#include <array>
#include <memory>
#include <vector>
#include <string>
#include "Square.hpp"
#include "../pieces/Piece.hpp"
#include "../pieces/Position.hpp"

namespace Chess {

class Board {
public:
    static const int BOARD_SIZE = 8;
    
    Board();
    Board(const Board& other);
    Board& operator=(const Board& other);
    ~Board();

    void initialize(); 
    void clear();     
    
    bool placePiece(std::shared_ptr<Piece> piece, const Position& pos);
    std::shared_ptr<Piece> removePiece(const Position& pos);
    std::shared_ptr<Piece> getPiece(const Position& pos) const;
    
    bool isValidPosition(const Position& pos) const;
    bool isEmpty(const Position& pos) const;
    bool isOccupied(const Position& pos) const;
    
    std::vector<Position> getAllPiecePositions() const;
    std::vector<Position> getPiecePositions(Color color) const;
    
    bool isSquareUnderAttack(const Position& pos, Color attackingColor) const;
    std::vector<Position> getAttackingSquares(const Position& pos) const;
    
    std::string getFEN() const;  // Получение позиции в формате FEN
    void setFromFEN(const std::string& fen);  // Установка позиции из FEN
    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& os, const Board& board);

private:
    std::array<std::array<Square, BOARD_SIZE>, BOARD_SIZE> squares;
    
    void copyBoard(const Board& other);
    void initializeSquares();
    bool isWithinBounds(int row, int col) const;
};

} 
