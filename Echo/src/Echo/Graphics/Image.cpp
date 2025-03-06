#include "pch.h"
#include "Image.h"
#include "Echo/Core/Application.h"
#include "Platform/Vulkan/VulkanImage.h"

namespace Echo 
{

	Ref<Image> Image::Create(const ImageDescription& desc)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanImage>(device, desc);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}