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
    if (!board) return Move(Position(-1, -1), Position(-1, -1));

    if (board->isCheck(color)) {
        Position kingPos;
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                Position pos(x, y);
                const Square* square = board->getSquare(pos);
                if (square && square->isOccupied() && 
                    square->getPiece()->getColor() == color &&
                    square->getPiece()->getType() == Piece::Type::King) {
                    kingPos = pos;
                    break;
                }
            }
        }

        auto kingMoves = MoveGenerator::generateLegalMoves(board, kingPos);
        std::vector<Move> safeMoves;
        for (const auto& move : kingMoves) {
            Board tempBoard(*board);
            if (tempBoard.movePiece(move.getFrom(), move.getTo())) {
                if (!tempBoard.isCheck(color)) {
                    const Square* targetSquare = board->getSquare(move.getTo());
                    if (targetSquare->isOccupied() && 
                        targetSquare->getPiece()->getColor() != color) {
                        return move;
                    }
                    safeMoves.push_back(move);
                }
            }
        }

        if (!safeMoves.empty()) {
            std::uniform_int_distribution<size_t> dist(0, safeMoves.size() - 1);
            return safeMoves[dist(rng)];
        }

        std::vector<Move> allDefensiveMoves;
        auto pieces = board->getPieces(color);
        for (auto piece : pieces) {
            if (piece->getType() != Piece::Type::King) {
                auto moves = MoveGenerator::generateLegalMoves(board, piece->getPosition());
                for (const auto& move : moves) {
                    Board tempBoard(*board);
                    if (tempBoard.movePiece(move.getFrom(), move.getTo()) && 
                        !tempBoard.isCheck(color)) {
                        const Square* targetSquare = board->getSquare(move.getTo());
                        if (targetSquare->isOccupied() && 
                            targetSquare->getPiece()->getColor() != color) {
                            return move;
                        }
                        allDefensiveMoves.push_back(move);
                    }
                }
            }
        }

        if (!allDefensiveMoves.empty()) {
            std::uniform_int_distribution<size_t> dist(0, allDefensiveMoves.size() - 1);
            return allDefensiveMoves[dist(rng)];
        }

        return Move(Position(-1, -1), Position(-1, -1));
    }

    std::vector<Move> allMoves = MoveGenerator::generateAllMoves(board, color);
    if (!allMoves.empty()) {
        std::uniform_int_distribution<size_t> dist(0, allMoves.size() - 1);
        return allMoves[dist(rng)];
    }

    return Move(Position(-1, -1), Position(-1, -1));
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