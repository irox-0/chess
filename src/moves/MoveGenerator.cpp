#include "MoveGenerator.hpp"
#include <algorithm>
#include <cmath>

namespace Chess {

MoveGenerator::MoveGenerator(const Board& board) : board(board) {}

std::vector<Move> MoveGenerator::generateAllMoves(Color color) const {
    std::vector<Move> moves;
    
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Position pos(row, col);
            const auto* piece = board.getPiece(pos);
            
            if (piece && piece->getColor() == color) {
                std::vector<Move> pieceMoves;
                
                switch (piece->getType()) {
                    case PieceType::PAWN:
                        pieceMoves = generatePawnMoves(pos);
                        break;
                    case PieceType::KNIGHT:
                        pieceMoves = generateKnightMoves(pos);
                        break;
                    case PieceType::BISHOP:
                        pieceMoves = generateBishopMoves(pos);
                        break;
                    case PieceType::ROOK:
                        pieceMoves = generateRookMoves(pos);
                        break;
                    case PieceType::QUEEN:
                        pieceMoves = generateQueenMoves(pos);
                        break;
                    case PieceType::KING:
                        pieceMoves = generateKingMoves(pos);
                        break;
                }
                
                moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
            }
        }
    }
    
    auto castlingMoves = generateCastlingMoves(color);
    moves.insert(moves.end(), castlingMoves.begin(), castlingMoves.end());
    
    return moves;
}

std::vector<Move> MoveGenerator::generateLegalMoves(Color color) const {
    std::vector<Move> allMoves = generateAllMoves(color);
    std::vector<Move> legalMoves;
    
    for (const auto& move : allMoves) {
        if (isLegalMove(move) && !leavesKingInCheck(move, color)) {
            legalMoves.push_back(move);
        }
    }
    
    return legalMoves;
}

std::vector<Move> MoveGenerator::generateLegalMoves(const Position& pos) const {
    const auto* piece = board.getPiece(pos);
    if (!piece) return {};
    
    std::vector<Move> moves;
    Color color = piece->getColor();
    
    switch (piece->getType()) {
        case PieceType::PAWN:
            moves = generatePawnMoves(pos);
            break;
        case PieceType::KNIGHT:
            moves = generateKnightMoves(pos);
            break;
        case PieceType::BISHOP:
            moves = generateBishopMoves(pos);
            break;
        case PieceType::ROOK:
            moves = generateRookMoves(pos);
            break;
        case PieceType::QUEEN:
            moves = generateQueenMoves(pos);
            break;
        case PieceType::KING:
            moves = generateKingMoves(pos);
            break;
    }
    
    std::vector<Move> legalMoves;
    std::copy_if(moves.begin(), moves.end(),
                 std::back_inserter(legalMoves),
                 [this, color](const Move& move) {
                     return isLegalMove(move) && !leavesKingInCheck(move, color);
                 });
    
    return legalMoves;
}

std::vector<Move> MoveGenerator::generatePawnMoves(const Position& pos) const {
    std::vector<Move> moves;
    const auto* piece = board.getPiece(pos);
    if (!piece || piece->getType() != PieceType::PAWN) return moves;
    
    Color color = piece->getColor();
    int direction = (color == Color::WHITE) ? -1 : 1;
    int startRank = (color == Color::WHITE) ? 6 : 1;
    
    Position oneStep(pos.getX() + direction, pos.getY());
    if (isWithinBoard(oneStep) && !board.getPiece(oneStep)) {
        addMove(moves, pos, oneStep);
        
        if (pos.getX() == startRank) {
            Position twoStep(pos.getX() + 2 * direction, pos.getY());
            if (!board.getPiece(twoStep)) {
                addMove(moves, pos, twoStep);
            }
        }
    }
    
    for (int colOffset : {-1, 1}) {
        Position capturePos(pos.getX() + direction, pos.getY() + colOffset);
        if (isWithinBoard(capturePos)) {
            const auto* targetPiece = board.getPiece(capturePos);
            if (targetPiece && targetPiece->getColor() != color) {
                addMove(moves, pos, capturePos);
            }
        }
    }
    
    auto enPassantMoves = generateEnPassantMoves(pos);
    moves.insert(moves.end(), enPassantMoves.begin(), enPassantMoves.end());
    
    return moves;
}

