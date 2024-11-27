#include <gtest/gtest.h>
#include "pieces/Rook.hpp"
#include "pieces/Pawn.cpp"
#include "board/Board.hpp"
#include "pieces/Queen.hpp"
#include "pieces/King.hpp"
#include "pieces/Knight.hpp"
#include "pieces/Bishop.hpp"
#include <vector>
#include <algorithm>

class PiecesTest : public ::testing::Test {
protected:
    void SetUp() override {
        board = new Board();
    }

    void TearDown() override {
        delete board;
    }

    bool containsPosition(const std::vector<Position>& moves, const std::string& pos) {
        Position target(pos);
        return std::find(moves.begin(), moves.end(), target) != moves.end();
    }

    bool containsPosition(const std::vector<Position>& moves, const Position& pos) {
        return std::find(moves.begin(), moves.end(), pos) != moves.end();
    }

    void clearBoard() {
        board->clear();
    }

    void setupBasicPosition() {
        clearBoard();
        board->initialize();
    }

    bool isKingInCheck(Piece::Color color) {
        Piece* king = nullptr;
        Position kingPos;
        
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                Position pos(x, y);
                Square* square = board->getSquare(pos);
                if (square->isOccupied()) {
                    Piece* piece = square->getPiece();
                    if (piece->getType() == Piece::Type::King && piece->getColor() == color) {
                        king = piece;
                        kingPos = pos;
                        break;
                    }
                }
            }
            if (king) break;
        }
        
        if (!king) return false;
        return board->isPositionAttacked(kingPos, (color == Piece::Color::White) ? 
                                       Piece::Color::Black : Piece::Color::White);
    }

    Board* board;
};;

TEST_F(PiecesTest, QueenBasicMoves) {
    Position queenPos("d4");
    board->placePiece(new Queen(Piece::Color::White), queenPos);
    
    auto moves = board->getSquare(queenPos)->getPiece()->getPossibleMoves(board);
    
    EXPECT_EQ(moves.size(), 27); 
    
    EXPECT_TRUE(containsPosition(moves, "d1"));
    EXPECT_TRUE(containsPosition(moves, "d8"));
    
    EXPECT_TRUE(containsPosition(moves, "a4"));
    EXPECT_TRUE(containsPosition(moves, "h4"));
    
    EXPECT_TRUE(containsPosition(moves, "a1"));
    EXPECT_TRUE(containsPosition(moves, "h8"));
    EXPECT_TRUE(containsPosition(moves, "a7"));
    EXPECT_TRUE(containsPosition(moves, "g1"));
}

TEST_F(PiecesTest, QueenCaptures) {
    board->placePiece(new Queen(Piece::Color::White), Position("d4"));
    board->placePiece(new Pawn(Piece::Color::Black), Position("d7")); 
    board->placePiece(new Pawn(Piece::Color::White), Position("d2")); 
    board->placePiece(new Pawn(Piece::Color::Black), Position("g7")); 
    
    auto moves = board->getSquare(Position("d4"))->getPiece()->getPossibleMoves(board);
    
    EXPECT_TRUE(containsPosition(moves, "d7"));
    EXPECT_TRUE(containsPosition(moves, "g7"));
    
    EXPECT_FALSE(containsPosition(moves, "d2"));
}

TEST_F(PiecesTest, QueenBlocked) {
    board->placePiece(new Queen(Piece::Color::White), Position("d4"));
    board->placePiece(new Pawn(Piece::Color::White), Position("d5")); 
    board->placePiece(new Pawn(Piece::Color::White), Position("e4")); 
    
    auto moves = board->getSquare(Position("d4"))->getPiece()->getPossibleMoves(board);
    
    EXPECT_FALSE(containsPosition(moves, "d6"));
    EXPECT_FALSE(containsPosition(moves, "d7"));
    EXPECT_FALSE(containsPosition(moves, "d8"));
    EXPECT_FALSE(containsPosition(moves, "f4"));
    EXPECT_FALSE(containsPosition(moves, "g4"));
    EXPECT_FALSE(containsPosition(moves, "h4"));
}

