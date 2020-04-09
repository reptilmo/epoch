#pragma once

namespace Epoch {

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
    };
}