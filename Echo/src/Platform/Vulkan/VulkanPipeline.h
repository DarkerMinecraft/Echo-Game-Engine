#pragma once

#include "Echo/Graphics/GraphicsPipeline.h"
#include "Platform/Vulkan/VulkanGraphicsContext.h"

#include <vulkan/vulkan.h>

namespace Echo
{
	class VulkanPipeline : public GraphicsPipeline
	{
	public:
		VulkanPipeline(const std::string& name, VulkanGraphicsContext* context, GraphicsPipelineData& data)
			: m_Data(data), m_Context(context), m_Name(name) {}

		virtual void Start() override;
		virtual void Finish() override;

		virtual const std::string& GetName() const override { return m_Name; };
	private:
		std::vector<char> ReadShader(const char* filePath); 

		VkShaderModule CreateShaderModule(const std::vector<char>& code);
	private:
		GraphicsPipelineData& m_Data;
		VulkanGraphicsContext* m_Context;

		VkShaderModule m_VertexShaderModule;
		VkShaderModule m_FragmentShaderModule;

		VkRenderPass m_RenderPass;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;

		const std::string& m_Name;
	};
}