#include <gtest/gtest.h>
#include "game/Game.hpp"
#include "pieces/King.hpp"
#include "pieces/Pawn.hpp"
#include "pieces/Bishop.hpp"
#include "pieces/Queen.hpp"
#include "pieces/Rook.hpp"

class GameTest : public ::testing::Test {
protected:
    void SetUp() override {
        game = new Game();
        game->initialize();
    }

    void TearDown() override {
        delete game;
    }

    Game* game;
};

TEST_F(GameTest, InitialState) {
    EXPECT_EQ(game->getCurrentTurn(), Piece::Color::White);
    EXPECT_FALSE(game->isGameOver());
    EXPECT_FALSE(game->isCheck());
    EXPECT_FALSE(game->isCheckmate());
    EXPECT_FALSE(game->isStalemate());
}

TEST_F(GameTest, BasicPawnMove) {
    EXPECT_TRUE(game->makeMove("e2", "e4"));
    EXPECT_EQ(game->getCurrentTurn(), Piece::Color::Black);
    
    const Board* board = game->getBoard();
    EXPECT_TRUE(board->getSquare(Position("e4"))->isOccupied());
    EXPECT_FALSE(board->getSquare(Position("e2"))->isOccupied());
}

TEST_F(GameTest, InvalidMoves) {
    EXPECT_FALSE(game->makeMove("e2", "e5")); 
    EXPECT_FALSE(game->makeMove("e7", "e6")); 
    EXPECT_FALSE(game->makeMove("a3", "a4")); 
    EXPECT_FALSE(game->makeMove("i1", "i2")); 
}

TEST_F(GameTest, Castling) {
    EXPECT_TRUE(game->makeMove("e2", "e4"));
    EXPECT_TRUE(game->makeMove("e7", "e5"));
    EXPECT_TRUE(game->makeMove("g1", "f3"));
    EXPECT_TRUE(game->makeMove("b8", "c6"));
    EXPECT_TRUE(game->makeMove("f1", "e2"));
    EXPECT_TRUE(game->makeMove("g8", "f6"));
    
    EXPECT_TRUE(game->makeMove("e1", "g1"));
    
    const Board* board = game->getBoard();
    EXPECT_TRUE(board->getSquare(Position("g1"))->isOccupied());
    EXPECT_TRUE(board->getSquare(Position("f1"))->isOccupied());
    EXPECT_FALSE(board->getSquare(Position("e1"))->isOccupied());
    EXPECT_FALSE(board->getSquare(Position("h1"))->isOccupied());
}

