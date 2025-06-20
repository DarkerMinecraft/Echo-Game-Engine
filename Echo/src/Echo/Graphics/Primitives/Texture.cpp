#include "pch.h"
#include "Texture.h"
#include "Core/Application.h"

#include "Vulkan/Primitives/VulkanTexture.h"

namespace Echo 
{

	Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path, const Texture2DSpecification& spec)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateRef<VulkanTexture2D>(device, path, spec);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, void* data)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateRef<VulkanTexture2D>(device, width, height, data);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}