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

// Тест начальных ходов пешки
TEST_F(MoveGeneratorTest, PawnInitialMoves) {
    Position pawnPos(1, 1); // b2
    board->placePiece(new Pawn(Piece::Color::White), pawnPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, pawnPos);
    
    EXPECT_EQ(moves.size(), 2);
    EXPECT_TRUE(containsMove(moves, pawnPos, Position(1, 2))); // b3
    EXPECT_TRUE(containsMove(moves, pawnPos, Position(1, 3))); // b4
}

// Тест взятия пешкой
TEST_F(MoveGeneratorTest, PawnCapture) {
    Position whitePawnPos(3, 3); // d4
    Position blackPawn1Pos(2, 4); // c5
    Position blackPawn2Pos(4, 4); // e5
    
    board->placePiece(new Pawn(Piece::Color::White), whitePawnPos);
    board->placePiece(new Pawn(Piece::Color::Black), blackPawn1Pos);
    board->placePiece(new Pawn(Piece::Color::Black), blackPawn2Pos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, whitePawnPos);
    
    EXPECT_EQ(moves.size(), 3);
    EXPECT_TRUE(containsMove(moves, whitePawnPos, blackPawn1Pos));
    EXPECT_TRUE(containsMove(moves, whitePawnPos, blackPawn2Pos));
    EXPECT_TRUE(containsMove(moves, whitePawnPos, Position(3, 4))); // d5
}

// Тест взятия на проходе
TEST_F(MoveGeneratorTest, EnPassant) {
    Position whitePawnPos(3, 4); // d5
    Position blackPawnPos(4, 4); // e5
    
    board->placePiece(new Pawn(Piece::Color::White), whitePawnPos);
    board->placePiece(new Pawn(Piece::Color::Black), blackPawnPos);
    board->setEnPassantPosition(Position(4, 5)); // e6
    
    auto moves = MoveGenerator::generateLegalMoves(board, whitePawnPos);
    
    bool hasEnPassant = std::any_of(moves.begin(), moves.end(),
        [](const Move& move) { return move.getType() == Move::Type::EnPassant; });
    
    EXPECT_TRUE(hasEnPassant);
}

// Тест ходов коня
TEST_F(MoveGeneratorTest, KnightMoves) {
    Position knightPos(3, 3); // d4
    board->placePiece(new Knight(Piece::Color::White), knightPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, knightPos);
    
    EXPECT_EQ(moves.size(), 8);
    
    const std::vector<Position> expectedPositions = {
        Position(1, 2), // b3
        Position(1, 4), // b5
        Position(2, 1), // c2
        Position(2, 5), // c6
        Position(4, 1), // e2
        Position(4, 5), // e6
        Position(5, 2), // f3
        Position(5, 4)  // f5
    };
    
    for (const auto& pos : expectedPositions) {
        EXPECT_TRUE(containsMove(moves, knightPos, pos));
    }
}

// Тест ходов слона
TEST_F(MoveGeneratorTest, BishopMoves) {
    Position bishopPos(3, 3); // d4
    board->placePiece(new Bishop(Piece::Color::White), bishopPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, bishopPos);
    
    EXPECT_EQ(moves.size(), 13); // Слон должен иметь 13 возможных ходов с d4 на пустой доске
}

// Тест ходов ладьи
TEST_F(MoveGeneratorTest, RookMoves) {
    Position rookPos(3, 3); // d4
    board->placePiece(new Rook(Piece::Color::White), rookPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, rookPos);
    
    EXPECT_EQ(moves.size(), 14); // Ладья должна иметь 14 возможных ходов с d4 на пустой доске
}

// Тест ходов ферзя
TEST_F(MoveGeneratorTest, QueenMoves) {
    Position queenPos(3, 3); // d4
    board->placePiece(new Queen(Piece::Color::White), queenPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, queenPos);
    
    EXPECT_EQ(moves.size(), 27); // Ферзь должен иметь 27 возможных ходов с d4 на пустой доске
}

