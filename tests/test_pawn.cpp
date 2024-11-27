#include <gtest/gtest.h>
#include "board/Board.hpp"
#include "pieces/Pawn.hpp"

class PawnTest : public ::testing::Test {
protected:
    void SetUp() override {
        board = new Board();
    }

    void TearDown() override {
        delete board;
    }

    Board* board;
};

TEST_F(PawnTest, BasicProperties) {
    Pawn whitePawn(Piece::Color::White, Position(4, 1));
    
    EXPECT_EQ(whitePawn.getColor(), Piece::Color::White);
    EXPECT_EQ(whitePawn.getType(), Piece::Type::Pawn);
    EXPECT_EQ(whitePawn.getPosition(), Position(4, 1));
    EXPECT_EQ(whitePawn.getValue(), 1);
    EXPECT_FALSE(whitePawn.hasMoved());
    EXPECT_EQ(whitePawn.getSymbol(), 'P');

    Pawn blackPawn(Piece::Color::Black, Position(4, 6));
    EXPECT_EQ(blackPawn.getSymbol(), 'p');
}

TEST_F(PawnTest, WhitePawnMoves) {
    Pawn* pawn = new Pawn(Piece::Color::White, Position(4, 1));
    board->placePiece(pawn, Position(4, 1));

    auto moves = pawn->getPossibleMoves(board);
    
    bool hasOneStep = false;
    bool hasTwoSteps = false;
    
    for (const auto& move : moves) {
        if (move == Position(4, 2)) hasOneStep = true;
        if (move == Position(4, 3)) hasTwoSteps = true;
    }
    
    EXPECT_TRUE(hasOneStep);
    EXPECT_TRUE(hasTwoSteps);
}

TEST_F(PawnTest, BlackPawnMoves) {
    Pawn* pawn = new Pawn(Piece::Color::Black, Position(4, 6));
    board->placePiece(pawn, Position(4, 6));

    auto moves = pawn->getPossibleMoves(board);
    EXPECT_EQ(moves.size(), 2);

    bool hasOneStep = false;
    bool hasTwoSteps = false;
    
    for (const auto& move : moves) {
        if (move == Position(4, 5)) hasOneStep = true;
        if (move == Position(4, 4)) hasTwoSteps = true;
    }
    
    EXPECT_TRUE(hasOneStep);
    EXPECT_TRUE(hasTwoSteps);
}

TEST_F(PawnTest, PawnCapture) {
    Pawn* whitePawn = new Pawn(Piece::Color::White, Position(4, 4));
    board->placePiece(whitePawn, Position(4, 4));
    
    Pawn* blackPawn1 = new Pawn(Piece::Color::Black, Position(3, 5));
    Pawn* blackPawn2 = new Pawn(Piece::Color::Black, Position(5, 5));
    board->placePiece(blackPawn1, Position(3, 5));
    board->placePiece(blackPawn2, Position(5, 5));

    auto moves = whitePawn->getPossibleMoves(board);
    EXPECT_EQ(moves.size(), 3); 

    bool hasForwardMove = false;
    bool hasLeftCapture = false;
    bool hasRightCapture = false;
    
    for (const auto& move : moves) {
        if (move == Position(4, 5)) hasForwardMove = true;
        if (move == Position(3, 5)) hasLeftCapture = true;
        if (move == Position(5, 5)) hasRightCapture = true;
    }
    
    EXPECT_TRUE(hasForwardMove);
    EXPECT_TRUE(hasLeftCapture);
    EXPECT_TRUE(hasRightCapture);
}

TEST_F(PawnTest, BlockedPawn) {
    Pawn* whitePawn = new Pawn(Piece::Color::White, Position(4, 4));
    board->placePiece(whitePawn, Position(4, 4));
    
    Pawn* blackPawn = new Pawn(Piece::Color::Black, Position(4, 5));
    board->placePiece(blackPawn, Position(4, 5));

    auto moves = whitePawn->getPossibleMoves(board);
    EXPECT_EQ(moves.size(), 0); 
}

TEST_F(PawnTest, EnPassant) {
    Pawn* whitePawn = new Pawn(Piece::Color::White, Position(4, 4));
    board->placePiece(whitePawn, Position(4, 4));
    
    Pawn* blackPawn = new Pawn(Piece::Color::Black, Position(5, 6));
    board->placePiece(blackPawn, Position(5, 6));

    board->movePiece(Position(5, 6), Position(5, 4));

    auto moves = whitePawn->getPossibleMoves(board);
    
    bool hasEnPassant = false;
    for (const auto& move : moves) {
        if (move == Position(5, 5)) {
            hasEnPassant = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasEnPassant);
}

TEST_F(PawnTest, PawnPromotion) {
    Pawn* whitePawn = new Pawn(Piece::Color::White, Position(4, 6));
    board->placePiece(whitePawn, Position(4, 6));

    EXPECT_FALSE(whitePawn->canBePromoted());
    
    auto moves = whitePawn->getPossibleMoves(board);
    ASSERT_EQ(moves.size(), 1);
    EXPECT_EQ(moves[0], Position(4, 7));

    board->movePiece(Position(4, 6), Position(4, 7));
    EXPECT_TRUE(whitePawn->canBePromoted());
}