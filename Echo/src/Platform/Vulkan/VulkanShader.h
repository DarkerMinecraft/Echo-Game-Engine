#pragma once

#include "Graphics/Shader.h"
#include "VulkanDevice.h"

namespace Echo
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(Device* device, const std::filesystem::path& shaderPath, bool useCurrentDirectory);
		virtual ~VulkanShader();

		virtual void Unload() override;
		virtual void Destroy() override;

		virtual const BufferLayout& GetVertexLayout() const override;
		virtual const std::vector<ShaderResourceBinding> GetResourceBindings() const override;

		virtual const std::string& GetName() const override { return m_Name; }
		virtual bool IsCompute() override { return m_IsCompute; };
	public:
		std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages() { return m_ShaderStages; };
	private:
		void CompileOrGetVulkanBinary(const std::filesystem::path& shaderPath);

		void CreateShaderModules(const std::filesystem::path& shaderPath);
	private:
		VulkanDevice* m_Device;
		std::string m_Name;
		bool m_Destroyed = false;
		bool m_IsCompute = false;

		ShaderReflection m_ShaderReflection;
		std::vector<VkShaderModule> m_ShaderModules;
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
		std::vector<ShaderStage> m_Stages;

		std::unordered_map<std::filesystem::path, long long> m_FileTimestamps;
	};
}