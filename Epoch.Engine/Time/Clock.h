#pragma once

#include "../Types.h"

namespace Epoch {

    /**
     * A simple class to be used for managing time in the system.
     */
    class Clock {
    public:

        /**
         * Creates a new clock with a granularity of milliseconds. TODO: Allow for configurable granularity.
         *
         * @param autoStart Indicates if this clock should start automatically on creation.
         */
        Clock( const bool autoStart );

        /**
         * Starts this clock. Calls Reset unless false is passed.
         *
         * @param reset Calls Reset() on this clock if true. Default: true.
         */
        void Start( const bool reset = true );

        /**
         * Stops this clock. Does not reset. GetTime will return the time the clock was stopped.
         */
        void Stop();

        /**
         * Resets this clock. Can be done while running.
         */
        void Reset();

        /**
         * Returns the total number of milliseconds this clock has been running.
         */
        const U64 GetTime();

        /**
         * Indicates if this clock is currently running.
         */
        const bool IsRunning() const { return _isRunning; }

    private:

        /**
         * Returns the abosolute time since epoch.
         */
        const U64 getAbsoluteTime() const;
    private:
        bool _isRunning = false;
        U64 _startTime;
        U64 _lastTime;
    };
}