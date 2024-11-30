#include <gtest/gtest.h>
#include "ai/AI.hpp"
#include "board/Board.hpp"
#include "pieces/King.hpp"
#include "pieces/Queen.hpp"
#include "pieces/Pawn.hpp"
#include "moves/MoveGenerator.hpp"

class AITest : public ::testing::Test {
protected:
    void SetUp() override {
        board = new Board();
        ai = new AI();
        ai->setSeed(12345); 
    }

    void TearDown() override {
        delete board;
        delete ai;
    }

    Board* board;
    AI* ai;
};

TEST_F(AITest, InitialPosition) {
    board->initialize();
    Move move = ai->getMove(board, Piece::Color::White);
    
    EXPECT_TRUE(move.getFrom().isValid());
    EXPECT_TRUE(move.getTo().isValid());
    EXPECT_NE(move.getFrom(), move.getTo());
}

TEST_F(AITest, HandleCheck) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Queen(Piece::Color::Black), Position("e8"));
    
    Move move = ai->getMove(board, Piece::Color::White);
    
    EXPECT_TRUE(move.getFrom().isValid());
    EXPECT_TRUE(move.getTo().isValid());
    
    Board tempBoard(*board);
    tempBoard.movePiece(move.getFrom(), move.getTo());
    EXPECT_FALSE(tempBoard.isCheck(Piece::Color::White)) 
        << "AI should make a move that gets out of check";
}

TEST_F(AITest, HandleCheckmate) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("h1"));
    board->placePiece(new Queen(Piece::Color::Black), Position("f1"));
    board->placePiece(new Queen(Piece::Color::Black), Position("g2"));
    
    Move move = ai->getMove(board, Piece::Color::White);
    EXPECT_FALSE(move.getFrom().isValid()) 
        << "AI should recognize checkmate position";
}

TEST_F(AITest, HandleStalemate) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("h1"));
    board->placePiece(new Queen(Piece::Color::Black), Position("f2"));
    
    Move move = ai->getMove(board, Piece::Color::White);
    EXPECT_FALSE(move.getFrom().isValid()) 
        << "AI should recognize stalemate position";
}

TEST_F(AITest, MakeLegalMoves) {
    for (int i = 0; i < 10; ++i) {
        board->initialize();
        Move move = ai->getMove(board, Piece::Color::White);
        
        EXPECT_TRUE(MoveGenerator::isMoveLegal(board, move)) 
            << "Move should be legal: " << move.toString();
    }
}

TEST_F(AITest, HandleEmptyBoard) {
    board->clear();
    Move move = ai->getMove(board, Piece::Color::White);
    EXPECT_FALSE(move.getFrom().isValid()) 
        << "AI should handle empty board gracefully";
}

TEST_F(AITest, HandlePawnPromotion) {
    board->clear();
    board->placePiece(new Pawn(Piece::Color::White), Position("e7"));
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new King(Piece::Color::Black), Position("h8"));
    
    Move move = ai->getMove(board, Piece::Color::White);
    EXPECT_TRUE(move.getFrom() == Position("e7"));
    EXPECT_TRUE(move.getTo() == Position("e8"));
    EXPECT_EQ(move.getType(), Move::Type::Promotion);
}

TEST_F(AITest, ConsistentBehaviorWithSameSeed) {
    AI ai1, ai2;
    ai1.setSeed(12345);
    ai2.setSeed(12345);
    
    board->initialize();
    
    Move move1 = ai1.getMove(board, Piece::Color::White);
    Move move2 = ai2.getMove(board, Piece::Color::White);
    
    EXPECT_EQ(move1.getFrom(), move2.getFrom());
    EXPECT_EQ(move1.getTo(), move2.getTo());
}

TEST_F(AITest, DifferentBehaviorWithDifferentSeeds) {
    AI ai1, ai2;
    ai1.setSeed(12345);
    ai2.setSeed(67890);
    
    board->initialize();
    
    bool foundDifferentMoves = false;
    for (int i = 0; i < 5; ++i) {
        Move move1 = ai1.getMove(board, Piece::Color::White);
        Move move2 = ai2.getMove(board, Piece::Color::White);
        
        if (move1.getFrom() != move2.getFrom() || move1.getTo() != move2.getTo()) {
            foundDifferentMoves = true;
            break;
        }
    }
    
    EXPECT_TRUE(foundDifferentMoves) 
        << "Different seeds should produce different moves";
}