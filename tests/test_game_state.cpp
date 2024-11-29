#include <gtest/gtest.h>
#include "game/GameState.hpp"
#include "board/Board.hpp"
#include "pieces/King.hpp"
#include "pieces/Queen.hpp"
#include "pieces/Rook.hpp"
#include "pieces/Pawn.hpp"
#include "pieces/Bishop.hpp"
#include "moves/Move.hpp"

class GameStateTest : public ::testing::Test {
protected:
    void SetUp() override {
        board = new Board();
        gameState = new GameState();
    }

    void TearDown() override {
        delete board;
        delete gameState;
    }

    Board* board;
    GameState* gameState;
};

TEST_F(GameStateTest, InitialState) {
    EXPECT_EQ(gameState->getCurrentTurn(), Piece::Color::White);
    EXPECT_EQ(gameState->getResult(), GameState::Result::None);
    EXPECT_EQ(gameState->getMoveCount(), 1);
    EXPECT_EQ(gameState->getHalfMoveCount(), 0);
    EXPECT_FALSE(gameState->isDrawOffered());
    EXPECT_TRUE(gameState->getMoveHistory().empty());
}

TEST_F(GameStateTest, MakeValidMove) {
    board->initialize();
    
    const Square* startSquare = board->getSquare(Position("e2"));
    EXPECT_TRUE(startSquare != nullptr && startSquare->isOccupied());
    EXPECT_EQ(startSquare->getPiece()->getType(), Piece::Type::Pawn);
    EXPECT_EQ(startSquare->getPiece()->getColor(), Piece::Color::White);
    
    Move move(Position("e2"), Position("e4"), Move::Type::DoublePawn);  
    
    bool moveResult = gameState->makeMove(move, board);
    EXPECT_TRUE(moveResult);
    
    if (moveResult) {
        const Square* endSquare = board->getSquare(Position("e4"));
        EXPECT_TRUE(endSquare != nullptr && endSquare->isOccupied());
        EXPECT_EQ(endSquare->getPiece()->getType(), Piece::Type::Pawn);
        EXPECT_EQ(endSquare->getPiece()->getColor(), Piece::Color::White);
        
        EXPECT_EQ(gameState->getCurrentTurn(), Piece::Color::Black);
        EXPECT_EQ(gameState->getMoveHistory().size(), 1);
    }
}

TEST_F(GameStateTest, MakeInvalidMove) {
    board->initialize();
    Move invalidMove(Position("e2"), Position("e5")); 
    
    EXPECT_FALSE(gameState->makeMove(invalidMove, board));
    EXPECT_EQ(gameState->getCurrentTurn(), Piece::Color::White);
    EXPECT_TRUE(gameState->getMoveHistory().empty());
}

TEST_F(GameStateTest, UndoMove) {
    board->initialize();
    Move move(Position("e2"), Position("e4"));
    
    gameState->makeMove(move, board);
    gameState->undoLastMove(board);
    
    EXPECT_EQ(gameState->getCurrentTurn(), Piece::Color::White);
    EXPECT_TRUE(gameState->getMoveHistory().empty());
    EXPECT_EQ(gameState->getMoveCount(), 1);
}

TEST_F(GameStateTest, DrawOffer) {
    gameState->offerDraw(Piece::Color::White);
    
    EXPECT_TRUE(gameState->isDrawOffered());
    EXPECT_EQ(gameState->getDrawOfferingColor(), Piece::Color::White);
    
    gameState->acceptDraw();
    
    EXPECT_EQ(gameState->getResult(), GameState::Result::Draw);
    EXPECT_EQ(gameState->getDrawReason(), GameState::DrawReason::MutualAgreement);
    EXPECT_FALSE(gameState->isDrawOffered());
}

TEST_F(GameStateTest, DrawDecline) {
    gameState->offerDraw(Piece::Color::White);
    gameState->declineDraw();
    
    EXPECT_FALSE(gameState->isDrawOffered());
    EXPECT_EQ(gameState->getResult(), GameState::Result::None);
}

TEST_F(GameStateTest, Resignation) {
    gameState->resign(Piece::Color::White);
    
    EXPECT_EQ(gameState->getResult(), GameState::Result::BlackWin);
    EXPECT_TRUE(gameState->isGameOver());
}

TEST_F(GameStateTest, GameReset) {
    board->initialize();
    Move move(Position("e2"), Position("e4"));
    gameState->makeMove(move, board);
    gameState->offerDraw(Piece::Color::White);
    
    gameState->reset();
    
    EXPECT_EQ(gameState->getCurrentTurn(), Piece::Color::White);
    EXPECT_EQ(gameState->getResult(), GameState::Result::None);
    EXPECT_EQ(gameState->getMoveCount(), 1);
    EXPECT_EQ(gameState->getHalfMoveCount(), 0);
    EXPECT_TRUE(gameState->getMoveHistory().empty());
    EXPECT_FALSE(gameState->isDrawOffered());
}

