#include "pch.h"
#include "Material.h"

#include "Device.h"
#include "Echo/Core/Application.h"

#include "Platform/Vulkan/VulkanMaterial.h"

namespace Echo 
{

	Ref<Material> Material::Create(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanMaterial>(device, vertexShaderPath, fragmentShaderPath, geometryShaderPath);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Material> Material::Create(const char* vertexShaderSource, const char* fragmentShaderSource, const char* shaderName, const char* geometryShaderSource)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanMaterial>(device, vertexShaderSource, fragmentShaderSource, shaderName, geometryShaderSource);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}