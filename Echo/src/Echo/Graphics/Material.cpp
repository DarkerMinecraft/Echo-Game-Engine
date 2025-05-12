#include "pch.h"
#include "Material.h"
#include "Core/Application.h"

#include "Vulkan/VulkanMaterial.h"

namespace Echo 
{

	Ref<Material> Material::Create(Ref<Shader> shader, Ref<Texture2D> texture, PipelineSpecification& spec)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanMaterial>(device, shader, texture, spec);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Material> Material::Create(Ref<Shader> shader, const glm::vec3& color, PipelineSpecification& spec)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanMaterial>(device, shader, color, spec);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}