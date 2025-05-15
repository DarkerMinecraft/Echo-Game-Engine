#pragma once

#include "Graphics/Shader.h"
#include "VulkanDevice.h"

namespace Echo
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(Device* device, const std::filesystem::path& shaderPath);
		VulkanShader(Device* device, const std::string& shaderSource, const std::string& name);
		virtual ~VulkanShader();

		virtual void Unload() override;
		virtual void Destroy() override;

		virtual const std::string& GetName() const override { return m_Name; }
		virtual ShaderResourceLayout& GetResourceLayout() override { return m_ResourceLayout; }

		virtual const ShaderReflection& GetReflection() const { return m_ShaderReflection; };
	public:
		VkShaderModule GetComputeShaderModule() { return m_ComputeShaderModule; }
		VkShaderModule GetVertexShaderModule() { return m_VertexShaderModule; }
		VkShaderModule GetFragmentShaderModule() { return m_FragmentShaderModule; }
		VkShaderModule GetGeometryShaderModule() { return m_GeometryShaderModule; }
	private:
		void CompileOrGetVulkanBinary(const std::filesystem::path& shaderPath);
		void CreateShader(const std::filesystem::path& shaderPath);
		void CreateShader(const char* shaderSource, const char* shaderName);

		VkShaderModule CreateShaderModule(const char* shaderSource, const char* shaderName);
		VkShaderModule CreateShaderModule(const std::filesystem::path& shaderPath);
	private:
		VulkanDevice* m_Device;
		std::string m_Name;
		bool m_Destroyed = false;
		bool m_IsLoaded = false;

		ShaderResourceLayout m_ResourceLayout;

		ShaderReflection m_ShaderReflection;

		std::unordered_map<std::filesystem::path, long long> m_FileTimestamps;

		VkShaderModule m_VertexShaderModule = VK_NULL_HANDLE;
		VkShaderModule m_FragmentShaderModule = VK_NULL_HANDLE;
		VkShaderModule m_GeometryShaderModule = VK_NULL_HANDLE;
		VkShaderModule m_ComputeShaderModule = VK_NULL_HANDLE;
	};
}