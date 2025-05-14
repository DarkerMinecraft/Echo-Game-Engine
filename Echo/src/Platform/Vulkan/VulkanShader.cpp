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

	VulkanShader::VulkanShader(Device* device, const ShaderSpecification& specification)
		: m_Device(static_cast<VulkanDevice*>(device)), m_Specification(specification)
	{
		// Set shader name
		if (specification.ShaderName)
			m_Name = specification.ShaderName;
		else if (specification.VertexShaderPath)
			m_Name = std::filesystem::path(specification.VertexShaderPath).stem().string();
		else if (specification.ComputeShaderPath)
			m_Name = std::filesystem::path(specification.ComputeShaderPath).stem().string();
		else
			m_Name = "UnnamedShader";

		// Check if it's a compute shader
		m_IsCompute = specification.ComputeShaderPath != nullptr || specification.ComputeShaderSource != nullptr;

		// Process shader files or source code
		CompileOrGetVulkanBinary();
		CreateShaderModule();
		RebuildShaderStages();

		m_IsLoaded = true;
	}

	VulkanShader::~VulkanShader()
	{
		Destroy();
	}

	void VulkanShader::Reload()
	{
		if (m_Destroyed)
		{
			EC_CORE_WARN("Trying to reload a destroyed shader: {0}", m_Name);
			return;
		}

		bool needsReload = false;

		// Check if any shader files have been modified
		if (m_Specification.VertexShaderPath)
		{
			long long currentTimestamp = GetFileTimestamp(m_Specification.VertexShaderPath);
			if (m_FileTimestamps.find(m_Specification.VertexShaderPath) == m_FileTimestamps.end() ||
				m_FileTimestamps[m_Specification.VertexShaderPath] != currentTimestamp)
			{
				m_FileTimestamps[m_Specification.VertexShaderPath] = currentTimestamp;
				needsReload = true;
			}
		}

		if (m_Specification.FragmentShaderPath)
		{
			long long currentTimestamp = GetFileTimestamp(m_Specification.FragmentShaderPath);
			if (m_FileTimestamps.find(m_Specification.FragmentShaderPath) == m_FileTimestamps.end() ||
				m_FileTimestamps[m_Specification.FragmentShaderPath] != currentTimestamp)
			{
				m_FileTimestamps[m_Specification.FragmentShaderPath] = currentTimestamp;
				needsReload = true;
			}
		}

		if (m_Specification.GeometryShaderPath)
		{
			long long currentTimestamp = GetFileTimestamp(m_Specification.GeometryShaderPath);
			if (m_FileTimestamps.find(m_Specification.GeometryShaderPath) == m_FileTimestamps.end() ||
				m_FileTimestamps[m_Specification.GeometryShaderPath] != currentTimestamp)
			{
				m_FileTimestamps[m_Specification.GeometryShaderPath] = currentTimestamp;
				needsReload = true;
			}
		}

		if (m_Specification.ComputeShaderPath)
		{
			long long currentTimestamp = GetFileTimestamp(m_Specification.ComputeShaderPath);
			if (m_FileTimestamps.find(m_Specification.ComputeShaderPath) == m_FileTimestamps.end() ||
				m_FileTimestamps[m_Specification.ComputeShaderPath] != currentTimestamp)
			{
				m_FileTimestamps[m_Specification.ComputeShaderPath] = currentTimestamp;
				needsReload = true;
			}
		}

		if (!needsReload)
			return;

		EC_CORE_INFO("Reloading shader: {0}", m_Name);

		// Unload current resources
		Unload();

		// Recompile the shader
		CompileOrGetVulkanBinary();
		CreateShaderModule();
		RebuildShaderStages();

		m_IsLoaded = true;
	}

	void VulkanShader::Unload()
	{
		if (!m_IsLoaded)
			return;

		// Destroy shader modules
		if (m_VertexShaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(m_Device->GetDevice(), m_VertexShaderModule, nullptr);
			m_VertexShaderModule = VK_NULL_HANDLE;
		}

		if (m_FragmentShaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(m_Device->GetDevice(), m_FragmentShaderModule, nullptr);
			m_FragmentShaderModule = VK_NULL_HANDLE;
		}

		if (m_GeometryShaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(m_Device->GetDevice(), m_GeometryShaderModule, nullptr);
			m_GeometryShaderModule = VK_NULL_HANDLE;
		}

		if (m_ComputeShaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(m_Device->GetDevice(), m_ComputeShaderModule, nullptr);
			m_ComputeShaderModule = VK_NULL_HANDLE;
		}

		// Clear shader stages
		m_ShaderStages.clear();

		m_IsLoaded = false;
	}

	void VulkanShader::Destroy()
	{
		if (m_Destroyed)
			return;

		Unload();
		m_Destroyed = true;
	}

	void VulkanShader::CompileOrGetVulkanBinary()
	{
		// Cache file timestamps for hot reloading
		if (m_Specification.VertexShaderPath)
			m_FileTimestamps[m_Specification.VertexShaderPath] = GetFileTimestamp(m_Specification.VertexShaderPath);
		if (m_Specification.FragmentShaderPath)
			m_FileTimestamps[m_Specification.FragmentShaderPath] = GetFileTimestamp(m_Specification.FragmentShaderPath);
		if (m_Specification.GeometryShaderPath)
			m_FileTimestamps[m_Specification.GeometryShaderPath] = GetFileTimestamp(m_Specification.GeometryShaderPath);
		if (m_Specification.ComputeShaderPath)
			m_FileTimestamps[m_Specification.ComputeShaderPath] = GetFileTimestamp(m_Specification.ComputeShaderPath);
	}

	void VulkanShader::CreateShaderModule()
	{
		if (m_Specification.ShaderName != nullptr)
		{
			if (m_Specification.ComputeShaderSource)
			{
				m_ComputeShaderModule = CreateShaderModule(m_Specification.ComputeShaderSource, m_Specification.ShaderName);
			}
			else
			{
				m_VertexShaderModule = CreateShaderModule(m_Specification.VertexShaderSource, m_Specification.ShaderName);
				m_FragmentShaderModule = CreateShaderModule(m_Specification.FragmentShaderSource, m_Specification.ShaderName);
				if (m_Specification.GeometryShaderSource != nullptr)
					m_GeometryShaderModule = CreateShaderModule(m_Specification.GeometryShaderSource, m_Specification.ShaderName);
			}
		}
		else
		{
			if (m_Specification.ComputeShaderPath)
			{
				m_ComputeShaderModule = CreateShaderModule(m_Specification.ComputeShaderPath);
			}
			else
			{
				m_VertexShaderModule = CreateShaderModule(m_Specification.VertexShaderPath);
				m_FragmentShaderModule = CreateShaderModule(m_Specification.FragmentShaderPath);
				if (m_Specification.GeometryShaderPath != nullptr)
					m_GeometryShaderModule = CreateShaderModule(m_Specification.GeometryShaderPath);
			}
		}
	}

	void VulkanShader::RebuildShaderStages()
	{
		m_ShaderStages.clear();

		if (m_IsCompute)
		{
			if (m_ComputeShaderModule != VK_NULL_HANDLE)
				m_ShaderStages.push_back(VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_COMPUTE_BIT, m_ComputeShaderModule));
		}
		else
		{
			if (m_VertexShaderModule != VK_NULL_HANDLE)
				m_ShaderStages.push_back(VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, m_VertexShaderModule));
			if (m_FragmentShaderModule != VK_NULL_HANDLE)
				m_ShaderStages.push_back(VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, m_FragmentShaderModule));
			if (m_GeometryShaderModule != VK_NULL_HANDLE)
				m_ShaderStages.push_back(VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_GEOMETRY_BIT, m_GeometryShaderModule));
		}
	}

	VkShaderModule VulkanShader::CreateShaderModule(const char* shaderPath)
	{
		return m_Device->GetShaderLibrary().AddSpirvShader(shaderPath);
	}

	VkShaderModule VulkanShader::CreateShaderModule(const char* shaderSource, const char* shaderName)
	{
		return m_Device->GetShaderLibrary().AddSpirvShader(shaderSource, shaderName);
	}
}