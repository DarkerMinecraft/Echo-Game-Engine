#include "pch.h"
#include "Framebuffer.h"
#include "Core/Application.h"
#include "Utils/DeferredInitManager.h"
#include "Core/Log.h"

#include "Vulkan/VulkanFramebuffer.h"

namespace Echo 
{

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specs)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		if (device->GetDeviceType() == DeviceType::Vulkan) {
			if (!device->IsInitialized()) {
				EC_CORE_WARN("[Framebuffer] Device not ready, deferring VulkanFramebuffer creation");
				Ref<Framebuffer> fb;
				Echo::DeferredInitManager::Enqueue([=, &fb] {
					EC_CORE_INFO("[Framebuffer] Running deferred VulkanFramebuffer creation");
					fb = CreateRef<VulkanFramebuffer>(device, specs);
				});
				return fb;
			} else {
				EC_CORE_INFO("[Framebuffer] Device ready, creating VulkanFramebuffer immediately");
				return CreateRef<VulkanFramebuffer>(device, specs);
			}
		}
		return nullptr;
	}

}