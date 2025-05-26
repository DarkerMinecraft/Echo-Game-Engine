#pragma once

#include "Graphics/Material.h"
#include "Graphics/Pipeline.h"

#include "VulkanDevice.h"
#include "Utils/VulkanDescriptors.h"

namespace Echo 
{

	class VulkanPipeline : public Pipeline 
	{
	public:
		VulkanPipeline(Device* device, Ref<Shader> shader, const PipelineSpecification& pipelineSpec);
		virtual ~VulkanPipeline();

		virtual void Bind(CommandBuffer* cmd) override;

		virtual PipelineType GetPipelineType() override { return m_PipelineType; }

		virtual void BindResource(uint32_t binding, uint32_t set, Ref<Texture2D> texture) override;
		virtual void BindResource(uint32_t binding, uint32_t set, Texture2D* texture) override;
		virtual void BindResource(uint32_t binding, uint32_t set, Ref<Texture2D> texture, uint32_t texIndex) override;
		virtual void BindResource(uint32_t binding, uint32_t set, Ref<UniformBuffer> buffer) override;
		virtual void BindResource(uint32_t binding, uint32_t set, Ref<Framebuffer> framebuffer, uint32_t attachmentIndex) override;
		virtual void BindResource(uint32_t binding, uint32_t set, Framebuffer* framebuffer, uint32_t attachmentIndex) override;

		virtual void ReconstructPipeline(Ref<Shader> shader) override;

		void Destroy();
	private:
		void CreateComputePipeline(Ref<Shader> computeShader, const PipelineSpecification& spec);
		void CreateGraphicsPipeline(Ref<Shader> graphicsShader, const PipelineSpecification& spec);

		std::vector<DescriptionSetLayout> CreateLayout(Ref<Shader> shader);
		void CreatePipelineLayout(std::vector<DescriptionSetLayout> descriptorSetLayout);
		void CreateDescriptorSet(std::vector<DescriptionSetLayout> descriptorSetLayout);
			
		void DestroyOldPipelineResources(VkPipeline oldPipeline,
										 VkPipelineLayout oldPipelineLayout,
										 const std::vector<VkDescriptorSetLayout>& oldDescriptorSetLayouts,
										 std::vector<DescriptorAllocatorGrowable>& oldDescriptorAllocators);

		bool HasDescriptorSet() { return !m_DescriptorSets.empty(); }
	private:
		VulkanDevice* m_Device;
		PipelineType m_PipelineType;

		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
		std::vector<VkDescriptorSet> m_DescriptorSets;
		std::vector<DescriptorAllocatorGrowable> m_DescriptorAllocators;
		VkDescriptorPool m_DescriptorPool;

		PipelineSpecification m_PipelineSpecification;

		bool m_Destroyed = false;

	};

}