TEST_F(PiecesTest, KingBasicMoves) {
    clearBoard();
    Position kingPos("e4");
    board->placePiece(new King(Piece::Color::White), kingPos);
    
    auto moves = board->getSquare(kingPos)->getPiece()->getPossibleMoves(board);
    
    EXPECT_EQ(moves.size(), 8); 
    
    EXPECT_TRUE(containsPosition(moves, "d3")); 
    EXPECT_TRUE(containsPosition(moves, "d4"));
    EXPECT_TRUE(containsPosition(moves, "d5")); 
    EXPECT_TRUE(containsPosition(moves, "e3")); 
    EXPECT_TRUE(containsPosition(moves, "e5")); 
    EXPECT_TRUE(containsPosition(moves, "f3")); 
    EXPECT_TRUE(containsPosition(moves, "f4")); 
    EXPECT_TRUE(containsPosition(moves, "f5")); 
}

TEST_F(PiecesTest, KingCastling) {
    clearBoard();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Rook(Piece::Color::White), Position("h1"));
    board->placePiece(new Rook(Piece::Color::White), Position("a1"));
    
    King* king = static_cast<King*>(board->getSquare(Position("e1"))->getPiece());
    auto moves = king->getPossibleMoves(board);
    
    EXPECT_TRUE(containsPosition(moves, "g1")); 
    EXPECT_TRUE(containsPosition(moves, "c1")); 
    
    king->setMoved(true);
    moves = king->getPossibleMoves(board);
    EXPECT_FALSE(containsPosition(moves, "g1"));
    EXPECT_FALSE(containsPosition(moves, "c1"));
}

TEST_F(PiecesTest, KingInCheck) {
    clearBoard();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Rook(Piece::Color::Black), Position("e8"));
    
    EXPECT_TRUE(isKingInCheck(Piece::Color::White));
    
    board->placePiece(new Bishop(Piece::Color::White), Position("e4"));
    EXPECT_FALSE(isKingInCheck(Piece::Color::White));
}

TEST_F(PiecesTest, KnightBasicMoves) {
    clearBoard();
    Position knightPos("d4");
    board->placePiece(new Knight(Piece::Color::White), knightPos);
    
    auto moves = board->getSquare(knightPos)->getPiece()->getPossibleMoves(board);
    
    EXPECT_EQ(moves.size(), 8); 
    
    EXPECT_TRUE(containsPosition(moves, "b3")); 
    EXPECT_TRUE(containsPosition(moves, "b5")); 
    EXPECT_TRUE(containsPosition(moves, "c2")); 
    EXPECT_TRUE(containsPosition(moves, "c6")); 
    EXPECT_TRUE(containsPosition(moves, "e2")); 
    EXPECT_TRUE(containsPosition(moves, "e6")); 
    EXPECT_TRUE(containsPosition(moves, "f3")); 
    EXPECT_TRUE(containsPosition(moves, "f5")); 
}

TEST_F(PiecesTest, KnightJumping) {
    clearBoard();
    board->placePiece(new Knight(Piece::Color::White), Position("d4"));
    board->placePiece(new Pawn(Piece::Color::White), Position("d3"));
    board->placePiece(new Pawn(Piece::Color::White), Position("d5"));
    board->placePiece(new Pawn(Piece::Color::White), Position("c4"));
    board->placePiece(new Pawn(Piece::Color::White), Position("e4"));
    
    auto moves = board->getSquare(Position("d4"))->getPiece()->getPossibleMoves(board);
    
    EXPECT_EQ(moves.size(), 8);
}

TEST_F(PiecesTest, KnightEdgeCases) {
    clearBoard();
    board->placePiece(new Knight(Piece::Color::White), Position("a1"));
    
    auto moves = board->getSquare(Position("a1"))->getPiece()->getPossibleMoves(board);
    
    EXPECT_EQ(moves.size(), 2);
    EXPECT_TRUE(containsPosition(moves, "b3"));
    EXPECT_TRUE(containsPosition(moves, "c2"));
}

TEST_F(PiecesTest, BishopBasicMoves) {
    clearBoard();
    Position bishopPos("d4");
    board->placePiece(new Bishop(Piece::Color::White), bishopPos);
    
    auto moves = board->getSquare(bishopPos)->getPiece()->getPossibleMoves(board);
    
    EXPECT_EQ(moves.size(), 13); 
    
    // Проверяем диагональные направления
    EXPECT_TRUE(containsPosition(moves, "a1")); 
    EXPECT_TRUE(containsPosition(moves, "g7")); 
    EXPECT_TRUE(containsPosition(moves, "a7")); 
    EXPECT_TRUE(containsPosition(moves, "g1")); 
}

