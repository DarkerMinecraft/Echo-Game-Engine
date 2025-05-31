#include "pch.h"
#include "Texture.h"
#include "Core/Application.h"
#include "Utils/DeferredInitManager.h"
#include "Core/Log.h"

#include "Vulkan/VulkanTexture.h"

namespace Echo 
{

	Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path, const Texture2DSpecification& spec)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		if (device->GetDeviceType() == DeviceType::Vulkan) {
			if (!device->IsInitialized()) {
				EC_CORE_WARN("[Texture2D] Device not ready, deferring VulkanTexture2D creation (from path)");
				Ref<Texture2D> tex;
				Echo::DeferredInitManager::Enqueue([=, &tex] {
					EC_CORE_INFO("[Texture2D] Running deferred VulkanTexture2D creation (from path)");
					tex = CreateRef<VulkanTexture2D>(device, path, spec);
				});
				return tex;
			} else {
				EC_CORE_INFO("[Texture2D] Device ready, creating VulkanTexture2D immediately (from path)");
				return CreateRef<VulkanTexture2D>(device, path, spec);
			}
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, void* data)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		if (device->GetDeviceType() == DeviceType::Vulkan) {
			if (!device->IsInitialized()) {
				EC_CORE_WARN("[Texture2D] Device not ready, deferring VulkanTexture2D creation (from data)");
				Ref<Texture2D> tex;
				Echo::DeferredInitManager::Enqueue([=, &tex] {
					EC_CORE_INFO("[Texture2D] Running deferred VulkanTexture2D creation (from data)");
					tex = CreateRef<VulkanTexture2D>(device, width, height, data);
				});
				return tex;
			} else {
				EC_CORE_INFO("[Texture2D] Device ready, creating VulkanTexture2D immediately (from data)");
				return CreateRef<VulkanTexture2D>(device, width, height, data);
			}
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}