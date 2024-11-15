#pragma once

#include "Echo/Graphics/Resource.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

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

	struct CompilationInfo
	{
		std::string FilePath;
		shaderc_shader_kind Kind;
		std::vector<char> Source;
		shaderc::CompileOptions Options;
	};

	class VulkanResource : public Resource 
	{
	public:
		VulkanResource(const ResourceCreateInfo& createInfo);
		virtual ~VulkanResource();

		virtual void Bind() override;
		virtual void LoadVertex(Vertex vertex) override;
		virtual void Unbind() override;

		void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
	private:
		void CreateGraphicsPipeline();
		VkShaderModule CreateShaderModule(const std::vector<char>& code);

		std::vector<char> ReadFile(const std::string& filePath);

		void PreprocessShader(CompilationInfo& info);
		void CompileFileToAssembly(CompilationInfo& info);
		std::vector<char> CompileFile(CompilationInfo& info);

		std::vector<VkVertexInputBindingDescription> GetBindingDescription()
		{
			return { {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX} };
		}

		std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
		{
			return { {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, Position)}, {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Color)} };;
		}
	private:
		VulkanDevice* m_Device;

		VkShaderModule m_VertexShaderModule;
		VkShaderModule m_FragmentShaderModule;

		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;
		VkGraphicsPipelineCreateInfo m_GraphicsPipelineInfo;

		const ResourceCreateInfo& m_CreateInfo;
	};

}