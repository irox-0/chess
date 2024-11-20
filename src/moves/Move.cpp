#include "Move.hpp"
#include "../pieces/Pawn.hpp"
#include "../pieces/King.hpp"
#include <sstream>

namespace Chess {

Move::Move(const Position& from, const Position& to)
    : from(from), to(to), type(Type::Normal), castleType(CastleType::None),
      capturedPiece(nullptr), promotedPiece(nullptr) {}

Move::Move(const Position& from, const Position& to, Type type)
    : from(from), to(to), type(type), castleType(CastleType::None),
      capturedPiece(nullptr), promotedPiece(nullptr) {}

Move::Move(const Position& from, const Position& to, Type type, 
           std::shared_ptr<Piece> capturedPiece)
    : from(from), to(to), type(type), castleType(CastleType::None),
      capturedPiece(capturedPiece), promotedPiece(nullptr) {}

Move::Move(const Position& from, const Position& to, Type type,
           std::shared_ptr<Piece> capturedPiece, std::shared_ptr<Piece> promotedPiece)
    : from(from), to(to), type(type), castleType(CastleType::None),
      capturedPiece(capturedPiece), promotedPiece(promotedPiece) {}

Position Move::getFrom() const { return from; }
Position Move::getTo() const { return to; }
Move::Type Move::getType() const { return type; }
Move::CastleType Move::getCastleType() const { return castleType; }
std::shared_ptr<Piece> Move::getCapturedPiece() const { return capturedPiece; }
std::shared_ptr<Piece> Move::getPromotedPiece() const { return promotedPiece; }

void Move::setCastleType(CastleType type) { castleType = type; }
void Move::setType(Type t) { type = t; }
void Move::setCapturedPiece(std::shared_ptr<Piece> piece) { capturedPiece = piece; }
void Move::setPromotedPiece(std::shared_ptr<Piece> piece) { promotedPiece = piece; }

bool Move::isCapture() const { return type == Type::Capture || type == Type::EnPassant; }
bool Move::isCastle() const { return type == Type::Castle; }
bool Move::isEnPassant() const { return type == Type::EnPassant; }
bool Move::isPawnPromotion() const { return type == Type::PawnPromotion; }
bool Move::isDoublePawnPush() const { return type == Type::DoublePawnPush; }

std::string Move::getPieceLetter() const {
    if (dynamic_cast<Pawn*>(capturedPiece.get())) {
        return "";
    }
    if (capturedPiece) {
        return capturedPiece->getSymbol();
    }
    return "";
}

std::string Move::getPromotionLetter() const {
    if (promotedPiece) {
        return "=" + promotedPiece->getSymbol();
    }
    return "";
}

std::string Move::getCaptureSymbol() const {
    return isCapture() ? "x" : "";
}

std::string Move::getCastleNotation() const {
    if (castleType == CastleType::KingSide) {
        return "O-O";
    } else if (castleType == CastleType::QueenSide) {
        return "O-O-O";
    }
    return "";
}

std::string Move::toAlgebraic() const {
    if (isCastle()) {
        return getCastleNotation();
    }

    std::stringstream ss;
    ss << getPieceLetter()
       << getCaptureSymbol()
       << to.toString()
       << getPromotionLetter();

    return ss.str();
}

std::string Move::toLongAlgebraic() const {
    if (isCastle()) {
        return getCastleNotation();
    }

    std::stringstream ss;
    ss << from.toString()
       << getCaptureSymbol()
       << to.toString()
       << getPromotionLetter();

    return ss.str();
}

bool Move::operator==(const Move& other) const {
    return from == other.from &&
           to == other.to &&
           type == other.type &&
           castleType == other.castleType &&
           ((capturedPiece == nullptr && other.capturedPiece == nullptr) ||
            (capturedPiece != nullptr && other.capturedPiece != nullptr &&
             *capturedPiece == *other.capturedPiece)) &&
           ((promotedPiece == nullptr && other.promotedPiece == nullptr) ||
            (promotedPiece != nullptr && other.promotedPiece != nullptr &&
             *promotedPiece == *other.promotedPiece));
}

bool Move::operator!=(const Move& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
    return os << move.toLongAlgebraic();
}

} 