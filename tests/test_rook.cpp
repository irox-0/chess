#include <gtest/gtest.h>
#include "pieces/Rook.hpp"
#include "board/Board.hpp"
#include "pieces/King.hpp"
#include "pieces/Queen.hpp"
#include "pieces/Bishop.hpp"

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
TEST_F(RookTest, DiagonallyPinnedMoves) {
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    Rook* rook = new Rook(Piece::Color::White);
    board->placePiece(rook, Position("d2"));
    board->placePiece(new Queen(Piece::Color::Black), Position("c3"));
    
    auto moves = rook->getPossibleMoves(board);
    EXPECT_TRUE(moves.empty()) 
        << "Rook pinned diagonally should not have any legal moves";
}

TEST_F(RookTest, MovesUnderCheckValidation) {
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    Rook* rook = new Rook(Piece::Color::White);
    board->placePiece(rook, Position("e2"));
    board->placePiece(new Rook(Piece::Color::Black), Position("e8"));
    
    auto moves = rook->getPossibleMoves(board);
    for (const auto& move : moves) {
        EXPECT_EQ(move.getX(), 4) // 'e' file
            << "Rook cannot move away from protecting the king";
    }
}

TEST_F(RookTest, CaptureToSaveKing) {
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    Rook* rook = new Rook(Piece::Color::White);
    board->placePiece(rook, Position("a1"));
    board->placePiece(new Queen(Piece::Color::Black), Position("e3"));
    
    auto moves = rook->getPossibleMoves(board);
    bool canCaptureQueen = false;
    for (const auto& move : moves) {
        if (move == Position("e3")) canCaptureQueen = true;
    }
    EXPECT_TRUE(canCaptureQueen) 
        << "Rook should be able to capture the piece threatening the king";
}

TEST_F(RookTest, LegalMovesValidation) {
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    Rook* rook = new Rook(Piece::Color::White);
    board->placePiece(rook, Position("a1"));
    board->placePiece(new Bishop(Piece::Color::Black), Position("c3"));
    
    auto moves = rook->getPossibleMoves(board);
    for (const auto& move : moves) {
        Board tempBoard(*board);
        tempBoard.movePiece(rook->getPosition(), move);
        EXPECT_FALSE(tempBoard.isCheck(Piece::Color::White))
            << "Rook move should not result in check to own king";
    }
}
