#pragma once

#include "VulkanBuffer.h"
#include "../../Vertex3D.h"

namespace Epoch {

    class VulkanRenderer;

    class VulkanVertex3DBuffer : public VulkanBuffer<Vertex3D> {
    public:
        VulkanVertex3DBuffer( VulkanRenderer* renderer );
        ~VulkanVertex3DBuffer();

        void SetData( std::vector<Vertex3D> data );
    };
}