#include "pch.h"
#include "Framebuffer.h"
#include "Core/Application.h"

#include "Vulkan/VulkanFramebuffer.h"

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