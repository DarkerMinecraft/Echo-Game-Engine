#include "pch.h"
#include "VulkanResource.h"
#include "VulkanSwapchain.h"

#include "Echo/Core/Application.h"

#include <fstream>
#include "VulkanCommandList.h"

namespace Echo 
{
	
	VulkanResource::VulkanResource(const ResourceCreateInfo& createInfo)
		: m_CreateInfo(createInfo), m_Device((VulkanDevice*) Application::Get().GetWindow().GetDevice())
	{
		switch (createInfo.Resource) 
		{
			case AssetResource::GraphicsShader: 
			{
				CreateGraphicsPipeline();
			}
		}
	}

	VulkanResource::~VulkanResource()
	{
		switch (m_CreateInfo.Resource)
		{
			case AssetResource::GraphicsShader:
			{
				vkDestroyShaderModule(m_Device->GetDevice(), m_FragmentShaderModule, nullptr);
				vkDestroyShaderModule(m_Device->GetDevice(), m_VertexShaderModule, nullptr);

				vkDestroyPipeline(m_Device->GetDevice(), m_GraphicsPipeline, nullptr);
				vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);
			}
		}
	}

	void VulkanResource::Bind()
	{
		switch (m_CreateInfo.Resource) 
		{
			case AssetResource::GraphicsShader: vkCmdBindPipeline(((VulkanCommandBuffer*)m_Device->GetCommandBuffer())->GetBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
		}
	}

	void VulkanResource::Unbind()
	{

	}

	void VulkanResource::DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo)
	{
		configInfo.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		configInfo.ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.ViewportInfo.viewportCount = 1;
		configInfo.ViewportInfo.pViewports = nullptr;
		configInfo.ViewportInfo.scissorCount = 1;
		configInfo.ViewportInfo.pScissors = nullptr;

		configInfo.RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.RasterizationInfo.depthClampEnable = VK_FALSE;
		configInfo.RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.RasterizationInfo.lineWidth = 1.0f;
		configInfo.RasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		configInfo.RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.RasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.RasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		configInfo.RasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		configInfo.RasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

		configInfo.MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.MultisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.MultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.MultisampleInfo.minSampleShading = 1.0f;           // Optional
		configInfo.MultisampleInfo.pSampleMask = nullptr;             // Optional
		configInfo.MultisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		configInfo.MultisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

		configInfo.ColorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		configInfo.ColorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		configInfo.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		configInfo.ColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.ColorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.ColorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		configInfo.ColorBlendInfo.attachmentCount = 1;
		configInfo.ColorBlendInfo.pAttachments = &configInfo.ColorBlendAttachment;
		configInfo.ColorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		configInfo.ColorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		configInfo.ColorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		configInfo.ColorBlendInfo.blendConstants[3] = 0.0f;  // Optional

		configInfo.DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.DepthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.DepthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.DepthStencilInfo.minDepthBounds = 0.0f;  // Optional
		configInfo.DepthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		configInfo.DepthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.DepthStencilInfo.front = {};  // Optional
		configInfo.DepthStencilInfo.back = {};   // Optional

		configInfo.DynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		configInfo.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		configInfo.DynamicStateInfo.pDynamicStates = configInfo.DynamicStateEnables.data();
		configInfo.DynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.DynamicStateEnables.size());
		configInfo.DynamicStateInfo.flags = 0;
	}

	void VulkanResource::CreateGraphicsPipeline()
	{
		auto vertCode = ReadFile(m_CreateInfo.VertexShader);
		auto fragCode = ReadFile(m_CreateInfo.FragmentShader);

		CompilationInfo vertexCompilationInfo{};
		vertexCompilationInfo.Kind = shaderc_vertex_shader;
		vertexCompilationInfo.Source = vertCode;
		vertexCompilationInfo.FilePath = m_CreateInfo.VertexShader.c_str();
		vertexCompilationInfo.Options.SetOptimizationLevel(shaderc_optimization_level_performance);
		vertexCompilationInfo.Options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		vertexCompilationInfo.Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		PreprocessShader(vertexCompilationInfo);
		CompileFileToAssembly(vertexCompilationInfo);
		m_VertexShaderModule = CreateShaderModule(CompileFile(vertexCompilationInfo));

		CompilationInfo fragCompilationInfo{};
		fragCompilationInfo.Kind = shaderc_fragment_shader;
		fragCompilationInfo.Source = fragCode;
		fragCompilationInfo.FilePath = m_CreateInfo.FragmentShader.c_str();
		fragCompilationInfo.Options.SetOptimizationLevel(shaderc_optimization_level_performance);
		fragCompilationInfo.Options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		fragCompilationInfo.Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		PreprocessShader(fragCompilationInfo);
		CompileFileToAssembly(fragCompilationInfo);
		m_FragmentShaderModule = CreateShaderModule(CompileFile(fragCompilationInfo));

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = m_VertexShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = m_FragmentShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(m_Device->GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}

		PipelineConfigInfo configInfo{};
		DefaultPipelineConfigInfo(configInfo);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

		auto bindingDescriptions = GetBindingDescription();
		auto attributeDescriptions = GetAttributeDescriptions();

		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.InputAssemblyInfo;
		pipelineInfo.pViewportState = &configInfo.ViewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.RasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.MultisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.ColorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.DepthStencilInfo;
		pipelineInfo.pDynamicState = &configInfo.DynamicStateInfo;

		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = ((VulkanSwapchain*)m_Device->GetSwapchain())->GetRenderPass();
		pipelineInfo.subpass = configInfo.Subpass;

		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline!");
		}
	}

	VkShaderModule VulkanResource::CreateShaderModule(const std::vector<char>& code)
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

	std::vector<char> VulkanResource::ReadFile(const std::string& filePath)
	{
		std::vector<char> result;
		std::ifstream file(filePath, std::ios::binary);
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
	}

	void VulkanResource::PreprocessShader(CompilationInfo& info)
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

	void VulkanResource::CompileFileToAssembly(CompilationInfo& info)
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

	std::vector<char> VulkanResource::CompileFile(CompilationInfo& info)
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
