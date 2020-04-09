##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=epoch
ConfigurationName      :=Debug
WorkspacePath          :=C:/Development/epoch
ProjectPath            :=C:/Development/epoch
IntermediateDirectory  :=$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=travis
Date                   :=06/04/2020
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=link.exe  /nologo
SharedObjectLinkerName :=link.exe /DLL  /nologo
ObjectSuffix           :=.obj
DependSuffix           :=
PreprocessSuffix       :=
DebugSwitch            :=/Zi 
IncludeSwitch          :=/I
LibrarySwitch          := 
OutputSwitch           :=/OUT:
LibraryPathSwitch      :=/LIBPATH:
PreprocessorSwitch     :=/D
SourceSwitch           :=
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=/Fo
ArchiveOutputSwitch    :=/OUT:
PreprocessOnlySwitch   :=
ObjectsFileList        :="epoch.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=windres
LinkOptions            :=  -O0
IncludePath            := $(IncludeSwitch)""C:/Program Files/Microsoft Visual Studio 8/VC/include"" $(IncludeSwitch).  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)C:\VulkanSDK\1.2.131.2\Include\ 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                :=$(LibraryPathSwitch)""C:/Program Files/Microsoft Visual Studio 8/VC/lib""  $(LibraryPathSwitch). $(LibraryPathSwitch). $(LibraryPathSwitch)Debug 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := lib.exe  /nologo
CXX      := cl.exe /nologo /c
CC       := cl.exe /nologo /c
CXXFLAGS :=  -g -Wall $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := NASM


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanCommandBuffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanCommandPool.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanFence.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanIndexBuffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanInternalBuffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanPipeline.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanQueue.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanRenderPassManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanImage.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanShader.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanSwapchain.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanUniformBuffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanUtilities.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanVertex3DBuffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Platform_FileHelper.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Math_Transform.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanRenderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanDebugger.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanRenderPass.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_TextureCache.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/Epoch.Engine_Assets_Image_ImageUtilities.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine.Test_pch.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanTextureSampler.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine.Test_LinkedList.Test.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Platform_TGLFWWindow.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Logger.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Platform_Windows_WindowsApplication.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanTexture.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Events_Event.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/Epoch.Engine_Engine.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanRenderTarget.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_String_TName.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_String_StringUtilities.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Math_Vector4.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Math_Vector3.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_String_TString.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Math_Rotator.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Assets_StaticMesh_Loaders_OBJLoader.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanDevice.cpp$(ObjectSuffix) \
	

Objects1=$(IntermediateDirectory)/Epoch.Engine_Math_Quaternion.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Math_Matrix4x4.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Math_TMath.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Events_EventManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Frontend_RendererFrontEnd.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanSemaphore.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Platform_Windows_WindowsWindow.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Math_Vector2.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Assets_StaticMesh_StaticMesh.cpp$(ObjectSuffix) $(IntermediateDirectory)/Epoch.Engine_Platform_Platform.cpp$(ObjectSuffix) \
	



