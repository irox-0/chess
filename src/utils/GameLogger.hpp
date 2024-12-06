#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "game/Game.hpp"
#include "moves/Move.hpp"
#include <direct.h>

class GameLogger {
public:
    GameLogger(Game* game);
    
    void logMove(const std::string& from, const std::string& to, 
                Piece::Type pieceType, Piece::Color pieceColor,
                bool isCapture = false, bool isCheck = false, 
                bool isCheckmate = false,
                Piece::Type promotionPiece = Piece::Type::Queen);
    
    bool saveGame(const std::string& filename);
    
    bool loadGame(const std::string& filename);
    
    bool loadPositionFromFile(const std::string& filename);

private:
    Game* game;
    std::vector<std::string> moveHistory;
    static const std::string SAVE_DIRECTORY;
    
    char getPieceNotation(Piece::Type type) const;
    
    std::string moveToNotation(const std::string& from, const std::string& to,
                              Piece::Type pieceType, Piece::Color pieceColor,
                              bool isCapture, bool isCheck,
                              bool isCheckmate, Piece::Type promotionPiece);
    
    bool parseNotation(const std::string& notation, std::string& from, 
                      std::string& to, Piece::Type& pieceType);

    bool isValidSquare(const std::string& square) {
    if (square.length() != 2) {
        return false;
    }
    return (square[0] >= 'a' && square[0] <= 'h') &&
           (square[1] >= '1' && square[1] <= '8');
}
};