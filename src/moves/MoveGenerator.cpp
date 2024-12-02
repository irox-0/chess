#include "MoveGenerator.hpp"
#include "board/Board.hpp"
#include "board/Square.hpp"
#include "pieces/Piece.hpp"
#include "pieces/Pawn.hpp"
#include "pieces/Rook.hpp"
#include "pieces/Knight.hpp"
#include "pieces/Bishop.hpp"
#include "pieces/Queen.hpp"
#include "pieces/King.hpp"
#include <algorithm>

std::vector<Move> MoveGenerator::generateAllMoves(const Board* board, Piece::Color color) {
    std::vector<Move> allMoves;
    if (!board) return allMoves;
    
    auto pieces = board->getPieces(color);
    for (const auto& piece : pieces) {
        auto pieceMoves = generateLegalMoves(board, piece->getPosition());
        allMoves.insert(allMoves.end(), pieceMoves.begin(), pieceMoves.end());
    }
    
    auto castlingMoves = getCastlingMoves(board, color);
    allMoves.insert(allMoves.end(), castlingMoves.begin(), castlingMoves.end());
    
    return allMoves;
}

std::vector<Move> MoveGenerator::generateLegalMoves(const Board* board, const Position& pos) {
    std::vector<Move> moves;
    if (!board) return moves;
    
    const Square* square = board->getSquare(pos);
    if (!square || !square->isOccupied()) return moves;
    
    Piece* piece = square->getPiece();
    
    switch (piece->getType()) {
        case Piece::Type::Pawn:
            moves = generatePawnMoves(board, pos);
            break;
        case Piece::Type::Knight:
            moves = generateKnightMoves(board, pos);
            break;
        case Piece::Type::Bishop:
            moves = generateBishopMoves(board, pos);
            break;
        case Piece::Type::Rook:
            moves = generateRookMoves(board, pos);
            break;
        case Piece::Type::Queen:
            moves = generateQueenMoves(board, pos);
            break;
        case Piece::Type::King:
            moves = generateKingMoves(board, pos);
            break;
    }
    
    const auto it = std::remove_if(moves.begin(), moves.end(),
        [board, piece](const Move& move) {
            return wouldResultInCheck(board, move, piece->getColor());
        });
    moves.erase(it, moves.end());
    
    return moves;
}

std::vector<Move> MoveGenerator::generateCaptureMoves(const Board* board, Piece::Color color) {
    std::vector<Move> captures;
    if (!board) return captures;
    
    auto allMoves = generateAllMoves(board, color);
    std::copy_if(allMoves.begin(), allMoves.end(), std::back_inserter(captures),
        [](const Move& move) {
            return move.getType() == Move::Type::Capture || 
                   move.getType() == Move::Type::EnPassant;
        });
    
    return captures;
}

bool MoveGenerator::isMoveLegal(const Board* board, const Move& move) {
    if (!board) return false;
    
    const Square* fromSquare = board->getSquare(move.getFrom());
    if (!fromSquare || !fromSquare->isOccupied()) return false;
    
    auto legalMoves = generateLegalMoves(board, move.getFrom());
    return std::find(legalMoves.begin(), legalMoves.end(), move) != legalMoves.end();
}

