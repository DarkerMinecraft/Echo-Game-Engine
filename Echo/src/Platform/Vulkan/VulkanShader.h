#pragma once

#include "Echo/Graphics/GraphicsShader.h"

#include "Interface/VulkanDevice.h"
#include "Interface/VulkanShaderModule.h"

#include <vector>
#include <shaderc/shaderc.hpp>

namespace Echo
{

	struct CompilationInfo
	{
		const char* FilePath;
		shaderc_shader_kind Kind;
		std::vector<char> Source;
		shaderc::CompileOptions Options;
	};

	class VulkanShader : public GraphicsShader
	{
	public:
		VulkanShader(const std::string& vertexFilepath, const std::string& fragmentFilepath);
		~VulkanShader() = default;

		virtual IShaderModule* GetVertexShaderModule() override { return new VulkanShaderModule(m_VertShaderModule); }
		virtual IShaderModule* GetFragmentShaderModule() override { return new VulkanShaderModule(m_FragShaderModule); }

		virtual void Destroy() override;
	private:
		void CreateShaders(const std::string& vertexFilepath, const std::string& fragmentFilepath);
		std::vector<char> ReadFile(const std::string& filePath);

		void PreprocessShader(CompilationInfo& info);
		void CompileFileToAssembly(CompilationInfo& info);
		std::vector<char> CompileFile(CompilationInfo& info);
		
		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
	private:
		VulkanDevice* m_Device;

		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;
	};
}