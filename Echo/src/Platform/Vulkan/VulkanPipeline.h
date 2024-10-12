#pragma once

#include "Echo/Graphics/GraphicsPipeline.h"
#include "Platform/Vulkan/VulkanGraphicsContext.h"

#include <vulkan/vulkan.h>

namespace Echo
{
	class VulkanPipeline : GraphicsPipeline
	{
	public:
		VulkanPipeline(VulkanGraphicsContext* context, GraphicsPipelineData& data)
			: m_Data(data), m_Context(context) {}

		virtual void Start() override;
		virtual void Finish() override;
	private:
		std::vector<unsigned int> ReadShader(VkShaderStageFlagBits stage, const char* filePath); 
		bool LoadShader(std::vector<unsigned int> source, VkShaderStageFlagBits stage);

		VkShaderModule CreateShaderModule(const std::vector<unsigned int>& code);
	private:
		GraphicsPipelineData& m_Data;
		VulkanGraphicsContext* m_Context;

		VkShaderModule m_VertexShaderModule;
		VkShaderModule m_FragmentShaderModule;

		VkRenderPass m_RenderPass;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;
	};
}