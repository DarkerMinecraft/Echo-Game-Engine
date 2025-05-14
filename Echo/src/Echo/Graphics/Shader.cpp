#include "pch.h"
#include "Shader.h"

#include "Core/Application.h"
#include "Vulkan/VulkanShader.h"

namespace Echo 
{
	Ref<Shader> Shader::Create(const ShaderSpecification& specification)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanShader>(device, specification);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::filesystem::path& yamlPath)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanShader>(device, yamlPath);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}