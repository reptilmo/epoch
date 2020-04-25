#pragma once

#include "Defines.h"

namespace Epoch {

    /**
     * Represents the logger system for this engine.
     */
    class EPOCH_API Logger final {
    public:

        /**
         * Writes the provided message at the Trace output level. This is the most
         * verbose output level.
         *
         * @param message The messsage to be written.
         */
        static void Trace( const char* message, ... );

        /**
         * Writes the provided message at the standard Log output level. This is the most
         * common output level.
         *
         * @param message The messsage to be written.
         */
        static void Log( const char* message, ... );

        /**
         * Writes the provided message at the Warning output level. This should be used when
         * abnormal operation can occur, but the application can still continue.
         *
         * @param message The messsage to be written.
         */
        static void Warn( const char* message, ... );

        /**
         * Writes the provided message at the Error output level. This should be used when
         * game- or engine-breaking events occur, but the application can still continue.
         *
         * @param message The messsage to be written.
         */
        static void Error( const char* message, ... );

        /**
         * Writes the provided message at the Warning output level. This should be used when
         * an issue is encountered that the application cannot handle. Calling this crashes
         * the application after logging the message.
         *
         * @param message The messsage to be written.
         */
        static void Fatal( const char* message, ... );
    };
}