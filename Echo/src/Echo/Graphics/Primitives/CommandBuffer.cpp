#include "pch.h"
#include "CommandBuffer.h"
#include "Core/Application.h"

#include "Vulkan/Primitives/VulkanCommandBuffer.h"

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