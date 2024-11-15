#include "pch.h"
#include "Device.h"

#include "Platform/Vulkan/VulkanDevice.h"

namespace Echo 
{

	Scope<Device> Device::Create(void* hwnd, const GraphicsDeviceCreateInfo& createInfo)
	{
		switch (createInfo.GraphicsAPI) 
		{
			case API::Vulkan: return CreateScope<VulkanDevice>(hwnd, createInfo);
			case API::DirectX: return nullptr;
			default: return nullptr;
		}

		return nullptr;
	}

}