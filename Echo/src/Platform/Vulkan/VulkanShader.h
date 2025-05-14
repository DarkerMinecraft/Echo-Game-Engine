#pragma once

#include "Graphics/Shader.h"
#include "VulkanDevice.h"

namespace Echo
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(Device* device, const ShaderSpecification& specification);
		VulkanShader(Device* device, std::filesystem::path& filePath);
		virtual ~VulkanShader();

		virtual void Reload() override;
		virtual void Unload() override;
		virtual void Destroy() override;

		virtual const std::string& GetName() const override { return m_Name; }
		virtual ShaderResourceLayout& GetResourceLayout() override { return m_ResourceLayout; }
		virtual bool IsCompute() override { return m_IsCompute; }
	public:
		VkShaderModule GetComputeShaderModule() { return m_ComputeShaderModule; }
		VkShaderModule GetVertexShaderModule() { return m_VertexShaderModule; }
		VkShaderModule GetFragmentShaderModule() { return m_FragmentShaderModule; }
		VkShaderModule GetGeometryShaderModule() { return m_GeometryShaderModule; }

		std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages() { return m_ShaderStages; }
	private:
		void CompileOrGetVulkanBinary();
		void CreateShaderModule();
		void ExtractResourceLayout();
		void ReflectShader(VkShaderModule shader, ShaderStage stage);
		VkShaderModule CreateShaderModule(const char* shaderSource, const char* shaderName);
		VkShaderModule CreateShaderModule(const char* shaderPath);
		void RebuildShaderStages();
	private:
		VulkanDevice* m_Device;
		std::string m_Name;
		bool m_IsCompute = false;
		bool m_Destroyed = false;
		bool m_IsLoaded = false;

		ShaderSpecification m_Specification;
		ShaderResourceLayout m_ResourceLayout;

		// Cached file timestamps for hot reloading
		std::unordered_map<std::string, long long> m_FileTimestamps;

		VkShaderModule m_VertexShaderModule = VK_NULL_HANDLE;
		VkShaderModule m_FragmentShaderModule = VK_NULL_HANDLE;
		VkShaderModule m_GeometryShaderModule = VK_NULL_HANDLE;
		VkShaderModule m_ComputeShaderModule = VK_NULL_HANDLE;

		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
	};
}