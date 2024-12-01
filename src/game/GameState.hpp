#pragma once
#include "board/Board.hpp"
#include "moves/Move.hpp"
#include <vector>
#include <string>

class GameState {
public:
    enum class Result {
        None,           
        WhiteWin,      
        BlackWin,      
        Draw,          
        Stalemate      
    };

    enum class DrawReason {
        None,
        Stalemate,              
        InsufficientMaterial,   
        ThreefoldRepetition,    
        FiftyMoveRule,          
        MutualAgreement         
    };
    
    GameState();
    ~GameState() = default;

    Piece::Color getCurrentTurn() const { return currentTurn; }
    Result getResult() const { return result; }
    DrawReason getDrawReason() const { return drawReason; }
    int getMoveCount() const { return moveCount; }
    int getHalfMoveCount() const { return halfMoveCount; }
    const std::vector<Move>& getMoveHistory() const { return moveHistory; }
    const std::vector<std::string>& getPositionHistory() const { return positionHistory; }

    bool makeMove(const Move& move, Board* board);
    void undoLastMove(Board* board);
    bool isMoveLegal(const Move& move, const Board* board) const;
    
    bool isGameOver() const;
    bool isDraw() const;
    bool isCheck(const Board* board) const;
    bool isCheckmate(const Board* board) const;
    bool isStalemate(const Board* board) const;
    bool isInsufficientMaterial(const Board* board) const;
    bool isThreefoldRepetition() const;
    bool isFiftyMoveRule() const;
    
    void offerDraw(Piece::Color color);
    void acceptDraw();
    void declineDraw();
    bool isDrawOffered() const { return drawOffered; }
    Piece::Color getDrawOfferingColor() const { return drawOfferingColor; }

    void resign(Piece::Color color);
    void setResult(Result newResult, DrawReason reason = DrawReason::None);

    std::string toString() const;
    void reset();

private:
    std::vector<Board> boardHistory;
    Piece::Color currentTurn;         
    Result result;                     
    DrawReason drawReason;            
    int moveCount;                     
    int halfMoveCount;                 
    std::vector<Move> moveHistory;     
    std::vector<std::string> positionHistory; 

    bool drawOffered;                  
    Piece::Color drawOfferingColor;    

    void switchTurn();
    void updateGameState(Board* board);
    void updatePositionHistory(const Board* board);
    bool isMovePossible(const Board* board) const;
    void clearDrawOffer();
};