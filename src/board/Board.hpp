#pragma once
#include "Square.hpp"
#include <vector>
#include <string>
#include <memory>

class Board {
public:
    static const int BOARD_SIZE = 8;
    
    Board();
    Board(const std::string& fen);
    Board(const Board& other);
    Board& operator=(const Board& other);
    
    Square* getSquare(const Position& pos);
    const Square* getSquare(const Position& pos) const;
    Square* getSquare(int x, int y);
    const Square* getSquare(int x, int y) const;
    
    bool placePiece(Piece* piece, const Position& pos);
    Piece* removePiece(const Position& pos);
    bool movePiece(const Position& from, const Position& to);
    
    bool isPositionValid(const Position& pos) const;
    bool isPositionAttacked(const Position& pos, Piece::Color attackerColor) const;
    bool isPositionDefended(const Position& pos, Piece::Color defenderColor) const;
    
    std::vector<Piece*> getPieces(Piece::Color color) const;
    std::vector<Position> getAttackedPositions(Piece::Color attackerColor) const;
    Piece* getKing(Piece::Color color) const;
    
    bool isCheck(Piece::Color color) const;
    bool isCheckmate(Piece::Color color) const;
    bool isStalemate(Piece::Color color) const;
    
    void clear();
    void initialize();
    std::string toFEN() const;
    std::string toString() const;

    Position getEnPassantPosition() const { return enPassantPosition; }
    void setEnPassantPosition(const Position& pos) { enPassantPosition = pos; }
    void clearEnPassantPosition() { enPassantPosition = Position(-1, -1); }
    void setupFromFEN(const std::string& fen);
    
private:
    std::vector<std::vector<Square>> squares;
    Position enPassantPosition;

    void setupEmptyBoard();

    bool isSquareAttackedByPawn(const Position& pos, Piece::Color attackerColor) const;
    bool isSquareAttackedByKnight(const Position& pos, Piece::Color attackerColor) const;
    bool isSquareAttackedByBishop(const Position& pos, Piece::Color attackerColor) const;
    bool isSquareAttackedByRook(const Position& pos, Piece::Color attackerColor) const;
    bool isSquareAttackedByQueen(const Position& pos, Piece::Color attackerColor) const;
    bool isSquareAttackedByKing(const Position& pos, Piece::Color attackerColor) const;
};