TEST_F(PiecesTest, BishopBlocked) {
    clearBoard();
    board->placePiece(new Bishop(Piece::Color::White), Position("d4"));
    board->placePiece(new Pawn(Piece::Color::White), Position("c5"));
    board->placePiece(new Pawn(Piece::Color::Black), Position("e5"));
    
    auto moves = board->getSquare(Position("d4"))->getPiece()->getPossibleMoves(board);
    
    EXPECT_FALSE(containsPosition(moves, "b6"));
    EXPECT_FALSE(containsPosition(moves, "a7"));
    
    EXPECT_TRUE(containsPosition(moves, "e5"));
    EXPECT_FALSE(containsPosition(moves, "f6"));
}

TEST_F(PiecesTest, BishopCaptures) {
    clearBoard();
    board->placePiece(new Bishop(Piece::Color::White), Position("d4"));
    board->placePiece(new Pawn(Piece::Color::Black), Position("f6"));
    board->placePiece(new Pawn(Piece::Color::Black), Position("b2"));
    board->placePiece(new Pawn(Piece::Color::White), Position("f2"));
    
    auto moves = board->getSquare(Position("d4"))->getPiece()->getPossibleMoves(board);
    
    EXPECT_TRUE(containsPosition(moves, "f6"));
    EXPECT_TRUE(containsPosition(moves, "b2"));
    
    EXPECT_FALSE(containsPosition(moves, "f2"));
}


TEST_F(PiecesTest, PiecesInteraction) {
    clearBoard();
    
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Pawn(Piece::Color::White), Position("e2"));
    board->placePiece(new Queen(Piece::Color::Black), Position("e8"));
    
    auto pawnMoves = board->getSquare(Position("e2"))->getPiece()->getPossibleMoves(board);
    
    EXPECT_EQ(pawnMoves.size(), 2) 
        << "Vertically pinned pawn should only move forward along the pin line";
        
    EXPECT_TRUE(std::find(pawnMoves.begin(), pawnMoves.end(), Position("e3")) != pawnMoves.end())
        << "Pinned pawn should be able to move one square forward";
        
    EXPECT_TRUE(std::find(pawnMoves.begin(), pawnMoves.end(), Position("e4")) != pawnMoves.end())
        << "Pinned pawn should be able to move two squares forward on its first move";

    EXPECT_TRUE(std::find(pawnMoves.begin(), pawnMoves.end(), Position("d3")) == pawnMoves.end())
        << "Pinned pawn should not be able to capture diagonally";
        
    EXPECT_TRUE(std::find(pawnMoves.begin(), pawnMoves.end(), Position("f3")) == pawnMoves.end())
        << "Pinned pawn should not be able to capture diagonally";
}

TEST_F(PiecesTest, PinnedPieces) {
    clearBoard();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Pawn(Piece::Color::White), Position("e2"));
    board->placePiece(new Rook(Piece::Color::Black), Position("e8"));
    
    auto pawnMoves = board->getSquare(Position("e2"))->getPiece()->getPossibleMoves(board);

    EXPECT_EQ(pawnMoves.size(), 2) 
        << "Vertically pinned pawn should have two forward moves";
        
    EXPECT_TRUE(std::find(pawnMoves.begin(), pawnMoves.end(), Position("e3")) != pawnMoves.end())
        << "Pinned pawn should move one square forward";
    EXPECT_TRUE(std::find(pawnMoves.begin(), pawnMoves.end(), Position("e4")) != pawnMoves.end())
        << "Pinned pawn should move two squares forward on first move";

    clearBoard();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Rook(Piece::Color::White), Position("d1"));
    board->placePiece(new Queen(Piece::Color::Black), Position("h1"));
    
    auto rookMoves = board->getSquare(Position("d1"))->getPiece()->getPossibleMoves(board);
    
    for (const auto& move : rookMoves) {
        EXPECT_EQ(move.getY(), 0) 
            << "Horizontally pinned rook should only move along the same rank";
        EXPECT_GE(move.getX(), 1) 
            << "Rook moves should be between the king and attacking piece";
        EXPECT_LE(move.getX(), 6) 
            << "Rook moves should be between the king and attacking piece";
    }
}

TEST_F(PiecesTest, AttackingLines) {
    clearBoard();
    board->placePiece(new Rook(Piece::Color::White), Position("a1"));
    
    for (int x = 1; x < 8; x++) {
        EXPECT_TRUE(board->isPositionAttacked(Position(x, 0), Piece::Color::White));
    }
}

TEST_F(PiecesTest, KingCheckEvasion) {
    clearBoard();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Rook(Piece::Color::Black), Position("e8"));
    
    auto kingMoves = board->getSquare(Position("e1"))->getPiece()->getPossibleMoves(board);
    
    EXPECT_TRUE(containsPosition(kingMoves, "d1"));
    EXPECT_TRUE(containsPosition(kingMoves, "f1"));
    EXPECT_FALSE(containsPosition(kingMoves, "e2")); 
}

