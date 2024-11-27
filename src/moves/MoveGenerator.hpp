#pragma once
#include "board/Board.hpp"
#include "board/Square.hpp"
#include "moves/Move.hpp"
#include "pieces/Piece.hpp"
#include <vector>

class MoveGenerator {
public:
    struct CastlingRights {
        bool kingsideWhite = true;
        bool queensideWhite = true;
        bool kingsideBlack = true;
        bool queensideBlack = true;
        
        void reset() {
            kingsideWhite = queensideWhite = kingsideBlack = queensideBlack = true;
        }
        
        void removeCastlingRights(Piece::Color color) {
            if (color == Piece::Color::White) {
                kingsideWhite = queensideWhite = false;
            } else {
                kingsideBlack = queensideBlack = false;
            }
        }
    };

    MoveGenerator() { castlingRights.reset(); }

    static std::vector<Move> generateAllMoves(const Board* board, Piece::Color color);
    static std::vector<Move> generateLegalMoves(const Board* board, const Position& pos);
    static std::vector<Move> generateCaptureMoves(const Board* board, Piece::Color color);
    
    static bool isMoveLegal(const Board* board, const Move& move);
    
    static std::vector<Move> getCastlingMoves(const Board* board, Piece::Color color);
    static bool canCastleKingside(const Board* board, Piece::Color color);
    static bool canCastleQueenside(const Board* board, Piece::Color color);
    
    static std::vector<Move> getPromotionMoves(const Board* board, const Position& from, const Position& to);
    
    const CastlingRights& getCastlingRights() const { return castlingRights; }
    void updateCastlingRights(const Move& move);

private:
    CastlingRights castlingRights;

    static std::vector<Move> generatePawnMoves(const Board* board, const Position& pos);
    static std::vector<Move> generateKnightMoves(const Board* board, const Position& pos);
    static std::vector<Move> generateBishopMoves(const Board* board, const Position& pos);
    static std::vector<Move> generateRookMoves(const Board* board, const Position& pos);
    static std::vector<Move> generateQueenMoves(const Board* board, const Position& pos);
    static std::vector<Move> generateKingMoves(const Board* board, const Position& pos);
    
    static bool isEnPassantPossible(const Board* board, const Position& from, const Position& to);
    static bool isPawnPromotion(const Board* board, const Position& from, const Position& to);
    static bool wouldResultInCheck(const Board* board, const Move& move, Piece::Color color);
    
    static bool areCastlingSquaresClear(const Board* board, const Position& kingPos, bool kingside);
    static bool areCastlingSquaresSafe(const Board* board, const Position& kingPos, bool kingside, Piece::Color color);
    
    static Piece::Color getOppositeColor(Piece::Color color);
};