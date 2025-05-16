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

		virtual const std::vector<ShaderReflection>& GetReflections() const { return m_ShaderReflections; }
		virtual const ShaderReflection& GetVertexReflection() const { return m_VertexReflection; }
		virtual const ShaderReflection& GetFragmentReflection() const { return m_FragmentReflection; }
		virtual const ShaderReflection& GetComputeRelection() const { return m_ComputeReflection; }

		virtual const std::string& GetName() const override { return m_Name; }
		virtual bool IsCompute() override { return m_IsCompute; };
	public:
		std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages() { return m_ShaderStages; };
	private:
		void CompileOrGetVulkanBinary(const std::filesystem::path& shaderPath);

		void CreateShaderModules(const char* shaderSource, const char* shaderName);
		void CreateShaderModules(const std::filesystem::path& shaderPath);
	private:
		VulkanDevice* m_Device;
		std::string m_Name;
		bool m_Destroyed = false;
		bool m_IsCompute = false;

		std::vector<ShaderReflection> m_ShaderReflections;
		std::vector<VkShaderModule> m_ShaderModules;
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

		ShaderReflection m_VertexReflection, m_FragmentReflection, m_GeometryReflection, m_ComputeReflection;

		std::unordered_map<std::filesystem::path, long long> m_FileTimestamps;
	};
}