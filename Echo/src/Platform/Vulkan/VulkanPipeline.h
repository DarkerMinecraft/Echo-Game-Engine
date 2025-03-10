#pragma once

#include "Echo/Graphics/Pipeline.h"

#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanPipeline : public Pipeline 
	{
	public:
		VulkanPipeline(Device* device, Ref<Material> material, PipelineDesc& desc);
		VulkanPipeline(Device* device, const char* computeFilePath, PipelineDesc& desc);
		virtual ~VulkanPipeline();

		virtual void Bind(CommandBuffer* cmd) override;

		virtual void WriteDesciptorStorageImage(Ref<Image> image, uint32_t binding) override;

		PipelineType GetType()  { return m_PipelineType; }
		bool HasDescriptorSet() { return m_DescriptorSet != nullptr; }

		VkDescriptorSet GetDescriptorSet() { return m_DescriptorSet; }
		VkPipeline GetPipeline() { return m_Pipeline; }
		VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }
	private:
		void CreateComputePipeline(const char* computeFilePath, PipelineDesc& desc);
		void CreateGraphicsPipeline(Ref<Material> material, PipelineDesc& desc);

		void CreatePipelineLayout(std::vector<PipelineDesc::DescriptionSetLayout> descriptorSetLayout);
		void CreateDescriptorSet(std::vector<PipelineDesc::DescriptionSetLayout> descriptorSetLayout, uint32_t maxSets);
	private:
		static ShaderLibrary s_ShaderLibrary;
	private:
		VulkanDevice* m_Device;
		PipelineType m_PipelineType;
		
		Ref<Material> m_Material;

		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkDescriptorSet m_DescriptorSet = nullptr;
		VkDescriptorPool m_DescriptorPool;

		VkShaderModule m_ComputeShaderModule;
	};

}