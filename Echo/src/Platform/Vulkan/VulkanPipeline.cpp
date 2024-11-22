#include "pch.h"
#include "VulkanPipeline.h"

#include "VulkanModel.h"

#include "Echo/Core/Application.h"

#include "Utils/VulkanInitializers.h"

#include <fstream>

namespace Echo 
{

	static shaderc_shader_kind ConvertShaderTypeToShaderC(ShaderType type)
	{
		switch (type) 
		{
			case ShaderType::ComputeShader:
				return shaderc_compute_shader;
			case ShaderType::VertexShader:
				return shaderc_vertex_shader;
			case ShaderType::FragmentShader:
				return shaderc_fragment_shader;
		}
	}

	static ShaderType ConvertStringToType(const std::string& type) 
	{
		if (type == "vertex")
			return ShaderType::VertexShader;
		else if (type == "pixel" || type == "fragment")
			return ShaderType::FragmentShader;
		else if (type == "compute")
			return ShaderType::ComputeShader;
	}

	VulkanPipeline::VulkanPipeline(PipelineType type, const std::string& filePath, size_t pushConstantsSize)
		: m_Device((VulkanDevice*) Application::Get().GetWindow().GetDevice()), m_Type(type), m_PushConstantSize(pushConstantsSize)
	{
		InitPipeline(pushConstantsSize);

		std::string shaderSource = ReadFile(filePath);
		auto shaderSources = PreProcess(shaderSource);

		Compile(filePath, shaderSources);

		InitBackgroundPipeline();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		if (m_Type == PipelineType::ComputePipeline)
		{
			vkDestroyShaderModule(m_Device->GetDevice(), m_ComputeShaderModule, nullptr);
		}
		else if (m_Type == PipelineType::GraphicsPipeline) 
		{
			vkDestroyShaderModule(m_Device->GetDevice(), m_VertexShaderModule, nullptr);
			vkDestroyShaderModule(m_Device->GetDevice(), m_FragmentShaderModule, nullptr);
		}

		vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);
		vkDestroyPipeline(m_Device->GetDevice(), m_Pipeline, nullptr);
	}

	void VulkanPipeline::Bind()
	{
		VkCommandBuffer cmd = m_Device->GetCurrentCommandBuffer();

		switch (m_Type) 
		{
			case PipelineType::ComputePipeline: 
			{
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);

				VkDescriptorSet descriptorSet = m_Device->GetDescriptorSet();
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_PipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
				break;
			}
			case PipelineType::GraphicsPipeline: 
			{
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

				GPUDrawPushConstants push_constants;
				push_constants.WorldMatrix = glm::mat4{ 1.f };
				
				vkCmdPushConstants(cmd, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
				break;
			}
		}
	}

	void VulkanPipeline::UpdatePushConstants(const void* pushConstants)
	{
		if (!pushConstants) return; 
		if (m_PushConstantSize == -1) return;

		vkCmdPushConstants(
			m_Device->GetCurrentCommandBuffer(),
			m_PipelineLayout,      
			VK_SHADER_STAGE_COMPUTE_BIT,
			0,                         
			static_cast<uint32_t>(this->m_PushConstantSize),
			pushConstants                        
		);
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
			compilationInfo.Options.SetTargetSpirv(shaderc_spirv_version_1_6);

			PreprocessShader(compilationInfo);
			CompileFileToAssembly(compilationInfo);
			switch (type)
			{
				case ShaderType::VertexShader:
				{
					m_VertexShaderModule = CreateShaderModule(CompileFile(compilationInfo));
					break;
				}
				case ShaderType::FragmentShader:
				{
					m_FragmentShaderModule = CreateShaderModule(CompileFile(compilationInfo));
					break;
				}
				case ShaderType::ComputeShader: 
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

	void VulkanPipeline::InitPipeline(size_t pushConstantsSize)
	{
		if (m_Type == PipelineType::ComputePipeline)
		{
			VkPipelineLayoutCreateInfo computeLayout{};
			computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			computeLayout.pNext = nullptr;
			VkDescriptorSetLayout layout = m_Device->GetDescriptorSetLayout();
			computeLayout.pSetLayouts = &layout;
			computeLayout.setLayoutCount = 1;

			if (pushConstantsSize != -1)
			{
				VkPushConstantRange pushConstant{};
				pushConstant.offset = 0;
				pushConstant.size = pushConstantsSize;
				pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

				computeLayout.pPushConstantRanges = &pushConstant;
				computeLayout.pushConstantRangeCount = 1;
			}

			if (vkCreatePipelineLayout(m_Device->GetDevice(), &computeLayout, nullptr, &m_PipelineLayout) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create pipeline layout");
			}
		}
		else if (m_Type == PipelineType::GraphicsPipeline)
		{
			VkPipelineLayoutCreateInfo graphicsLayout{};
			graphicsLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			graphicsLayout.pNext = nullptr;

			if (pushConstantsSize != -1)
			{
				VkPushConstantRange bufferRange{};
				bufferRange.offset = 0;
				bufferRange.size = sizeof(GPUDrawPushConstants);
				bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

				graphicsLayout.pPushConstantRanges = &bufferRange;
				graphicsLayout.pushConstantRangeCount = 1;
			}

			if (vkCreatePipelineLayout(m_Device->GetDevice(), &graphicsLayout, nullptr, &m_PipelineLayout) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create pipeline layout");
			}
		}
	}

	void VulkanPipeline::InitBackgroundPipeline()
	{
		if (m_Type == PipelineType::ComputePipeline)
		{
			VkPipelineShaderStageCreateInfo stageinfo = VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_COMPUTE_BIT, m_ComputeShaderModule);

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
		else if (m_Type == PipelineType::GraphicsPipeline) 
		{
			PipelineConfig config;

			config.SetShaders(m_VertexShaderModule, m_FragmentShaderModule);
			config.SetVertexInput(VulkanModel::GetBindingDescription(), VulkanModel::GetAttributeDescriptions());
			config.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
			config.SetPolygonMode(VK_POLYGON_MODE_FILL);
			config.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
			config.SetMutisamplingNone();
			config.DisableBlending();
			config.DisableDepthTest();
			
			config.SetColorAttachmentFormat(m_Device->GetAllocatedImage().ImageFormat);
			config.SetDepthFormat(VK_FORMAT_UNDEFINED);

			m_Pipeline = config.BuildPipeline(m_Device->GetDevice(), m_PipelineLayout);
		}
	}

	VkPipeline PipelineConfig::BuildPipeline(VkDevice device, VkPipelineLayout layout)
	{
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext = nullptr;

		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.pNext = nullptr;

		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &ColorBlendAttachment;

		VkGraphicsPipelineCreateInfo pipelineInfo = { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		pipelineInfo.pNext = &RenderInfo;

		pipelineInfo.stageCount = (uint32_t)ShaderStages.size();
		pipelineInfo.pStages = ShaderStages.data();
		pipelineInfo.pVertexInputState = &VertexInputInfo;
		pipelineInfo.pInputAssemblyState = &InputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &Rasterizer;
		pipelineInfo.pMultisampleState = &Multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDepthStencilState = &DepthStencil;
		pipelineInfo.layout = layout;

		VkDynamicState state[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo dynamicInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dynamicInfo.pDynamicStates = &state[0];
		dynamicInfo.dynamicStateCount = 2;

		pipelineInfo.pDynamicState = &dynamicInfo;

		VkPipeline newPipeline;
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
			nullptr, &newPipeline)
			!= VK_SUCCESS)
		{
			EC_CORE_ERROR("Failed to create pipeline");
			return VK_NULL_HANDLE; 
		}
		else
		{
			return newPipeline;
		}
	}

	void PipelineConfig::SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader)
	{
		ShaderStages.push_back(
			VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader));

		ShaderStages.push_back(
			VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader));
	}

	void PipelineConfig::SetVertexInput(VkVertexInputBindingDescription bindingDescription, std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions)
	{
		VertexInputInfo.vertexBindingDescriptionCount = 1;
		VertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		VertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	}

	void PipelineConfig::SetInputTopology(VkPrimitiveTopology topology)
	{
		InputAssembly.topology = topology;
		InputAssembly.primitiveRestartEnable = VK_FALSE;
	}

	void PipelineConfig::SetPolygonMode(VkPolygonMode mode)
	{
		Rasterizer.polygonMode = mode;
		Rasterizer.lineWidth = 1.f;
	}

	void PipelineConfig::SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace)
	{
		Rasterizer.cullMode = cullMode;
		Rasterizer.frontFace = frontFace;
	}

	void PipelineConfig::SetMutisamplingNone()
	{
		Multisampling.sampleShadingEnable = VK_FALSE;
		Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		Multisampling.minSampleShading = 1.0f;
		Multisampling.pSampleMask = nullptr;
		Multisampling.alphaToCoverageEnable = VK_FALSE;
		Multisampling.alphaToOneEnable = VK_FALSE;
	}

	void PipelineConfig::DisableBlending()
	{
		ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		ColorBlendAttachment.blendEnable = VK_FALSE;
	}

	void PipelineConfig::SetColorAttachmentFormat(VkFormat format)
	{
		ColorAttachmentFormat = format;

		RenderInfo.colorAttachmentCount = 1;
		RenderInfo.pColorAttachmentFormats = &ColorAttachmentFormat;
	}

	void PipelineConfig::SetDepthFormat(VkFormat format)
	{
		RenderInfo.depthAttachmentFormat = format;
	}

	void PipelineConfig::DisableDepthTest()
	{
		DepthStencil.depthTestEnable = VK_FALSE;
		DepthStencil.depthWriteEnable = VK_FALSE;
		DepthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
		DepthStencil.depthBoundsTestEnable = VK_FALSE;
		DepthStencil.stencilTestEnable = VK_FALSE;
		DepthStencil.front = {};
		DepthStencil.back = {};
		DepthStencil.minDepthBounds = 0.f;
		DepthStencil.maxDepthBounds = 1.f;
	}

	void PipelineConfig::Clear()
	{
		VertexInputInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		InputAssembly = { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		Rasterizer = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		ColorBlendAttachment = {};
		Multisampling = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		DepthStencil = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		RenderInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
		ShaderStages.clear();
	}

}