TEST_F(PiecesTest, DiscoveredCheck) {
    clearBoard();
    
    board->placePiece(new King(Piece::Color::Black), Position("e8"));
    board->placePiece(new Bishop(Piece::Color::White), Position("e6")); 
    board->placePiece(new Rook(Piece::Color::White), Position("e1")); 
    
    auto bishopMoves = board->getSquare(Position("e6"))->getPiece()->getPossibleMoves(board);
    

    for (const auto& move : bishopMoves) {
        EXPECT_EQ(move.getX(), 4) // 4 = вертикаль 'e' (0-based)
            << "Bishop should only be able to move along the e-file to avoid discovered check";
    }

    std::vector<Position> illegalMoves = {
        Position("f7"), Position("g8"), Position("f5"), Position("g4"), 
        Position("h3"), Position("d5"), Position("c4"), Position("b3"), Position("a2")
    };
    
    Board tempBoard(*board);
    for (const auto& destination : illegalMoves) {
        Board testBoard(tempBoard);
        testBoard.movePiece(Position("e6"), destination);
        EXPECT_TRUE(testBoard.isCheck(Piece::Color::Black)) 
            << "Move to " << destination.toAlgebraic() << " should create discovered check";
    }
}

TEST_F(PiecesTest, PieceMobility) {
    clearBoard();
    
    board->placePiece(new Knight(Piece::Color::White), Position("d4"));
    auto centerKnightMoves = board->getSquare(Position("d4"))->getPiece()->getPossibleMoves(board);
    EXPECT_EQ(centerKnightMoves.size(), 8); // Конь в центре имеет 8 ходов
    
    board->placePiece(new Knight(Piece::Color::White), Position("a1"));
    auto cornerKnightMoves = board->getSquare(Position("a1"))->getPiece()->getPossibleMoves(board);
    EXPECT_EQ(cornerKnightMoves.size(), 2); // Конь в углу имеет 2 хода
}

TEST_F(PiecesTest, BlockedPieces) {
    clearBoard();
    board->placePiece(new Rook(Piece::Color::White), Position("a1"));
    board->placePiece(new Pawn(Piece::Color::White), Position("a2"));
    
    auto rookMoves = board->getSquare(Position("a1"))->getPiece()->getPossibleMoves(board);
    
    EXPECT_FALSE(containsPosition(rookMoves, "a3"));
    EXPECT_TRUE(containsPosition(rookMoves, "b1"));
}
TEST_F(PiecesTest, QueenCombinedMoves) {
    clearBoard();
    board->placePiece(new Queen(Piece::Color::White), Position("d4"));
    auto queenMoves = board->getSquare(Position("d4"))->getPiece()->getPossibleMoves(board);
    
    EXPECT_TRUE(containsPosition(queenMoves, Position("a1")));
    EXPECT_TRUE(containsPosition(queenMoves, Position("h8")));
    
    EXPECT_TRUE(containsPosition(queenMoves, Position("d1")));
    EXPECT_TRUE(containsPosition(queenMoves, Position("d8")));
    EXPECT_TRUE(containsPosition(queenMoves, Position("a4")));
    EXPECT_TRUE(containsPosition(queenMoves, Position("h4")));
}

TEST_F(PiecesTest, KingCastlingSafety) {
    clearBoard();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Rook(Piece::Color::White), Position("h1"));
    board->placePiece(new Rook(Piece::Color::Black), Position("e8"));
    
    auto kingMoves = board->getSquare(Position("e1"))->getPiece()->getPossibleMoves(board);
    
    EXPECT_FALSE(containsPosition(kingMoves, "g1"));
}

TEST_F(PiecesTest, PieceValues) {
    Pawn pawn(Piece::Color::White);
    Knight knight(Piece::Color::White);
    Bishop bishop(Piece::Color::White);
    Rook rook(Piece::Color::White);
    Queen queen(Piece::Color::White);
    King king(Piece::Color::White);
    
    EXPECT_EQ(pawn.getValue(), 1);
    EXPECT_EQ(knight.getValue(), 3);
    EXPECT_EQ(bishop.getValue(), 3);
    EXPECT_EQ(rook.getValue(), 5);
    EXPECT_EQ(queen.getValue(), 9);
    EXPECT_EQ(king.getValue(), 0); 
}