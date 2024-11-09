#include "pch.h"
#include "VulkanShader.h"

#include "Echo/Core/Application.h"

#include <fstream>

namespace Echo 
{

	VulkanShader::VulkanShader(const std::string& vertexFilepath, const std::string& fragmentFilepath)
		: m_Device((VulkanDevice*) Application::Get().GetWindow().GetDevice())
	{
		CreateShaders(vertexFilepath, fragmentFilepath);
	}

	void VulkanShader::Destroy()
	{
		vkDestroyShaderModule(m_Device->GetDevice(), m_VertShaderModule, nullptr);
		vkDestroyShaderModule(m_Device->GetDevice(), m_FragShaderModule, nullptr);
	}

	void VulkanShader::CreateShaders(const std::string& vertexFilepath, const std::string& fragmentFilepath)
	{
		auto vertSrc = ReadFile(vertexFilepath);
		auto fragSrc = ReadFile(fragmentFilepath);

		CompilationInfo vertexCompilationInfo;
		vertexCompilationInfo.Kind = shaderc_vertex_shader;
		vertexCompilationInfo.Source = vertSrc;
		vertexCompilationInfo.FilePath = vertexFilepath.c_str();
		vertexCompilationInfo.Options.SetOptimizationLevel(shaderc_optimization_level_performance);
		vertexCompilationInfo.Options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		vertexCompilationInfo.Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		PreprocessShader(vertexCompilationInfo);
		CompileFileToAssembly(vertexCompilationInfo);
		auto vertCode = CompileFile(vertexCompilationInfo);

		CompilationInfo fragCompilationInfo;
		fragCompilationInfo.Kind = shaderc_fragment_shader;
		fragCompilationInfo.Source = fragSrc;
		fragCompilationInfo.FilePath = fragmentFilepath.c_str();
		fragCompilationInfo.Options.SetOptimizationLevel(shaderc_optimization_level_performance);
		fragCompilationInfo.Options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		fragCompilationInfo.Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		PreprocessShader(fragCompilationInfo);
		CompileFileToAssembly(fragCompilationInfo);
		auto fragCode = CompileFile(fragCompilationInfo);

		CreateShaderModule(vertCode, &m_VertShaderModule);
		CreateShaderModule(fragCode, &m_FragShaderModule);
	}

	std::vector<char> VulkanShader::ReadFile(const std::string& filePath)
	{
		std::vector<char> result;
		std::ifstream file(filePath, std::ios::binary); // Open in binary mode
		if (!file.is_open())
		{
			EC_CORE_CRITICAL("Failed to open file {0}", filePath)
			return result;
		}

		std::string line;
		while (std::getline(file, line))
		{
			if (line.find("//") != std::string::npos)
				continue;

			result.insert(result.end(), line.begin(), line.end());
			result.push_back('\n');
		}

		file.close();
		return result;
		return result;
	}

	void VulkanShader::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_Device->GetDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to create shader module")
		}
	}

	void VulkanShader::PreprocessShader(CompilationInfo& info)
	{
		shaderc::Compiler compiler;

		shaderc::PreprocessedSourceCompilationResult result = compiler.PreprocessGlsl(info.Source.data(), info.Source.size(),
																					  info.Kind, info.FilePath, info.Options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			EC_CORE_ERROR(result.GetErrorMessage());
		}

		const char* src = result.cbegin();
		size_t newSize = result.cend() - src;
		info.Source.resize(newSize);
		memcpy(info.Source.data(), src, newSize);
	}

	void VulkanShader::CompileFileToAssembly(CompilationInfo& info)
	{
		shaderc::Compiler compiler;

		shaderc::AssemblyCompilationResult result = compiler.CompileGlslToSpvAssembly(info.Source.data(), info.Source.size(),
																					  info.Kind, info.FilePath, info.Options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			EC_CORE_ERROR(result.GetErrorMessage());
		}

		const char* src = result.cbegin();
		size_t newSize = result.cend() - src;
		info.Source.resize(newSize);
		memcpy(info.Source.data(), src, newSize);
	}

	std::vector<char> VulkanShader::CompileFile(CompilationInfo& info)
	{
		shaderc::Compiler compiler;
		shaderc::SpvCompilationResult result = compiler.AssembleToSpv(info.Source.data(), info.Source.size(), info.Options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			EC_CORE_ERROR(result.GetErrorMessage());
		}

		const uint32_t* src = result.cbegin();
		size_t newSize = result.cend() - src;
		std::vector<uint32_t> output(newSize);
		memcpy(output.data(), src, newSize * sizeof(uint32_t));

		size_t byteSize = output.size() * sizeof(uint32_t);
		std::vector<char> cOutput(byteSize);

		memcpy(cOutput.data(), output.data(), byteSize);
		return cOutput;
	}

}