#pragma once

#include "VulkanTexture.h"

namespace Epoch {

    class VulkanRenderTarget :public VulkanTexture {
        VulkanRenderTarget( VulkanImage* image, const char* name, const bool destroy );
        ~VulkanRenderTarget();
    };
}