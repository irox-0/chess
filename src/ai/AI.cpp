#include "ai/AI.hpp"
#include "moves/MoveGenerator.hpp"
#include <chrono>
#include <algorithm>

AI::AI() : rng(std::chrono::system_clock::now().time_since_epoch().count()) {
}

void AI::setSeed(unsigned int seed) {
    rng.seed(seed);
}

Move AI::getMove(const Board* board, Piece::Color color) const {
    if (!board) {
        return Move();
    }

    Move selectedMove;
    if (handleSpecialCases(board, color, selectedMove)) {
        return selectedMove;
    }

    auto pieces = board->getPieces(color);
    if (pieces.empty()) {
        return Move();
    }

    Piece* pieceWithMoves = findPieceWithMoves(board, color);
    if (!pieceWithMoves) {
        return Move();
    }

    auto moves = MoveGenerator::generateLegalMoves(board, pieceWithMoves->getPosition());
    if (moves.empty()) {
        return Move();
    }

    return selectRandomMove(moves);
}

Piece* AI::selectRandomPiece(const std::vector<Piece*>& pieces) const {
    if (pieces.empty()) {
        return nullptr;
    }
    
    std::uniform_int_distribution<size_t> dist(0, pieces.size() - 1);
    return pieces[dist(rng)];
}

Move AI::selectRandomMove(const std::vector<Move>& moves) const {
    if (moves.empty()) {
        return Move();
    }
    
    std::uniform_int_distribution<size_t> dist(0, moves.size() - 1);
    return moves[dist(rng)];
}

bool AI::handleSpecialCases(const Board* board, Piece::Color color, Move& selectedMove) const {
    if (board->isCheckmate(color) || board->isStalemate(color)) {
        return true;
    }

    if (board->isCheck(color)) {
        auto pieces = board->getPieces(color);
        for (auto piece : pieces) {
            auto moves = MoveGenerator::generateLegalMoves(board, piece->getPosition());
            if (!moves.empty()) {
                selectedMove = selectRandomMove(moves);
                return true;
            }
        }
    }

    return false;
}

bool AI::isCriticalPosition(const Board* board, Piece::Color color) const {
    return board->isCheck(color) || 
           board->isCheckmate(color) || 
           board->isStalemate(color);
}

Piece* AI::findPieceWithMoves(const Board* board, Piece::Color color) const {
    auto pieces = board->getPieces(color);
    std::vector<Piece*> piecesWithMoves;

    for (auto piece : pieces) {
        auto moves = MoveGenerator::generateLegalMoves(board, piece->getPosition());
        if (!moves.empty()) {
            piecesWithMoves.push_back(piece);
        }
    }

    return selectRandomPiece(piecesWithMoves);
}