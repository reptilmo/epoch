#pragma once

#include <vulkan/vulkan.h>

#include "../../../String/TString.h"
#include "../../../Events/IEventHandler.h"
#include "../../IShader.h"
#include "../../../Math/Matrix4x4.h"

#include "../../UniformObject.h"

#define BUILTIN_SHADER_NAME_UNLIT "Builtin.UnlitShader"

#define VULKAN_MAX_DESC_SETS 16384
#define VULKAN_MAX_IMAGE_SAMPLERS 12384
#define VULKAN_MAX_UNIFORM_BUFFERS 8192

namespace Epoch {

    struct Extent2D;
    class ICommandBuffer;
    class IUniformBuffer;
    class VulkanDevice;
    class VulkanImage;
    class VulkanTextureSampler;
    class VulkanGraphicsPipeline;
    class VulkanUniformBuffer;

    class VulkanShaderModule {
    public:
        VulkanShaderModule( VulkanDevice* device, const char* name, ShaderType type );
        ~VulkanShaderModule();

        VkShaderModule GetHandle() { return _handle; }
        VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() { return _shaderStageCreateInfo; }
    private:
        const char* _name;
        VulkanDevice* _device;
        VkShaderModule _handle;
        VkPipelineShaderStageCreateInfo _shaderStageCreateInfo;
    };

    /**
     * Represents a Vulkan-specific shader, complete with the appropriate stage modules.
     */
    class VulkanShader : public IShader, public IEventHandler {
    public:
        VulkanShader( VulkanDevice* device, const char* name, const U32 imageCount, const TString& renderPassName, const bool hasVertex, const bool hasFragment, const bool hasGeometry, const bool hasCompute );
        virtual ~VulkanShader();

        void OnEvent( const Event* event ) override;

        virtual void SetGlobalUniform( ICommandBuffer* commandBuffer, const GlobalUniformObject& uniform, const U32 frameIndex ) override;
        virtual void SetModel( const Matrix4x4& model ) override;

        virtual void ResetDescriptors( const U32 frameIndex ) override;
        virtual void BindPipeline( ICommandBuffer* commandBuffer ) override;
        virtual void UpdateDescriptor( ICommandBuffer* commandBuffer, const U32 frameIndex, const U32 objectIndex, BaseMaterial* material ) override = 0;
        virtual void BindDescriptor( ICommandBuffer* commandBuffer, const U32 frameIndex, const U32 objectIndex ) override;

        const bool HasVertexStage() const override { return _vertexModule != nullptr; }
        const bool HasFragmentStage() const override { return _fragmentModule != nullptr; }
        const bool HasGeometryStage() const override { return _geometryModule != nullptr; }
        const bool HasComputeStage() const override { return _computeModule != nullptr; }

        VulkanShaderModule* GetVertexStage() { return _vertexModule; }
        VulkanShaderModule* GetFragmentStage() { return _fragmentModule; }
        VulkanShaderModule* GetGeometryStage() { return _geometryModule; }
        VulkanShaderModule* GetComputeStage() { return _computeModule; }

        const VulkanGraphicsPipeline* GetPipeline() const { return _graphicsPipeline; }

    protected:
        virtual void intialize();
        virtual void destroyPipeline();

        virtual void createDescriptorSetLayout() = 0;
        virtual void createDescriptorPools() = 0;
        virtual void createTextureSamplers() = 0;
        virtual void createPipeline( const Extent2D& extent ) = 0;
        virtual void createUniformBuffers() = 0;

    protected:
        bool _needsReset = true;
        TString _renderPassName;

        // Global descriptors (one pool/set per frame)
        U32 _globalDescriptorPoolCount;
        VkDescriptorPool* _globalDescriptorPools;
        VkDescriptorSetLayout _globalDescriptorSetLayout;
        U32 _globalDescriptorSetFrameCount;
        VkDescriptorSet* _globalDescriptorSets; // one per frame

        // Object descriptors (one pool/frame, one set per frame per object)
        U32 _objectDescriptorPoolCount;
        VkDescriptorPool* _objectDescriptorPools;
        VkDescriptorSetLayout _objectDescriptorSetLayout;

        U32 _objectDescriptorSetFrameCount;
        U32 _objectDescriptorSetObjectCount;
        VkDescriptorSet** _objectDescriptorSets; // one per frame, per object

        U32 _textureSamplerCount;
        VulkanTextureSampler** _textureSamplers;

        VulkanGraphicsPipeline* _graphicsPipeline;

        // 1 per swap chain image
        U32 _globalUBOCount;
        U32 _objectUBOCount;
        VulkanBuffer<Epoch::GlobalUniformObject>** _globalUniformBuffers;
        VulkanBuffer<Epoch::UnlitUniformObject>** _objectUniformBuffers;
        UnlitUniformObject* _objectUbos;
        GlobalUniformObject* _globalUbos;

        U32 _imageCount;
        VulkanDevice* _device;
        VulkanShaderModule* _vertexModule = nullptr;
        VulkanShaderModule* _fragmentModule = nullptr;
        VulkanShaderModule* _geometryModule = nullptr;
        VulkanShaderModule* _computeModule = nullptr;

        Matrix4x4 _model;
    };

    /**
     * A flat, unlit shader.
     */
    class VulkanUnlitShader : public VulkanShader {
    public:
        VulkanUnlitShader( VulkanDevice* device, const U32 imageCount, const TString& renderPassName );
        

        virtual void UpdateDescriptor( ICommandBuffer* commandBuffer, const U32 frameIndex, const U32 objectIndex, BaseMaterial* material );
    protected:
        virtual void createDescriptorSetLayout() override;
        virtual void createDescriptorPools() override;
        virtual void createTextureSamplers() override;
        virtual void createPipeline( const Extent2D& extent ) override;
        virtual void createUniformBuffers() override;
    };
}
