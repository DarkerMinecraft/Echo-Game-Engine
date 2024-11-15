#include "pch.h"
#include "Resource.h"
#include "Device.h"

#include "Echo/Core/Application.h"

#include "Platform/Vulkan/VulkanResource.h"

namespace Echo 
{

	Ref<Resource> Resource::Create(const ResourceCreateInfo& createInfo)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		switch (device->GetGraphicsAPI()) 
		{
			case API::DirectX: return nullptr;
			case API::Vulkan: return CreateRef<VulkanResource>(createInfo);
		}
	}

}