#include "game/Game.hpp"
#include "moves/MoveGenerator.hpp"
#include <cctype>

Game::Game() : board(new Board()), gameState(new GameState()) {
}

Game::~Game() {
    delete board;
    delete gameState;
}

void Game::initialize() {
    board->initialize();
    gameState->reset();
}

void Game::reset() {
    board->clear();
    board->initialize();
    gameState->reset();
}

bool Game::makeMove(const std::string& from, const std::string& to) {
    if (from.length() != 2 || to.length() < 2 || to.length() > 3) {
        return false;
    }

    Position fromPos(from);
    Position toPos(to.substr(0, 2));

    if (!fromPos.isValid() || !toPos.isValid()) {
        return false;
    }

    const Square* fromSquare = board->getSquare(fromPos);
    if (!fromSquare || !fromSquare->isOccupied()) {
        return false;
    }

    Piece* piece = fromSquare->getPiece();
    if (piece->getColor() != getCurrentTurn()) {
        return false;
    }

    Move move(fromPos, toPos);

    const Square* toSquare = board->getSquare(toPos);
    
    if (toSquare->isOccupied()) {
        move.setType(Move::Type::Capture);
    }

    if (piece->getType() == Piece::Type::Pawn) {
        if (std::abs(toPos.getY() - fromPos.getY()) == 2) {
            move.setType(Move::Type::DoublePawn);
        }
        else if (toPos == board->getEnPassantPosition()) {
            move.setType(Move::Type::EnPassant);
        }
        int promotionRank = (piece->getColor() == Piece::Color::White) ? 7 : 0;
        if (toPos.getY() == promotionRank) {
            if (to.length() != 3) {
                return false;
            }
            move.setType(Move::Type::Promotion);
            char promotionChar = std::tolower(to[2]);
            switch (promotionChar) {
                case 'q': move.setPromotionPiece(Piece::Type::Queen); break;
                case 'r': move.setPromotionPiece(Piece::Type::Rook); break;
                case 'b': move.setPromotionPiece(Piece::Type::Bishop); break;
                case 'n': move.setPromotionPiece(Piece::Type::Knight); break;
                default: return false;
            }
        }
    }
    else if (piece->getType() == Piece::Type::King && 
             std::abs(toPos.getX() - fromPos.getX()) == 2) {
        move.setType(Move::Type::Castling);
    }

    return gameState->makeMove(move, board);
}

void Game::undoLastMove() {
    gameState->undoLastMove(board);
}

void Game::resign(Piece::Color color) {
    gameState->resign(color);
}

void Game::offerDraw(Piece::Color color) {
    gameState->offerDraw(color);
}

void Game::acceptDraw() {
    gameState->acceptDraw();
}

void Game::declineDraw() {
    gameState->declineDraw();
}

bool Game::isGameOver() const {
    return gameState->isGameOver();
}

bool Game::isCheck() const {
    return gameState->isCheck(board);
}

bool Game::isCheckmate() const {
    return gameState->isCheckmate(board);
}

bool Game::isStalemate() const {
    return gameState->isStalemate(board);
}

bool Game::isDraw() const {
    return gameState->isDraw();
}

GameState::Result Game::getResult() const {
    return gameState->getResult();
}

Piece::Color Game::getCurrentTurn() const {
    return gameState->getCurrentTurn();
}

std::string Game::getBoardString() const {
    return board->toString();
}

std::string Game::getGameStateString() const {
    return gameState->toString();
}

bool Game::isValidMove(const std::string& from, const std::string& to) const {
    if (from.length() != 2 || to.length() < 2 || to.length() > 3) {
        return false;
    }

    Position fromPos(from);
    Position toPos(to.substr(0, 2));
    
    if (!fromPos.isValid() || !toPos.isValid()) {
        return false;
    }

    const Square* fromSquare = board->getSquare(fromPos);
    if (!fromSquare || !fromSquare->isOccupied()) {
        return false;
    }

    Piece* piece = fromSquare->getPiece();
    if (piece->getColor() != getCurrentTurn()) {
        return false;
    }

    Move move = createMove(from, to);
    return gameState->isMoveLegal(move, board);
}

std::vector<Move> Game::getLegalMoves(const std::string& pos) const {
    if (pos.length() != 2) {
        return std::vector<Move>();
    }

    Position position(pos);
    if (!position.isValid()) {
        return std::vector<Move>();
    }

    return MoveGenerator::generateLegalMoves(board, position);
}

Move Game::createMove(const std::string& from, const std::string& to) const {
    Position fromPos(from);
    Position toPos(to.substr(0, 2));
    
    const Square* fromSquare = board->getSquare(fromPos);
    const Square* toSquare = board->getSquare(toPos);
    
    Move::Type type = Move::Type::Normal;
    
    if (toSquare->isOccupied()) {
        type = Move::Type::Capture;
    }
    
    if (fromSquare->getPiece()->getType() == Piece::Type::Pawn) {
        if (std::abs(toPos.getY() - fromPos.getY()) == 2) {
            type = Move::Type::DoublePawn;
        } else if (toPos == board->getEnPassantPosition()) {
            type = Move::Type::EnPassant;
        }
    }
    
    if (fromSquare->getPiece()->getType() == Piece::Type::King && 
        std::abs(toPos.getX() - fromPos.getX()) == 2) {
        type = Move::Type::Castling;
    }
    
    return Move(fromPos, toPos, type);
}

bool Game::isPromotionMove(const Move& move) const {
    const Square* fromSquare = board->getSquare(move.getFrom());
    if (!fromSquare || !fromSquare->isOccupied()) {
        return false;
    }

    Piece* piece = fromSquare->getPiece();
    if (piece->getType() != Piece::Type::Pawn) {
        return false;
    }

    int promotionRank = (piece->getColor() == Piece::Color::White) ? 7 : 0;
    return move.getTo().getY() == promotionRank;
}

Piece::Type Game::getPromotionPieceType(const std::string& to) const {
    if (to.length() <= 2) return Piece::Type::Queen;
    
    char promotionChar = std::tolower(to[2]);
    switch (promotionChar) {
        case 'q': return Piece::Type::Queen;
        case 'r': return Piece::Type::Rook;
        case 'b': return Piece::Type::Bishop;
        case 'n': return Piece::Type::Knight;
        default: return Piece::Type::Queen;
    }
}