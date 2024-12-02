#pragma once
#include "game/Game.hpp"
#include <string>
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

class Console {
public:
    Console(Game* gamePtr);
    ~Console() = default;

    void displayBoard() const;
    void displayGameStatus() const;
    void displayMoveHistory() const;
    void displayLegalMoves(const std::string& position) const;
    
    bool getMove(std::string& from, std::string& to);
    char getPromotionPiece() const;
    bool getYesNoInput(const std::string& prompt) const;
    std::string getStringInput(const std::string& prompt) const;


    void clearScreen() const;
    void showHelp() const;
    void showError(const std::string& message) const;
    void showMessage(const std::string& message) const;

    void addMoveToHistory(const std::string& from, const std::string& to, 
                             Piece::Type pieceType, Piece::Color pieceColor, 
                             bool isCapture, Piece::Type capturedType,
                             Piece::Color capturedColor);

    
private:
    Game* game;
    bool isRunning;
    struct MoveInfo {
        std::string from;
        std::string to;
        Piece::Type pieceType;
        Piece::Color pieceColor;
        bool isCapture;
        Piece::Type capturedPieceType;  
        Piece::Color capturedPieceColor;

        MoveInfo(const std::string& f, const std::string& t, 
                Piece::Type pt, Piece::Color pc, bool cap = false,
                Piece::Type cpt = Piece::Type::Pawn, 
                Piece::Color cpc = Piece::Color::White) 
            : from(f), to(t), pieceType(pt), pieceColor(pc), 
              isCapture(cap), capturedPieceType(cpt), capturedPieceColor(cpc) {}
    };
    std::vector<MoveInfo> moveHistory;


    void handleGameOver();
    void handleDraw();
    void handleUndo();
    void handleResignation();
    void handlePromotion(const std::string& from, std::string& to);
    
    bool isValidPosition(const std::string& pos) const;
    bool isValidMoveInput(const std::string& input) const;
    bool isPromotionMove(const std::string& from, const std::string& to) const;
    bool isPromotionPiece(char c) const;

    std::string colorToString(Piece::Color color) const;
    std::string resultToString(GameState::Result result) const;
    std::string formatMove(const std::string& from, const std::string& to) const;
    std::string formatPosition(const Position& pos) const;
    char figureNotation(const MoveInfo& move, bool isAtacking) const;
    static const char* PIECE_SYMBOLS[];
    static const char* COLOR_NAMES[];
    static const std::string BOARD_BORDER;
    static const std::string COLUMN_LABELS;
    static const std::string HELP_TEXT;

};