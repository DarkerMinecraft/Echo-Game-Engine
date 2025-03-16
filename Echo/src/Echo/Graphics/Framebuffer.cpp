#include "pch.h"
#include "Framebuffer.h"
#include "Echo/Core/Application.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"

namespace Echo 
{

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specs)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateRef<VulkanFramebuffer>(device, specs);
		}
	}

}