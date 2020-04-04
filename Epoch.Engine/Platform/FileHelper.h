#pragma once

#include "../Types.h"

namespace Epoch {

    class FileHelper final {
    public:

        /**
         * Reads the contents of the file provided in path into a dynamic array. The caller
         * is responsible for cleaning up the array.
         * 
         * @param path The full path to the file to be read.
         * @param fileSize A pointer to a number which is populated with the size of the file.
         * 
         * @returns A dynamic array of the file's contents.
         */
        static const char* ReadFileBinaryToArray( const char* path, U64* fileSize );

    private:
        // This class is a singleton, so hide these.
        FileHelper() {}
        ~FileHelper() {}

    };
}