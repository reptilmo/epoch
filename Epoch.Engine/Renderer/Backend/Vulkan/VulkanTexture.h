#pragma once

#include "../../../Resources/ITexture.h"
#include "../../../String/TString.h"

namespace Epoch {

    class IImage;
    class VulkanImage;
    class VulkanDevice;

    class VulkanTexture : public ITexture {
    public:
        VulkanTexture( VulkanDevice* device, const char* name, const char* path );
        VulkanTexture( VulkanImage* image, const char* name, const bool destroy );
        virtual ~VulkanTexture();

        const char* GetName() const { return _name.CStr(); }

        IImage* GetImage() { return (IImage*)_textureImage; }

    private:
        bool _destroy;
        VulkanDevice* _device;
        TString _name;
        VulkanImage* _textureImage;
    };
}