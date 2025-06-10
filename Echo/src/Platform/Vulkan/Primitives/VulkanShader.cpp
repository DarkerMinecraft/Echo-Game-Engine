#include "pch.h"
#include "VulkanShader.h"

#include "Vulkan/Utils/VulkanInitializers.h"

#include <filesystem>
#include <chrono>

namespace Echo
{

	VulkanShader::VulkanShader(Device* device, const std::filesystem::path& shaderPath, bool shouldRecompile, bool* didCompile)
		: m_Device((VulkanDevice*)device), m_Name(shaderPath.stem().string())
	{
		bool compile = CreateShaderModules(shaderPath, shouldRecompile);

		if (didCompile != nullptr)
		{
			didCompile = &compile;
		}
	}

	VulkanShader::~VulkanShader()
	{
		Destroy();
	}

	void VulkanShader::Unload()
	{
		EC_PROFILE_FUNCTION();
		for (auto& module : m_ShaderModules)
		{
			vkDestroyShaderModule(m_Device->GetDevice(), module, nullptr);
			module = VK_NULL_HANDLE;
		}
	}

	void VulkanShader::Destroy()
	{
		EC_PROFILE_FUNCTION();
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

	bool VulkanShader::CreateShaderModules(const std::filesystem::path& shaderPath, bool shouldRecompile)
	{
		EC_PROFILE_FUNCTION();

		bool didCompile;
		m_ShaderModules = m_Device->GetShaderLibrary().AddSpirvShader(shaderPath, shouldRecompile, &m_ShaderReflection, &didCompile);

		if (didCompile == false) 
		{
			return false;
		}

		uint32_t index = 0;
		for (auto& entryPointData : m_ShaderReflection.GetEntryPointData())
		{
			VkPipelineShaderStageCreateInfo createInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
			createInfo.pNext = nullptr;
			createInfo.module = m_ShaderModules[index];
			createInfo.pName = entryPointData.EntryPointName.c_str();
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
		return true;
	}
}