#include "pch.h"
#include "VulkanPipeline.h"
#include "VulkanFramebuffer.h"
#include "VulkanMaterial.h"
#include "VulkanCommandBuffer.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "VulkanRenderCaps.h"

#include <unordered_set>

namespace Echo
{

	static VkFormat ShaderDataTypeToVulkanFormat(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:     return VK_FORMAT_R32_SFLOAT;
			case ShaderDataType::Float2:    return VK_FORMAT_R32G32_SFLOAT;
			case ShaderDataType::Float3:    return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::Float4:    return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::Int:       return VK_FORMAT_R32_SINT;
			case ShaderDataType::Int2:      return VK_FORMAT_R32G32_SINT;
			case ShaderDataType::Int3:      return VK_FORMAT_R32G32B32_SINT;
			case ShaderDataType::Int4:      return VK_FORMAT_R32G32B32A32_SINT;
			case ShaderDataType::Bool:      return VK_FORMAT_R8_UINT;
			default: return VK_FORMAT_UNDEFINED;
		}
	}

	VulkanPipeline::VulkanPipeline(Device* device, Ref<Shader> shader, const PipelineSpecification& spec)
		: m_Device((VulkanDevice*) device)
	{
		if (shader->IsCompute()) 
		{
			m_PipelineType = PipelineType::Compute;
			CreateComputePipeline(shader, spec);
		}
		else 
		{
			m_PipelineType = PipelineType::Graphics;
			CreateGraphicsPipeline(shader, spec);
		}
	}

	VulkanPipeline::~VulkanPipeline()
	{
		Destroy();
	}

	void VulkanPipeline::Bind(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();

		if (m_PipelineType == PipelineType::Graphics)
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
			if (HasDescriptorSet())
			{
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
			}
		}
		else if (m_PipelineType == PipelineType::Graphics)
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);
			if (HasDescriptorSet())
			{
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
			}
		}
	}

	void VulkanPipeline::BindResource(uint32_t binding, Ref<Texture2D> texture)
	{
		VulkanTexture2D* tex = (VulkanTexture2D*)texture.get();

		DescriptorWriter writer;
		writer.WriteImage(binding, tex->GetTexture().ImageView, tex->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		writer.UpdateSet(m_Device->GetDevice(), m_DescriptorSet);
	}

	void VulkanPipeline::BindResource(uint32_t binding, Ref<Framebuffer> framebuffer, uint32_t index)
	{
		VulkanFramebuffer* fb = (VulkanFramebuffer*)framebuffer.get();

		if (fb->GetCurrentLayout(index) != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
			{
				fb->TransitionImageLayout(cmd, index, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			});
		}

		DescriptorWriter writer;
		writer.WriteImage(binding, fb->GetImage(index).ImageView, fb->GetSampler(index), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		writer.UpdateSet(m_Device->GetDevice(), m_DescriptorSet);
	}

	void VulkanPipeline::BindResource(uint32_t binding, Ref<Texture2D> tex, uint32_t index)
	{
		VulkanTexture2D* texture = (VulkanTexture2D*)tex.get();

		DescriptorWriter writer;
		writer.WriteImage(index, binding, texture->GetTexture().ImageView, texture->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		writer.UpdateSet(m_Device->GetDevice(), m_DescriptorSet);
	}

	void VulkanPipeline::BindResource(uint32_t binding, Ref<UniformBuffer> uniformBuffer)
	{
		VulkanUniformBuffer* ubo = (VulkanUniformBuffer*)uniformBuffer.get();

		DescriptorWriter writer;
		writer.WriteBuffer(binding, ubo->GetBuffer().Buffer, ubo->GetSize(), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		writer.UpdateSet(m_Device->GetDevice(), m_DescriptorSet);
	}

	void VulkanPipeline::Destroy()
	{
		if (m_Destroyed) return;

		if (m_DescriptorSet != nullptr)
		{
			vkDestroyDescriptorSetLayout(m_Device->GetDevice(), m_DescriptorSetLayout, nullptr);
			m_DescriptorAllocator.DestroyPools(m_Device->GetDevice());
		}

		vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);
		vkDestroyPipeline(m_Device->GetDevice(), m_Pipeline, nullptr);

		m_Destroyed = true;
	}

	void VulkanPipeline::CreateComputePipeline(Ref<Shader> computeShader, const PipelineSpecification& spec)
	{
		std::vector<DescriptionSetLayout> layouts = CreateLayout(computeShader);
		CreatePipelineLayout(layouts);
		CreateDescriptorSet(layouts);

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = ((VulkanShader*)computeShader.get())->GetShaderStages();

		VkComputePipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.pNext = nullptr;
		pipelineCreateInfo.layout = m_PipelineLayout;
		pipelineCreateInfo.stage = shaderStages[0];

		vkCreateComputePipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_Pipeline);
	}

	void VulkanPipeline::CreateGraphicsPipeline(Ref<Shader> graphicsShader, const PipelineSpecification& spec)
	{
		std::vector<DescriptionSetLayout> layouts = CreateLayout(graphicsShader);
		CreatePipelineLayout(layouts);
		CreateDescriptorSet(layouts);

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

		BufferLayout layout = graphicsShader->GetVertexReflection().GetVertexLayout();
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = layout.GetStride();
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		bindingDescriptions.push_back(bindingDescription);

		uint32_t count = 0;
		for (const auto& element : layout)
		{
			VkVertexInputAttributeDescription attributeDescription{};
			attributeDescription.binding = 0;
			attributeDescription.location = count;
			attributeDescription.format = ShaderDataTypeToVulkanFormat(element.Type);
			attributeDescription.offset = element.Offset;
			attributeDescriptions.push_back(attributeDescription);

			count++;
		}

		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		switch (spec.GraphicsTopology)
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
		rasterizer.polygonMode = (spec.FillMode == Fill::Wireframe) ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
		rasterizer.cullMode = (spec.CullMode == Cull::Back) ? VK_CULL_MODE_BACK_BIT :
			(spec.CullMode == Cull::Front) ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_NONE;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.lineWidth = 1.0f;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = spec.EnableDepthTest ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable = spec.EnableDepthWrite ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext = nullptr;

		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VulkanFramebuffer* fb = (VulkanFramebuffer*)spec.RenderTarget.get();

		VkPipelineRenderingCreateInfo renderInfo{};
		renderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		renderInfo.colorAttachmentCount = fb->GetColorFormats().size();
		std::vector<VkFormat> formats = fb->GetColorFormats();
		renderInfo.pColorAttachmentFormats = formats.data();

		if (fb->HasDepthImage())
		{
			renderInfo.depthAttachmentFormat = fb->GetDepthImage().ImageFormat;
		}

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(formats.size());

		for (size_t i = 0; i < formats.size(); i++)
		{
			if (formats[i] == VK_FORMAT_R32_SINT)
			{
				// Integer formats don't support blending
				colorBlendAttachments[i].blendEnable = VK_FALSE;
				colorBlendAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT; // Only R component for R32_SINT
			}
			else
			{
				// Regular color formats
				colorBlendAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
					VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				colorBlendAttachments[i].blendEnable = spec.EnableBlending ? VK_TRUE : VK_FALSE;

				if (spec.EnableBlending)
				{
					// Set up blend factors only if blending is enabled
					colorBlendAttachments[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
					colorBlendAttachments[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
					colorBlendAttachments[i].colorBlendOp = VK_BLEND_OP_ADD;
					colorBlendAttachments[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
					colorBlendAttachments[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
					colorBlendAttachments[i].alphaBlendOp = VK_BLEND_OP_ADD;
				}
			}
		}

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.attachmentCount = colorBlendAttachments.size();
		colorBlending.pAttachments = colorBlendAttachments.data();

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = ((VulkanShader*)graphicsShader.get())->GetShaderStages();

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

	void VulkanPipeline::CreatePipelineLayout(std::vector<DescriptionSetLayout> descriptorSetLayout)
	{
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

		DescriptorLayoutBuilder builder;
		int count = 0;
		for (const auto& layout : descriptorSetLayout)
		{
			builder.AddBinding(layout.Binding, layout.Count, MapShaderStage(layout.Stage), MapDescriptorType(layout.Type));
			count++;
		}

		if (count != 0)
			m_DescriptorSetLayout = builder.Build(m_Device->GetDevice());

		VkPipelineLayoutCreateInfo layoutCreateInfo{};

		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.pNext = nullptr;

		if (count != 0)
		{
			layoutCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
			layoutCreateInfo.setLayoutCount = 1;
		}

		if (vkCreatePipelineLayout(m_Device->GetDevice(), &layoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout.");
		}
	}
	std::vector<DescriptionSetLayout> VulkanPipeline::CreateLayout(Ref<Shader> shader)
	{
		std::vector<DescriptionSetLayout> descriptorSetLayout;

		for (auto& reflection : shader->GetReflections())
		{
			for (auto ubo : reflection.GetUniformBuffers())
			{
				descriptorSetLayout.push_back({ ubo.Binding, DescriptorType::UniformBuffer, 1, ubo.Stage });
			}

			for (auto rbo : reflection.GetResourceBindings())
			{
				descriptorSetLayout.push_back({ rbo.Binding, rbo.Type, rbo.Count, rbo.Stage });
			}
		}

		return descriptorSetLayout;
	}

	void VulkanPipeline::CreateDescriptorSet(std::vector<DescriptionSetLayout> descriptorSetLayout)
	{
		if (descriptorSetLayout.empty()) return;
		int maxSets = descriptorSetLayout.size();

		m_DescriptorAllocator = DescriptorAllocatorGrowable();

		std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> poolSizes;
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
			DescriptorAllocatorGrowable::PoolSizeRatio poolSize{};
			poolSize.Type = type;
			poolSize.Ratio = count;

			poolSizes.push_back(poolSize);
		}

		m_DescriptorAllocator.Init(m_Device->GetDevice(), maxSets, poolSizes);
		m_DescriptorSet = m_DescriptorAllocator.Allocate(m_Device->GetDevice(), m_DescriptorSetLayout);
	}

}