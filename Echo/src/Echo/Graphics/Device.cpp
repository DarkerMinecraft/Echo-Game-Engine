#include "pch.h"
#include "Device.h"

#include "Platform/Vulkan/VulkanDevice.h"

#include "Echo/Core/Base.h"

namespace Echo 
{

	Scope<Device> Device::Create(DeviceType type, const void* window, unsigned int width, unsigned int height)
	{
		switch (type)
		{
			case DeviceType::Vulkan: return CreateScope<VulkanDevice>(window, width, height);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}