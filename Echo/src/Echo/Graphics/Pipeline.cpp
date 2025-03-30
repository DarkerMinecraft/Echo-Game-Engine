#include "pch.h"
#include "Pipeline.h"
#include "Echo/Core/Application.h"

#include "Platform/Vulkan/VulkanPipeline.h" 

namespace Echo 
{

	Ref<Pipeline> Pipeline::Create(Ref<Material> material, PipelineDesc& desc)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType()) 
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanPipeline>(device, material, desc);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Pipeline> Pipeline::Create(Ref<Shader> computeShader, PipelineDesc& desc)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateScope<VulkanPipeline>(device, computeShader, desc);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}