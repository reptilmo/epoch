#include "../Logger.h"

#include "FileHandle.h"


namespace Epoch {

    FileHandle::FileHandle( const TString& filePath, const bool isBinary ) {
        _filePath = filePath;
        _isBinary = isBinary;
    }

    FileHandle::~FileHandle() {
        _filePath.Clear();
        if( checkIfOpen() ) {
            Close();
        }
    }

    const bool FileHandle::TryOpen( const FileMode fileMode ) {
        if( checkIfOpen() ) {
            return true;
        }

        I64 mode = std::ios::binary;
        if( fileMode == FileMode::FILE_MODE_INPUT || fileMode == FileMode::FILE_MODE_INPUT_OUTPUT ) {
            mode |= std::ios::in;
        }
        if( fileMode == FileMode::FILE_MODE_OUTPUT || fileMode == FileMode::FILE_MODE_INPUT_OUTPUT ) {
            mode |= std::ios::out;
        }

        _handle = new std::fstream( _filePath.CStr(), ( std::ios::openmode )mode );
        if( _handle->fail() ) {
            Logger::Warn( TString::Format( "Unable to obtain a FileHandle to file %s", _filePath.CStr() ) );
            return false;
        }

        // Grab the file size
        _handle->seekg( 0, std::ios::end );
        _fileSize = _handle->tellg();
        _handle->seekg( 0 );

        return true;
    }
    void FileHandle::Close() {
        if( checkIfOpen() ) {
            _handle->close();
            delete _handle;
            _handle = nullptr;
        }
    }

    const bool FileHandle::ReadString( TString* outValue, const U64 stringSize ) {
        if( _handle && _handle->good() ) {
            if( stringSize > 0 ) {
                outValue->Fill( ' ', (U32)stringSize );
                _handle->read( outValue->Data(), sizeof( char ) * stringSize );
            }
            return true;
        }
        return false;
    }

    const bool FileHandle::ReadLine( TString* outValue ) {
        outValue->Clear();
        if( checkIfOpen() ) {

            // Read until a newline is found, then return;
            char c;
            while( _handle->get( c ) ) {
                if( c == '\n' ) {
                    return true;
                } else {
                    outValue->Append( c );
                }
            }
            return true;
        }

        return false;
    }

    const bool FileHandle::WriteString( const TString& inValue ) {
        if( _handle && _handle->good() ) {
            if( inValue.Length() > 0 ) {
                _handle->write( (char*)inValue.CStr(), sizeof( char ) * inValue.Length() );
            }
            return true;
        }
        return false;
    }

    const bool FileHandle::WriteLine( const TString& text ) {
        if( checkIfOpen() ) {
            _handle->write( text.CStr(), sizeof( char ) * text.Length() );
            _handle->write( (char*)'\n', 1 );
            return true;
        }

        return false;
    }

    const bool FileHandle::Seek( const I64 position ) {
        if( _handle && _handle->good() ) {
            if( position >= (I64)_fileSize ) {
                Logger::Warn( TString::Format( "FileHandle::Seek - Unable to position %d in a file which is %d bytes.", position, _fileSize ) );
                return false;
            }
            _handle->seekg( 0, ( std::ios_base::seekdir )position );
            return true;
        }
        return false;
    }

    const bool FileHandle::SeekBeginning() {
        if( _handle && _handle->good() ) {
            _handle->seekg( 0 );
            return true;
        } else {
            Logger::Warn( "FileHandle::SeekBeginning - Unable to the beginnind of a file which is not open." );
            return false;
        }
    }

    const bool FileHandle::SeekEnd() {
        if( _handle && _handle->good() ) {
            _handle->seekg( _fileSize - 1 );
            return true;
        } else {
            Logger::Warn( "FileHandle::SeekBeginning - Unable to the beginnind of a file which is not open." );
            return false;
        }
    }

    const bool FileHandle::IsEndOfFile() const {
        if( _handle && _handle->good() ) {
            return _handle->eof();
        }
        return true;
    }

    const U64 FileHandle::GetPosition() const {
        if( _handle && _handle->good() ) {
            return _handle->tellg();
        }
        return 0;
    }

    const bool FileHandle::checkIfOpen() const {
        if( _handle && _handle->good() ) {
            return _handle->is_open();
        }
        return false;
    }
}