std::vector<Move> MoveGenerator::generatePawnMoves(const Board* board, const Position& pos) {
    std::vector<Move> moves;
    const Square* square = board->getSquare(pos);
    if (!square || !square->isOccupied()) return moves;
    
    Piece* pawn = square->getPiece();
    if (pawn->getType() != Piece::Type::Pawn) return moves;
    
    int direction = (pawn->getColor() == Piece::Color::White) ? 1 : -1;
    int startRank = (pawn->getColor() == Piece::Color::White) ? 1 : 6;
    
    Position oneStep(pos.getX(), pos.getY() + direction);
    if (board->isPositionValid(oneStep) && !board->getSquare(oneStep)->isOccupied()) {
        if (isPawnPromotion(board, pos, oneStep)) {
            auto promotionMoves = getPromotionMoves(board, pos, oneStep);
            moves.insert(moves.end(), promotionMoves.begin(), promotionMoves.end());
        } else {
            moves.emplace_back(pos, oneStep, Move::Type::Normal);
            
            if (pos.getY() == startRank) {
                Position twoSteps(pos.getX(), pos.getY() + 2 * direction);
                if (board->isPositionValid(twoSteps) && 
                    !board->getSquare(twoSteps)->isOccupied()) {
                    moves.emplace_back(pos, twoSteps, Move::Type::DoublePawn);
                }
            }
        }
    }
    
    for (int dx : {-1, 1}) {
        Position capturePos(pos.getX() + dx, pos.getY() + direction);
        if (!board->isPositionValid(capturePos)) continue;
        
        const Square* captureSquare = board->getSquare(capturePos);
        if (captureSquare->isOccupied() && 
            captureSquare->getPiece()->getColor() != pawn->getColor()) {
            if (isPawnPromotion(board, pos, capturePos)) {
                auto promotionMoves = getPromotionMoves(board, pos, capturePos);
                moves.insert(moves.end(), promotionMoves.begin(), promotionMoves.end());
            } else {
                moves.emplace_back(pos, capturePos, Move::Type::Capture);
            }
        } else if (isEnPassantPossible(board, pos, capturePos)) {
            moves.emplace_back(pos, capturePos, Move::Type::EnPassant);
        }
    }
    
    return moves;
}


std::vector<Move> MoveGenerator::generateKnightMoves(const Board* board, const Position& pos) {
    std::vector<Move> moves;
    const Square* square = board->getSquare(pos);
    if (!square || !square->isOccupied()) return moves;
    
    Piece* knight = square->getPiece();
    const std::vector<std::pair<int, int>> offsets = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2}, {1, 2}, {2, -1}, {2, 1}
    };
    
    for (const auto& move : offsets) {
        Position newPos(pos.getX() + move.first, pos.getY() + move.second);
        if (!board->isPositionValid(newPos)) continue;
        
        const Square* targetSquare = board->getSquare(newPos);
        if (!targetSquare->isOccupied()) {
            moves.emplace_back(pos, newPos, Move::Type::Normal);
        } else if (targetSquare->getPiece()->getColor() != knight->getColor()) {
            moves.emplace_back(pos, newPos, Move::Type::Capture);
        }
    }
    
    return moves;
}

std::vector<Move> MoveGenerator::generateBishopMoves(const Board* board, const Position& pos) {
    std::vector<Move> moves;
    const Square* square = board->getSquare(pos);
    if (!square || !square->isOccupied()) return moves;
    
    Piece* bishop = square->getPiece();
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };
    
    for (const auto& move : directions) {
        Position current = pos;
        while (true) {
            current = current + Position(move.first, move.second);
            if (!board->isPositionValid(current)) break;
            
            const Square* targetSquare = board->getSquare(current);
            if (targetSquare->isOccupied()) {
                if (targetSquare->getPiece()->getColor() != bishop->getColor()) {
                    moves.emplace_back(pos, current, Move::Type::Capture);
                }
                break;
            }
            moves.emplace_back(pos, current, Move::Type::Normal);
        }
    }
    
    return moves;
}

std::vector<Move> MoveGenerator::generateRookMoves(const Board* board, const Position& pos) {
    std::vector<Move> moves;
    const Square* square = board->getSquare(pos);
    if (!square || !square->isOccupied()) return moves;
    
    Piece* rook = square->getPiece();
    const std::vector<std::pair<int, int>> directions = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0}
    };
    
    for (const auto& move : directions) {
        Position current = pos;
        while (true) {
            current = current + Position(move.first,  move.second);
            if (!board->isPositionValid(current)) break;
            
            const Square* targetSquare = board->getSquare(current);
            if (targetSquare->isOccupied()) {
                if (targetSquare->getPiece()->getColor() != rook->getColor()) {
                    moves.emplace_back(pos, current, Move::Type::Capture);
                }
                break;
            }
            moves.emplace_back(pos, current, Move::Type::Normal);
        }
    }
    
    return moves;
}

std::vector<Move> MoveGenerator::generateQueenMoves(const Board* board, const Position& pos) {
    std::vector<Move> moves;
    
    auto bishopMoves = generateBishopMoves(board, pos);
    auto rookMoves = generateRookMoves(board, pos);
    
    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
    
    return moves;
}

