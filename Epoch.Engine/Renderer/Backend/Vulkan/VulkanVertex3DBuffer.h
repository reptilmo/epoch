#pragma once

#include "VulkanBuffer.h"
#include "../../Vertex3D.h"

namespace Epoch {

    class VulkanDevice;

    class VulkanVertex3DBuffer : public VulkanBuffer<Vertex3D> {
    public:
        VulkanVertex3DBuffer( VulkanDevice* device );
        ~VulkanVertex3DBuffer();

        void SetData( std::vector<Vertex3D> data );
    };
}