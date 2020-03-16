
#include "ImageUtilities.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../External/stb_image/stb_image.h"

namespace Epoch {

    byte* ImageUtilities::LoadImage( const char* path, I32* width, I32* height, I32* componentCount ) {

        // NOTE: may replace this with a custom loader at some point.
        byte* pixelData = stbi_load( path, width, height, componentCount, STBI_rgb_alpha );

        return pixelData;
    }
}