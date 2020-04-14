#pragma once

#include "../Types.h"
#include <vector>
#include <string>

namespace Epoch {

    class IRenderTarget;

    enum class RenderTargetLoadOperation {

        // Clear contents
        CLEAR,
        LOAD,
        DONT_CARE
    };

    enum class RenderTargetStoreOperation {

        // Preserve after rendering
        STORE,
        DONT_CARE
    };

    struct RenderTargetOptions {

        // Image format.
        U32 Format = 0;

        // per-pixel samples. Default: 1.
        U8 SampleCount = 1;
        RenderTargetLoadOperation LoadOperation = RenderTargetLoadOperation::DONT_CARE;
        RenderTargetStoreOperation StoreOperation = RenderTargetStoreOperation::DONT_CARE;
        RenderTargetLoadOperation StencilLoadOperation = RenderTargetLoadOperation::DONT_CARE;
        RenderTargetStoreOperation StencilStoreOperation = RenderTargetStoreOperation::DONT_CARE;
        U32 InputLayout = 0;
        U32 OutputLayout = 0;
    };

    struct RenderPassData {

        std::string Name;

        // Used for the color buffer.
        std::vector<RenderTargetOptions> ColorRenderTargetOptions;

        // Depth/stencil buffer.
        bool HasDepthRenderTarget;
        RenderTargetOptions DepthRenderTargetOptions;

        // TODO: input, resolve, preserve attachments
        
    };
}