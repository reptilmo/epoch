#pragma once

namespace Epoch {

    struct GlobalUniformObject;

    class ICommandBuffer;
    class IUniformBuffer;
    class BaseMaterial;
    class Matrix4x4;

    enum class ShaderType {
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

        virtual ~IShader() {}

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
         * Should always be done once before any object is drawn.
         *
         * @param frameIndex The current index of the frame (or swapchain image) being drawn to.
         */
        virtual void ResetDescriptors( const U32 frameIndex ) = 0;

        /**
         * Binds the internal graphics pipeline to the provided command buffer. Should be called once per object, per frame.
         *
         * @param commandBuffer The command buffer to which the pipeline should be bound.
         */
        virtual void BindPipeline( ICommandBuffer* commandBuffer ) = 0;

        /**
         * Sets global uniform information on this shader (such as view and projection matrices).
         *
         * @param commandBuffer The command buffer to bind to.
         * @param uniform The uniform data to be set.
         * @param frameIndex The current index of the frame (or swapchain image) being drawn to.
         */
        virtual void SetGlobalUniform( ICommandBuffer* commandBuffer, const GlobalUniformObject& uniform, const U32 frameIndex ) = 0;

        /**
         * Sets the model matrix on this shader.
         *
         * @param model A reference to the model matrix to be used.
         */
        virtual void SetModel( const Matrix4x4& model ) = 0;

        /**
         * Updates the descriptors for the given frame and object indices. Should be done after BindPipeline.
         * Should be called once per object, per frame.
         *
         * @param commandBuffer The commandBuffer currently being recorded to.
         * @param frameIndex The current index of the frame (or swapchain image) being drawn to.
         * @param objectIndex The current list index of the object being rendered.
         * @param material A pointer to the material whose properties should be bound.
         */
        virtual void UpdateDescriptor( ICommandBuffer* commandBuffer, const U32 frameIndex, const U32 objectIndex, BaseMaterial* material ) = 0;

        /**
         * Binds the internal descriptor to the provided pipeline. Should be completed after UpdateDescriptor.
         * Should be called once per object, per frame.
         *
         * @param commandBuffer The commandBuffer currently being recorded to.
         * @param frameIndex The current index of the frame (or swapchain image) being drawn to.
         * @param objectIndex The current list index of the object being rendered.
         */
        virtual void BindDescriptor( ICommandBuffer* commandBuffer, const U32 frameIndex, const U32 objectIndex ) = 0;
    };
}