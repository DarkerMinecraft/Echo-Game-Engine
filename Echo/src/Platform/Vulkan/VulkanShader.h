#pragma once

#include "Graphics/Shader.h"
#include "VulkanDevice.h"

namespace Echo
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(Device* device, const std::filesystem::path& shaderPath, bool useCurrentDirectory);
		VulkanShader(Device* device, const std::string& shaderSource, const std::string& name);
		virtual ~VulkanShader();

		virtual void Unload() override;
		virtual void Destroy() override;

		virtual const std::string& GetName() const override { return m_Name; }

		virtual const ShaderReflection& GetReflection() const { return m_ShaderReflection; };
	public:
		VkShaderModule GetShaderModule() { return m_ShaderModule; }
	private:
		void CompileOrGetVulkanBinary(const std::filesystem::path& shaderPath);

		VkShaderModule CreateShaderModule(const char* shaderSource, const char* shaderName);
		VkShaderModule CreateShaderModule(const std::filesystem::path& shaderPath);
	private:
		VulkanDevice* m_Device;
		std::string m_Name;
		bool m_Destroyed = false;

		ShaderReflection m_ShaderReflection;

		VkShaderModule m_ShaderModule;

		std::unordered_map<std::filesystem::path, long long> m_FileTimestamps;
	};
}