std::vector<Move> MoveGenerator::generateKnightMoves(const Position& pos) const {
    std::vector<Move> moves;
    const auto* piece = board.getPiece(pos);
    if (!piece || piece->getType() != PieceType::KNIGHT) return moves;
    
    const std::vector<std::pair<int, int>> knightMoves = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2}, {1, 2}, {2, -1}, {2, 1}
    };
    
    for (const auto& [rowOffset, colOffset] : knightMoves) {
        Position newPos(pos.getX() + rowOffset, pos.getY() + colOffset);
        if (isWithinBoard(newPos)) {
            const auto* targetPiece = board.getPiece(newPos);
            if (!targetPiece || targetPiece->getColor() != piece->getColor()) {
                addMove(moves, pos, newPos);
            }
        }
    }
    
    return moves;
}

std::vector<Move> MoveGenerator::generateBishopMoves(const Position& pos) const {
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };
    std::vector<Move> moves;
    addSlidingMoves(moves, pos, directions);
    return moves;
}

std::vector<Move> MoveGenerator::generateRookMoves(const Position& pos) const {
    const std::vector<std::pair<int, int>> directions = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };
    std::vector<Move> moves;
    addSlidingMoves(moves, pos, directions);
    return moves;
}

std::vector<Move> MoveGenerator::generateQueenMoves(const Position& pos) const {
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };
    std::vector<Move> moves;
    addSlidingMoves(moves, pos, directions);
    return moves;
}

std::vector<Move> MoveGenerator::generateKingMoves(const Position& pos) const {
    std::vector<Move> moves;
    const auto* piece = board.getPiece(pos);
    if (!piece || piece->getType() != PieceType::KING) return moves;
    
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };
    
    for (const auto& [rowOffset, colOffset] : directions) {
        Position newPos(pos.getX() + rowOffset, pos.getY() + colOffset);
        if (isWithinBoard(newPos)) {
            const auto* targetPiece = board.getPiece(newPos);
            if (!targetPiece || targetPiece->getColor() != piece->getColor()) {
                if (!isSquareAttacked(newPos, piece->getColor() == Color::WHITE ? Color::BLACK : Color::WHITE)) {
                    addMove(moves, pos, newPos);
                }
            }
        }
    }
    
    return moves;
}

void MoveGenerator::addSlidingMoves(std::vector<Move>& moves, const Position& from,
                                  const std::vector<std::pair<int, int>>& directions) const {
    const auto* piece = board.getPiece(from);
    if (!piece) return;
    
    for (const auto& [rowDir, colDir] : directions) {
        Position current = from;
        while (true) {
            current = Position(current.getX() + rowDir, current.getY() + colDir);
            if (!isWithinBoard(current)) break;
            
            const auto* targetPiece = board.getPiece(current);
            if (!targetPiece) {
                addMove(moves, from, current);
            } else {
                if (targetPiece->getColor() != piece->getColor()) {
                    addMove(moves, from, current);
                }
                break;
            }
        }
    }
}

bool MoveGenerator::isLegalMove(const Move& move) const {
    if (!isWithinBoard(move.getFrom()) || !isWithinBoard(move.getTo())) {
        return false;
    }
    
    const auto* piece = board.getPiece(move.getFrom());
    if (!piece) return false;
    
    const auto* targetPiece = board.getPiece(move.getTo());
    if (targetPiece && targetPiece->getColor() == piece->getColor()) {
        return false;
    }
    
    if (piece->getType() != PieceType::KNIGHT && !isPathClear(move.getFrom(), move.getTo())) {
        return false;
    }
    
    return true;
}

bool MoveGenerator::leavesKingInCheck(const Move& move, Color color) const {
    Board tempBoard = board;
    
    
    
    Position kingPos;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Position pos(row, col);
            const auto* piece = tempBoard.getPiece(pos);
            if (piece && piece->getType() == PieceType::KING && piece->getColor() == color) {
                kingPos = pos;
                break;
            }
        }
    }
    
    return isSquareAttacked(kingPos, color == Color::WHITE ? Color::BLACK : Color::WHITE);
}

bool MoveGenerator::isWithinBoard(const Position& pos) {
    return pos.getX() >= 0 && pos.getX() < 8 && 
           pos.getY() >= 0 && pos.getY() < 8;
}

bool MoveGenerator::isPathClear(const Position& from, const Position& to) const {
    int rowDir = (to.getX() > from.getX()) ? 1 : (to.getX() < from.getX()) ? -1 : 0;
    int colDir = (to.getY() > from.getY()) ? 1 : (to.getY() < from.getY()) ? -1 : 0;
    
    Position current = from;
    while (current != to) {
        current = Position(current.getX() + rowDir, current.getY() + colDir);
        if (current != to && board.getPiece(current)) {
            return false;
        }
    }
    
    return true;
}

void MoveGenerator::addMove(std::vector<Move>& moves, const Position& from, 
                          const Position& to, Move::Type type) const {
    moves.emplace_back(from, to, type);
}

} 