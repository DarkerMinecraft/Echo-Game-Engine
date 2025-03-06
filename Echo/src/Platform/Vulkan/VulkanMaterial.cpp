#include "pch.h"
#include "VulkanMaterial.h"

namespace Echo 
{



	VulkanMaterial::VulkanMaterial(Device* device, const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath)
		: m_Device((VulkanDevice*)device)
	{
		LoadShaders(vertexShaderPath, fragmentShaderPath, geometryShaderPath);
	}

	VulkanMaterial::VulkanMaterial(Device* device, const char* vertexShaderSource, const char* fragmentShaderSource, const char* shaderName, const char* geometryShaderPath)
		: m_Device((VulkanDevice*)device)
	{
		LoadShadersSource(vertexShaderSource, fragmentShaderSource, shaderName, geometryShaderPath);
	}

	void VulkanMaterial::Destroy()
	{
		vkDestroyShaderModule(m_Device->GetDevice(), m_VertexShaderModule, nullptr);
		vkDestroyShaderModule(m_Device->GetDevice(), m_FragmentShaderModule, nullptr);
		if (m_GeometryShaderModule != nullptr)
		{
			vkDestroyShaderModule(m_Device->GetDevice(), m_GeometryShaderModule, nullptr);
		}
	}

	void VulkanMaterial::LoadShaders(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath)
	{
		auto vertexShader = m_Device->GetShaderLibrary().AddSpirvShader(std::string(vertexShaderPath), ShaderType::VertexShader);
		m_VertexShaderModule = CreateShaderModule(vertexShader);
		auto fragmentShader = m_Device->GetShaderLibrary().AddSpirvShader(std::string(fragmentShaderPath), ShaderType::FragmentShader);
		m_FragmentShaderModule = CreateShaderModule(fragmentShader);
		if (geometryShaderPath != nullptr) 
		{
			auto geometryShader = m_Device->GetShaderLibrary().AddSpirvShader(std::string(geometryShaderPath), ShaderType::GeometryShader);
			m_GeometryShaderModule = CreateShaderModule(geometryShader);
		}

		CreateShaderStages();
	}

	void VulkanMaterial::LoadShadersSource(const char* vertexShaderSource, const char* fragmentShaderSource, const char* shaderName, const char* geometryShaderSource)
	{
		auto vertexShader = m_Device->GetShaderLibrary().AddSpirvShader(vertexShaderSource, std::string(shaderName) + "_vertex", ShaderType::VertexShader);
		m_VertexShaderModule = CreateShaderModule(vertexShader);
		auto fragmentShader = m_Device->GetShaderLibrary().AddSpirvShader(fragmentShaderSource, std::string(shaderName) + "_fragment", ShaderType::FragmentShader);
		m_FragmentShaderModule = CreateShaderModule(fragmentShader);
		if (geometryShaderSource != nullptr) 
		{
			auto geometryShader = m_Device->GetShaderLibrary().AddSpirvShader(geometryShaderSource, std::string(shaderName) + "_geometry", ShaderType::GeometryShader);
			m_GeometryShaderModule = CreateShaderModule(geometryShader);
		}

		CreateShaderStages();
	}

	void VulkanMaterial::CreateShaderStages()
	{
		VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
		vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderStageInfo.module = m_VertexShaderModule;
		vertexShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
		fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragmentShaderStageInfo.module = m_FragmentShaderModule;
		fragmentShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo geometryShaderStageInfo{};
		if (m_GeometryShaderModule != nullptr)
		{
			geometryShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			geometryShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
			geometryShaderStageInfo.module = m_GeometryShaderModule;
			geometryShaderStageInfo.pName = "main";
		}

		m_ShaderStages.push_back(vertexShaderStageInfo);
		m_ShaderStages.push_back(fragmentShaderStageInfo);
		if (m_GeometryShaderModule != nullptr)
		{
			m_ShaderStages.push_back(geometryShaderStageInfo);
		}
	}

	VkShaderModule VulkanMaterial::CreateShaderModule(SpirvData data)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = data.GetSize();
		createInfo.pCode = data.GetData();
		VkShaderModule shaderModule;

		if (vkCreateShaderModule(m_Device->GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module!");
		}

		return shaderModule;
	}

}