Objects=$(Objects0) $(Objects1) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	@echo $(Objects1) >> $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@$(MakeDirCommand) "$(ConfigurationName)"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "$(ConfigurationName)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanCommandBuffer.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanCommandBuffer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanCommandBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanCommandBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanCommandPool.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanCommandPool.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanCommandPool.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanCommandPool.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanFence.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanFence.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanFence.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanFence.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanIndexBuffer.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanIndexBuffer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanIndexBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanIndexBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanInternalBuffer.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanInternalBuffer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanInternalBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanInternalBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanPipeline.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanPipeline.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanPipeline.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanPipeline.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanQueue.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanQueue.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanQueue.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanQueue.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanRenderPassManager.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanRenderPassManager.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanRenderPassManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanRenderPassManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanImage.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanImage.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanImage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanImage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanShader.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanShader.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanShader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanShader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanSwapchain.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanSwapchain.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanSwapchain.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanSwapchain.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanUniformBuffer.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanUniformBuffer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanUniformBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanUniformBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanUtilities.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanUtilities.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanUtilities.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanUtilities.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanVertex3DBuffer.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanVertex3DBuffer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanVertex3DBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanVertex3DBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Platform_FileHelper.cpp$(ObjectSuffix): Epoch.Engine/Platform/FileHelper.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Platform/FileHelper.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Platform_FileHelper.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Math_Transform.cpp$(ObjectSuffix): Epoch.Engine/Math/Transform.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Math/Transform.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Math_Transform.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanRenderer.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanRenderer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanRenderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanRenderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanDebugger.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanDebugger.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanDebugger.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanDebugger.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanRenderPass.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanRenderPass.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanRenderPass.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanRenderPass.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_TextureCache.cpp$(ObjectSuffix): Epoch.Engine/Renderer/TextureCache.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/TextureCache.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_TextureCache.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Assets_Image_ImageUtilities.cpp$(ObjectSuffix): Epoch.Engine/Assets/Image/ImageUtilities.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Assets/Image/ImageUtilities.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Assets_Image_ImageUtilities.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_main.cpp$(ObjectSuffix): Epoch.Engine/main.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine.Test_pch.cpp$(ObjectSuffix): Epoch.Engine.Test/pch.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine.Test/pch.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine.Test_pch.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanTextureSampler.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanTextureSampler.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanTextureSampler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanTextureSampler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine.Test_LinkedList.Test.cpp$(ObjectSuffix): Epoch.Engine.Test/LinkedList.Test.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine.Test/LinkedList.Test.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine.Test_LinkedList.Test.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Platform_TGLFWWindow.cpp$(ObjectSuffix): Epoch.Engine/Platform/TGLFWWindow.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Platform/TGLFWWindow.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Platform_TGLFWWindow.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Logger.cpp$(ObjectSuffix): Epoch.Engine/Logger.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Logger.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Logger.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Platform_Windows_WindowsApplication.cpp$(ObjectSuffix): Epoch.Engine/Platform/Windows/WindowsApplication.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Platform/Windows/WindowsApplication.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Platform_Windows_WindowsApplication.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanTexture.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanTexture.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanTexture.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanTexture.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Events_Event.cpp$(ObjectSuffix): Epoch.Engine/Events/Event.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Events/Event.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Events_Event.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Engine.cpp$(ObjectSuffix): Epoch.Engine/Engine.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Engine.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Engine.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanRenderTarget.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanRenderTarget.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanRenderTarget.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanRenderTarget.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_String_TName.cpp$(ObjectSuffix): Epoch.Engine/String/TName.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/String/TName.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_String_TName.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_String_StringUtilities.cpp$(ObjectSuffix): Epoch.Engine/String/StringUtilities.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/String/StringUtilities.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_String_StringUtilities.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Math_Vector4.cpp$(ObjectSuffix): Epoch.Engine/Math/Vector4.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Math/Vector4.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Math_Vector4.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Math_Vector3.cpp$(ObjectSuffix): Epoch.Engine/Math/Vector3.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Math/Vector3.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Math_Vector3.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_String_TString.cpp$(ObjectSuffix): Epoch.Engine/String/TString.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/String/TString.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_String_TString.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Math_Rotator.cpp$(ObjectSuffix): Epoch.Engine/Math/Rotator.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Math/Rotator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Math_Rotator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Assets_StaticMesh_Loaders_OBJLoader.cpp$(ObjectSuffix): Epoch.Engine/Assets/StaticMesh/Loaders/OBJLoader.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Assets/StaticMesh/Loaders/OBJLoader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Assets_StaticMesh_Loaders_OBJLoader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanDevice.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanDevice.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanDevice.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanDevice.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Math_Quaternion.cpp$(ObjectSuffix): Epoch.Engine/Math/Quaternion.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Math/Quaternion.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Math_Quaternion.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Math_Matrix4x4.cpp$(ObjectSuffix): Epoch.Engine/Math/Matrix4x4.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Math/Matrix4x4.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Math_Matrix4x4.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Math_TMath.cpp$(ObjectSuffix): Epoch.Engine/Math/TMath.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Math/TMath.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Math_TMath.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Events_EventManager.cpp$(ObjectSuffix): Epoch.Engine/Events/EventManager.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Events/EventManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Events_EventManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Frontend_RendererFrontEnd.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Frontend/RendererFrontEnd.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Frontend/RendererFrontEnd.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Frontend_RendererFrontEnd.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanSemaphore.cpp$(ObjectSuffix): Epoch.Engine/Renderer/Backend/Vulkan/VulkanSemaphore.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Renderer/Backend/Vulkan/VulkanSemaphore.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Renderer_Backend_Vulkan_VulkanSemaphore.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Platform_Windows_WindowsWindow.cpp$(ObjectSuffix): Epoch.Engine/Platform/Windows/WindowsWindow.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Platform/Windows/WindowsWindow.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Platform_Windows_WindowsWindow.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Math_Vector2.cpp$(ObjectSuffix): Epoch.Engine/Math/Vector2.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Math/Vector2.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Math_Vector2.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Assets_StaticMesh_StaticMesh.cpp$(ObjectSuffix): Epoch.Engine/Assets/StaticMesh/StaticMesh.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Assets/StaticMesh/StaticMesh.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Assets_StaticMesh_StaticMesh.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Epoch.Engine_Platform_Platform.cpp$(ObjectSuffix): Epoch.Engine/Platform/Platform.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Development/epoch/Epoch.Engine/Platform/Platform.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Epoch.Engine_Platform_Platform.cpp$(ObjectSuffix) $(IncludePath)
##
## Clean
##
clean:
	$(RM) -r $(ConfigurationName)/


