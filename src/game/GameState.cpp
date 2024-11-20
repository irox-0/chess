#include "GameState.hpp"
#include "Game.hpp"
#include <sstream>

namespace Chess {

GameState::GameState()
    : whiteTurn(true)
    , result(GameResult::IN_PROGRESS)
    , fullMoveNumber(1)
    , halfMoveClock(0) {
    timeControl.initialMinutes = 0;
    timeControl.incrementSeconds = 0;
    timeControl.whiteTimeRemaining = std::chrono::milliseconds(0);
    timeControl.blackTimeRemaining = std::chrono::milliseconds(0);
    timeControl.lastMoveTime = std::nullopt;
}

bool GameState::isWhiteTurn() const {
    return whiteTurn;
}

GameResult GameState::getResult() const {
    return result;
}

int GameState::getFullMoveNumber() const {
    return fullMoveNumber;
}

int GameState::getHalfMoveClock() const {
    return halfMoveClock;
}

void GameState::switchTurn() {
    updateRemainingTime();
    
    if (timeControl.incrementSeconds > 0) {
        auto increment = std::chrono::seconds(timeControl.incrementSeconds);
        if (whiteTurn) {
            timeControl.whiteTimeRemaining += increment;
        } else {
            timeControl.blackTimeRemaining += increment;
        }
    }
    
    whiteTurn = !whiteTurn;
    
    if (!whiteTurn) {
        ++fullMoveNumber;
    }
    
    timeControl.lastMoveTime = std::chrono::steady_clock::now();
}

void GameState::setResult(GameResult result) {
    this->result = result;
    stopTimer();
}

void GameState::incrementFullMoveNumber() {
    ++fullMoveNumber;
}

void GameState::resetHalfMoveClock() {
    halfMoveClock = 0;
}

void GameState::incrementHalfMoveClock() {
    ++halfMoveClock;
}

void GameState::startTimer() {
    if (!timeControl.lastMoveTime) {
        timeControl.lastMoveTime = std::chrono::steady_clock::now();
    }
}

void GameState::stopTimer() {
    updateRemainingTime();
    timeControl.lastMoveTime = std::nullopt;
}

void GameState::setTimeControl(int minutes, int incrementSeconds) {
    timeControl.initialMinutes = minutes;
    timeControl.incrementSeconds = incrementSeconds;
    timeControl.whiteTimeRemaining = std::chrono::minutes(minutes);
    timeControl.blackTimeRemaining = std::chrono::minutes(minutes);
    timeControl.lastMoveTime = std::nullopt;
}

int GameState::getRemainingTime(bool white) const {
    auto remaining = white ? timeControl.whiteTimeRemaining : timeControl.blackTimeRemaining;
    
    if (timeControl.lastMoveTime && (white == whiteTurn)) {
        auto elapsed = std::chrono::steady_clock::now() - *timeControl.lastMoveTime;
        remaining -= std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
    }
    
    return static_cast<int>(remaining.count());
}

std::string GameState::serialize() const {
    std::stringstream ss;
    ss << (whiteTurn ? "1" : "0") << ","
       << static_cast<int>(result) << ","
       << fullMoveNumber << ","
       << halfMoveClock << ","
       << timeControl.initialMinutes << ","
       << timeControl.incrementSeconds << ","
       << timeControl.whiteTimeRemaining.count() << ","
       << timeControl.blackTimeRemaining.count();
    return ss.str();
}

GameState GameState::deserialize(const std::string& data) {
    GameState state;
    std::stringstream ss(data);
    std::string item;
    
    std::getline(ss, item, ',');
    state.whiteTurn = (item == "1");
    
    std::getline(ss, item, ',');
    state.result = static_cast<GameResult>(std::stoi(item));
    
    std::getline(ss, item, ',');
    state.fullMoveNumber = std::stoi(item);
    
    std::getline(ss, item, ',');
    state.halfMoveClock = std::stoi(item);
    
    std::getline(ss, item, ',');
    state.timeControl.initialMinutes = std::stoi(item);
    
    std::getline(ss, item, ',');
    state.timeControl.incrementSeconds = std::stoi(item);
    
    std::getline(ss, item, ',');
    state.timeControl.whiteTimeRemaining = std::chrono::milliseconds(std::stoll(item));
    
    std::getline(ss, item, ',');
    state.timeControl.blackTimeRemaining = std::chrono::milliseconds(std::stoll(item));
    
    return state;
}

void GameState::reset() {
    whiteTurn = true;
    result = GameResult::IN_PROGRESS;
    fullMoveNumber = 1;
    halfMoveClock = 0;
    
    auto initialTime = std::chrono::minutes(timeControl.initialMinutes);
    timeControl.whiteTimeRemaining = initialTime;
    timeControl.blackTimeRemaining = initialTime;
    timeControl.lastMoveTime = std::nullopt;
}

void GameState::updateRemainingTime() {
    if (!timeControl.lastMoveTime) {
        return;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - *timeControl.lastMoveTime);
    
    if (whiteTurn) {
        timeControl.whiteTimeRemaining -= elapsed;
        if (timeControl.whiteTimeRemaining < std::chrono::milliseconds(0)) {
            timeControl.whiteTimeRemaining = std::chrono::milliseconds(0);
            if (result == GameResult::IN_PROGRESS) {
                result = GameResult::BLACK_WINS;
            }
        }
    } else {
        timeControl.blackTimeRemaining -= elapsed;
        if (timeControl.blackTimeRemaining < std::chrono::milliseconds(0)) {
            timeControl.blackTimeRemaining = std::chrono::milliseconds(0);
            if (result == GameResult::IN_PROGRESS) {
                result = GameResult::WHITE_WINS;
            }
        }
    }
}

} 