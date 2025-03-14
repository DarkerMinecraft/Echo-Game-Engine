#pragma once

#include "Echo/Graphics/Pipeline.h"

#include "VulkanDevice.h"
#include "Utils/VulkanDescriptors.h"

namespace Echo 
{

	class VulkanPipeline : public Pipeline 
	{
	public:
		VulkanPipeline(Device* device, Ref<Material> material, PipelineDesc& desc);
		VulkanPipeline(Device* device, const char* computeFilePath, PipelineDesc& desc);
		virtual ~VulkanPipeline();

		virtual void Bind(CommandBuffer* cmd) override;

		virtual void WriteDescriptorStorageImage(Ref<Image> image, uint32_t binding = 0) override;

		virtual void WriteDescriptorCombinedTexture(Ref<Texture> tex, uint32_t binding = 0) override;
		virtual void WriteDescriptorCombinedImage(Ref<Image> img, uint32_t binding = 0) override;

		virtual void WriteDescriptorCombinedTextureArray(Ref<Texture> tex, int index, uint32_t binding = 0) override;
		virtual void WriteDescriptorUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding = 0) override;

		PipelineType GetType()  { return m_PipelineType; }
		bool HasDescriptorSet() { return m_DescriptorSet != nullptr; }

		VkDescriptorSet GetDescriptorSet() { return m_DescriptorSet; }
		VkPipeline GetPipeline() { return m_Pipeline; }
		VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }
	private:
		void CreateComputePipeline(const char* computeFilePath, PipelineDesc& desc);
		void CreateGraphicsPipeline(Ref<Material> material, PipelineDesc& desc);

		void CreatePipelineLayout(std::vector<PipelineDesc::DescriptionSetLayout> descriptorSetLayout);
		void CreateDescriptorSet(std::vector<PipelineDesc::DescriptionSetLayout> descriptorSetLayout);
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

		DescriptorAllocatorGrowable m_DescriptorAllocator;

		VkShaderModule m_ComputeShaderModule;
	};

}