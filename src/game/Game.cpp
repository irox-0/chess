#include "Game.hpp"
#include "../moves/MoveGenerator.hpp"
#include <fstream>
#include <sstream>

namespace Chess {

Game::Game() : board(std::make_unique<Board>()), currentState() {
    reset();
}

void Game::reset() {
    board->setupInitialPosition();
    moveHistory.clear();
    currentState = GameState();
    updateGameState();
}

bool Game::makeMove(const Move& move) {
    if (!isMoveLegal(move)) {
        return false;
    }

    moveHistory.push_back(move);
    
    board->makeMove(move);
    
    switchTurn();
    updateGameState();
    
    return true;
}

bool Game::undoLastMove() {
    if (moveHistory.empty()) {
        return false;
    }

    const Move& lastMove = moveHistory.back();
    board->undoMove(lastMove);
    moveHistory.pop_back();
    
    switchTurn();
    updateGameState();
    
    return true;
}

std::vector<Move> Game::getValidMoves() const {
    MoveGenerator moveGen(*board);
    return moveGen.generateLegalMoves(isWhiteTurn());
}

bool Game::isWhiteTurn() const {
    return currentState.isWhiteTurn();
}

GameResult Game::getGameResult() const {
    return currentState.getResult();
}

const Board& Game::getBoard() const {
    return *board;
}

bool Game::isCheck() const {
    return isKingInCheck(isWhiteTurn());
}

bool Game::isCheckmate() const {
    if (!isCheck()) {
        return false;
    }
    return getValidMoves().empty();
}

bool Game::isStalemate() const {
    if (isCheck()) {
        return false;
    }
    return getValidMoves().empty();
}

const std::vector<Move>& Game::getMoveHistory() const {
    return moveHistory;
}

bool Game::saveGame(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& move : moveHistory) {
        file << move.toString() << "\n";
    }

    return true;
}

bool Game::loadGame(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    reset();
    std::string line;
    while (std::getline(file, line)) {
        Move move = Move::fromString(line);
        if (!makeMove(move)) {
            reset();
            return false;
        }
    }

    return true;
}

bool Game::isMoveLegal(const Move& move) const {
    auto validMoves = getValidMoves();
    return std::find(validMoves.begin(), validMoves.end(), move) != validMoves.end();
}

void Game::updateGameState() {
    if (isCheckmate()) {
        currentState.setResult(isWhiteTurn() ? GameResult::BLACK_WINS : GameResult::WHITE_WINS);
    } else if (isStalemate()) {
        currentState.setResult(GameResult::STALEMATE);
    }
    
}

bool Game::isKingInCheck(bool white) const {
    return board->isKingInCheck(white);
}

void Game::switchTurn() {
    currentState.switchTurn();
}

} 