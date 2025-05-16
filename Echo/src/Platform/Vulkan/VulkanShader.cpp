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

	VulkanShader::VulkanShader(Device* device, const std::string& name, const std::string& source)
		: m_Device((VulkanDevice*)device)
	{
		CreateShaderModules(name.c_str(), source.c_str());
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

	void VulkanShader::CompileOrGetVulkanBinary(const std::filesystem::path& shaderPath)
	{
		m_FileTimestamps[shaderPath] = GetFileTimestamp(shaderPath);
	}

	void VulkanShader::CreateShaderModules(const std::filesystem::path& shaderPath)
	{
		std::vector<ShaderData> shaderDatas = m_Device->GetShaderLibrary().AddSpirvShader(shaderPath);
		for (auto& data : shaderDatas) 
		{
			m_ShaderModules.push_back(data.Module);
			m_ShaderReflections.push_back(data.Reflection);

			VkPipelineShaderStageCreateInfo createInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
			createInfo.pNext = nullptr;
			createInfo.module = data.Module;
			createInfo.pName = data.Reflection.GetEntryPointName();
			m_ShaderStages.push_back(createInfo);

			if (data.Reflection.GetShaderStage() == ShaderStage::Compute)
			{
				m_IsCompute = true;
				m_ComputeReflection = data.Reflection;
			}
			else if (data.Reflection.GetShaderStage() == ShaderStage::Vertex) 
			{
				m_VertexReflection = data.Reflection;
			}
			else if (data.Reflection.GetShaderStage() == ShaderStage::Fragment) 
			{
				m_FragmentReflection = data.Reflection;
			}
			else if (data.Reflection.GetShaderStage() == ShaderStage::Geometry) 
			{
				m_GeometryReflection = data.Reflection;
			}
		}
	}

	void VulkanShader::CreateShaderModules(const char* shaderSource, const char* shaderName)
	{
		std::vector<ShaderData> shaderDatas = m_Device->GetShaderLibrary().AddSpirvShader(shaderSource, shaderName);
		for (auto& data : shaderDatas)
		{
			m_ShaderModules.push_back(data.Module);
			m_ShaderReflections.push_back(data.Reflection);
			
			VkPipelineShaderStageCreateInfo createInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
			createInfo.pNext = nullptr;
			createInfo.module = data.Module;
			createInfo.pName = data.Reflection.GetEntryPointName();
			m_ShaderStages.push_back(createInfo);

			if (data.Reflection.GetShaderStage() == ShaderStage::Compute)
			{
				m_IsCompute = true;
				m_ComputeReflection = data.Reflection;
			}
			else if (data.Reflection.GetShaderStage() == ShaderStage::Vertex)
			{
				m_VertexReflection = data.Reflection;
			}
			else if (data.Reflection.GetShaderStage() == ShaderStage::Fragment)
			{
				m_FragmentReflection = data.Reflection;
			}
			else if (data.Reflection.GetShaderStage() == ShaderStage::Geometry)
			{
				m_GeometryReflection = data.Reflection;
			}
		}
	}
}