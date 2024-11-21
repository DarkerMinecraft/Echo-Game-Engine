#pragma once

#include "Echo/Graphics/Pipeline.h"
#include "VulkanDevice.h"

#include <unordered_map>
#include <shaderc/shaderc.hpp>

namespace Echo 
{

	struct CompilationInfo
	{
		std::string FilePath;
		shaderc_shader_kind Kind;
		std::vector<char> Source;
		shaderc::CompileOptions Options;
	};

	class VulkanPipeline : public Pipeline 
	{
	public:
		VulkanPipeline(PipelineType type, const std::string& filePath);
		virtual ~VulkanPipeline();

		virtual PipelineType GetPipelineType() override { return m_Type; }

		virtual void Bind() override;

	private:
		std::string ReadFile(const std::string& filePath); 
		std::unordered_map<ShaderType, std::vector<char>> PreProcess(const std::string& source);

		void Compile(const std::string& filePath, const std::unordered_map<ShaderType, std::vector<char>>& shaders);

		void PreprocessShader(CompilationInfo& info);
		void CompileFileToAssembly(CompilationInfo& info);
		std::vector<char> CompileFile(CompilationInfo& info);

		VkShaderModule CreateShaderModule(const std::vector<char>& code);

		void InitPipeline();
		void InitBackgroundPipeline();
	private:
		VulkanDevice* m_Device;
		PipelineType m_Type;

		VkShaderModule m_VertexShaderModule;
		VkShaderModule m_FragmentShaderModule;
		VkShaderModule m_ComputeShaderModule;

		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}