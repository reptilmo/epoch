
#include <stdarg.h>
#include <stdio.h>
#include "String/TString.h"

#include "Defines.h"

#define LOGGER_DEFAULT_FORMAT_STRING "%s%s\n"

#include "Logger.h"

namespace Epoch {

    static void writeLog( const char* message ) {
        printf( message );
    }

    void Logger::Trace( const char* message, ... ) {
        va_list args;
        va_start( args, message );
        TString formatted;
        tvsprintf( formatted, message, args );
        writeLog( TString::Format( LOGGER_DEFAULT_FORMAT_STRING, "[TRACE]: ", formatted.CStr() ).CStr() );
        //writeLog( "[TRACE]: ", message, args );
        va_end( args );
    }

    void Logger::Log( const char* message, ... ) {
        va_list args;
        va_start( args, message );
        TString formatted;
        tvsprintf( formatted, message, args );
        writeLog( TString::Format( LOGGER_DEFAULT_FORMAT_STRING, "[LOG]: ", formatted.CStr() ).CStr() );
        //writeLog( "[LOG]: ", message, args );
        va_end( args );
    }

    void Logger::Warn( const char* message, ... ) {
        va_list args;
        va_start( args, message );
        TString formatted;
        tvsprintf( formatted, message, args );
        writeLog( TString::Format( LOGGER_DEFAULT_FORMAT_STRING, "[WARN]: ", formatted.CStr() ).CStr() );
        //writeLog( "[WARN]: ", message, args );
        va_end( args );
    }

    void Logger::Error( const char* message, ... ) {
        va_list args;
        va_start( args, message );
        TString formatted;
        tvsprintf( formatted, message, args );
        writeLog( TString::Format( LOGGER_DEFAULT_FORMAT_STRING, "[ERROR]: ", formatted.CStr() ).CStr() );
        //writeLog( "[ERROR]: ", message, args );
        va_end( args );
    }

    void Logger::Fatal( const char* message, ... ) {
        va_list args;
        va_start( args, message );
        TString formatted;
        tvsprintf( formatted, message, args );
        writeLog( TString::Format( LOGGER_DEFAULT_FORMAT_STRING, "[FATAL]: ", formatted.CStr() ).CStr() );
        //writeLog( "[FATAL]: ", message, args );
        va_end( args );

        ASSERT_MSG( false, message );
    }
}