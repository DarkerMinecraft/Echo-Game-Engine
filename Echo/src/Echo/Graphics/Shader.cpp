#include "pch.h"
#include "Shader.h"

#include "Core/Application.h"
#include "Vulkan/VulkanShader.h"

namespace Echo 
{
	Ref<Shader> Shader::Create(const std::filesystem::path shaderPath)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanShader>(device, shaderPath);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& shaderSource, const std::string& shaderName)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanShader>(device, shaderSource, shaderName);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}