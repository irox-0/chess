#include "Move.hpp"
#include <sstream>

Move::Move(const Position& from, const Position& to)
    : from(from)
    , to(to)
    , type(Type::Normal)
    , promotionPiece(Piece::Type::Pawn)
{
}

Move::Move(const Position& from, const Position& to, Type type)
    : from(from)
    , to(to)
    , type(type)
    , promotionPiece(Piece::Type::Pawn)
{
}

Move::Move(const Position& from, const Position& to, Type type, Piece::Type promotionPiece)
    : from(from)
    , to(to)
    , type(type)
    , promotionPiece(promotionPiece)
{
}

bool Move::operator==(const Move& other) const {
    return from == other.from && 
           to == other.to && 
           type == other.type &&
           (type != Type::Promotion || promotionPiece == other.promotionPiece);
}

bool Move::operator!=(const Move& other) const {
    return !(*this == other);
}

std::string Move::toAlgebraic() const {
    std::string notation = from.toAlgebraic() + to.toAlgebraic();
    
    if (type == Type::Promotion) {
        char promotionChar;
        switch (promotionPiece) {
            case Piece::Type::Queen:  promotionChar = 'q'; break;
            case Piece::Type::Rook:   promotionChar = 'r'; break;
            case Piece::Type::Bishop: promotionChar = 'b'; break;
            case Piece::Type::Knight: promotionChar = 'n'; break;
            default: promotionChar = 'q'; break;
        }
        notation += promotionChar;
    }
    
    return notation;
}

std::string Move::toString() const {
    std::stringstream ss;
    ss << "Move(" << from.toAlgebraic() << " -> " << to.toAlgebraic();
    
    switch (type) {
        case Type::Normal:    ss << ", Normal"; break;
        case Type::Capture:   ss << ", Capture"; break;
        case Type::EnPassant: ss << ", En Passant"; break;
        case Type::Castling:  ss << ", Castling"; break;
        case Type::DoublePawn: ss << ", Double Pawn"; break;
        case Type::Promotion: 
            ss << ", Promotion to ";
            switch (promotionPiece) {
                case Piece::Type::Queen:  ss << "Queen"; break;
                case Piece::Type::Rook:   ss << "Rook"; break;
                case Piece::Type::Bishop: ss << "Bishop"; break;
                case Piece::Type::Knight: ss << "Knight"; break;
                default: ss << "Unknown"; break;
            }
            break;
    }
    
    ss << ")";
    return ss.str();
}