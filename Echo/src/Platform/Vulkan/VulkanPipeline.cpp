#include "pch.h"

#include "VulkanPipeline.h"
#include "VulkanFrameBuffer.h"

namespace Echo 
{

	VulkanPipeline::VulkanPipeline(VulkanDevice* device, ShaderLibrary library, const PipelineDesc& pipelineDescription)
		: m_Device(device), m_Library(library)
	{
		CreateShaderModules(pipelineDescription);
		InitPipelineLayout(pipelineDescription);
		CreateDescriptorSet(pipelineDescription);
		InitPipeline(pipelineDescription);
	}

	VulkanPipeline::~VulkanPipeline()
	{
		if (m_PipelineType == Compute) 
		{
			vkDestroyShaderModule(m_Device->GetDevice(), m_ComputeShader, nullptr);
		} else if (m_PipelineType == Graphics) 
		{
			vkDestroyShaderModule(m_Device->GetDevice(), m_VertexShader, nullptr);
			vkDestroyShaderModule(m_Device->GetDevice(), m_FragmentShader, nullptr);
			if (m_GeometryShader) 
			{
				vkDestroyShaderModule(m_Device->GetDevice(), m_GeometryShader, nullptr);
			}
		}

		vkDestroyDescriptorSetLayout(m_Device->GetDevice(), m_DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(m_Device->GetDevice(), m_DescriptorPool, nullptr);

		vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);
		vkDestroyPipeline(m_Device->GetDevice(), m_Pipeline, nullptr);

	}

