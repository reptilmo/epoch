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
        U32 Format;

        // per-pixel samples. Default: 1.
        U8 SampleCount = 1;
        RenderTargetLoadOperation LoadOperation;
        RenderTargetStoreOperation StoreOperation;
        RenderTargetLoadOperation StencilLoadOperation;
        RenderTargetStoreOperation StencilStoreOperation;
        U32 InputLayout;
        U32 OutputLayout;
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