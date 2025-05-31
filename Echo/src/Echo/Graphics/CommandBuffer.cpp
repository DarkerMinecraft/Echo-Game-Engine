#include "pch.h"
#include "CommandBuffer.h"
#include "Core/Application.h"
#include "Utils/DeferredInitManager.h"
#include "Core/Log.h"

#include "Vulkan/VulkanCommandBuffer.h"

namespace Echo
{

	Ref<CommandBuffer> CommandBuffer::Create()
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		if (device->GetDeviceType() == DeviceType::Vulkan) {
			if (!device->IsInitialized()) {
				EC_CORE_WARN("[CommandBuffer] Device not ready, deferring VulkanCommandBuffer creation");
				Ref<CommandBuffer> cmdBuf;
				Echo::DeferredInitManager::Enqueue([=, &cmdBuf] {
					EC_CORE_INFO("[CommandBuffer] Running deferred VulkanCommandBuffer creation");
					cmdBuf = CreateRef<VulkanCommandBuffer>(device);
				});
				return cmdBuf;
			} else {
				EC_CORE_INFO("[CommandBuffer] Device ready, creating VulkanCommandBuffer immediately");
				return CreateRef<VulkanCommandBuffer>(device);
			}
		}
		return nullptr;
	}

}