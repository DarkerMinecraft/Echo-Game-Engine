#include "pch.h"
#include "Material.h"
#include "Core/Application.h"

#include "Vulkan/VulkanMaterial.h"

namespace Echo 
{

	Ref<Material> Material::Create(Ref<Pipeline> pipeline)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanMaterial>(device, pipeline);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}