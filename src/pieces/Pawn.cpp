#include "Pawn.hpp"

Pawn::Pawn(Color color) 
    : Piece(color, Type::Pawn, Position(0, 0)) {  
}

Pawn::Pawn(Color color, Position position) 
    : Piece(color, Type::Pawn, position) {
}

std::vector<Position> Pawn::getPossibleMoves(const Board* board) const {
    std::vector<Position> moves;
    if (!board) return moves;

    int direction = (color == Color::White) ? 1 : -1;
    auto pinInfo = checkIfPinned(board);
    
    if (pinInfo.isPinned) {
        if (pinInfo.pinDirection.getX() == 0) {
            Position oneStep(position.getX(), position.getY() + direction);
            if (board->isPositionValid(oneStep) && !board->getSquare(oneStep)->isOccupied()) {
                moves.push_back(oneStep);
                
                if (!moved && position.getY() == getStartRank()) {
                    Position twoSteps(position.getX(), position.getY() + 2 * direction);
                    if (board->isPositionValid(twoSteps) && !board->getSquare(twoSteps)->isOccupied()) {
                        moves.push_back(twoSteps);
                    }
                }
            }
        }
        else if (std::abs(pinInfo.pinDirection.getX()) == 1 && 
                pinInfo.pinDirection.getY() / std::abs(pinInfo.pinDirection.getY()) == direction) {
            Position capturePos(position.getX() + pinInfo.pinDirection.getX(), 
                              position.getY() + direction);
            if (board->isPositionValid(capturePos)) {
                const Square* square = board->getSquare(capturePos);
                if ((square->isOccupied() && square->getPiece()->getColor() != color) ||
                    capturePos == board->getEnPassantPosition()) {
                    moves.push_back(capturePos);
                }
            }
        }
        return moves;
    }

    Position oneStep(position.getX(), position.getY() + direction);
    if (board->isPositionValid(oneStep) && !board->getSquare(oneStep)->isOccupied()) {
        moves.push_back(oneStep);
        
        if (!moved && position.getY() == getStartRank()) {
            Position twoSteps(position.getX(), position.getY() + 2 * direction);
            if (board->isPositionValid(twoSteps) && !board->getSquare(twoSteps)->isOccupied()) {
                moves.push_back(twoSteps);
            }
        }
    }

    for (int dx : {-1, 1}) {
        Position capturePos(position.getX() + dx, position.getY() + direction);
        if (!board->isPositionValid(capturePos)) continue;

        const Square* square = board->getSquare(capturePos);
        // Обычное взятие
        if (square->isOccupied() && square->getPiece()->getColor() != color) {
            moves.push_back(capturePos);
        }
        // Взятие на проходе
        else if (capturePos == board->getEnPassantPosition()) {
            moves.push_back(capturePos);
        }
    }

    std::vector<Position> legalMoves;
    for (const auto& move : moves) {
        Board tempBoard(*board);
        tempBoard.movePiece(position, move);
        if (!tempBoard.isCheck(color)) {
            legalMoves.push_back(move);
        }
    }

    return legalMoves;
}

std::vector<Position> Pawn::getAttackedSquares(const Board* board) const {
    std::vector<Position> attackedSquares;
    int direction = getDirection();
    
    Position leftCapture(position.getX() - 1, position.getY() + direction);
    Position rightCapture(position.getX() + 1, position.getY() + direction);
    
    if (leftCapture.isValid()) {
        attackedSquares.push_back(leftCapture);
    }
    if (rightCapture.isValid()) {
        attackedSquares.push_back(rightCapture);
    }
    
    return attackedSquares;
}

bool Pawn::canMoveTo(const Position& target, const Board* board) const {
    if (!board || !target.isValid()) {
        return false;
    }

    auto possibleMoves = getPossibleMoves(board);
    
    for (const auto& move : possibleMoves) {
        if (move == target) {
            return true;
        }
    }
    
    return false;
}

char Pawn::getSymbol() const {
    return color == Color::White ? 'P' : 'p';
}

Piece* Pawn::clone() const {
    return new Pawn(*this);
}

bool Pawn::canBePromoted() const {
    return position.getY() == getPromotionRank();
}

bool Pawn::isEnPassantPossible(const Position& target, const Board* board) const {
    if (!board || !target.isValid()) {
        return false;
    }

    int expectedRank = (color == Color::White) ? 4 : 3;
    if (position.getY() != expectedRank) {
        return false;
    }

    int deltaX = target.getX() - position.getX();
    int deltaY = target.getY() - position.getY();
    if (std::abs(deltaX) != 1 || deltaY != getDirection()) {
        return false;
    }

    Position enemyPos(target.getX(), position.getY());
    const Square* enemySquare = board->getSquare(enemyPos);
    if (!enemySquare || !enemySquare->isOccupied()) {
        return false;
    }

    Piece* enemyPiece = enemySquare->getPiece();
    if (enemyPiece->getType() != Type::Pawn || enemyPiece->getColor() == color) {
        return false;
    }

    return target == board->getEnPassantPosition();
}

std::vector<Position> Pawn::getForwardMoves(const Board* board) const {
    std::vector<Position> moves;
    int direction = getDirection();
    
    Position oneStep(position.getX(), position.getY() + direction);
    if (oneStep.isValid() && !board->getSquare(oneStep)->isOccupied()) {
        moves.push_back(oneStep);
        
        if (!moved && position.getY() == getStartRank()) {
            Position twoSteps(position.getX(), position.getY() + 2 * direction);
            if (twoSteps.isValid() && !board->getSquare(twoSteps)->isOccupied()) {
                moves.push_back(twoSteps);
            }
        }
    }
    
    return moves;
}


std::vector<Position> Pawn::getCaptureMoves(const Board* board) const {
    std::vector<Position> moves;
    int direction = getDirection();
    
    Position leftCapture(position.getX() - 1, position.getY() + direction);
    Position rightCapture(position.getX() + 1, position.getY() + direction);
    
    if (leftCapture.isValid()) {
        const Square* square = board->getSquare(leftCapture);
        if (square->isOccupied() && square->getPiece()->getColor() != color) {
            moves.push_back(leftCapture);
        }
        else if (leftCapture == board->getEnPassantPosition()) {
            moves.push_back(leftCapture);
        }
    }
    
    if (rightCapture.isValid()) {
        const Square* square = board->getSquare(rightCapture);
        if (square->isOccupied() && square->getPiece()->getColor() != color) {
            moves.push_back(rightCapture);
        }
        else if (rightCapture == board->getEnPassantPosition()) {
            moves.push_back(rightCapture);
        }
    }
    
    return moves;
}


int Pawn::getDirection() const {
    return (color == Color::White) ? 1 : -1;
}

int Pawn::getStartRank() const {
    return (color == Color::White) ? 1 : 6;
}

int Pawn::getPromotionRank() const {
    return (color == Color::White) ? 7 : 0;
}