std::vector<Move> MoveGenerator::generateKingMoves(const Board* board, const Position& pos) {
    std::vector<Move> moves;
    const Square* square = board->getSquare(pos);
    if (!square || !square->isOccupied()) return moves;
    
    Piece* king = square->getPiece();
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };
    
    for (const auto& move : directions) {
        Position newPos(pos.getX() + move.first, pos.getY() + move.second);
        if (!board->isPositionValid(newPos)) continue;
        
        const Square* targetSquare = board->getSquare(newPos);
        if (!targetSquare->isOccupied()) {
            if (!board->isPositionAttacked(newPos, getOppositeColor(king->getColor()))) {
                moves.emplace_back(pos, newPos, Move::Type::Normal);
            }
        } else if (targetSquare->getPiece()->getColor() != king->getColor()) {
            if (!board->isPositionAttacked(newPos, getOppositeColor(king->getColor()))) {
                moves.emplace_back(pos, newPos, Move::Type::Capture);
            }
        }
    }
    auto castlingMoves = getCastlingMoves(board, king->getColor());
        moves.insert(moves.end(), castlingMoves.begin(), castlingMoves.end());
    
    return moves;
}

std::vector<Move> MoveGenerator::getCastlingMoves(const Board* board, Piece::Color color) {
    std::vector<Move> moves;
    if (!board) return moves;
    
    const int rank = (color == Piece::Color::White) ? 0 : 7;
    Position kingPos(4, rank);
    
    const Square* kingSquare = board->getSquare(kingPos);
    if (!kingSquare || !kingSquare->isOccupied()) return moves;
    
    Piece* king = kingSquare->getPiece();
    if (king->getType() != Piece::Type::King || 
        king->hasMoved() || 
        board->isCheck(color)) {
        return moves;
    }
    
    if (canCastleKingside(board, color)) {
        moves.emplace_back(kingPos, Position(6, rank), Move::Type::Castling);
    }
    
    if (canCastleQueenside(board, color)) {
        moves.emplace_back(kingPos, Position(2, rank), Move::Type::Castling);
    }
    
    return moves;
}

bool MoveGenerator::canCastleKingside(const Board* board, Piece::Color color) {
    if (!board) return false;
    
    int rank = (color == Piece::Color::White) ? 0 : 7;
    const Position kingPos(4, rank);
    const Position rookPos(7, rank);
    
    const Square* kingSquare = board->getSquare(kingPos);
    const Square* rookSquare = board->getSquare(rookPos);
    
    if (!kingSquare || !rookSquare || 
        !kingSquare->isOccupied() || !rookSquare->isOccupied()) {
        return false;
    }
    
    Piece* king = kingSquare->getPiece();
    Piece* rook = rookSquare->getPiece();
    
    if (king->hasMoved() || rook->hasMoved() ||
        king->getType() != Piece::Type::King ||
        rook->getType() != Piece::Type::Rook ||
        rook->getColor() != color) {
        return false;
    }
    
    for (int file = 5; file <= 6; ++file) {
        Position pos(file, rank);
        if (board->getSquare(pos)->isOccupied() ||
            board->isPositionAttacked(pos, getOppositeColor(color))) {
            return false;
        }
    }
    
    return true;
}

bool MoveGenerator::canCastleQueenside(const Board* board, Piece::Color color) {
    if (!board) return false;
    
    int rank = (color == Piece::Color::White) ? 0 : 7;
    Position kingPos(4, rank);
    Position rookPos(0, rank);
    
    const Square* kingSquare = board->getSquare(kingPos);
    const Square* rookSquare = board->getSquare(rookPos);
    
    if (!kingSquare || !rookSquare || 
        !kingSquare->isOccupied() || !rookSquare->isOccupied()) {
        return false;
    }
    
    Piece* king = kingSquare->getPiece();
    Piece* rook = rookSquare->getPiece();
    
    if (king->hasMoved() || rook->hasMoved() ||
        king->getType() != Piece::Type::King ||
        rook->getType() != Piece::Type::Rook ||
        rook->getColor() != color) {
        return false;
    }
    
    for (int file = 1; file <= 3; ++file) {
        Position pos(file, rank);
        if (board->getSquare(pos)->isOccupied()) {
            return false;
        }
    }
    
    for (int file = 2; file <= 4; ++file) {
        Position pos(file, rank);
        if (board->isPositionAttacked(pos, getOppositeColor(color))) {
            return false;
        }
    }
    
    return true;
}