TEST_F(GameTest, PawnPromotionExplicit) {
    std::cout << "\n=== Starting PawnPromotionExplicit Test ===\n";
    
    game->reset();
    Board* board = const_cast<Board*>(game->getBoard());
    std::cout << "Initial board state cleared\n";
    board->clear();
    
    std::cout << "Setting up initial position...\n";
    board->placePiece(new Pawn(Piece::Color::White), Position("d7"));
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new King(Piece::Color::Black), Position("h8"));
    std::cout << "Initial position set up completed\n";
    std::cout << "Current board state:\n" << board->toString() << "\n";

    std::cout << "Testing invalid promotion without piece specification...\n";
    EXPECT_FALSE(game->makeMove("d7", "d8")) << "Move without promotion piece specification should fail";

    std::cout << "\nTesting promotion to Queen...\n";
    EXPECT_TRUE(game->makeMove("d7", "d8q")) << "Promotion to Queen failed";
    const Square* square = board->getSquare(Position("d8"));
    EXPECT_TRUE(square->isOccupied()) << "d8 square should be occupied after promotion";
    if (square->isOccupied()) {
        std::cout << "Piece type on d8: " << static_cast<int>(square->getPiece()->getType()) << "\n";
        EXPECT_EQ(square->getPiece()->getType(), Piece::Type::Queen) << "Piece should be Queen after promotion";
    }
    std::cout << "Board state after Queen promotion:\n" << board->toString() << "\n";
    
    std::cout << "\nResetting position for Rook promotion test...\n";
    game->reset();
    board->clear();
    board->placePiece(new Pawn(Piece::Color::White), Position("d7"));
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new King(Piece::Color::Black), Position("h8"));
    std::cout << "Board state before Rook promotion:\n" << board->toString() << "\n";

    std::cout << "Testing promotion to Rook...\n";
    EXPECT_TRUE(game->makeMove("d7", "d8r")) << "Promotion to Rook failed";
    square = board->getSquare(Position("d8"));
    EXPECT_TRUE(square->isOccupied()) << "d8 square should be occupied after promotion to Rook";
    if (square->isOccupied()) {
        std::cout << "Piece type on d8: " << static_cast<int>(square->getPiece()->getType()) << "\n";
        EXPECT_EQ(square->getPiece()->getType(), Piece::Type::Rook) << "Piece should be Rook after promotion";
    }
    std::cout << "Board state after Rook promotion:\n" << board->toString() << "\n";

    std::cout << "\nResetting position for Bishop promotion test...\n";
    game->reset();
    board->clear();
    board->placePiece(new Pawn(Piece::Color::White), Position("d7"));
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new King(Piece::Color::Black), Position("h8"));
    std::cout << "Board state before Bishop promotion:\n" << board->toString() << "\n";

    std::cout << "Testing promotion to Bishop...\n";
    EXPECT_TRUE(game->makeMove("d7", "d8b")) << "Promotion to Bishop failed";
    square = board->getSquare(Position("d8"));
    EXPECT_TRUE(square->isOccupied()) << "d8 square should be occupied after promotion to Bishop";
    if (square->isOccupied()) {
        std::cout << "Piece type on d8: " << static_cast<int>(square->getPiece()->getType()) << "\n";
        EXPECT_EQ(square->getPiece()->getType(), Piece::Type::Bishop) << "Piece should be Bishop after promotion";
    }
    std::cout << "Board state after Bishop promotion:\n" << board->toString() << "\n";

    std::cout << "\nResetting position for Knight promotion test...\n";
    game->reset();
    board->clear();
    board->placePiece(new Pawn(Piece::Color::White), Position("d7"));
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new King(Piece::Color::Black), Position("h8"));
    std::cout << "Board state before Knight promotion:\n" << board->toString() << "\n";

    std::cout << "Testing promotion to Knight...\n";
    EXPECT_TRUE(game->makeMove("d7", "d8n")) << "Promotion to Knight failed";
    square = board->getSquare(Position("d8"));
    EXPECT_TRUE(square->isOccupied()) << "d8 square should be occupied after promotion to Knight";
    if (square->isOccupied()) {
        std::cout << "Piece type on d8: " << static_cast<int>(square->getPiece()->getType()) << "\n";
        EXPECT_EQ(square->getPiece()->getType(), Piece::Type::Knight) << "Piece should be Knight after promotion";
    }
    std::cout << "Board state after Knight promotion:\n" << board->toString() << "\n";

    std::cout << "=== PawnPromotionExplicit Test Completed ===\n";
}

TEST_F(GameTest, EnPassant) {
    EXPECT_TRUE(game->makeMove("e2", "e4"));
    EXPECT_TRUE(game->makeMove("a7", "a6"));
    EXPECT_TRUE(game->makeMove("e4", "e5"));
    EXPECT_TRUE(game->makeMove("f7", "f5"));
    
    EXPECT_TRUE(game->makeMove("e5", "f6"));
    
    const Board* board = game->getBoard();
    EXPECT_TRUE(board->getSquare(Position("f6"))->isOccupied());
    EXPECT_FALSE(board->getSquare(Position("f5"))->isOccupied());
    EXPECT_FALSE(board->getSquare(Position("e5"))->isOccupied());
}

TEST_F(GameTest, Resignation) {
    game->resign(Piece::Color::White);
    
    EXPECT_TRUE(game->isGameOver());
    EXPECT_EQ(game->getResult(), GameState::Result::BlackWin);
}

TEST_F(GameTest, DrawOffer) {
    game->offerDraw(Piece::Color::White);
    EXPECT_TRUE(game->getGameState()->isDrawOffered());
    
    game->acceptDraw();
    EXPECT_TRUE(game->isGameOver());
    EXPECT_TRUE(game->isDraw());
    EXPECT_EQ(game->getResult(), GameState::Result::Draw);
}

