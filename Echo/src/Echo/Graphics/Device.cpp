#include "pch.h"
#include "Device.h"

#include "Platform/Vulkan/VulkanDevice.h"

namespace Echo 
{

	Scope<Device> Device::Create(GraphicsAPI api, void* windowHwnd, int width, int height)
	{
		switch (api) 
		{
			case Vulkan: return CreateScope<VulkanDevice>(windowHwnd, width, height);
			case DirectX12: return nullptr;
		}
		
		return nullptr;
	}

}