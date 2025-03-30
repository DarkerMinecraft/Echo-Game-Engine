#pragma once

#include "Echo/Graphics/Shader.h"

#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(Device* device, const ShaderSpecification& specification);
		virtual ~VulkanShader();

		virtual void Destroy() override;
	public:
		VkShaderModule GetComputeShaderModule() { return m_ComputeShaderModule; }

		std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages() { return m_ShaderStages; }
	private:
		VkShaderModule CreateShaderModule(const char* shaderSource, const char* shaderName);
		VkShaderModule CreateShaderModule(const char* shaderPath);
	private:
		VulkanDevice* m_Device;

		VkShaderModule m_VertexShaderModule;
		VkShaderModule m_FragmentShaderModule;

		VkShaderModule m_GeometryShaderModule = nullptr;

		VkShaderModule m_ComputeShaderModule = nullptr;

		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

		bool m_Destroyed = false;
	};

}