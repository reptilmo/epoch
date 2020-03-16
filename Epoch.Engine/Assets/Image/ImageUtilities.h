#pragma once

#include "../../Types.h"

namespace Epoch {

    class ImageUtilities {
    public:

        /**
         * Loads the image at the provided path. The caller is responsible for freeing this data.
         * 
         * @param path The relative path to the image.
         * @param width A pointer to an integer to which the loaded image width is assigned.
         * @param height A pointer to an integer to which the loaded image height is assigned.
         * @param componentCount A pointer to an integer to which the loaded image number of components (RGBA) is assigned.
         * 
         * @return An array of image bytes. The caller is responsible for freeing this data.
         */
        static byte* LoadImage( const char* path, I32* width, I32* height, I32* componentCount );
    };

}