#pragma once

#include <chrono>
#include <optional>
#include <string>

namespace Chess {

enum class GameResult;

class GameState {
public:
    GameState();
    
    bool isWhiteTurn() const;
    GameResult getResult() const;
    int getFullMoveNumber() const;
    int getHalfMoveClock() const;
    
    void switchTurn();
    void setResult(GameResult result);
    void incrementFullMoveNumber();
    void resetHalfMoveClock();
    void incrementHalfMoveClock();
    
    void startTimer();
    void stopTimer();
    void setTimeControl(int minutes, int incrementSeconds = 0);
    int getRemainingTime(bool white) const;  
    
    std::string serialize() const;
    static GameState deserialize(const std::string& data);
    
    void reset();

private:
    bool whiteTurn;                  
    GameResult result;               
    int fullMoveNumber;             
    int halfMoveClock;              
    
    struct TimeControl {
        int initialMinutes;
        int incrementSeconds;
        std::chrono::milliseconds whiteTimeRemaining;
        std::chrono::milliseconds blackTimeRemaining;
        std::optional<std::chrono::steady_clock::time_point> lastMoveTime;
    } timeControl;
    
    void updateRemainingTime();
};

} 