TEST_F(GameStateTest, EnPassantMove) {
    board->clear();
    board->placePiece(new Pawn(Piece::Color::White), Position("e5"));
    board->placePiece(new Pawn(Piece::Color::Black), Position("f5"));
    board->setEnPassantPosition(Position("f6"));
    
    Move enPassant(Position("e5"), Position("f6"), Move::Type::EnPassant);
    EXPECT_TRUE(gameState->makeMove(enPassant, board));
}

TEST_F(GameStateTest, PawnPromotion) {
    board->clear();
    board->placePiece(new Pawn(Piece::Color::White), Position("e7"));
    
    Move promotion(Position("e7"), Position("e8"), Move::Type::Promotion, Piece::Type::Queen);
    EXPECT_TRUE(gameState->makeMove(promotion, board));
}

TEST_F(GameStateTest, CheckState) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Queen(Piece::Color::Black), Position("e8"));
    
    Move checkMove(Position("e8"), Position("e2"));
    gameState->makeMove(checkMove, board);
    EXPECT_TRUE(gameState->isCheck(board));
}

TEST_F(GameStateTest, PinnedPieceMove) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Rook(Piece::Color::White), Position("e2"));
    board->placePiece(new Queen(Piece::Color::Black), Position("e8"));
    
    Move illegalMove(Position("e2"), Position("f2"));
    EXPECT_FALSE(gameState->makeMove(illegalMove, board));
}

TEST_F(GameStateTest, MoveAfterGameOver) {
    gameState->resign(Piece::Color::White);
    
    Move move(Position("e2"), Position("e4"));
    EXPECT_FALSE(gameState->makeMove(move, board));
}

TEST_F(GameStateTest, UndoMoveEmptyHistory) {
    gameState->undoLastMove(board);
    EXPECT_EQ(gameState->getCurrentTurn(), Piece::Color::White);
}

TEST_F(GameStateTest, MultipleMoveSequence) {
    board->initialize();
    Move move1(Position("e2"), Position("e4"), Move::Type::DoublePawn);
    Move move2(Position("e7"), Position("e5"), Move::Type::DoublePawn);
    Move move3(Position("g1"), Position("f3"));    
    EXPECT_TRUE(gameState->makeMove(move1, board));
    EXPECT_TRUE(gameState->makeMove(move2, board));
    EXPECT_TRUE(gameState->makeMove(move3, board));
    
    EXPECT_EQ(gameState->getMoveHistory().size(), 3);
    EXPECT_EQ(gameState->getMoveCount(), 2); 
}
TEST_F(GameStateTest, InsufficientMaterialDraw) {
    board->clear();
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new King(Piece::Color::Black), Position("e8"));
    board->placePiece(new Bishop(Piece::Color::White), Position("c1"));
    
    Move move(Position("c1"), Position("d2"));
    gameState->makeMove(move, board);
    
    EXPECT_TRUE(gameState->isInsufficientMaterial(board));
    EXPECT_EQ(gameState->getResult(), GameState::Result::Draw);
}

TEST_F(GameStateTest, InvalidMoveOutOfBoard) {
    board->initialize();
    Move invalidMove(Position("e2"), Position("e9"));
    EXPECT_FALSE(gameState->makeMove(invalidMove, board));
}

TEST_F(GameStateTest, MoveWrongColorPiece) {
    board->initialize();
    Move invalidMove(Position("e7"), Position("e6")); 
    EXPECT_FALSE(gameState->makeMove(invalidMove, board));
}

TEST_F(GameStateTest, RepeatedPosition) {
    board->clear();
    King* whiteKing = new King(Piece::Color::White);
    King* blackKing = new King(Piece::Color::Black);
    Rook* whiteRook = new Rook(Piece::Color::White);
    
    board->placePiece(whiteKing, Position("e1"));
    board->placePiece(blackKing, Position("e8"));
    board->placePiece(whiteRook, Position("a1"));
    
    for (int i = 0; i < 6; i++) {
        Move whiteMove(Position(i % 2 == 0 ? "a1" : "a2"), 
                      Position(i % 2 == 0 ? "a2" : "a1"));
        EXPECT_TRUE(gameState->makeMove(whiteMove, board));
        
        Move blackMove(Position(i % 2 == 0 ? "e8" : "e7"), 
                      Position(i % 2 == 0 ? "e7" : "e8"));
        EXPECT_TRUE(gameState->makeMove(blackMove, board));
    }
    
    EXPECT_TRUE(gameState->isThreefoldRepetition());
}

