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
		CreateShaderModules(path);
	}

	VulkanShader::~VulkanShader()
	{
		Destroy();
	}

	void VulkanShader::Unload()
	{
		for (auto& module : m_ShaderModules)
		{
			vkDestroyShaderModule(m_Device->GetDevice(), module, nullptr);
			module = VK_NULL_HANDLE;
		}
	}

	void VulkanShader::Destroy()
	{
		if (m_Destroyed)
			return;

		Unload();
		m_Destroyed = true;
	}

	const BufferLayout& VulkanShader::GetVertexLayout() const
	{
		return m_ShaderReflection.GetVertexLayout();
	}

	const std::vector<ShaderResourceBinding> VulkanShader::GetResourceBindings() const
	{
		return m_ShaderReflection.GetResourceBindings();
	}

	void VulkanShader::CompileOrGetVulkanBinary(const std::filesystem::path& shaderPath)
	{
		m_FileTimestamps[shaderPath] = GetFileTimestamp(shaderPath);
	}

	void VulkanShader::CreateShaderModules(const std::filesystem::path& shaderPath)
	{
		ShaderReflection reflections;
		m_ShaderModules = m_Device->GetShaderLibrary().AddSpirvShader(shaderPath, &m_ShaderReflection);

		uint32_t index = 0;
		for (auto& entryPointData : m_ShaderReflection.GetEntryPointData())
		{
			VkPipelineShaderStageCreateInfo createInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
			createInfo.pNext = nullptr;
			createInfo.module = m_ShaderModules[index];
			createInfo.pName = entryPointData.EntryPointName;
			switch (entryPointData.Stage)
			{
				case ShaderStage::Vertex:
					createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
					break;
				case ShaderStage::Fragment:
					createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
					break;
				case ShaderStage::Compute:
					createInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
					break;
				case ShaderStage::Geometry:
					createInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
					break;
				default:
					EC_CORE_ERROR("Unknown shader stage");
					break;
			}
			index++;
			m_ShaderStages.push_back(createInfo);
		}
	}
}