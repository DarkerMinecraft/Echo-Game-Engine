#include "pch.h"
#include "VulkanPipeline.h"

#include "Echo/Core/Application.h"

#include <fstream>

namespace Echo 
{

	static shaderc_shader_kind ConvertShaderTypeToShaderC(ShaderType type)
	{
		switch (type) 
		{
			case ShaderType::Compute:
				return shaderc_compute_shader;
			case ShaderType::Vertex:
				return shaderc_vertex_shader;
			case ShaderType::Fragment:
				return shaderc_fragment_shader;
		}
	}

	static ShaderType ConvertStringToType(const std::string& type) 
	{
		if (type == "vertex")
			return ShaderType::Vertex;
		else if (type == "pixel" || type == "fragment")
			return ShaderType::Fragment;
		else if (type == "compute")
			return ShaderType::Compute;
		else 
			return ShaderType::Unknown;
	}

	VulkanPipeline::VulkanPipeline(PipelineType type, const std::string& filePath)
		: m_Device((VulkanDevice*) Application::Get().GetWindow().GetDevice()), m_Type(type)
	{
		InitPipeline();

		std::string shaderSource = ReadFile(filePath);
		auto shaderSources = PreProcess(shaderSource);

		Compile(filePath, shaderSources);

		InitBackgroundPipeline();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		vkDestroyShaderModule(m_Device->GetDevice(), m_ComputeShaderModule, nullptr);

		vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);
		vkDestroyPipeline(m_Device->GetDevice(), m_Pipeline, nullptr);
	}

	void VulkanPipeline::Bind()
	{
		VkCommandBuffer cmd = (VkCommandBuffer)m_Device->GetCurrentFrame().Buffer->GetBuffer();

		switch (m_Type) 
		{
			case PipelineType::ComputePipeline: 
			{
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);

				VkDescriptorSet descriptorSet = m_Device->GetDescriptorSet();
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_PipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
			}
		}
	}

	std::string VulkanPipeline::ReadFile(const std::string& filePath)
	{
		std::string result;
		std::ifstream in(filePath, std::ios::ate | std::ios::binary);

		if (in) 
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else 
		{
			EC_CORE_ERROR("Could not open file: {0}", filePath);
		}

		return result;
	}

	std::unordered_map<ShaderType, std::vector<char>> VulkanPipeline::PreProcess(const std::string& source)
	{
		std::unordered_map<ShaderType, std::vector<char>> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos) 
		{
			size_t eol = source.find_first_of("\r\n", pos);
			EC_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			EC_CORE_ASSERT(type == "vertex" || type == "pixel" || type == "fragment" || type == "compute", "Invalid shader type specificed");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);

			std::string code = source.substr(nextLinePos,
											 pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
			shaderSources[ConvertStringToType(type)] = std::vector<char>(code.begin(), code.end());
		}

		return shaderSources;
	}

	void VulkanPipeline::Compile(const std::string& filePath, const std::unordered_map<ShaderType, std::vector<char>>& shaders)
	{
		for (auto& kv : shaders) 
		{
			ShaderType type = kv.first;
			std::vector<char> source = kv.second;

			CompilationInfo compilationInfo{};
			compilationInfo.Kind = ConvertShaderTypeToShaderC(type);
			compilationInfo.Source = source;
			compilationInfo.FilePath = filePath;
			compilationInfo.Options.SetOptimizationLevel(shaderc_optimization_level_performance);
			compilationInfo.Options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
			compilationInfo.Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

			PreprocessShader(compilationInfo);
			CompileFileToAssembly(compilationInfo);
			switch (type)
			{
				case ShaderType::Vertex:
				{
					m_VertexShaderModule = CreateShaderModule(CompileFile(compilationInfo));
					break;
				}
				case ShaderType::Fragment:
				{
					m_FragmentShaderModule = CreateShaderModule(CompileFile(compilationInfo));
					break;
				}
				case ShaderType::Compute: 
				{
					m_ComputeShaderModule = CreateShaderModule(CompileFile(compilationInfo));
					break;
				}
			}
		}
	}

	void VulkanPipeline::PreprocessShader(CompilationInfo& info)
	{
		shaderc::Compiler compiler;

		shaderc::PreprocessedSourceCompilationResult result = compiler.PreprocessGlsl(info.Source.data(), info.Source.size(),
																					  info.Kind, info.FilePath.c_str(), info.Options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			EC_CORE_ERROR(result.GetErrorMessage());
		}

		const char* src = result.cbegin();
		size_t newSize = result.cend() - src;
		info.Source.resize(newSize);
		memcpy(info.Source.data(), src, newSize);
	}

	void VulkanPipeline::CompileFileToAssembly(CompilationInfo& info)
	{
		shaderc::Compiler compiler;

		shaderc::AssemblyCompilationResult result = compiler.CompileGlslToSpvAssembly(info.Source.data(), info.Source.size(),
																					  info.Kind, info.FilePath.c_str(), info.Options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			EC_CORE_ERROR(result.GetErrorMessage());
		}

		const char* src = result.cbegin();
		size_t newSize = result.cend() - src;
		info.Source.resize(newSize);
		memcpy(info.Source.data(), src, newSize);
	}

	std::vector<char> VulkanPipeline::CompileFile(CompilationInfo& info)
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

	VkShaderModule VulkanPipeline::CreateShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		VkShaderModule shaderModule;

		if (vkCreateShaderModule(m_Device->GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module!");
		}

		return shaderModule;
	}

	void VulkanPipeline::InitPipeline()
	{
		VkPipelineLayoutCreateInfo computeLayout{};
		computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		computeLayout.pNext = nullptr;
		VkDescriptorSetLayout layout = m_Device->GetDescriptorSetLayout();
		computeLayout.pSetLayouts = &layout;
		computeLayout.setLayoutCount = 1;

		if (vkCreatePipelineLayout(m_Device->GetDevice(), &computeLayout, nullptr, &m_PipelineLayout) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void VulkanPipeline::InitBackgroundPipeline()
	{
		VkPipelineShaderStageCreateInfo stageinfo{};
		stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageinfo.pNext = nullptr;
		stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stageinfo.module = m_ComputeShaderModule;
		stageinfo.pName = "main";

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.layout = m_PipelineLayout;
		computePipelineCreateInfo.stage = stageinfo;

		if (vkCreateComputePipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_Pipeline) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create compute pipeline");
		}
	}

}