#include "pch.h"
#include "Material.h"

#include "Device.h"
#include "Echo/Core/Application.h"

#include "Platform/Vulkan/VulkanMaterial.h"

namespace Echo 
{

	Ref<Material> Material::Create(Ref<Shader> shader, Ref<Texture2D> texture)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanMaterial>(device, shader, texture);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Material> Material::Create(Ref<Shader> shader, glm::vec3 color)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanMaterial>(device, shader, color);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}