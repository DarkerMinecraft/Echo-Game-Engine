#pragma once

#include "Echo/Graphics/Pipeline.h"
#include "VulkanDevice.h"

#include <unordered_map>
#include <shaderc/shaderc.hpp>

namespace Echo 
{

	struct CompilationInfo
	{
		std::string FilePath;
		shaderc_shader_kind Kind;
		std::vector<char> Source;
		shaderc::CompileOptions Options;
	};

	class PipelineConfig 
	{
	public:
		PipelineConfig() { Clear(); }

		std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;

		VkPipelineVertexInputStateCreateInfo VertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo InputAssembly;
		VkPipelineRasterizationStateCreateInfo Rasterizer;
		VkPipelineColorBlendAttachmentState ColorBlendAttachment;
		VkPipelineMultisampleStateCreateInfo Multisampling;
		VkPipelineDepthStencilStateCreateInfo DepthStencil;
		VkPipelineRenderingCreateInfo RenderInfo;
		VkFormat ColorAttachmentFormat;

		VkPipeline BuildPipeline(VkDevice device, VkPipelineLayout layout);
		void SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
		void SetVertexInput(VkVertexInputBindingDescription bindingDescription, std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions);
		void SetInputTopology(VkPrimitiveTopology topology);
		void SetPolygonMode(VkPolygonMode mode);
		void SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
		void SetMutisamplingNone(); 
		void DisableBlending();
		void SetColorAttachmentFormat(VkFormat format);
		void SetDepthFormat(VkFormat format);
		void DisableDepthTest();

		void Clear();
	};

	class VulkanPipeline : public Pipeline 
	{
	public:
		VulkanPipeline(PipelineType type, const std::string& filePath, size_t pushConstantsSize = -1);
		virtual ~VulkanPipeline();

		virtual PipelineType GetPipelineType() override { return m_Type; }

		virtual void Bind() override;
		virtual void UpdatePushConstants(const void* pushConstants) override;
	private:
		std::string ReadFile(const std::string& filePath); 
		std::unordered_map<ShaderType, std::vector<char>> PreProcess(const std::string& source);

		void Compile(const std::string& filePath, const std::unordered_map<ShaderType, std::vector<char>>& shaders);

		void PreprocessShader(CompilationInfo& info);
		void CompileFileToAssembly(CompilationInfo& info);
		std::vector<char> CompileFile(CompilationInfo& info);

		VkShaderModule CreateShaderModule(const std::vector<char>& code);

		void InitPipeline(size_t pushConstantsSize);
		void InitBackgroundPipeline();
	private:
		VulkanDevice* m_Device;
		PipelineType m_Type;

		VkShaderModule m_VertexShaderModule;
		VkShaderModule m_FragmentShaderModule;
		VkShaderModule m_ComputeShaderModule;

		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;

		GPUMeshBuffers m_MeshBuffers;

		size_t m_PushConstantSize;
	};
}