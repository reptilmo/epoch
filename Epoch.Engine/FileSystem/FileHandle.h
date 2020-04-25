#pragma once

#include "../Types.h"
#include "../String/TString.h"
#include <fstream>

namespace Epoch {

    /**
     * File modes to be used with the FileSystem.
     */
    enum FileMode {

        /**
         * File should be opened in read-only mode.
         */
        FILE_MODE_INPUT,

        /**
         * File should be opened in write-only mode.
         */
         FILE_MODE_OUTPUT,

         /**
          * File should be opened in read-write mode.
          */
          FILE_MODE_INPUT_OUTPUT
    };

    class FileHandle {
    public:
        FileHandle( const TString& filePath, const bool isBinary );
        ~FileHandle();

        const bool TryOpen( const FileMode fileMode );
        void Close();

        template<typename T>
        const bool Read( T* outValue );

        template<typename T>
        const bool ReadArray( T** outValue, const U64 length );

        const bool ReadString( TString* outValue, const U64 stringSize );

        const bool ReadLine( TString* outValue );

        template<typename T>
        const bool Write( const T outValue );

        template<typename T>
        const bool WriteArray( const T* outValue, const U64 length );

        const bool WriteString( const TString& inValue );

        const bool WriteLine( const TString& text );

        const bool Seek( const I64 position );
        const bool SeekBeginning();
        const bool SeekEnd();
        const bool IsEndOfFile() const;

        const U64 GetPosition() const;

        const U64 GetSize() const { return _fileSize; }
        const bool IsOpen() const { return checkIfOpen(); }
    private:
        const bool checkIfOpen() const;
    private:
        bool _isBinary;
        U64 _fileSize = 0;
        TString _filePath;
        std::fstream* _handle;
    };

    template<typename T>
    const bool FileHandle::Read( T* outValue ) {
        if( _handle && _handle->good() ) {
            _handle->read( (char*)outValue, sizeof( *outValue ) );
            return true;
        }
        return false;
    }

    template<typename T>
    const bool FileHandle::ReadArray( T** outValue, const U64 length ) {
        if( _handle && _handle->good() ) {
            *outValue = new T[length];
            _handle->read( (char*)outValue[0], sizeof( outValue[0] ) * length );
            U64 pos = _handle->tellg();
            return true;
        }
        return false;
    }

    template<typename T>
    const bool FileHandle::Write( const T outValue ) {
        if( _handle && _handle->good() ) {
            _handle->write( (char*)&outValue, sizeof( T ) );
            return true;
        }
        return false;
    }

    template<typename T>
    const bool FileHandle::WriteArray( const T* outValue, const U64 length ) {
        if( _handle && _handle->good() ) {
            _handle->write( (char*)outValue, sizeof( outValue[0] ) * length );
            return true;
        }
        return false;
    }
}