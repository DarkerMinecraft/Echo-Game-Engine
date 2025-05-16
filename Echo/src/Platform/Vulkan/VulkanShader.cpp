#include "pch.h"
#include "VulkanShader.h"

#include "Utils/VulkanInitializers.h"

#include <filesystem>
#include <chrono>

namespace Echo
{
	static long long GetFileTimestamp(const std::filesystem::path filepath)
	{
		std::filesystem::file_time_type timestamp = std::filesystem::last_write_time(filepath);
		auto systemTime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
			timestamp - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
		return systemTime.time_since_epoch().count();
	}

	VulkanShader::VulkanShader(Device* device, const std::filesystem::path& shaderPath, bool useCurrentDirectory)
		: m_Device((VulkanDevice*)device), m_Name(shaderPath.stem().string())
	{
		std::filesystem::path path = useCurrentDirectory ? std::filesystem::current_path() / shaderPath : shaderPath;
		CompileOrGetVulkanBinary(path);
		m_ShaderModule = CreateShaderModule(path);
	}

	VulkanShader::VulkanShader(Device* device, const std::string& name, const std::string& source)
		: m_Device((VulkanDevice*)device)
	{
		m_ShaderModule = CreateShaderModule(name.c_str(), source.c_str());
	}

	VulkanShader::~VulkanShader()
	{
		Destroy();
	}

	void VulkanShader::Unload()
	{
		vkDestroyShaderModule(m_Device->GetDevice(), m_ShaderModule, nullptr);
		m_ShaderModule = VK_NULL_HANDLE;
	}

	void VulkanShader::Destroy()
	{
		if (m_Destroyed)
			return;

		Unload();
		m_Destroyed = true;
	}

	void VulkanShader::CompileOrGetVulkanBinary(const std::filesystem::path& shaderPath)
	{
		m_FileTimestamps[shaderPath] = GetFileTimestamp(shaderPath);
	}

	VkShaderModule VulkanShader::CreateShaderModule(const std::filesystem::path& shaderPath)
	{
		VkShaderModule module;
		m_Device->GetShaderLibrary().AddSpirvShader(shaderPath, &module, &m_ShaderReflection);

		return module;
	}

	VkShaderModule VulkanShader::CreateShaderModule(const char* shaderSource, const char* shaderName)
	{
		VkShaderModule module;
		m_Device->GetShaderLibrary().AddSpirvShader(shaderSource, shaderName, &module, &m_ShaderReflection);

		return module;
	}
}