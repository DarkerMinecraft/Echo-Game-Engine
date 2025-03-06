#include "pch.h"
#include "VulkanPipeline.h"
#include "VulkanImage.h"
#include "VulkanMaterial.h"
#include "VulkanCommandBuffer.h"
#include <unordered_set>

namespace Echo 
{

	VulkanPipeline::VulkanPipeline(Device* device, Ref<Material> material, PipelineDesc& desc)
		: m_Device((VulkanDevice*)device), m_Material(material)
	{
		m_PipelineType = Graphics;
		CreateGraphicsPipeline(material, desc);
	}

	VulkanPipeline::VulkanPipeline(Device* device, const char* computeFilePath, PipelineDesc& desc)
		: m_Device((VulkanDevice*)device)
	{
		m_PipelineType = Compute;
		CreateComputePipeline(computeFilePath, desc);
	}

	VulkanPipeline::~VulkanPipeline()
	{
		if (m_PipelineType == Compute)
		{
			vkDestroyShaderModule(m_Device->GetDevice(), m_ComputeShaderModule, nullptr);
		}
		else if (m_PipelineType == Graphics)
		{
			m_Material->Destroy();
		}

		if (m_DescriptorSet != nullptr)
		{
			vkDestroyDescriptorSetLayout(m_Device->GetDevice(), m_DescriptorSetLayout, nullptr);
			vkDestroyDescriptorPool(m_Device->GetDevice(), m_DescriptorPool, nullptr);
		}

		vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);
		vkDestroyPipeline(m_Device->GetDevice(), m_Pipeline, nullptr);
	}

	void VulkanPipeline::Bind(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();

		if (m_PipelineType == Graphics)
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
			if (HasDescriptorSet())
			{
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
			}
		}
		else if (m_PipelineType == Compute)
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);
			if (HasDescriptorSet())
			{
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
			}
		}
	}

	void VulkanPipeline::WriteDesciptorStorageImage(Ref<Image> image, uint32_t binding)
	{
		VulkanImage* img = (VulkanImage*)image.get();

		VkDescriptorImageInfo imgInfo{};
		imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imgInfo.imageView = img->GetImage().ImageView;

		VkWriteDescriptorSet drawImageWrite = {};
		drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		drawImageWrite.pNext = nullptr;

		drawImageWrite.dstBinding = 0;
		drawImageWrite.dstSet = m_DescriptorSet;
		drawImageWrite.descriptorCount = 1;
		drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		drawImageWrite.pImageInfo = &imgInfo;

		vkUpdateDescriptorSets(m_Device->GetDevice(), 1, &drawImageWrite, 0, nullptr);
	}

	void VulkanPipeline::CreateComputePipeline(const char* computeFilePath, PipelineDesc& desc)
	{
		auto source = m_Device->GetShaderLibrary().AddSpirvShader(std::string(computeFilePath), ShaderType::ComputeShader);
		
		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = source.GetSize();
		shaderModuleCreateInfo.pCode = source.GetData();
		vkCreateShaderModule(m_Device->GetDevice(), &shaderModuleCreateInfo, nullptr, &m_ComputeShaderModule);

		CreatePipelineLayout(desc.DescriptionSetLayouts);
		CreateDescriptorSet(desc.DescriptionSetLayouts, desc.MaxSets);

		VkComputePipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.layout = m_PipelineLayout;

		VkPipelineShaderStageCreateInfo shaderStageCreateInfo{};
		shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		shaderStageCreateInfo.module = m_ComputeShaderModule;
		shaderStageCreateInfo.pName = "main";

		pipelineCreateInfo.stage = shaderStageCreateInfo;

		vkCreateComputePipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_Pipeline);
	}

	void VulkanPipeline::CreateGraphicsPipeline(Ref<Material> material, PipelineDesc& desc)
	{
		CreatePipelineLayout(desc.DescriptionSetLayouts);
		CreateDescriptorSet(desc.DescriptionSetLayouts, desc.MaxSets);

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

		std::unordered_set<uint32_t> createdBindings;

		for (const auto& attr : desc.VertexAttributes)
		{
			if (createdBindings.find(attr.Binding) == createdBindings.end())
			{
				createdBindings.insert(attr.Binding);

				VkVertexInputBindingDescription binding{};
				binding.binding = attr.Binding;
				binding.stride = attr.Stride;
				binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				bindingDescriptions.push_back(binding);
			}

			VkVertexInputAttributeDescription attribute{};
			attribute.binding = attr.Binding;
			attribute.location = attr.Location;
			attribute.format = MapFormat(attr.Format);
			attribute.offset = attr.Offset;
			attributeDescriptions.push_back(attribute);
		}

		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		switch (desc.GraphicsTopology)
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
		rasterizer.polygonMode = (desc.FillMode == Fill::Wireframe) ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
		rasterizer.cullMode = (desc.CullMode == Cull::Back) ? VK_CULL_MODE_BACK_BIT :
			(desc.CullMode == Cull::Front) ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_NONE;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = desc.EnableDepthTest ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable = desc.EnableDepthWrite ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = desc.EnableBlending ? VK_TRUE : VK_FALSE;

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

		auto shaderStages = ((VulkanMaterial*)material.get())->GetShaderStages();

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext = nullptr;

		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VulkanImage* img = (VulkanImage*)desc.RenderTarget.get();

		VkPipelineRenderingCreateInfo renderInfo{};
		renderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		renderInfo.colorAttachmentCount = 1;
		VkFormat format = img->GetImage().ImageFormat;
		renderInfo.pColorAttachmentFormats = &format;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = &renderInfo;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = VK_NULL_HANDLE;
		pipelineInfo.subpass = 0;

		vkCreateGraphicsPipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);
	}

	void VulkanPipeline::CreatePipelineLayout(std::vector<PipelineDesc::DescriptionSetLayout> descriptorSetLayout)
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

		for (const auto& layout : descriptorSetLayout)
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

		if (bindings.size() > 0)
		{
			vkCreateDescriptorSetLayout(m_Device->GetDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout);
		}

		VkPipelineLayoutCreateInfo layoutCreateInfo{};

		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.pNext = nullptr;

		if (bindings.size() > 0)
		{
			layoutCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
			layoutCreateInfo.setLayoutCount = 1;
		}

		vkCreatePipelineLayout(m_Device->GetDevice(), &layoutCreateInfo, nullptr, &m_PipelineLayout);
	}

	void VulkanPipeline::CreateDescriptorSet(std::vector<PipelineDesc::DescriptionSetLayout> descriptorSetLayout, uint32_t maxSets)
	{
		if (maxSets == 0) return;

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

		for (const auto& layout : descriptorSetLayout)
		{
			VkDescriptorType vkType = MapDescriptorType(layout.Type);
			descriptorCounts[vkType] += layout.Count * maxSets;
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
		poolInfo.maxSets = maxSets;

		vkCreateDescriptorPool(m_Device->GetDevice(), &poolInfo, nullptr, &m_DescriptorPool);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_DescriptorSetLayout;

		vkAllocateDescriptorSets(m_Device->GetDevice(), &allocInfo, &m_DescriptorSet);
	}

}