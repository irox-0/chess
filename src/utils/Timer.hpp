#pragma once
#include <chrono>
#include <string>

class Timer {
public:
    Timer(int minutes);
    
    void start();
    void stop();
    void reset();
    
    bool isRunning() const;
    bool isTimeUp() const;
    
    void update();
    std::string getTimeString() const;
    
private:
    std::chrono::minutes totalTime;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::duration elapsedTime;
    bool running;
    
    std::string formatTime(std::chrono::steady_clock::duration remainingTime) const;
};