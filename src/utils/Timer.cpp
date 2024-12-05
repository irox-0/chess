#include "Timer.hpp"
#include <iomanip>
#include <sstream>

Timer::Timer(int minutes) 
    : totalTime(minutes)
    , elapsedTime(std::chrono::steady_clock::duration::zero())
    , running(false) {
}

void Timer::start() {
    if (!running) {
        startTime = std::chrono::steady_clock::now();
        running = true;
    }
}

void Timer::stop() {
    if (running) {
        elapsedTime += std::chrono::steady_clock::now() - startTime;
        running = false;
    }
}

void Timer::reset() {
    elapsedTime = std::chrono::steady_clock::duration::zero();
    running = false;
}

bool Timer::isRunning() const {
    return running;
}

bool Timer::isTimeUp() const {
    auto totalElapsed = elapsedTime;
    if (running) {
        totalElapsed += std::chrono::steady_clock::now() - startTime;
    }
    return totalElapsed >= totalTime;
}

void Timer::update() {
    if (running) {
        auto currentTime = std::chrono::steady_clock::now();
        elapsedTime += currentTime - startTime;
        startTime = currentTime;
    }
}

std::string Timer::getTimeString() const {
    auto totalElapsed = elapsedTime;
    if (running) {
        totalElapsed += std::chrono::steady_clock::now() - startTime;
    }
    
    auto remainingTime = totalTime - totalElapsed;
    if (remainingTime < std::chrono::steady_clock::duration::zero()) {
        remainingTime = std::chrono::steady_clock::duration::zero();
    }
    
    return formatTime(remainingTime);
}

std::string Timer::formatTime(std::chrono::steady_clock::duration remainingTime) const {
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(remainingTime);
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(remainingTime - minutes);
    
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << minutes.count() << ":"
       << std::setfill('0') << std::setw(2) << seconds.count();
    return ss.str();
}