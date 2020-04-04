#include <string>
#include <map>

#include "../../../Logger.h"
#include "../../RenderPassData.h"

#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanRenderPassManager.h"

namespace Epoch {

    std::map<std::string, VulkanRenderPass*> _renderpasses;

    void VulkanRenderPassManager::CreateRenderPass( VulkanDevice* device, const RenderPassData renderPassData ) {
        _renderpasses.emplace( renderPassData.Name, new VulkanRenderPass( device, renderPassData ) );
    }

    VulkanRenderPass* VulkanRenderPassManager::GetRenderPass( const char* renderPassName ) {
        auto it = _renderpasses.find( renderPassName );
        if( it != _renderpasses.end() ) {
            return it->second;
        }

        Logger::Fatal( "VulkanRenderPassManager::GetRenderPass() - Unable to find render pass: '%s'. Are you sure you created it?", renderPassName );
        return nullptr;
    }

    void VulkanRenderPassManager::DestroyRenderPass( VulkanDevice* device, const char* renderPassName ) {
        auto it = _renderpasses.find( renderPassName );
        if( it != _renderpasses.end() ) {
            delete it->second;
            _renderpasses.erase( renderPassName );
        } else {

            // Technically this doesn't hurt anything... but the user should be slapped for it.
            Logger::Warn( "VulkanRenderPassManager::DestroyRenderPass() - Unable to find destroy pass: '%s'. Are you sure you created it?", renderPassName );
        }
    }
}