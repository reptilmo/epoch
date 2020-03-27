
#include "VulkanRenderTarget.h"

namespace Epoch {

    VulkanRenderTarget::VulkanRenderTarget( VulkanImage* image, const char* name, const bool destroy ) : VulkanTexture( image, name, destroy ) {

    }

    VulkanRenderTarget::~VulkanRenderTarget() {

    }
}