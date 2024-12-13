#include "AI.hpp"
#include "moves/MoveGenerator.hpp"
#include <chrono>
#include <algorithm>

const std::map<Piece::Type, int> AI::PIECE_VALUES = {
    {Piece::Type::Pawn, 100},
    {Piece::Type::Knight, 320},
    {Piece::Type::Bishop, 330},
    {Piece::Type::Rook, 500},
    {Piece::Type::Queen, 900},
    {Piece::Type::King, 20000}
};

const int AI::PAWN_POSITION_BONUS[8][8] = {
    {0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    {5,  5, 10, 25, 25, 10,  5,  5},
    {0,  0,  0, 20, 20,  0,  0,  0},
    {5, -5,-10,  0,  0,-10, -5,  5},
    {5, 10, 10,-20,-20, 10, 10,  5},
    {0,  0,  0,  0,  0,  0,  0,  0}
};

const int AI::KNIGHT_POSITION_BONUS[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

AI::AI() : rng(std::chrono::system_clock::now().time_since_epoch().count()) {
}

void AI::setSeed(unsigned int seed) const {
    rng.seed(seed);
}

Move AI::getMove(const Board* board, Piece::Color color) const {
    if (!board) return Move(Position(-1, -1), Position(-1, -1));

    if (board->isCheckmate(color) || board->isStalemate(color)) {
        return Move(Position(-1, -1), Position(-1, -1));
    }

    std::vector<Move> possibleMoves = MoveGenerator::generateAllMoves(board, color);
    if (possibleMoves.empty()) {
        return Move(Position(-1, -1), Position(-1, -1));
    }

    if (board->isCheck(color)) {
        std::vector<Move> defendingMoves;
        for (const Move& move : possibleMoves) {
            Board tempBoard(*board);
            if (tempBoard.movePiece(move.getFrom(), move.getTo()) && !tempBoard.isCheck(color)) {
                defendingMoves.push_back(move);
            }
        }
        if (!defendingMoves.empty()) {
            possibleMoves = defendingMoves;
        }
    }

    Move bestMove = possibleMoves[0];
    int bestScore = -999999;

    for (const Move& move : possibleMoves) {
        Board tempBoard(*board);
        if (tempBoard.movePiece(move.getFrom(), move.getTo())) {
            int score = -negamax(&tempBoard, maxDepth - 1, -999999, 999999, 
                               color == Piece::Color::White ? Piece::Color::Black : Piece::Color::White);
            
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
        }
    }

    return bestMove;
}

int AI::negamax(Board* board, int depth, int alpha, int beta, Piece::Color color) const {
    if (depth == 0) {
        return evaluatePosition(board, color);
    }

    std::vector<Move> moves = MoveGenerator::generateAllMoves(board, color);
    if (moves.empty()) {
        if (board->isCheckmate(color)) {
            return -999999;
        }
        return 0; 
    }

    for (const Move& move : moves) {
        Board tempBoard(*board);
        if (tempBoard.movePiece(move.getFrom(), move.getTo())) {
            int score = -negamax(&tempBoard, depth - 1, -beta, -alpha,
                               color == Piece::Color::White ? Piece::Color::Black : Piece::Color::White);
            
            if (score >= beta) {
                return beta;
            }
            alpha = std::max(alpha, score);
        }
    }

    return alpha;
}

int AI::evaluatePosition(const Board* board, Piece::Color color) const {
    int score = 0;
    
    score += evaluateMaterial(board, color) * 2;
    
    score += evaluatePositionalAdvantage(board, color);
    
    score += evaluateKingSafety(board, color);
    
    score += evaluateCenterControl(board, color);
    
    score += evaluateDevelopment(board, color);
    
    return score;
}

int AI::evaluateMaterial(const Board* board, Piece::Color color) const {
    int score = 0;
    Piece::Color opponent = (color == Piece::Color::White) ? 
                           Piece::Color::Black : Piece::Color::White;

    auto pieces = board->getPieces(color);
    auto opponentPieces = board->getPieces(opponent);

    for (const auto& piece : pieces) {
        score += PIECE_VALUES.at(piece->getType());
    }

    for (const auto& piece : opponentPieces) {
        score -= PIECE_VALUES.at(piece->getType());
    }

    return score;
}

int AI::evaluatePositionalAdvantage(const Board* board, Piece::Color color) const {
    int score = 0;
    
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Position pos(x, y);
            const Square* square = board->getSquare(pos);
            if (square && square->isOccupied()) {
                Piece* piece = square->getPiece();
                if (piece->getColor() == color) {
                    if (piece->getType() == Piece::Type::Pawn) {
                        score += evaluatePawnPosition(pos, color);
                    }
                    else if (piece->getType() == Piece::Type::Knight) {
                        score += evaluateKnightPosition(pos, color);
                    }
                }
            }
        }
    }
    
    return score;
}

