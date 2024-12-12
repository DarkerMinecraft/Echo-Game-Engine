#include "pch.h"
#include "Pipeline.h"
#include "Device.h"

#include "Echo/Core/Application.h"

#include "Platform/Vulkan/VulkanPipeline.h"

namespace Echo 
{

	Ref<Pipeline> Pipeline::Create(PipelineType type, const std::string& filePath, size_t pushConstantsSize, std::vector<DescriptorType> types)
	{
		GraphicsAPI api = Application::Get().GetWindow().GetDevice()->GetGraphicsAPI();
		switch (api) 
		{
			case GraphicsAPI::Vulkan: return CreateRef<VulkanPipeline>(type, filePath, pushConstantsSize, types);
			case GraphicsAPI::DirectX12: return nullptr;
			default: return nullptr;
		}
		return nullptr;
	}

}