#include "pch.h"
#include "VulkanPipeline.h"

#include <fstream>

#include <dxc/dxcapi.h>

namespace Echo 
{

	VulkanPipeline::VulkanPipeline(VulkanDevice* device, const PipelineDesc& pipelineDescription)
		: m_Device(device), m_PushConstantOffset(pipelineDescription.PushConstants.Offset), m_PushConstantSize(pipelineDescription.PushConstants.Size)
	{
		CreateShaderModules(pipelineDescription);
		InitPipelineLayout(pipelineDescription);
		CreateDescriptorSet(pipelineDescription);
		InitPipeline(pipelineDescription);
	}

	VulkanPipeline::~VulkanPipeline()
	{
		if (m_Type == Compute) 
		{
			vkDestroyShaderModule(m_Device->GetDevice(), m_ComputeShader, nullptr);
		} else if (m_Type == Graphics) 
		{
			vkDestroyShaderModule(m_Device->GetDevice(), m_VertexShader, nullptr);
			vkDestroyShaderModule(m_Device->GetDevice(), m_FragmentShader, nullptr);
			if (m_GeometryShader) 
			{
				vkDestroyShaderModule(m_Device->GetDevice(), m_GeometryShader, nullptr);
			}
		}

		vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);
		vkDestroyPipeline(m_Device->GetDevice(), m_Pipeline, nullptr);
	}

	void VulkanPipeline::Bind()
	{
		VkCommandBuffer cmd = m_Device->GetActiveCommandBuffer();

		switch (m_Type) 
		{
			case Compute:
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
				break;
			case Graphics:
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
				break;
		}
	}

	void VulkanPipeline::WritePushConstants(const void* pushConstants)
	{
		if (!pushConstants) return;
		VkCommandBuffer cmd = m_Device->GetActiveCommandBuffer();

		switch (m_Type) 
		{
			case Compute:
				vkCmdPushConstants(cmd, m_PipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, m_PushConstantOffset, m_PushConstantSize, pushConstants);
				break;
			case Graphics:
				vkCmdPushConstants(cmd, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, m_PushConstantOffset, m_PushConstantSize, pushConstants);
				break;
		}
	}

	void VulkanPipeline::CreateShaderModules(const PipelineDesc& pipelineDescription)
	{
		if (pipelineDescription.ComputeShaderPath != "") 
		{
			m_Type = PipelineType::Compute;

			auto source = ReadFile(pipelineDescription.ComputeShaderPath);
			m_ComputeShader = CreateShaderModule(source);
		}

		if (pipelineDescription.VertexShaderPath != "")
		{
			m_Type = PipelineType::Graphics;

			auto source = ReadFile(pipelineDescription.VertexShaderPath);
			m_VertexShader = CreateShaderModule(source);
		}

		if (pipelineDescription.FragmentShaderPath != "")
		{
			m_Type = PipelineType::Graphics;

			auto source = ReadFile(pipelineDescription.FragmentShaderPath);
			m_FragmentShader = CreateShaderModule(source);
		}

		if (pipelineDescription.GeometryShaderPath != "")
		{
			m_Type = PipelineType::Graphics;

			auto source = ReadFile(pipelineDescription.GeometryShaderPath);
			m_GeometryShader = CreateShaderModule(source);
		}

		if (pipelineDescription.RayTracingShaderPath != "")
		{
			m_Type = PipelineType::Raytracing;

			auto source = ReadFile(pipelineDescription.RayTracingShaderPath);
			m_RaytracingShader = CreateShaderModule(source);
		}
	}

	VkShaderModule VulkanPipeline::CreateShaderModule(const std::vector<uint32_t>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = code.data();
		VkShaderModule shaderModule;

		if (vkCreateShaderModule(m_Device->GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module!");
		}

		return shaderModule;
	}

	void VulkanPipeline::InitPipelineLayout(const PipelineDesc& pipelineDescription)
	{
		VkPipelineLayoutCreateInfo layoutCreateInfo{};

		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.pNext = nullptr;
		m_DescriptorSetLayout = CreateDescriptorSetLayout(pipelineDescription);
		layoutCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
		layoutCreateInfo.setLayoutCount = 1;
		auto pushConstants = CreatePushConstants(pipelineDescription);
		layoutCreateInfo.pPushConstantRanges = pushConstants.data();
		layoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());

		if (vkCreatePipelineLayout(m_Device->GetDevice(), &layoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void VulkanPipeline::CreateDescriptorSet(const PipelineDesc& pipelineDescription)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;

		std::unordered_map<VkDescriptorType, uint32_t> descriptorCounts;

		auto MapDescriptorType = [](PipelineDesc::DescriptorSetLayout::DescriptorType type) -> VkDescriptorType
		{
			switch (type)
			{
				case PipelineDesc::DescriptorSetLayout::DescriptorType::UniformBuffer:
					return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				case PipelineDesc::DescriptorSetLayout::DescriptorType::StorageBuffer:
					return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				case PipelineDesc::DescriptorSetLayout::DescriptorType::SampledImage:
					return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				case PipelineDesc::DescriptorSetLayout::DescriptorType::StorageImage:
					return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				default:
					throw std::runtime_error("Unknown Descriptor Type");
			}
		};

		for (const auto& layout : pipelineDescription.DescriptorSetLayouts)
		{
			VkDescriptorType vkType = MapDescriptorType(layout.Type);
			descriptorCounts[vkType] += layout.Count * pipelineDescription.MaxSets;
		}

		for (auto& [type, count] : descriptorCounts)
		{
			VkDescriptorPoolSize poolSize{};
			poolSize.type = type;
			poolSize.descriptorCount = count;
			poolSizes.push_back(poolSize);
		}

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = pipelineDescription.MaxSets;

		if (vkCreateDescriptorPool(m_Device->GetDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor pool!");
		}

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;  
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_DescriptorSetLayout;

		if (vkAllocateDescriptorSets(m_Device->GetDevice(), &allocInfo, &m_DescriptorSet) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate descriptor set!");
		}
	}

	void VulkanPipeline::InitPipeline(const PipelineDesc& pipelineDescription)
	{
		if (m_Type == Compute) 
		{
			VkComputePipelineCreateInfo computePipelineInfo{};
			computePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			computePipelineInfo.layout = m_PipelineLayout;

			VkPipelineShaderStageCreateInfo computeShaderStage{};
			computeShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			computeShaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
			computeShaderStage.module = m_ComputeShader;
			computeShaderStage.pName = "main";

			computePipelineInfo.stage = computeShaderStage;

			if (vkCreateComputePipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &computePipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create compute pipeline!");
			}
		}

		if (m_Type == Graphics) 
		{
			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			std::vector<VkVertexInputBindingDescription> bindingDescriptions;
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

			auto MapFormat = [](PipelineDesc::VertexAttribute::Format format) -> VkFormat
			{
				switch (format)
				{
					case PipelineDesc::VertexAttribute::Format::Float:  return VK_FORMAT_R32_SFLOAT;
					case PipelineDesc::VertexAttribute::Format::Float2: return VK_FORMAT_R32G32_SFLOAT;
					case PipelineDesc::VertexAttribute::Format::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
					case PipelineDesc::VertexAttribute::Format::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
					case PipelineDesc::VertexAttribute::Format::Int:    return VK_FORMAT_R32_SINT;
					case PipelineDesc::VertexAttribute::Format::Int2:   return VK_FORMAT_R32G32_SINT;
					case PipelineDesc::VertexAttribute::Format::Int3:   return VK_FORMAT_R32G32B32_SINT;
					case PipelineDesc::VertexAttribute::Format::Int4:   return VK_FORMAT_R32G32B32A32_SINT;
					default: throw std::runtime_error("Unknown vertex attribute format.");
				}
			};

			for (const auto& attr : pipelineDescription.VertexAttributes)
			{
				VkVertexInputBindingDescription binding{};
				binding.binding = attr.Binding;
				binding.stride = attr.Stride;
				binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				bindingDescriptions.push_back(binding);

				VkVertexInputAttributeDescription attribute{};
				attribute.binding = attr.Binding;
				attribute.location = attr.Location;
				attribute.format = MapFormat(attr.VertexFormat);
				attributeDescriptions.push_back(attribute);
			}

			vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			switch (pipelineDescription.GraphicsTopology)
			{
				case PipelineDesc::Topology::TriangleList:
					inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
					break;
				case PipelineDesc::Topology::TriangleStrip:
					inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
					break;
				case PipelineDesc::Topology::LineList:
					inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
					break;
				case PipelineDesc::Topology::LineStrip:
					inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
					break;
				case PipelineDesc::Topology::PointList:
					inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
					break;
			}
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			VkPipelineRasterizationStateCreateInfo rasterizer{};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = (pipelineDescription.FillMode == PipelineDesc::FillMode::Wireframe) ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
			rasterizer.cullMode = (pipelineDescription.CullMode == PipelineDesc::CullMode::Back) ? VK_CULL_MODE_BACK_BIT :
				(pipelineDescription.CullMode == PipelineDesc::CullMode::Front) ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_NONE;
			rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterizer.lineWidth = 1.0f;

			VkPipelineMultisampleStateCreateInfo multisampling{};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

			VkPipelineDepthStencilStateCreateInfo depthStencil{};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable = pipelineDescription.EnableDepthTest ? VK_TRUE : VK_FALSE;
			depthStencil.depthWriteEnable = pipelineDescription.EnableDepthWrite ? VK_TRUE : VK_FALSE;
			depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
			depthStencil.depthBoundsTestEnable = VK_FALSE;
			depthStencil.stencilTestEnable = VK_FALSE;

			VkPipelineColorBlendAttachmentState colorBlendAttachment{};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = pipelineDescription.EnableBlending ? VK_TRUE : VK_FALSE;

			VkPipelineColorBlendStateCreateInfo colorBlending{};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment;

			std::vector<VkDynamicState> dynamicStates = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};

			VkPipelineDynamicStateCreateInfo dynamicState{};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			dynamicState.pDynamicStates = dynamicStates.data();

			std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};

			shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
			shaderStages[0].module = m_VertexShader;
			shaderStages[0].pName = "main";

			shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			shaderStages[1].module = m_FragmentShader;
			shaderStages[1].pName = "main";

			if (pipelineDescription.GeometryShaderPath != "")
			{
				shaderStages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStages[2].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
				shaderStages[2].module = m_GeometryShader;
				shaderStages[2].pName = "main";
			}

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
			pipelineInfo.pStages = shaderStages.data();
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = nullptr;  
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = &depthStencil;
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = &dynamicState;
			pipelineInfo.layout = m_PipelineLayout;
			pipelineInfo.renderPass = VK_NULL_HANDLE;
			pipelineInfo.subpass = 0;
		}
	}

	std::vector<uint32_t> VulkanPipeline::ReadFile(const std::string& filePath)
	{
		const char* source = LoadShaderCode(filePath);

		// Initialize DXC compiler and utils
		CComPtr<IDxcCompiler3> compiler;
		CComPtr<IDxcUtils> utils;
		CComPtr<IDxcIncludeHandler> includeHandler;

		if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))) ||
			FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))))
		{
			throw std::runtime_error("Failed to initialize DXC compiler.");
		}

		utils->CreateDefaultIncludeHandler(&includeHandler);

		// Create a blob from the shader source
		CComPtr<IDxcBlobEncoding> sourceBlob;
		utils->CreateBlob(source, static_cast<UINT32>(sizeof(source)), CP_UTF8, &sourceBlob);

		// Prepare DXC arguments
		LPCWSTR args[] = {
			L"-E", L"main",           // Entry point
			L"-T", L"vs_6_0",         // Target profile (vs_6_0, ps_6_0, cs_6_0, etc.)
			L"-spirv",                // Compile to SPIR-V
			L"-fvk-use-dx-layout"     // Use DX-like layout for Vulkan
		};

		// Compile to SPIR-V
		DxcBuffer sourceBuffer;
		sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
		sourceBuffer.Size = sourceBlob->GetBufferSize();
		sourceBuffer.Encoding = DXC_CP_UTF8;

		CComPtr<IDxcResult> result;
		compiler->Compile(
			&sourceBuffer,
			args, _countof(args),
			includeHandler,
			IID_PPV_ARGS(&result)
		);

		// Check for compilation errors
		CComPtr<IDxcBlobUtf8> errors;
		result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
		if (errors && errors->GetStringLength() > 0)
		{
			std::cerr << "Shader Compilation Failed:\n" << errors->GetStringPointer() << std::endl;
			throw std::runtime_error("Shader compilation failed.");
		}

		// Retrieve the SPIR-V binary
		CComPtr<IDxcBlob> spirvBlob;
		result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&spirvBlob), nullptr);

		// Convert to std::vector<uint32_t>
		std::vector<uint32_t> spirv(spirvBlob->GetBufferSize() / sizeof(uint32_t));
		memcpy(spirv.data(), spirvBlob->GetBufferPointer(), spirvBlob->GetBufferSize());

		return spirv;
	}

	const char* VulkanPipeline::LoadShaderCode(const std::string& filePath)
	{
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open shader file.");
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::string buffer(fileSize, '\0');

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer.c_str();
	}

	VkDescriptorSetLayout VulkanPipeline::CreateDescriptorSetLayout(const PipelineDesc& desc)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		auto MapDescriptorType = [](PipelineDesc::DescriptorSetLayout::DescriptorType type) -> VkDescriptorType
		{
			switch (type)
			{
				case PipelineDesc::DescriptorSetLayout::DescriptorType::UniformBuffer:
					return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				case PipelineDesc::DescriptorSetLayout::DescriptorType::StorageBuffer:
					return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				case PipelineDesc::DescriptorSetLayout::DescriptorType::SampledImage:
					return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				case PipelineDesc::DescriptorSetLayout::DescriptorType::StorageImage:
					return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				default:
					throw std::runtime_error("Unknown Descriptor Type");
			}
		};

		auto MapShaderStage = [](PipelineDesc::DescriptorSetLayout::ShaderStage stage) -> VkShaderStageFlags
		{
			switch (stage)
			{
				case PipelineDesc::DescriptorSetLayout::ShaderStage::Vertex:
					return VK_SHADER_STAGE_VERTEX_BIT;
				case PipelineDesc::DescriptorSetLayout::ShaderStage::Fragment:
					return VK_SHADER_STAGE_FRAGMENT_BIT;
				case PipelineDesc::DescriptorSetLayout::ShaderStage::Compute:
					return VK_SHADER_STAGE_COMPUTE_BIT;
				case PipelineDesc::DescriptorSetLayout::ShaderStage::All:
					return VK_SHADER_STAGE_ALL;
				default:
					return VK_SHADER_STAGE_ALL;
			}
		};

		for (const auto& layout : desc.DescriptorSetLayouts)
		{
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = layout.Binding;
			binding.descriptorType = MapDescriptorType(layout.Type);
			binding.descriptorCount = layout.Count;
			binding.stageFlags = MapShaderStage(layout.Stage);
			binding.pImmutableSamplers = nullptr;  

			bindings.push_back(binding);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		VkDescriptorSetLayout descriptorSetLayout;
		if (vkCreateDescriptorSetLayout(m_Device->GetDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor set layout!");
		}

		return descriptorSetLayout;
	}


	std::vector<VkPushConstantRange> VulkanPipeline::CreatePushConstants(const PipelineDesc& desc)
	{
		std::vector<VkPushConstantRange> pushConstantRanges;

		auto MapShaderStage = [](PipelineDesc::DescriptorSetLayout::ShaderStage stage) -> VkShaderStageFlags
		{
			switch (stage)
			{
				case PipelineDesc::DescriptorSetLayout::ShaderStage::Vertex:
					return VK_SHADER_STAGE_VERTEX_BIT;
				case PipelineDesc::DescriptorSetLayout::ShaderStage::Fragment:
					return VK_SHADER_STAGE_FRAGMENT_BIT;
				case PipelineDesc::DescriptorSetLayout::ShaderStage::Compute:
					return VK_SHADER_STAGE_COMPUTE_BIT;
				case PipelineDesc::DescriptorSetLayout::ShaderStage::All:
					return VK_SHADER_STAGE_ALL;
				default:
					return VK_SHADER_STAGE_ALL;
			}
		};

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.offset = desc.PushConstants.Offset;
		pushConstantRange.size = desc.PushConstants.Size;
		pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;

		pushConstantRanges.push_back(pushConstantRange);

		return pushConstantRanges;
	}

}