TEST_F(GameTest, UndoMove) {
    std::cout << "\n=== Starting UndoMove Test ===\n";
    
    std::cout << "Initial board state:\n" << game->getBoardString();
    
    EXPECT_TRUE(game->makeMove("e2", "e4")) << "Initial pawn move failed";
    std::cout << "\nBoard after e2-e4:\n" << game->getBoardString();
    
    const Board* boardAfterMove = game->getBoard();
    EXPECT_TRUE(boardAfterMove->getSquare(Position("e4"))->isOccupied()) 
        << "e4 should be occupied after move";
    EXPECT_FALSE(boardAfterMove->getSquare(Position("e2"))->isOccupied()) 
        << "e2 should be empty after move";
    EXPECT_EQ(game->getCurrentTurn(), Piece::Color::Black) 
        << "Should be Black's turn after move";
    
    game->undoLastMove();
    std::cout << "\nBoard after undo:\n" << game->getBoardString();
    
    const Board* boardAfterUndo = game->getBoard();
    EXPECT_FALSE(boardAfterUndo->getSquare(Position("e4"))->isOccupied()) 
        << "e4 should be empty after undo";
    EXPECT_TRUE(boardAfterUndo->getSquare(Position("e2"))->isOccupied()) 
        << "e2 should be occupied after undo";
    
    const Square* e2Square = boardAfterUndo->getSquare(Position("e2"));
    EXPECT_TRUE(e2Square->isOccupied()) << "e2 should have a piece";
    if (e2Square->isOccupied()) {
        EXPECT_EQ(e2Square->getPiece()->getType(), Piece::Type::Pawn) 
            << "Piece on e2 should be a pawn";
        EXPECT_EQ(e2Square->getPiece()->getColor(), Piece::Color::White) 
            << "Piece on e2 should be white";
    }
    
    EXPECT_EQ(game->getCurrentTurn(), Piece::Color::White) 
        << "Should be White's turn after undo";
}

TEST_F(GameTest, Checkmate) {
    std::cout << "\n=== Starting Checkmate Test ===\n";
    game->reset();
    Board* board = const_cast<Board*>(game->getBoard());
    
    EXPECT_TRUE(game->makeMove("e2", "e4")) << "1. e4 failed";
    EXPECT_TRUE(game->makeMove("e7", "e5")) << "1... e5 failed";
    EXPECT_TRUE(game->makeMove("f1", "c4")) << "2. Bc4 failed";
    EXPECT_TRUE(game->makeMove("b8", "c6")) << "2... Nc6 failed";
    EXPECT_TRUE(game->makeMove("d1", "h5")) << "3. Qh5 failed";
    EXPECT_TRUE(game->makeMove("g8", "f6")) << "3... Nf6 failed";
    
    std::cout << "Position before Qxf7#:\n" << board->toString() << "\n";
    std::cout << "Is check before Qxf7#: " << board->isCheck(Piece::Color::Black) << "\n";
    
    EXPECT_TRUE(game->makeMove("h5", "f7")) << "4. Qxf7# failed";
    std::cout << "Position after Qxf7#:\n" << board->toString() << "\n";
    
    std::cout << "Checking final position:\n";
    std::cout << "Is check: " << board->isCheck(Piece::Color::Black) << "\n";
    
    const King* blackKing = nullptr;
    Position kingPos;
    
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            const Square* square = board->getSquare(x, y);
            if (square->isOccupied() && 
                square->getPiece()->getType() == Piece::Type::King &&
                square->getPiece()->getColor() == Piece::Color::Black) {
                blackKing = static_cast<const King*>(square->getPiece());
                kingPos = Position(x, y);
                break;
            }
        }
        if (blackKing) break;
    }
    
    if (blackKing) {
        std::cout << "Black king found at: " << kingPos.toAlgebraic() << "\n";
        auto kingMoves = blackKing->getPossibleMoves(board);
        std::cout << "Possible king moves: " << kingMoves.size() << "\n";
        for (const Position& move : kingMoves) {
            std::cout << "  " << move.toAlgebraic();
            Board tempBoard(*board);
            if (tempBoard.movePiece(kingPos, move)) {
                std::cout << " (legal)";
                if (!tempBoard.isCheck(Piece::Color::Black)) {
                    std::cout << " - escapes check!";
                }
            }
            std::cout << "\n";
        }
    }
    
    auto blackPieces = board->getPieces(Piece::Color::Black);
    for (Piece* piece : blackPieces) {
        if (piece->getType() == Piece::Type::King) continue;
        std::cout << "Checking moves for " << piece->toString() 
                 << " at " << piece->getPosition().toAlgebraic() << ":\n";
        auto moves = piece->getPossibleMoves(board);
        for (const Position& move : moves) {
            std::cout << "  " << move.toAlgebraic();
            Board tempBoard(*board);
            if (tempBoard.movePiece(piece->getPosition(), move)) {
                std::cout << " (legal)";
                if (!tempBoard.isCheck(Piece::Color::Black)) {
                    std::cout << " - defends king!";
                }
            }
            std::cout << "\n";
        }
    }
    
    EXPECT_TRUE(game->isCheckmate()) << "Position should be checkmate";
    EXPECT_TRUE(game->isGameOver()) << "Game should be over";
    EXPECT_EQ(game->getResult(), GameState::Result::WhiteWin) << "White should win";
    
    std::cout << "=== Checkmate Test Completed ===\n";
}

