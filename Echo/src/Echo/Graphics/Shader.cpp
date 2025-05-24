#include "pch.h"
#include "Shader.h"

#include "Core/Application.h"
#include "Vulkan/VulkanShader.h"

namespace Echo 
{
	Ref<Shader> Shader::Create(const std::filesystem::path shaderPath, bool shouldRecompile, bool* didCompile)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanShader>(device, shaderPath, shouldRecompile, didCompile);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}