// Тест рокировки
TEST_F(MoveGeneratorTest, Castling) {
    // Устанавливаем начальную позицию для рокировки
    Position kingPos(4, 0);
    Position kingsideRookPos(7, 0);
    Position queensideRookPos(0, 0);
    
    // Создаем фигуры и явно устанавливаем флаг hasMoved в false
    King* king = new King(Piece::Color::White);
    king->setMoved(false);
    
    Rook* kingsideRook = new Rook(Piece::Color::White);
    kingsideRook->setMoved(false);
    
    Rook* queensideRook = new Rook(Piece::Color::White);
    queensideRook->setMoved(false);
    
    // Размещаем фигуры на доске
    ASSERT_TRUE(board->placePiece(king, kingPos));
    ASSERT_TRUE(board->placePiece(kingsideRook, kingsideRookPos));
    ASSERT_TRUE(board->placePiece(queensideRook, queensideRookPos));
    
    auto castlingMoves = MoveGenerator::getCastlingMoves(board, Piece::Color::White);
    
    // Проверяем наличие обеих рокировок
    EXPECT_EQ(castlingMoves.size(), 2);
    EXPECT_TRUE(containsMove(castlingMoves, kingPos, Position(6, 0))); // Короткая
    EXPECT_TRUE(containsMove(castlingMoves, kingPos, Position(2, 0))); // Длинная
}

// Тест блокированной рокировки
TEST_F(MoveGeneratorTest, BlockedCastling) {
    Position kingPos(4, 0); // e1
    Position rookPos(7, 0); // h1
    Position blockingPos(6, 0); // g1
    
    board->placePiece(new King(Piece::Color::White), kingPos);
    board->placePiece(new Rook(Piece::Color::White), rookPos);
    board->placePiece(new Bishop(Piece::Color::White), blockingPos);
    
    auto castlingMoves = MoveGenerator::getCastlingMoves(board, Piece::Color::White);
    
    EXPECT_TRUE(castlingMoves.empty());
}

// Тест рокировки под шахом
TEST_F(MoveGeneratorTest, CastlingUnderCheck) {
    Position kingPos(4, 0); // e1
    Position rookPos(7, 0); // h1
    Position enemyQueenPos(4, 7); // e8
    
    board->placePiece(new King(Piece::Color::White), kingPos);
    board->placePiece(new Rook(Piece::Color::White), rookPos);
    board->placePiece(new Queen(Piece::Color::Black), enemyQueenPos);
    
    auto castlingMoves = MoveGenerator::getCastlingMoves(board, Piece::Color::White);
    
    EXPECT_TRUE(castlingMoves.empty());
}

// Тест превращения пешки
TEST_F(MoveGeneratorTest, PawnPromotion) {
    Position whitePawnPos(1, 6); // b7
    board->placePiece(new Pawn(Piece::Color::White), whitePawnPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, whitePawnPos);
    
    int promotionMoves = std::count_if(moves.begin(), moves.end(),
        [](const Move& move) { return move.getType() == Move::Type::Promotion; });
    
    EXPECT_EQ(promotionMoves, 4); // Должно быть 4 варианта превращения
}

// Тест защиты от шаха
TEST_F(MoveGeneratorTest, CheckEvasion) {
    Position whiteKingPos(4, 0); // e1
    Position blackQueenPos(4, 7); // e8
    
    board->placePiece(new King(Piece::Color::White), whiteKingPos);
    board->placePiece(new Queen(Piece::Color::Black), blackQueenPos);
    
    auto moves = MoveGenerator::generateLegalMoves(board, whiteKingPos);
    
    for (const auto& move : moves) {
        Board tempBoard(*board);
        tempBoard.movePiece(move.getFrom(), move.getTo());
        EXPECT_FALSE(tempBoard.isCheck(Piece::Color::White));
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}