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

		virtual void BindResource(uint32_t binding, Ref<Texture2D> texture) override;
		virtual void BindResource(uint32_t binding, Ref<Texture2D> texture, uint32_t texIndex) override;
		virtual void BindResource(uint32_t binding, Ref<UniformBuffer> buffer) override;
		virtual void BindResource(uint32_t binding, Ref<Framebuffer> framebuffer, uint32_t attachmentIndex) override;

		void Destroy();
	private:
		void CreateComputePipeline(Ref<Shader> computeShader, const PipelineSpecification& spec);
		void CreateGraphicsPipeline(Ref<Shader> graphicsShader, const PipelineSpecification& spec);

		std::vector<DescriptionSetLayout> CreateLayout(Ref<Shader> shader);
		void CreatePipelineLayout(std::vector<DescriptionSetLayout> descriptorSetLayout);
		void CreateDescriptorSet(std::vector<DescriptionSetLayout> descriptorSetLayout);

		bool HasDescriptorSet() { return m_DescriptorSet != nullptr; }
	private:
		VulkanDevice* m_Device;
		PipelineType m_PipelineType;

		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkDescriptorSet m_DescriptorSet = nullptr;
		VkDescriptorPool m_DescriptorPool;

		bool m_Destroyed = false;

		DescriptorAllocatorGrowable m_DescriptorAllocator;
	};

}