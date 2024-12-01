#include "GameState.hpp"
#include "moves/MoveGenerator.hpp"
#include "pieces/Bishop.hpp"
#include "pieces/Knight.hpp"
#include "pieces/Queen.hpp"
#include "pieces/Rook.hpp"
#include <algorithm>
#include <sstream>

GameState::GameState()
    : currentTurn(Piece::Color::White)
    , result(Result::None)
    , drawReason(DrawReason::None)
    , moveCount(1)
    , halfMoveCount(0)
    , drawOffered(false)
    , drawOfferingColor(Piece::Color::White)
{
}

bool GameState::makeMove(const Move& move, Board* board) {
    if (!board || !isMoveLegal(move, board)) {
        return false;
    }

    boardHistory.push_back(*board);

    const Square* fromSquare = board->getSquare(move.getFrom());
    const Square* toSquare = board->getSquare(move.getTo());
    
    bool isCapture = toSquare->isOccupied();
    Piece::Type capturedType = Piece::Type::Pawn;
    Piece::Color capturedColor = Piece::Color::White;
    
    if (isCapture) {
        capturedType = toSquare->getPiece()->getType();
        capturedColor = toSquare->getPiece()->getColor();
    }

    bool moveResult = board->movePiece(move.getFrom(), move.getTo());
    if (!moveResult) {
        boardHistory.pop_back();
        return false;
    }

    if (move.getType() == Move::Type::EnPassant) {
        isCapture = true;
        capturedType = Piece::Type::Pawn;
        capturedColor = (currentTurn == Piece::Color::White) ? 
                       Piece::Color::Black : Piece::Color::White;
    }

    if (move.getType() == Move::Type::Promotion) {
        Square* square = board->getSquare(move.getTo());
        if (square->isOccupied()) {
            Piece* oldPiece = square->removePiece();
            Piece* newPiece = nullptr;
            
            switch (move.getPromotionPiece()) {
                case Piece::Type::Queen:
                    newPiece = new Queen(currentTurn, move.getTo());
                    break;
                case Piece::Type::Rook:
                    newPiece = new Rook(currentTurn, move.getTo());
                    break;
                case Piece::Type::Bishop:
                    newPiece = new Bishop(currentTurn, move.getTo());
                    break;
                case Piece::Type::Knight:
                    newPiece = new Knight(currentTurn, move.getTo());
                    break;
                default:
                    delete oldPiece;
                    return false;
            }
            
            delete oldPiece;
            square->setPiece(newPiece);
            newPiece->setMoved(true);
        }
    }

    moveHistory.push_back(move);

    if (currentTurn == Piece::Color::Black) {
        moveCount++;
    }

    if (move.getType() == Move::Type::Capture || 
        move.getType() == Move::Type::EnPassant ||
        board->getSquare(move.getTo())->getPiece()->getType() == Piece::Type::Pawn) {
        halfMoveCount = 0;
    } else {
        halfMoveCount++;
    }

    switchTurn();
    updateGameState(board);
    clearDrawOffer();

    return true;
}

void GameState::undoLastMove(Board* board) {
    if (boardHistory.empty() || !board || moveHistory.empty()) {
        return;
    }

    *board = boardHistory.back();
    boardHistory.pop_back();
    
    moveHistory.pop_back();
    
    if (currentTurn == Piece::Color::White) {
        moveCount--;
    }
    
    switchTurn();
    
    result = Result::None;
    drawReason = DrawReason::None;
}

