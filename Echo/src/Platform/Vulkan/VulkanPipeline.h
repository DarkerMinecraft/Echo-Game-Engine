#pragma once

#include "Echo/Graphics/Pipeline.h"

#include "VulkanDevice.h"

#include <vector>

namespace Echo
{
	
	enum PipelineType 
	{
		Graphics,
		Compute,
		Raytracing
	};

	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(VulkanDevice* device, const PipelineDesc& pipelineDescription);
		virtual ~VulkanPipeline();
		
		virtual void Bind() override;
		virtual void WritePushConstants(const void* pushConstants) override;
	private:
		void CreateShaderModules(const PipelineDesc& pipelineDescription);
		VkShaderModule CreateShaderModule(const std::vector<uint32_t>& code);

		void InitPipelineLayout(const PipelineDesc& pipelineDescription);
		void CreateDescriptorSet(const PipelineDesc& pipelineDescription);
		void InitPipeline(const PipelineDesc& pipelineDescription);

		std::vector<uint32_t> ReadFile(const std::string& filePath);
		const char* LoadShaderCode(const std::string& filePath);
		VkDescriptorSetLayout CreateDescriptorSetLayout(const PipelineDesc& desc);
		std::vector<VkPushConstantRange> CreatePushConstants(const PipelineDesc& desc);
	private:
		VulkanDevice* m_Device; 

		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;

		VkDescriptorPool m_DescriptorPool;
		VkDescriptorSet m_DescriptorSet;
		VkDescriptorSetLayout m_DescriptorSetLayout;

		uint32_t m_PushConstantSize;
		uint32_t m_PushConstantOffset;

		VkShaderModule m_VertexShader, m_FragmentShader, m_ComputeShader, m_GeometryShader, m_RaytracingShader;
		PipelineType m_Type;
	};

}