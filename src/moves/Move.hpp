#pragma once
#include <memory>
#include <string>
#include "../pieces/Piece.hpp"
#include "../pieces/Position.hpp"

namespace Chess {

class Move {
public:
    // Типы специальных ходов
    enum class Type {
        Normal,         // Обычный ход
        Capture,        // Взятие фигуры
        Castle,         // Рокировка
        EnPassant,      // Взятие на проходе
        PawnPromotion, // Превращение пешки
        DoublePawnPush // Ход пешкой на два поля
    };

    enum class CastleType {
        None,
        KingSide,
        QueenSide
    };

    // Конструкторы
    Move(const Position& from, const Position& to);
    Move(const Position& from, const Position& to, Type type);
    Move(const Position& from, const Position& to, Type type, std::shared_ptr<Piece> capturedPiece);
    Move(const Position& from, const Position& to, Type type, std::shared_ptr<Piece> capturedPiece, 
         std::shared_ptr<Piece> promotedPiece);

    // Геттеры
    Position getFrom() const;
    Position getTo() const;
    Type getType() const;
    CastleType getCastleType() const;
    std::shared_ptr<Piece> getCapturedPiece() const;
    std::shared_ptr<Piece> getPromotedPiece() const;

    // Сеттеры
    void setCastleType(CastleType type);
    void setType(Type type);
    void setCapturedPiece(std::shared_ptr<Piece> piece);
    void setPromotedPiece(std::shared_ptr<Piece> piece);

    // Методы проверки типа хода
    bool isCapture() const;
    bool isCastle() const;
    bool isEnPassant() const;
    bool isPawnPromotion() const;
    bool isDoublePawnPush() const;

    // Преобразование в алгебраическую нотацию
    std::string toAlgebraic() const;
    std::string toLongAlgebraic() const;
    
    // Операторы сравнения
    bool operator==(const Move& other) const;
    bool operator!=(const Move& other) const;

    // Вывод в поток
    friend std::ostream& operator<<(std::ostream& os, const Move& move);

private:
    Position from;
    Position to;
    Type type;
    CastleType castleType;
    std::shared_ptr<Piece> capturedPiece;
    std::shared_ptr<Piece> promotedPiece;

    // Вспомогательные методы для преобразования в нотацию
    std::string getPieceLetter() const;
    std::string getPromotionLetter() const;
    std::string getCaptureSymbol() const;
    std::string getCastleNotation() const;
};

} // namespace Chess
