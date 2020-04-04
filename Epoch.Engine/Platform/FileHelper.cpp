
#include <fstream>

#include "../Logger.h"
#include "../Types.h"

#include "FileHelper.h"

namespace Epoch {

    const char* FileHelper::ReadFileBinaryToArray( const char* path, U64* fileSize ) {

        std::ifstream file( path, std::ios::ate | std::ios::binary );
        if( !file.is_open() ) {
            Logger::Fatal( "FileHelper::ReadFileBinaryToArray unable to open file: %s", path );
        }

        *fileSize = (U64)file.tellg();
        char* fileBuffer = (char*)malloc( *fileSize );
        file.seekg( 0 );
        file.read( fileBuffer, *fileSize );
        file.close();

        return fileBuffer;
    }
}