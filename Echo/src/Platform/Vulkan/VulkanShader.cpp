#include "pch.h"
#include "VulkanShader.h"

#include "Utils/VulkanInitializers.h"

namespace Echo 
{



	VulkanShader::VulkanShader(Device* device, const ShaderSpecification& specification)
		: m_Device(static_cast<VulkanDevice*>(device))
	{
		if (specification.ShaderName != nullptr) 
		{
			if (specification.ComputeShaderSource)
			{
				m_ComputeShaderModule = CreateShaderModule(specification.ComputeShaderSource, specification.ShaderName);
			}
			else 
			{
				m_VertexShaderModule = CreateShaderModule(specification.VertexShaderSource, specification.ShaderName);
				m_FragmentShaderModule = CreateShaderModule(specification.FragmentShaderSource, specification.ShaderName);
				if (specification.GeometryShaderSource != nullptr)
					m_GeometryShaderModule = CreateShaderModule(specification.GeometryShaderSource, specification.ShaderName);

				m_ShaderStages.push_back(VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, m_VertexShaderModule));
				m_ShaderStages.push_back(VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, m_FragmentShaderModule));
				if (m_GeometryShaderModule != nullptr)
					m_ShaderStages.push_back(VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_GEOMETRY_BIT, m_GeometryShaderModule));
			}
		}
		else
		{
			if (specification.ComputeShaderPath)
			{
				m_ComputeShaderModule = CreateShaderModule(specification.ComputeShaderPath);
			}
			else
			{
				m_VertexShaderModule = CreateShaderModule(specification.VertexShaderPath);
				m_FragmentShaderModule = CreateShaderModule(specification.FragmentShaderPath);
				if (specification.GeometryShaderPath != nullptr)
					m_GeometryShaderModule = CreateShaderModule(specification.GeometryShaderPath);

				m_ShaderStages.push_back(VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, m_VertexShaderModule));
				m_ShaderStages.push_back(VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, m_FragmentShaderModule));
				if (m_GeometryShaderModule != nullptr)
					m_ShaderStages.push_back(VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_GEOMETRY_BIT, m_GeometryShaderModule));
			}
		}
	}

	VulkanShader::~VulkanShader()
	{
		Destroy();
	}

	void VulkanShader::Destroy()
	{
		if (m_Destroyed)
			return;

		if (m_VertexShaderModule)
			vkDestroyShaderModule(m_Device->GetDevice(), m_VertexShaderModule, nullptr);
		if (m_FragmentShaderModule)
			vkDestroyShaderModule(m_Device->GetDevice(), m_FragmentShaderModule, nullptr);
		if (m_GeometryShaderModule)
			vkDestroyShaderModule(m_Device->GetDevice(), m_GeometryShaderModule, nullptr);
		if (m_ComputeShaderModule)
			vkDestroyShaderModule(m_Device->GetDevice(), m_ComputeShaderModule, nullptr);

		m_Destroyed = true;
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