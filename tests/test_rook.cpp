#include <gtest/gtest.h>
#include "pieces/Rook.hpp"
#include "board/Board.hpp"

class RookTest : public ::testing::Test {
protected:
    void SetUp() override {
        board = new Board();
    }

    void TearDown() override {
        delete board;
    }

    Board* board;
};

TEST_F(RookTest, InitialPosition) {
    Rook rook(Piece::Color::White, Position(0, 0));
    EXPECT_EQ(rook.getPosition(), Position(0, 0));
    EXPECT_EQ(rook.getColor(), Piece::Color::White);
    EXPECT_EQ(rook.getType(), Piece::Type::Rook);
}

TEST_F(RookTest, ValidMoves) {
    Rook rook(Piece::Color::White, Position(3, 3));
    board->placePiece(&rook, Position(3, 3));
    
    auto moves = rook.getPossibleMoves(board);
    
    EXPECT_EQ(moves.size(), 14);
}

TEST_F(RookTest, BlockedMoves) {
    Rook rook(Piece::Color::White, Position(3, 3));
    board->placePiece(&rook, Position(3, 3));
    
    Piece* blockingPiece1 = new Rook(Piece::Color::White, Position(3, 4));
    Piece* blockingPiece2 = new Rook(Piece::Color::White, Position(4, 3));
    board->placePiece(blockingPiece1, Position(3, 4));
    board->placePiece(blockingPiece2, Position(4, 3));
    
    auto moves = rook.getPossibleMoves(board);
    
    EXPECT_LT(moves.size(), 14);
    
    delete blockingPiece1;
    delete blockingPiece2;
}

TEST_F(RookTest, CaptureMove) {
    Rook rook(Piece::Color::White, Position(3, 3));
    board->placePiece(&rook, Position(3, 3));
    
    Piece* enemyPiece = new Rook(Piece::Color::Black, Position(3, 6));
    board->placePiece(enemyPiece, Position(3, 6));
    
    EXPECT_TRUE(rook.canMoveTo(Position(3, 6), board));
    
    delete enemyPiece;
}

TEST_F(RookTest, InvalidMoves) {
    Rook rook(Piece::Color::White, Position(3, 3));
    board->placePiece(&rook, Position(3, 3));
    
    EXPECT_FALSE(rook.canMoveTo(Position(4, 4), board));
    EXPECT_FALSE(rook.canMoveTo(Position(2, 4), board));
    
    EXPECT_FALSE(rook.canMoveTo(Position(5, 4), board));
}

TEST_F(RookTest, AttackedSquares) {
    Rook rook(Piece::Color::White, Position(3, 3));
    board->placePiece(&rook, Position(3, 3));
    
    auto attackedSquares = rook.getAttackedSquares(board);
    auto possibleMoves = rook.getPossibleMoves(board);
    
    EXPECT_EQ(attackedSquares.size(), possibleMoves.size());
}