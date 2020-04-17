
#include <chrono>

#include "Clock.h"

namespace Epoch {

    Clock::Clock( const bool autoStart ) {

        _startTime = 0;
        _lastTime = 0;
        if( autoStart ) {
            Start();
        }
    }

    void Clock::Stop() {
        _isRunning = false;
        _lastTime = getAbsoluteTime();
    }

    void Clock::Reset() {
        _startTime = getAbsoluteTime();
        _lastTime = _startTime;
    }

    void Clock::Start( const bool reset ) {
        _isRunning = true;
        if( reset ) {
            Reset();
        }
    }

    const U64 Clock::GetTime() {
        _lastTime = getAbsoluteTime();
        return _lastTime - _startTime;
    }

    const U64 Clock::getAbsoluteTime() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() ).count();
    }
}