bool GameState::isMoveLegal(const Move& move, const Board* board) const {
    if (!board) return false;
    
    const Square* fromSquare = board->getSquare(move.getFrom());
    if (!fromSquare || !fromSquare->isOccupied() || 
        fromSquare->getPiece()->getColor() != currentTurn) {
        return false;
    }
    
    if (move.getType() == Move::Type::Castling) {
        const Piece* king = fromSquare->getPiece();
        if (king->getType() != Piece::Type::King || king->hasMoved()) {
            return false;
        }
        
        int rookFile = (move.getTo().getX() == 6) ? 7 : 0;
        Position rookPos(rookFile, move.getFrom().getY());
        const Square* rookSquare = board->getSquare(rookPos);
        
        if (!rookSquare || !rookSquare->isOccupied() ||
            rookSquare->getPiece()->getType() != Piece::Type::Rook ||
            rookSquare->getPiece()->hasMoved()) {
            return false;
        }
        
        if (board->isCheck(currentTurn)) {
            return false;
        }

        int step = (move.getTo().getX() > move.getFrom().getX()) ? 1 : -1;
        for (int x = move.getFrom().getX() + step; 
             x != rookFile; x += step) {
            Position pos(x, move.getFrom().getY());
            if (board->getSquare(pos)->isOccupied() || 
                board->isPositionAttacked(pos, currentTurn == Piece::Color::White ? 
                                      Piece::Color::Black : Piece::Color::White)) {
                return false;
            }
        }
        return true;
    }
    
    return MoveGenerator::isMoveLegal(board, move);
}

bool GameState::isGameOver() const {
    return result != Result::None;
}

bool GameState::isDraw() const {
    return result == Result::Draw;
}

bool GameState::isCheck(const Board* board) const {
    if (!board) return false;
    return board->isCheck(currentTurn);
}

bool GameState::isCheckmate(const Board* board) const {
    if (!board) return false;
    return board->isCheckmate(currentTurn);
}

bool GameState::isStalemate(const Board* board) const {
    if (!board) return false;
    return board->isStalemate(currentTurn);
}

bool GameState::isInsufficientMaterial(const Board* board) const {
    if (!board) return false;

    auto whitePieces = board->getPieces(Piece::Color::White);
    auto blackPieces = board->getPieces(Piece::Color::Black);
    
    if (whitePieces.size() == 1 && blackPieces.size() == 1) {
        return true;
    }
    
    if ((whitePieces.size() == 2 && blackPieces.size() == 1) ||
        (whitePieces.size() == 1 && blackPieces.size() == 2)) {
        for (const auto& piece : whitePieces) {
            if (piece->getType() == Piece::Type::Bishop || 
                piece->getType() == Piece::Type::Knight) {
                return true;
            }
        }
        for (const auto& piece : blackPieces) {
            if (piece->getType() == Piece::Type::Bishop || 
                piece->getType() == Piece::Type::Knight) {
                return true;
            }
        }
    }
    
    if (whitePieces.size() == 2 && blackPieces.size() == 2) {
        bool whiteBishop = false;
        bool blackBishop = false;
        Position whiteBishopPos;
        Position blackBishopPos;
        
        for (const auto& piece : whitePieces) {
            if (piece->getType() == Piece::Type::Bishop) {
                whiteBishop = true;
                whiteBishopPos = piece->getPosition();
            }
        }
        for (const auto& piece : blackPieces) {
            if (piece->getType() == Piece::Type::Bishop) {
                blackBishop = true;
                blackBishopPos = piece->getPosition();
            }
        }
        
        if (whiteBishop && blackBishop) {
            return ((whiteBishopPos.getX() + whiteBishopPos.getY()) % 2) ==
                   ((blackBishopPos.getX() + blackBishopPos.getY()) % 2);
        }
    }
    
    return false;
}

bool GameState::isThreefoldRepetition() const {
    if (positionHistory.size() < 6) return false;
    
    const std::string& currentPosition = positionHistory.back();
    int repetitions = 0;
    
    for (const auto& position : positionHistory) {
        if (position == currentPosition) {
            repetitions++;
            if (repetitions >= 3) {
                return true;
            }
        }
    }
    return false;
}

bool GameState::isFiftyMoveRule() const {
    return halfMoveCount >= 100;
}

void GameState::offerDraw(Piece::Color color) {
    if (!drawOffered) {
        drawOffered = true;
        drawOfferingColor = color;
    }
}