int AI::evaluatePawnPosition(const Position& pos, Piece::Color color) const {
    int x = pos.getX();
    int y = pos.getY();
    
    if (color == Piece::Color::Black) {
        y = 7 - y;
    }
    
    return PAWN_POSITION_BONUS[y][x];
}

int AI::evaluateKnightPosition(const Position& pos, Piece::Color color) const {
    int x = pos.getX();
    int y = pos.getY();
    
    if (color == Piece::Color::Black) {
        y = 7 - y;
    }
    
    return KNIGHT_POSITION_BONUS[y][x];
}

int AI::evaluateKingSafety(const Board* board, Piece::Color color) const {
    int score = 0;
    Piece* king = board->getKing(color);
    if (!king) return 0;

    Position kingPos = king->getPosition();
    
    score += evaluateKingPawnShield(board, kingPos, color);
    
    score -= evaluateOpenFilesNearKing(board, kingPos, color);
    
    if (board->isCheck(color)) {
        score -= 50;
    }
    
    return score;
}

int AI::evaluateKingPawnShield(const Board* board, const Position& kingPos, Piece::Color color) const {
    int score = 0;
    int baseRank = (color == Piece::Color::White) ? 0 : 7;
    
    for (int dx = -1; dx <= 1; dx++) {
        int x = kingPos.getX() + dx;
        if (x >= 0 && x < 8) {
            for (int dy = 1; dy <= 2; dy++) {
                int y = baseRank + (color == Piece::Color::White ? dy : -dy);
                if (y >= 0 && y < 8) {
                    Position pos(x, y);
                    const Square* square = board->getSquare(pos);
                    if (square && square->isOccupied() && 
                        square->getPiece()->getType() == Piece::Type::Pawn &&
                        square->getPiece()->getColor() == color) {
                        score += 10;
                    }
                }
            }
        }
    }
    
    return score;
}

int AI::evaluateOpenFilesNearKing(const Board* board, const Position& kingPos, Piece::Color color) const {
    int penalty = 0;
    
    for (int dx = -1; dx <= 1; dx++) {
        int x = kingPos.getX() + dx;
        if (x >= 0 && x < 8) {
            bool hasOwnPawn = false;
            for (int y = 0; y < 8; y++) {
                Position pos(x, y);
                const Square* square = board->getSquare(pos);
                if (square && square->isOccupied() && 
                    square->getPiece()->getType() == Piece::Type::Pawn &&
                    square->getPiece()->getColor() == color) {
                    hasOwnPawn = true;
                    break;
                }
            }
            if (!hasOwnPawn) {
                penalty += 10;
            }
        }
    }
    
    return penalty;
}

int AI::evaluateCenterControl(const Board* board, Piece::Color color) const {
    int score = 0;
    const std::vector<Position> centerSquares = {
        Position(3, 3), Position(3, 4),
        Position(4, 3), Position(4, 4)
    };

    for (const auto& pos : centerSquares) {
        if (board->isPositionAttacked(pos, color)) {
            score += 10;
        }
        
        const Square* square = board->getSquare(pos);
        if (square && square->isOccupied() && 
            square->getPiece()->getColor() == color) {
            score += 20;
        }
    }

    return score;
}

int AI::evaluateDevelopment(const Board* board, Piece::Color color) const {
    int score = 0;
    int baseRank = (color == Piece::Color::White) ? 0 : 7;
    
    for (int x = 1; x < 7; x++) {
        Position pos(x, baseRank);
        const Square* square = board->getSquare(pos);
        if (square && square->isOccupied() && 
            square->getPiece()->getColor() == color &&
            !square->getPiece()->hasMoved()) {
            score -= 10;
        }
    }
    
    return score;
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
            for (const auto& move : moves) {
                Board tempBoard(*board);
                if (tempBoard.movePiece(move.getFrom(), move.getTo()) && 
                    !tempBoard.isCheck(color)) {
                    selectedMove = move;
                    return true;
                }
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