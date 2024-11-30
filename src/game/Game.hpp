#pragma once
#include "board/Board.hpp"
#include "game/GameState.hpp"
#include "moves/Move.hpp"
#include <string>
#include <functional>

class Game {
public:
    Game();
    ~Game();

    void initialize();
    void reset();
    bool makeMove(const std::string& from, const std::string& to);
    void undoLastMove();
    
    void resign(Piece::Color color);
    void offerDraw(Piece::Color color);
    void acceptDraw();
    void declineDraw();
    
    bool isGameOver() const;
    bool isCheck() const;
    bool isCheckmate() const;
    bool isStalemate() const;
    bool isDraw() const;
    GameState::Result getResult() const;
    Piece::Color getCurrentTurn() const;
    
    std::string getBoardString() const;
    std::string getGameStateString() const;
    
    bool isValidMove(const std::string& from, const std::string& to) const;
    std::vector<Move> getLegalMoves(const std::string& pos) const;
    
    Board* getBoard() const { return board; }
    const GameState* getGameState() const { return gameState; }

private:
    Board* board;
    GameState* gameState;
    
    Move createMove(const std::string& from, const std::string& to) const;
    bool isPromotionMove(const Move& move) const;
    Piece::Type getPromotionPieceType(const std::string& to) const;
};