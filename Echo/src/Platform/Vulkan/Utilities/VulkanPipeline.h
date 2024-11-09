#pragma once

#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/Interface/VulkanDevice.h"

#include <vector>
#include <string>

namespace Echo 
{

	struct PipelineConfigInfo
	{
		VkPipelineViewportStateCreateInfo ViewportInfo;
		VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo RasterizationInfo;
		VkPipelineMultisampleStateCreateInfo MultisampleInfo;
		VkPipelineColorBlendAttachmentState ColorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
		std::vector<VkDynamicState> DynamicStateEnables;
		VkPipelineDynamicStateCreateInfo DynamicStateInfo;
		VkPipelineLayout PipelineLayout = nullptr;
		VkRenderPass RenderPass = nullptr;

		uint32_t Subpass = 0;
	};

	class VulkanPipeline
	{
	public:
		VulkanPipeline(Ref<GraphicsShader> shader, const PipelineConfigInfo& configInfo);
		~VulkanPipeline();

		static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

		void Bind(VkCommandBuffer commandBuffer);
	private:
		void CreateGraphicsPipeline(const PipelineConfigInfo& configInfo);
	private: 
		Ref<GraphicsShader> m_Shader;
		VulkanDevice* m_Device;

		VkPipeline m_GraphicsPipeline;
	};
}