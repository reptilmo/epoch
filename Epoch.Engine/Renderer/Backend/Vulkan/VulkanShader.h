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

        std::vector<VkDescriptorPool> _objectDescriptorPools;
        VkDescriptorSetLayout _objectDescriptorSetLayout;
        std::vector<std::vector<VkDescriptorSet>> _objectDescriptorSets; // one per frame, per object

        std::vector<VkDescriptorPool> _globalDescriptorPools;
        VkDescriptorSetLayout _globalDescriptorSetLayout;
        std::vector<VkDescriptorSet> _globalDescriptorSets;
        
        std::vector<VulkanTextureSampler*> _textureSamplers;

        VulkanGraphicsPipeline* _graphicsPipeline;

        // 1 per swap chain image
        std::vector<VulkanBuffer<Epoch::GlobalUniformObject>*> _globalUniformBuffers;
        std::vector<VulkanBuffer<Epoch::UnlitUniformObject>*> _objectUniformBuffers;
        std::vector<UnlitUniformObject> _objectUbos;
        std::vector<GlobalUniformObject> _globalUbos;

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
