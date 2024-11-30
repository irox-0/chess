#include <gtest/gtest.h>
#include "board/Board.hpp"
#include "pieces/Piece.hpp"
#include "moves/Move.hpp"
#include "pieces/Bishop.hpp"
#include "pieces/King.hpp"
#include "pieces/Queen.hpp"
#include "pieces/Rook.hpp"
#include "pieces/Knight.hpp"
#include "pieces/Pawn.hpp"
#include "moves/MoveGenerator.hpp"
#include <algorithm>

class ComplexCasesTest : public ::testing::Test {
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
};

TEST_F(ComplexCasesTest, DoubleCheck) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Bishop(Piece::Color::Black), Position("c3"));
    board->placePiece(new Knight(Piece::Color::Black), Position("d3"));
    board->placePiece(new King(Piece::Color::Black), Position("h8"));

    auto allMoves = MoveGenerator::generateAllMoves(board, Piece::Color::White);
    
    EXPECT_TRUE(board->isCheck(Piece::Color::White)) 
        << "Position should be check before testing double check";
    
    for (const auto& move : allMoves) {
        EXPECT_EQ(board->getSquare(move.getFrom())->getPiece()->getType(), Piece::Type::King)
            << "Only king moves should be possible in double check";
    }
}

TEST_F(ComplexCasesTest, StalemateDueToPins) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("h1")); 
    board->placePiece(new Pawn(Piece::Color::White), Position("h2")); 
    board->placePiece(new Pawn(Piece::Color::Black), Position("h3")); 
    board->placePiece(new Queen(Piece::Color::Black), Position("f2")); 
    board->placePiece(new King(Piece::Color::Black), Position("e4")); 

    EXPECT_FALSE(board->isCheck(Piece::Color::White))
        << "Position should not be check for stalemate";
    
    auto kingMoves = board->getKing(Piece::Color::White)->getPossibleMoves(board);
    std::cout << "Available king moves: " << kingMoves.size() << std::endl;
    
    auto pawnMoves = board->getSquare(Position("h2"))->getPiece()->getPossibleMoves(board);
    std::cout << "Available pawn moves: " << pawnMoves.size() << std::endl;

    auto allMoves = MoveGenerator::generateAllMoves(board, Piece::Color::White);
    EXPECT_EQ(allMoves.size(), 0)
        << "No legal moves should be available in stalemate position. Found " << allMoves.size() << " moves";

    EXPECT_TRUE(board->isStalemate(Piece::Color::White))
        << "Position should be stalemate";
}

TEST_F(ComplexCasesTest, EnPassantPinAndCheck) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Pawn(Piece::Color::White), Position("e5"));
    board->placePiece(new Pawn(Piece::Color::Black), Position("f5"));
    board->placePiece(new Rook(Piece::Color::Black), Position("e8"));
    board->placePiece(new King(Piece::Color::Black), Position("h8"));
    
    board->setEnPassantPosition(Position("f6"));
    
    auto moves = MoveGenerator::generateLegalMoves(board, Position("e5"));
    
    for (const auto& move : moves) {
        Board tempBoard(*board);
        tempBoard.movePiece(move.getFrom(), move.getTo());
        EXPECT_FALSE(tempBoard.isCheck(Piece::Color::White))
            << "Move leaves king in check";
        
        if (move.getType() == Move::Type::EnPassant) {
            FAIL() << "En passant should not be possible as it would expose king to check";
        }
    }
}

TEST_F(ComplexCasesTest, CastlingThroughCheckAndPin) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Rook(Piece::Color::White), Position("h1"));
    board->placePiece(new Rook(Piece::Color::Black), Position("e8")); 
    board->placePiece(new Bishop(Piece::Color::Black), Position("b4")); 
    board->placePiece(new King(Piece::Color::Black), Position("h8"));

    auto kingMoves = MoveGenerator::generateLegalMoves(board, Position("e1"));
    
    for (const auto& move : kingMoves) {
        if (move.getType() == Move::Type::Castling) {
            Board tempBoard(*board);
            Position intermediate(move.getFrom().getX() + 
                               (move.getTo().getX() - move.getFrom().getX()) / 2,
                               move.getFrom().getY());
            EXPECT_TRUE(tempBoard.isPositionAttacked(intermediate, Piece::Color::Black))
                << "Castling through attacked square should not be possible";
            FAIL() << "Castling should not be possible through check";
        }
    }
}

TEST_F(ComplexCasesTest, ComplexPawnPromotion) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Pawn(Piece::Color::White), Position("e7"));
    board->placePiece(new Rook(Piece::Color::Black), Position("f8")); 
    board->placePiece(new Bishop(Piece::Color::Black), Position("d8")); 
    board->placePiece(new King(Piece::Color::Black), Position("h8"));

    auto pawnMoves = MoveGenerator::generateLegalMoves(board, Position("e7"));
    
    bool hasNormalPromotion = false;
    bool hasCapturePromotion = false;
    
    for (const auto& move : pawnMoves) {
        if (move.getType() == Move::Type::Promotion) {
            EXPECT_TRUE(move.getPromotionPiece() == Piece::Type::Queen ||
                       move.getPromotionPiece() == Piece::Type::Rook ||
                       move.getPromotionPiece() == Piece::Type::Bishop ||
                       move.getPromotionPiece() == Piece::Type::Knight)
                << "Invalid promotion piece type";
            
            Board tempBoard(*board);
            tempBoard.movePiece(move.getFrom(), move.getTo());
            EXPECT_FALSE(tempBoard.isCheck(Piece::Color::White))
                << "Promotion move leaves king in check";

            if (move.getTo() == Position("e8")) hasNormalPromotion = true;
            if (move.getTo() == Position("d8") || move.getTo() == Position("f8")) 
                hasCapturePromotion = true;
        }
    }

    EXPECT_TRUE(hasNormalPromotion || hasCapturePromotion)
        << "Pawn should have at least one type of promotion available";
}

