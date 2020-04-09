#pragma once

namespace Epoch {

    class ICommandBuffer;
    class IUniformBuffer;
    class BaseMaterial;

    enum ShaderType {
        Vertex,
        Fragment,
        Compute,
        Geometry
    };

    /**
     * Represents a shader in the system. These are used by the renderer to draw
     * geometry using effects provided by the shader.
     */
    class IShader {
    public:
        /**
         * Indicates if this shader has a vertex stage.
         */
        virtual const bool HasVertexStage() const = 0;

        /**
         * Indicates if this shader has a fragment stage.
         */
        virtual const bool HasFragmentStage() const = 0;

        /**
         * Indicates if this shader has a geometry stage.
         */
        virtual const bool HasGeometryStage() const = 0;

        /**
         * Indicates if this shader has a compute stage.
         */
        virtual const bool HasComputeStage() const = 0;

        /**
         * Resets all descriptors in this shader. Typically done at the start of a frame before any binding is done.
         *
         * @param frameIndex The current index of the frame (or swapchain image) being drawn to.
         */
        virtual void ResetDescriptors( const U32 frameIndex ) = 0;

        /**
         * Updates the descriptors for the given frame and object indices.
         * 
         * @param commandBuffer The commandBuffer currently being recorded to.
         * @param frameIndex The current index of the frame (or swapchain image) being drawn to.
         * @param objectIndex The current list index of the object being rendered.
         * @param uniformBuffer The uniform buffer to be bound.
         * @param material A pointer to the material whose properties should be bound.
         */
        virtual void UpdateDescriptor( ICommandBuffer* commandBuffer, const U32 frameIndex, const U32 objectIndex, IUniformBuffer* uniformBuffer, BaseMaterial* material ) = 0;
    };
}