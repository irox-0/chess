#pragma once
#include "pieces/Position.hpp"
#include "pieces/Piece.hpp"

class Move {
public:
    enum class Type {
        Normal,         
        Capture,       
        EnPassant,     
        Castling,      
        Promotion,     
        DoublePawn     
    };

    Move(const Position& from, const Position& to);
    Move(const Position& from, const Position& to, Type type);
    Move(const Position& from, const Position& to, Type type, Piece::Type promotionPiece);

    Position getFrom() const { return from; }
    Position getTo() const { return to; }
    Type getType() const { return type; }
    Piece::Type getPromotionPiece() const { return promotionPiece; }

    void setType(Type newType) { type = newType; }
    void setPromotionPiece(Piece::Type piece) { promotionPiece = piece; }

    bool operator==(const Move& other) const;
    bool operator!=(const Move& other) const;

    std::string toAlgebraic() const;
    std::string toString() const;

private:
    Position from;
    Position to;
    Type type;
    Piece::Type promotionPiece;
};