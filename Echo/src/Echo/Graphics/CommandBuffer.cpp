#include "pch.h"
#include "CommandBuffer.h"
#include "Echo/Core/Application.h"
#include "Platform/Vulkan/VulkanCommandBuffer.h"

namespace Echo
{
	Scope<CommandBuffer> CommandBuffer::Create()
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanCommandBuffer>(device);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}