bool MoveGenerator::areCastlingSquaresClear(const Board* board, const Position& kingPos, bool kingside) {
    int rank = kingPos.getY();
    int startFile = kingside ? 5 : 1;
    int endFile = kingside ? 6 : 3;
    
    for (int file = startFile; file <= endFile; file++) {
        Position pos(file, rank);
        if (board->getSquare(pos)->isOccupied()) {
            return false;
        }
    }
    return true;
}

bool MoveGenerator::areCastlingSquaresSafe(const Board* board, const Position& kingPos, bool kingside, Piece::Color color) {
    int rank = kingPos.getY();
    int startFile = kingside ? 4 : 2;
    int endFile = kingside ? 6 : 4;
    
    for (int file = startFile; file <= endFile; file++) {
        Position pos(file, rank);
        if (board->isPositionAttacked(pos, getOppositeColor(color))) {
            return false;
        }
    }
    return true;
}

std::vector<Move> MoveGenerator::getPromotionMoves(const Board* board, const Position& from, const Position& to) {
    std::vector<Move> moves;
    
    if (!isPawnPromotion(board, from, to)) return moves;
    
    moves.emplace_back(from, to, Move::Type::Promotion, Piece::Type::Queen);
    moves.emplace_back(from, to, Move::Type::Promotion, Piece::Type::Rook);
    moves.emplace_back(from, to, Move::Type::Promotion, Piece::Type::Bishop);
    moves.emplace_back(from, to, Move::Type::Promotion, Piece::Type::Knight);
    
    return moves;
}

bool MoveGenerator::isPawnPromotion(const Board* board, const Position& from, const Position& to) {
    const Square* fromSquare = board->getSquare(from);
    if (!fromSquare || !fromSquare->isOccupied()) return false;
    
    Piece* piece = fromSquare->getPiece();
    if (piece->getType() != Piece::Type::Pawn) return false;
    
    int promotionRank = (piece->getColor() == Piece::Color::White) ? 7 : 0;
    return to.getY() == promotionRank;
}

bool MoveGenerator::isEnPassantPossible(const Board* board, const Position& from, const Position& to) {
    const Square* fromSquare = board->getSquare(from);
    if (!fromSquare || !fromSquare->isOccupied()) return false;
    
    Piece* piece = fromSquare->getPiece();
    if (piece->getType() != Piece::Type::Pawn) return false;
    
    if (to != board->getEnPassantPosition()) return false;
    
    int expectedRank = (piece->getColor() == Piece::Color::White) ? 4 : 3;
    return from.getY() == expectedRank;
}

bool MoveGenerator::wouldResultInCheck(const Board* board, const Move& move, Piece::Color color) {
    Board tempBoard(*board);
    tempBoard.movePiece(move.getFrom(), move.getTo());
    return tempBoard.isCheck(color);
}

void MoveGenerator::updateCastlingRights(const Move& move) {
    if (move.getType() == Move::Type::Castling) {
        castlingRights.removeCastlingRights(
            move.getFrom().getY() == 0 ? Piece::Color::White : Piece::Color::Black
        );
        return;
    }
    
    const Position& from = move.getFrom();
    const Position& to = move.getTo();
    
    if (from == Position(0, 0) || to == Position(0, 0)) castlingRights.queensideWhite = false;
    if (from == Position(7, 0) || to == Position(7, 0)) castlingRights.kingsideWhite = false;
    if (from == Position(0, 7) || to == Position(0, 7)) castlingRights.queensideBlack = false;
    if (from == Position(7, 7) || to == Position(7, 7)) castlingRights.kingsideBlack = false;
    
    if (from == Position(4, 0)) castlingRights.removeCastlingRights(Piece::Color::White);
    if (from == Position(4, 7)) castlingRights.removeCastlingRights(Piece::Color::Black);
}

Piece::Color MoveGenerator::getOppositeColor(Piece::Color color) {
    return (color == Piece::Color::White) ? Piece::Color::Black : Piece::Color::White;
}