	void VulkanPipeline::Bind()
	{
		VkCommandBuffer cmd = m_Device->GetActiveCommandBuffer();

		switch (m_PipelineType) 
		{
			case Compute:
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);
				if (m_DescriptorSet)
				{
					vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
				}
				break;
			case Graphics:
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
				if (m_DescriptorSet)
				{
					vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
				}
				break;
		}
	}

	void VulkanPipeline::WriteDescriptorBuffer(Ref<Buffer> buffer, uint32_t bufferSize, uint32_t binding)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = (VkBuffer) buffer->GetNativeBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = bufferSize;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_DescriptorSet;
		descriptorWrite.dstBinding = binding;  
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(m_Device->GetDevice(), 1, &descriptorWrite, 0, nullptr);
	}

	void VulkanPipeline::WriteDescriptorStorageImage(Ref<FrameBuffer> frameBuffer, uint32_t binding)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageView = ((VulkanFrameBuffer*)frameBuffer.get())->GetColorAttachment(0).ImageView;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		VkWriteDescriptorSet writeDescriptorSet{};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = m_DescriptorSet;
		writeDescriptorSet.dstBinding = binding;  
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		writeDescriptorSet.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(m_Device->GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
	}

	void VulkanPipeline::CreateShaderModules(const PipelineDesc& pipelineDescription)
	{
		if (pipelineDescription.ComputeShaderPath != nullptr) 
		{
			m_PipelineType = PipelineType::Compute;
			m_ShaderType = ShaderType::ComputeShader;

			auto source = m_Library.AddSpirvShader(pipelineDescription.ComputeShaderPath, m_ShaderType);
			m_ComputeShader = CreateShaderModule(source);
		}

		if (pipelineDescription.VertexShaderPath != nullptr)
		{
			m_PipelineType = PipelineType::Graphics;
			m_ShaderType = ShaderType::VertexShader;

			auto source = m_Library.AddSpirvShader(pipelineDescription.VertexShaderPath, m_ShaderType);
			m_VertexShader = CreateShaderModule(source);
		}

		if (pipelineDescription.FragmentShaderPath != nullptr)
		{
			m_PipelineType = PipelineType::Graphics;
			m_ShaderType = ShaderType::FragmentShader;

			auto source = m_Library.AddSpirvShader(pipelineDescription.FragmentShaderPath, m_ShaderType);
			m_FragmentShader = CreateShaderModule(source);
		}

		if (pipelineDescription.GeometryShaderPath != nullptr)
		{
			m_PipelineType = PipelineType::Graphics;
			m_ShaderType = ShaderType::GeometryShader;

			auto source = m_Library.AddSpirvShader(pipelineDescription.GeometryShaderPath, m_ShaderType);
			m_GeometryShader = CreateShaderModule(source);
		}

		if (pipelineDescription.RayTracingShaderPath != nullptr)
		{
			m_PipelineType = PipelineType::Raytracing;

			auto source = m_Library.AddSpirvShader(pipelineDescription.RayTracingShaderPath, m_ShaderType);
			m_RaytracingShader = CreateShaderModule(source);
		}
	}

	VkShaderModule VulkanPipeline::CreateShaderModule(SpirvData code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.GetSize();
		createInfo.pCode = code.GetData();
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

		if (vkCreatePipelineLayout(m_Device->GetDevice(), &layoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void VulkanPipeline::CreateDescriptorSet(const PipelineDesc& pipelineDescription)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;
		std::unordered_map<VkDescriptorType, uint32_t> descriptorCounts;

		auto MapDescriptorType = [](DescriptorType type) -> VkDescriptorType
		{
			switch (type)
			{
				case DescriptorType::UniformBuffer:
					return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				case DescriptorType::StorageBuffer:
					return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				case DescriptorType::SampledImage:
					return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				case DescriptorType::StorageImage:
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

		if (poolInfo.maxSets == 0) return;

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
		if (m_PipelineType == Compute) 
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

		if (m_PipelineType == Graphics) 
		{
			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			std::vector<VkVertexInputBindingDescription> bindingDescriptions;
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

			auto MapFormat = [](VertexFormat format) -> VkFormat
			{
				switch (format)
				{
					case VertexFormat::Float:  return VK_FORMAT_R32_SFLOAT;
					case VertexFormat::Float2: return VK_FORMAT_R32G32_SFLOAT;
					case VertexFormat::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
					case VertexFormat::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
					case VertexFormat::Int:    return VK_FORMAT_R32_SINT;
					case VertexFormat::Int2:   return VK_FORMAT_R32G32_SINT;
					case VertexFormat::Int3:   return VK_FORMAT_R32G32B32_SINT;
					case VertexFormat::Int4:   return VK_FORMAT_R32G32B32A32_SINT;
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
				attribute.format = MapFormat(attr.Format);
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
				case Topology::TriangleList:
					inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
					break;
				case Topology::TriangleStrip:
					inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
					break;
				case Topology::LineList:
					inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
					break;
				case Topology::LineStrip:
					inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
					break;
				case Topology::PointList:
					inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
					break;
			}
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			VkPipelineRasterizationStateCreateInfo rasterizer{};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = (pipelineDescription.FillMode == Fill::Wireframe) ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
			rasterizer.cullMode = (pipelineDescription.CullMode == Cull::Back) ? VK_CULL_MODE_BACK_BIT :
				(pipelineDescription.CullMode == Cull::Front) ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_NONE;
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

			if (pipelineDescription.GeometryShaderPath != nullptr)
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

			if (vkCreateGraphicsPipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create graphics pipeline!");
			}
		}
	}

	VkDescriptorSetLayout VulkanPipeline::CreateDescriptorSetLayout(const PipelineDesc& desc)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		auto MapDescriptorType = [](DescriptorType type) -> VkDescriptorType
		{
			switch (type)
			{
				case DescriptorType::UniformBuffer:
					return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				case DescriptorType::StorageBuffer:
					return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				case DescriptorType::SampledImage:
					return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				case DescriptorType::StorageImage:
					return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				default:
					throw std::runtime_error("Unknown Descriptor Type");
			}
		};

		auto MapShaderStage = [](ShaderStage stage) -> VkShaderStageFlags
		{
			switch (stage)
			{
				case ShaderStage::Vertex:
					return VK_SHADER_STAGE_VERTEX_BIT;
				case ShaderStage::Fragment:
					return VK_SHADER_STAGE_FRAGMENT_BIT;
				case ShaderStage::Compute:
					return VK_SHADER_STAGE_COMPUTE_BIT;
				case ShaderStage::All:
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

}