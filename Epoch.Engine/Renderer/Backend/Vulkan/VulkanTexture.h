#pragma once

#include "../../../Resources/ITexture.h"

namespace Epoch {

    class VulkanImage;
    class VulkanRenderer;

    class VulkanTexture : public ITexture {
    public:
        VulkanTexture( VulkanRenderer* renderer, const char* name, const char* path );
        ~VulkanTexture();

        const char* GetName() const { return _name; }

        VulkanImage* GetImage() { return _textureImage; }

    private:
        VulkanRenderer* _renderer;
        const char* _name;
        VulkanImage* _textureImage;
    };
}