TEST_F(ComplexCasesTest, ComplexCheckDefense) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Queen(Piece::Color::White), Position("d1"));
    board->placePiece(new Bishop(Piece::Color::Black), Position("h4")); 
    board->placePiece(new Rook(Piece::Color::Black), Position("d8")); 
    board->placePiece(new King(Piece::Color::Black), Position("h8"));

    EXPECT_TRUE(board->isCheck(Piece::Color::White))
        << "Position should be check";

    auto allMoves = MoveGenerator::generateAllMoves(board, Piece::Color::White);
    
    bool hasKingMove = false;
    for (const auto& move : allMoves) {
        Board tempBoard(*board);
        tempBoard.movePiece(move.getFrom(), move.getTo());
        EXPECT_FALSE(tempBoard.isCheck(Piece::Color::White))
            << "Legal move should not leave king in check";

        if (board->getSquare(move.getFrom())->getPiece()->getType() == Piece::Type::King) {
            hasKingMove = true;
        }
    }

    EXPECT_TRUE(hasKingMove) << "King should have at least one legal move";
}

TEST_F(ComplexCasesTest, LongDiagonalBlockage) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("h1"));
    board->placePiece(new Bishop(Piece::Color::White), Position("f3"));
    board->placePiece(new Queen(Piece::Color::Black), Position("a8")); 
    board->placePiece(new King(Piece::Color::Black), Position("h8"));

    auto bishopMoves = MoveGenerator::generateLegalMoves(board, Position("f3"));
    
    int validMoveCount = 0;
    bool canBlockDiagonal = false;
    
    for (const auto& move : bishopMoves) {
        Board tempBoard(*board);
        tempBoard.movePiece(move.getFrom(), move.getTo());
        if (!tempBoard.isCheck(Piece::Color::White)) {
            validMoveCount++;
            if (move.getTo() == Position("g2")) {
                canBlockDiagonal = true;
            }
        }
    }
    
    EXPECT_GT(validMoveCount, 0) 
        << "Bishop should have at least one legal move";
    EXPECT_TRUE(canBlockDiagonal)
        << "Bishop should be able to block the diagonal to protect the king";
}

TEST_F(ComplexCasesTest, ComplexCheckmate) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("h1"));
    board->placePiece(new Pawn(Piece::Color::White), Position("h2"));
    board->placePiece(new Pawn(Piece::Color::White), Position("g2"));
    board->placePiece(new Queen(Piece::Color::Black), Position("f1")); 
    board->placePiece(new Bishop(Piece::Color::Black), Position("c6")); 
    board->placePiece(new King(Piece::Color::Black), Position("e8"));

    EXPECT_TRUE(board->isCheck(Piece::Color::White))
        << "Position should be check before confirming checkmate";

    EXPECT_TRUE(board->isCheckmate(Piece::Color::White))
        << "Position should be checkmate";
    
    auto allMoves = MoveGenerator::generateAllMoves(board, Piece::Color::White);
    EXPECT_EQ(allMoves.size(), 0)
        << "No legal moves should be available in checkmate position";
        
    auto king = board->getKing(Piece::Color::White);
    auto potentialKingMoves = king->getPossibleMoves(board);
    for (const auto& move : potentialKingMoves) {
        Board tempBoard(*board);
        tempBoard.movePiece(king->getPosition(), move);
        EXPECT_TRUE(tempBoard.isCheck(Piece::Color::White))
            << "King move to " << move.toAlgebraic() << " should not escape check";
    }
}

TEST_F(ComplexCasesTest, MultiplePins) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("e4"));
    board->placePiece(new Rook(Piece::Color::White), Position("e5")); 
    board->placePiece(new Bishop(Piece::Color::White), Position("f4")); 
    board->placePiece(new Queen(Piece::Color::Black), Position("e8")); 
    board->placePiece(new Rook(Piece::Color::Black), Position("h4")); 
    board->placePiece(new King(Piece::Color::Black), Position("h8"));

    auto rookMoves = MoveGenerator::generateLegalMoves(board, Position("e5"));
    for (const auto& move : rookMoves) {
        EXPECT_EQ(move.getTo().getX(), move.getFrom().getX())
            << "Pinned rook should only move vertically";
        EXPECT_TRUE(move.getTo().getY() > 4 && move.getTo().getY() < 8)
            << "Pinned rook moves should stay between king and attacking piece";
    }

    auto bishopMoves = MoveGenerator::generateLegalMoves(board, Position("f4"));
    EXPECT_EQ(bishopMoves.size(), 0) 
        << "Bishop pinned horizontally should have no legal moves";
}