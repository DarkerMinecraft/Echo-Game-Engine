#include "pch.h"
#include "Pipeline.h"
#include "Core/Application.h"

#include "Vulkan/VulkanPipeline.h" 

namespace Echo 
{

	Ref<Pipeline> Pipeline::Create(Ref<Shader> computeShader, PipelineSpecification& spec)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateScope<VulkanPipeline>(device, computeShader);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}