void GameState::acceptDraw() {
    if (drawOffered) {
        setResult(Result::Draw, DrawReason::MutualAgreement);
        clearDrawOffer();
    }
}

void GameState::declineDraw() {
    clearDrawOffer();
}

void GameState::resign(Piece::Color color) {
    setResult(color == Piece::Color::White ? Result::BlackWin : Result::WhiteWin);
}

void GameState::setResult(Result newResult, DrawReason reason) {
    result = newResult;
    if (result == Result::Draw) {
        drawReason = reason;
    } else {
        drawReason = DrawReason::None;
    }
}

std::string GameState::toString() const {
    std::stringstream ss;
    
    ss << "Current turn: " << (currentTurn == Piece::Color::White ? "White" : "Black") << "\n";
    ss << "Move count: " << moveCount << "\n";
    ss << "Half-move count: " << halfMoveCount << "\n";
    
    ss << "Game result: ";
    switch (result) {
        case Result::None: ss << "Ongoing"; break;
        case Result::WhiteWin: ss << "White wins"; break;
        case Result::BlackWin: ss << "Black wins"; break;
        case Result::Draw: ss << "Draw"; break;
        case Result::Stalemate: ss << "Stalemate"; break;
    }
    ss << "\n";
    
    if (drawReason != DrawReason::None) {
        ss << "Draw reason: ";
        switch (drawReason) {
            case DrawReason::Stalemate: ss << "Stalemate"; break;
            case DrawReason::InsufficientMaterial: ss << "Insufficient material"; break;
            case DrawReason::ThreefoldRepetition: ss << "Threefold repetition"; break;
            case DrawReason::FiftyMoveRule: ss << "Fifty-move rule"; break;
            case DrawReason::MutualAgreement: ss << "Mutual agreement"; break;
            default: break;
        }
        ss << "\n";
    }
    
    if (drawOffered) {
        ss << "Draw offered by " 
           << (drawOfferingColor == Piece::Color::White ? "White" : "Black") 
           << "\n";
    }
    
    return ss.str();
}

void GameState::reset() {
    currentTurn = Piece::Color::White;
    result = Result::None;
    drawReason = DrawReason::None;
    moveCount = 1;
    halfMoveCount = 0;
    moveHistory.clear();
    positionHistory.clear();
    clearDrawOffer();
}

void GameState::switchTurn() {
    currentTurn = (currentTurn == Piece::Color::White) ? 
                  Piece::Color::Black : Piece::Color::White;
}

void GameState::updateGameState(Board* board) {
    if (!board) return;

    if (isInsufficientMaterial(board)) {
        setResult(Result::Draw, DrawReason::InsufficientMaterial);
        return;
    }

    if (isFiftyMoveRule()) {
        setResult(Result::Draw, DrawReason::FiftyMoveRule);
        return;
    }
    
    if (board->isCheckmate(currentTurn)) {
        setResult(currentTurn == Piece::Color::White ? 
                 Result::BlackWin : Result::WhiteWin);
        return;
    }
    
    if (board->isStalemate(currentTurn)) {
        setResult(Result::Draw, DrawReason::Stalemate);
        return;
    }
    
    if (isThreefoldRepetition()) {
        setResult(Result::Draw, DrawReason::ThreefoldRepetition);
        return;
    }
}

void GameState::updatePositionHistory(const Board* board) {
    if (!board) return;
    std::string currentPosition = board->toFEN();
    positionHistory.push_back(currentPosition);
}

bool GameState::isMovePossible(const Board* board) const {
    if (!board) return false;
    
    auto pieces = board->getPieces(currentTurn);
    for (const auto& piece : pieces) {
        auto moves = piece->getPossibleMoves(board);
        if (!moves.empty()) {
            return true;
        }
    }
    return false;
}

void GameState::clearDrawOffer() {
    drawOffered = false;
    drawOfferingColor = Piece::Color::White;
}