TEST_F(GameTest, Stalemate) {
    game->reset();
    Board* board = const_cast<Board*>(game->getBoard());
    board->clear();
    
    board->placePiece(new King(Piece::Color::Black), Position("h8"));
    board->placePiece(new Queen(Piece::Color::White), Position("g5"));
    board->placePiece(new King(Piece::Color::White), Position("g7"));

    EXPECT_TRUE(game->makeMove("g5", "g6")); 
    
    EXPECT_TRUE(game->isStalemate());
    EXPECT_FALSE(game->isCheck());
    EXPECT_TRUE(game->isGameOver());
    EXPECT_EQ(game->getResult(), GameState::Result::Draw);
}

TEST_F(GameTest, InsufficientMaterial) {
    game->reset();
    Board* board = const_cast<Board*>(game->getBoard());
    board->clear();
    
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new King(Piece::Color::Black), Position("e8"));
    board->placePiece(new Bishop(Piece::Color::White), Position("c1"));
    
    EXPECT_TRUE(game->getGameState()->isInsufficientMaterial(board));
    EXPECT_EQ(game->getResult(), GameState::Result::Draw);
}

TEST_F(GameTest, GetLegalMoves) {
    auto moves = game->getLegalMoves("e2");
    EXPECT_EQ(moves.size(), 2);
    
    moves = game->getLegalMoves("b1");
    EXPECT_EQ(moves.size(), 2);
    
    moves = game->getLegalMoves("i9");
    EXPECT_TRUE(moves.empty());
}

TEST_F(GameTest, Check) {
    EXPECT_TRUE(game->makeMove("e2", "e4"));
    EXPECT_TRUE(game->makeMove("e7", "e5"));
    EXPECT_TRUE(game->makeMove("d1", "h5"));
    EXPECT_TRUE(game->makeMove("b8", "c6"));
    EXPECT_TRUE(game->makeMove("h5", "f7"));
    
    EXPECT_TRUE(game->isCheck());
    EXPECT_FALSE(game->isCheckmate());
}

TEST_F(GameTest, PinnedPiece) {
    EXPECT_TRUE(game->makeMove("e2", "e4"));
    EXPECT_TRUE(game->makeMove("e7", "e5"));
    EXPECT_TRUE(game->makeMove("d2", "d4"));
    EXPECT_TRUE(game->makeMove("f8", "b4"));
    
    EXPECT_FALSE(game->makeMove("d2", "d3")); 
}

TEST_F(GameTest, CastlingThroughCheck) {
    game->reset();
    Board* board = game->getBoard();
    board->clear();
    
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new Rook(Piece::Color::White), Position("h1"));
    board->placePiece(new Rook(Piece::Color::Black), Position("e8"));
    
    EXPECT_FALSE(game->makeMove("e1", "g1"));
}

TEST_F(GameTest, PromotionVariants) {
    game->reset();
    Board* board = game->getBoard();
    board->clear();
    
    board->placePiece(new Pawn(Piece::Color::White), Position("e7"));
    board->placePiece(new King(Piece::Color::White), Position("e1"));
    board->placePiece(new King(Piece::Color::Black), Position("e8"));
    
    EXPECT_TRUE(game->makeMove("e7", "e8q")); 
    game->undoLastMove();
    EXPECT_TRUE(game->makeMove("e7", "e8r")); 
    game->undoLastMove();
    EXPECT_TRUE(game->makeMove("e7", "e8b")); 
    game->undoLastMove();
    EXPECT_TRUE(game->makeMove("e7", "e8n")); 
}

TEST_F(GameTest, GameStringRepresentation) {
    std::string boardStr = game->getBoardString();
    EXPECT_FALSE(boardStr.empty());
    
    std::string stateStr = game->getGameStateString();
    EXPECT_FALSE(stateStr.empty());
}

TEST_F(GameTest, ThreefoldRepetition) {
    for (int i = 0; i < 4; ++i) {
        EXPECT_TRUE(game->makeMove("g1", "f3"));
        EXPECT_TRUE(game->makeMove("g8", "f6"));
        EXPECT_TRUE(game->makeMove("f3", "g1"));
        EXPECT_TRUE(game->makeMove("f6", "g8"));
    }
    
    EXPECT_TRUE(game->isDraw());
    EXPECT_EQ(game->getResult(), GameState::Result::Draw);
}