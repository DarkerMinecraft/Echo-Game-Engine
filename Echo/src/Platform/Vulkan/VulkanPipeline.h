#pragma once

#include "Graphics/Pipeline.h"

#include "VulkanDevice.h"
#include "Utils/VulkanDescriptors.h"

namespace Echo 
{

	class VulkanPipeline : public Pipeline 
	{
	public:
		VulkanPipeline(Device* device, Material* material);
		VulkanPipeline(Device* device, Ref<Shader> computeShader, PipelineSpecification& spec);
		virtual ~VulkanPipeline();

		virtual void Bind(CommandBuffer* cmd) override;

		virtual void WriteDescriptorStorageImage(Ref<Framebuffer> framebuffer, uint32_t index, uint32_t binding = 0) override;

		virtual void WriteDescriptorCombinedTexture(Ref<Texture> tex, uint32_t binding = 0) override;
		virtual void WriteDescriptorCombinedImage(Ref<Framebuffer> framebuffer, uint32_t index, uint32_t binding = 0) override;

		virtual void WriteDescriptorCombinedTextureArray(Ref<Texture> tex, int index, uint32_t binding = 0) override;
		virtual void WriteDescriptorUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding = 0) override;

		virtual void Destroy() override;

		PipelineType GetType()  { return m_PipelineType; }
		bool HasDescriptorSet() { return m_DescriptorSet != nullptr; }

		VkDescriptorSet GetDescriptorSet() { return m_DescriptorSet; }
		VkPipeline GetPipeline() { return m_Pipeline; }
		VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }
	private:
		void CreateComputePipeline(Ref<Shader> computeShader, PipelineSpecification& specs);
		void CreateGraphicsPipeline(Material* material);

		void CreatePipelineLayout(std::vector<PipelineSpecification::DescriptionSetLayout> descriptorSetLayout);
		void CreateDescriptorSet(std::vector<PipelineSpecification::DescriptionSetLayout> descriptorSetLayout);
	private:
		static ShaderLibrary s_ShaderLibrary;
	private:
		VulkanDevice* m_Device;
		PipelineType m_PipelineType;
		
		Material* m_Material;
		Ref<Shader> m_ComputeShader;

		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkDescriptorSet m_DescriptorSet = nullptr;
		VkDescriptorPool m_DescriptorPool;

		bool m_Destroyed = false;

		DescriptorAllocatorGrowable m_DescriptorAllocator;
	};

}