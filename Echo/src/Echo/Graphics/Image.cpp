#include "pch.h"
#include "Image.h"
#include "Echo/Core/Application.h"
#include "Platform/Vulkan/VulkanImage.h"

namespace Echo 
{

	Ref<Image> Image::Create(const ImageDescription& desc)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		Ref<Image> image;

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  image = CreateRef<VulkanImage>(device, desc);
		}

		if (desc.DrawToImGui) 
		{
			device->AddImGuiImage(image);
		}

		return image;
	}

}