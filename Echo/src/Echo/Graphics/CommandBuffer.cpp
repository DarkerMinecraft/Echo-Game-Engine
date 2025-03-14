#include "pch.h"
#include "CommandBuffer.h"
#include "Echo/Core/Application.h"
#include "Platform/Vulkan/VulkanCommandBuffer.h"

namespace Echo
{
	Ref<CommandBuffer> CommandBuffer::Create()
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:
				return CreateRef<VulkanCommandBuffer>(device);
		}
		return nullptr;
	}
}