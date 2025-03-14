#include "pch.h"
#include "Device.h"

#include "Platform/Vulkan/VulkanDevice.h"

namespace Echo 
{

	Scope<Device> Device::Create(DeviceType type, Window* window, unsigned int width, unsigned int height)
	{
		switch (type)
		{
			case DeviceType::Vulkan: return CreateScope<VulkanDevice>(window, width, height);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}