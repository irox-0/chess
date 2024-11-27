#include <gtest/gtest.h>
#include "moves/MoveGenerator.hpp"
#include "board/Board.hpp"
#include "pieces/Pawn.hpp"
#include "pieces/Rook.hpp"
#include "pieces/Knight.hpp"
#include "pieces/Bishop.hpp"
#include "pieces/Queen.hpp"
#include "pieces/King.hpp"
#include <algorithm>

class MoveGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        board = new Board();
    }

    void TearDown() override {
        delete board;
    }

    bool containsMove(const std::vector<Move>& moves, const Position& from, const Position& to) {
        return std::find_if(moves.begin(), moves.end(),
            [&](const Move& move) {
                return move.getFrom() == from && move.getTo() == to;
            }) != moves.end();
    }

    Board* board;
    MoveGenerator moveGenerator;
};

TEST_F(MoveGeneratorTest, PawnInitialMoves) {
    Position pawnPos(1, 1); 
    board->placePiece(new Pawn(Piece::Color::White), pawnPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, pawnPos);
    
    EXPECT_EQ(moves.size(), 2);
    EXPECT_TRUE(containsMove(moves, pawnPos, Position(1, 2))); // b3
    EXPECT_TRUE(containsMove(moves, pawnPos, Position(1, 3))); // b4
}

TEST_F(MoveGeneratorTest, PawnCapture) {
    Position whitePawnPos(3, 3); 
    Position blackPawn1Pos(2, 4); 
    Position blackPawn2Pos(4, 4); 
    
    board->placePiece(new Pawn(Piece::Color::White), whitePawnPos);
    board->placePiece(new Pawn(Piece::Color::Black), blackPawn1Pos);
    board->placePiece(new Pawn(Piece::Color::Black), blackPawn2Pos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, whitePawnPos);
    
    EXPECT_EQ(moves.size(), 3);
    EXPECT_TRUE(containsMove(moves, whitePawnPos, blackPawn1Pos));
    EXPECT_TRUE(containsMove(moves, whitePawnPos, blackPawn2Pos));
    EXPECT_TRUE(containsMove(moves, whitePawnPos, Position(3, 4))); 
}

TEST_F(MoveGeneratorTest, EnPassant) {
    Position whitePawnPos(3, 4); 
    Position blackPawnPos(4, 4); 
    
    board->placePiece(new Pawn(Piece::Color::White), whitePawnPos);
    board->placePiece(new Pawn(Piece::Color::Black), blackPawnPos);
    board->setEnPassantPosition(Position(4, 5)); 
    
    auto moves = MoveGenerator::generateLegalMoves(board, whitePawnPos);
    
    bool hasEnPassant = std::any_of(moves.begin(), moves.end(),
        [](const Move& move) { return move.getType() == Move::Type::EnPassant; });
    
    EXPECT_TRUE(hasEnPassant);
}

TEST_F(MoveGeneratorTest, KnightMoves) {
    Position knightPos(3, 3); 
    board->placePiece(new Knight(Piece::Color::White), knightPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, knightPos);
    
    EXPECT_EQ(moves.size(), 8);
    
    const std::vector<Position> expectedPositions = {
        Position(1, 2), 
        Position(1, 4), 
        Position(2, 1), 
        Position(2, 5), 
        Position(4, 1), 
        Position(4, 5), 
        Position(5, 2), 
        Position(5, 4)  
    };
    
    for (const auto& pos : expectedPositions) {
        EXPECT_TRUE(containsMove(moves, knightPos, pos));
    }
}

TEST_F(MoveGeneratorTest, BishopMoves) {
    Position bishopPos(3, 3); 
    board->placePiece(new Bishop(Piece::Color::White), bishopPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, bishopPos);
    
    EXPECT_EQ(moves.size(), 13); 
}

TEST_F(MoveGeneratorTest, RookMoves) {
    Position rookPos(3, 3); 
    board->placePiece(new Rook(Piece::Color::White), rookPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, rookPos);
    
    EXPECT_EQ(moves.size(), 14); 
}

TEST_F(MoveGeneratorTest, QueenMoves) {
    Position queenPos(3, 3); 
    board->placePiece(new Queen(Piece::Color::White), queenPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, queenPos);
    
    EXPECT_EQ(moves.size(), 27); 
}

TEST_F(MoveGeneratorTest, Castling) {
    Position kingPos(4, 0);
    Position kingsideRookPos(7, 0);
    Position queensideRookPos(0, 0);
    
    King* king = new King(Piece::Color::White);
    king->setMoved(false);
    
    Rook* kingsideRook = new Rook(Piece::Color::White);
    kingsideRook->setMoved(false);
    
    Rook* queensideRook = new Rook(Piece::Color::White);
    queensideRook->setMoved(false);
    
    ASSERT_TRUE(board->placePiece(king, kingPos));
    ASSERT_TRUE(board->placePiece(kingsideRook, kingsideRookPos));
    ASSERT_TRUE(board->placePiece(queensideRook, queensideRookPos));
    
    auto castlingMoves = MoveGenerator::getCastlingMoves(board, Piece::Color::White);
    
    EXPECT_EQ(castlingMoves.size(), 2);
    EXPECT_TRUE(containsMove(castlingMoves, kingPos, Position(6, 0))); 
    EXPECT_TRUE(containsMove(castlingMoves, kingPos, Position(2, 0)));
}

TEST_F(MoveGeneratorTest, BlockedCastling) {
    Position kingPos(4, 0); 
    Position rookPos(7, 0); 
    Position blockingPos(6, 0); 
    board->placePiece(new King(Piece::Color::White), kingPos);
    board->placePiece(new Rook(Piece::Color::White), rookPos);
    board->placePiece(new Bishop(Piece::Color::White), blockingPos);
    
    auto castlingMoves = MoveGenerator::getCastlingMoves(board, Piece::Color::White);
    
    EXPECT_TRUE(castlingMoves.empty());
}

TEST_F(MoveGeneratorTest, CastlingUnderCheck) {
    Position kingPos(4, 0); 
    Position rookPos(7, 0); 
    Position enemyQueenPos(4, 7); 
    
    board->placePiece(new King(Piece::Color::White), kingPos);
    board->placePiece(new Rook(Piece::Color::White), rookPos);
    board->placePiece(new Queen(Piece::Color::Black), enemyQueenPos);
    
    auto castlingMoves = MoveGenerator::getCastlingMoves(board, Piece::Color::White);
    
    EXPECT_TRUE(castlingMoves.empty());
}

TEST_F(MoveGeneratorTest, PawnPromotion) {
    Position whitePawnPos(1, 6); 
    board->placePiece(new Pawn(Piece::Color::White), whitePawnPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, whitePawnPos);
    
    int promotionMoves = std::count_if(moves.begin(), moves.end(),
        [](const Move& move) { return move.getType() == Move::Type::Promotion; });
    
    EXPECT_EQ(promotionMoves, 4);
}

TEST_F(MoveGeneratorTest, CheckEvasion) {
    Position whiteKingPos(4, 0);
    Position blackQueenPos(4, 7); 
    board->placePiece(new King(Piece::Color::White), whiteKingPos);
    board->placePiece(new Queen(Piece::Color::Black), blackQueenPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, whiteKingPos);
    
    for (const auto& move : moves) {
        Board tempBoard(*board);
        tempBoard.movePiece(move.getFrom(), move.getTo());
        EXPECT_FALSE(tempBoard.isCheck(Piece::Color::White));
    }
}

