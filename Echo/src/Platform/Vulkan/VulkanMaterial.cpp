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
		m_VertexShaderModule = m_Device->GetShaderLibrary().AddSpirvShader(vertexShaderPath);
		m_FragmentShaderModule = m_Device->GetShaderLibrary().AddSpirvShader(fragmentShaderPath);
		if (geometryShaderPath != nullptr) 
		{
			m_GeometryShaderModule = m_Device->GetShaderLibrary().AddSpirvShader(geometryShaderPath);
		}

		CreateShaderStages();
	}

	void VulkanMaterial::LoadShadersSource(const char* vertexShaderSource, const char* fragmentShaderSource, const char* shaderName, const char* geometryShaderSource)
	{
		m_VertexShaderModule = m_Device->GetShaderLibrary().AddSpirvShader(vertexShaderSource, (shaderName + std::string(" Vertex")).c_str());
		m_FragmentShaderModule = m_Device->GetShaderLibrary().AddSpirvShader(fragmentShaderSource, (shaderName + std::string(" Pixel")).c_str());
		if (geometryShaderSource != nullptr)
		{
			m_GeometryShaderModule = m_Device->GetShaderLibrary().AddSpirvShader(geometryShaderSource, (shaderName + std::string(" Geom")).c_str());
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

}