TEST_F(GameStateTest, FiftyMovesRule) {
    board->clear();
    
    King* whiteKing = new King(Piece::Color::White);
    King* blackKing = new King(Piece::Color::Black);
    Rook* whiteRook = new Rook(Piece::Color::White);
    
    board->placePiece(whiteKing, Position("e1"));
    board->placePiece(blackKing, Position("e8"));
    board->placePiece(whiteRook, Position("a1"));
    
    gameState->reset();
    
    for (int i = 0; i < 100; i++) {
        bool isWhiteMove = (i % 2 == 0);
        Position from, to;
        
        if (isWhiteMove) {
            from = Position(i % 4 < 2 ? "a1" : "a2");
            to = Position(i % 4 < 2 ? "a2" : "a1");
        } else {
            from = Position(i % 4 < 2 ? "e8" : "d8");
            to = Position(i % 4 < 2 ? "d8" : "e8");
        }
        
        Move move(from, to);
        EXPECT_TRUE(gameState->makeMove(move, board)) 
            << "Move failed: " << from.toAlgebraic() << "-" << to.toAlgebraic() 
            << " at half-move " << i;
            
        if (i < 98) {
            EXPECT_EQ(gameState->getHalfMoveCount(), i + 1);
            EXPECT_FALSE(gameState->isFiftyMoveRule());
        }
    }
    
    EXPECT_EQ(gameState->getHalfMoveCount(), 100);
    EXPECT_TRUE(gameState->isFiftyMoveRule());
    EXPECT_EQ(gameState->getResult(), GameState::Result::Draw);
    EXPECT_EQ(gameState->getDrawReason(), GameState::DrawReason::FiftyMoveRule);
}

TEST_F(GameStateTest, CastlingMove) {
    board->clear();
    
    King* king = new King(Piece::Color::White);
    Rook* rook = new Rook(Piece::Color::White);
    
    king->setMoved(false);
    rook->setMoved(false);
    
    EXPECT_TRUE(board->placePiece(king, Position("e1")));
    EXPECT_TRUE(board->placePiece(rook, Position("h1")));
    
    EXPECT_TRUE(board->getSquare(Position("e1"))->isOccupied());
    EXPECT_TRUE(board->getSquare(Position("h1"))->isOccupied());
    EXPECT_FALSE(board->getSquare(Position("f1"))->isOccupied());
    EXPECT_FALSE(board->getSquare(Position("g1"))->isOccupied());
    
    Move castling(Position("e1"), Position("g1"), Move::Type::Castling);
    EXPECT_TRUE(gameState->makeMove(castling, board)) 
        << "Castling move failed";
        
    EXPECT_FALSE(board->getSquare(Position("e1"))->isOccupied()) 
        << "e1 should be empty after castling";
    EXPECT_FALSE(board->getSquare(Position("h1"))->isOccupied()) 
        << "h1 should be empty after castling";
    EXPECT_TRUE(board->getSquare(Position("g1"))->isOccupied() && 
                board->getSquare(Position("g1"))->getPiece()->getType() == Piece::Type::King) 
        << "King should be on g1";
    EXPECT_TRUE(board->getSquare(Position("f1"))->isOccupied() && 
                board->getSquare(Position("f1"))->getPiece()->getType() == Piece::Type::Rook) 
        << "Rook should be on f1";
}

TEST_F(GameStateTest, QueensideCastlingMove) {
    board->clear();
    
    // Размещаем короля и ладью для длинной рокировки
    King* king = new King(Piece::Color::White);
    Rook* rook = new Rook(Piece::Color::White);
    
    king->setMoved(false);
    rook->setMoved(false);
    
    EXPECT_TRUE(board->placePiece(king, Position("e1")));
    EXPECT_TRUE(board->placePiece(rook, Position("a1")));
    
    // Проверяем начальные позиции
    EXPECT_TRUE(board->getSquare(Position("e1"))->isOccupied());
    EXPECT_TRUE(board->getSquare(Position("a1"))->isOccupied());
    EXPECT_FALSE(board->getSquare(Position("d1"))->isOccupied());
    EXPECT_FALSE(board->getSquare(Position("c1"))->isOccupied());
    EXPECT_FALSE(board->getSquare(Position("b1"))->isOccupied());
    
    // Выполняем длинную рокировку
    Move castling(Position("e1"), Position("c1"), Move::Type::Castling);
    EXPECT_TRUE(gameState->makeMove(castling, board)) 
        << "Queenside castling move failed";
        
    // Проверяем, что исходные позиции пусты
    EXPECT_FALSE(board->getSquare(Position("e1"))->isOccupied()) 
        << "e1 should be empty after castling";
    EXPECT_FALSE(board->getSquare(Position("a1"))->isOccupied()) 
        << "a1 should be empty after castling";
    
    // Проверяем, что король и ладья находятся на правильных позициях
    EXPECT_TRUE(board->getSquare(Position("c1"))->isOccupied() && 
                board->getSquare(Position("c1"))->getPiece()->getType() == Piece::Type::King) 
        << "King should be on c1";
    EXPECT_TRUE(board->getSquare(Position("d1"))->isOccupied() && 
                board->getSquare(Position("d1"))->getPiece()->getType() == Piece::Type::Rook) 
        << "Rook should be on d1";
        
    // Проверяем, что фигуры отмечены как сделавшие ход
    EXPECT_TRUE(board->getSquare(Position("c1"))->getPiece()->hasMoved())
        << "King should be marked as moved";
    EXPECT_TRUE(board->getSquare(Position("d1"))->getPiece()->hasMoved())
        << "Rook should be marked as moved";
}