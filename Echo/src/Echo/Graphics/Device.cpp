#include "pch.h"
#include "Device.h"

#include "Platform/Vulkan/VulkanDevice.h"

namespace Echo 
{

	Scope<Device> Device::Create(GraphicsAPI api, void* window)
	{
		switch (api) 
		{
			case GraphicsAPI::Vulkan: return CreateScope<VulkanDevice>(window);
			case GraphicsAPI::DirectX12: return nullptr;
			default: return nullptr;
		}

		return nullptr;
	}

}