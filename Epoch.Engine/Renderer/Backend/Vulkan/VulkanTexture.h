#pragma once

#include "../../../Resources/ITexture.h"

namespace Epoch {

    class VulkanImage;
    class VulkanDevice;

    class VulkanTexture : public ITexture {
    public:
        VulkanTexture( VulkanDevice* device, const char* name, const char* path );
        ~VulkanTexture();

        const char* GetName() const { return _name; }

        VulkanImage* GetImage() { return _textureImage; }

    private:
        VulkanDevice* _device;
        const char* _name;
        VulkanImage* _textureImage;
    };
}