#pragma once

#include "Echo/Graphics/Material.h"
#include "Echo/Graphics/Device.h"

#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanMaterial : public Material 
	{
	public:
		VulkanMaterial(Device* device, const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath);
		VulkanMaterial(Device* device, const char* vertexShaderSource, const char* fragmentShaderSource, const char* shaderName, const char* geometryShaderPath);

		virtual void Destroy() override;

		std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages() { return m_ShaderStages; }
	private:
		void LoadShaders(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath);
		void LoadShadersSource(const char* vertexShaderSource, const char* fragmentShaderSource, const char* shaderName, const char* geometryShaderSource);

		void CreateShaderStages();

		VkShaderModule CreateShaderModule(SpirvData data);
	private:
		VulkanDevice* m_Device;

		VkShaderModule m_VertexShaderModule, m_FragmentShaderModule, m_GeometryShaderModule = nullptr;
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
	};


}