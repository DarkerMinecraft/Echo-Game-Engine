#pragma once

#include "Echo/Graphics/Pipeline.h"
#include "Platform/Shader/ShaderCompiler.h"
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
		VulkanPipeline(VulkanDevice* device, ShaderLibrary library, const PipelineDesc& pipelineDescription);
		virtual ~VulkanPipeline();
		
		virtual void Bind() override;
		virtual void WriteDescriptorBuffer(Ref<Buffer> buffer, uint32_t bufferSize, uint32_t binding) override;
		virtual void WriteDescriptorStorageImage(Ref<FrameBuffer> frameBuffer, uint32_t binding) override;
	private:
		void CreateShaderModules(const PipelineDesc& pipelineDescription);
		VkShaderModule CreateShaderModule(SpirvData code);

		void InitPipelineLayout(const PipelineDesc& pipelineDescription);
		void CreateDescriptorSet(const PipelineDesc& pipelineDescription);
		void InitPipeline(const PipelineDesc& pipelineDescription);

		VkDescriptorSetLayout CreateDescriptorSetLayout(const PipelineDesc& desc);
	private:
		VulkanDevice* m_Device; 

		ShaderLibrary m_Library;

		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;

		VkDescriptorPool m_DescriptorPool;
		VkDescriptorSet m_DescriptorSet;
		VkDescriptorSetLayout m_DescriptorSetLayout;

		VkShaderModule m_VertexShader, m_FragmentShader, m_ComputeShader, m_GeometryShader, m_RaytracingShader;
		PipelineType m_PipelineType;
		ShaderType m_ShaderType;
	};

}