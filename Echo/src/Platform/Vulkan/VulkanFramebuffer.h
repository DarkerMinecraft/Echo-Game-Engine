#pragma once

#include "Echo/Graphics/Framebuffer.h"

#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanFramebuffer : public Framebuffer 
	{
	public:
		VulkanFramebuffer(Device* device, const FramebufferSpecification& specification);
		~VulkanFramebuffer();

		virtual uint32_t GetWidth() override;
		virtual uint32_t GetHeight() override;
		virtual void* GetImGuiTexture(uint32_t index) override;

		virtual void Resize(uint32_t width, uint32_t height);

		virtual void Destroy() override;

		void UpdateSize();

		void TransitionImageLayout(VkCommandBuffer cmd, uint32_t index, VkImageLayout newLayout);
		VkImageLayout GetCurrentLayout(uint32_t index) { return m_Framebuffers[index].ImageLayout; }

		VkSampler GetSampler(uint32_t index) { return m_Framebuffers[index].Sampler; }

		bool IsDepthTexture(uint32_t index) { return m_Framebuffers[index].DepthTexture; } 
		AllocatedImage GetImage(uint32_t index) { return m_Framebuffers[index]; }

		uint32_t GetFramebuffersSize() { return m_Framebuffers.size(); }
	private:
		void CreateAllocatedFramebuffers(const FramebufferSpecification& specification);
		void CreateImage(uint32_t index, uint32_t width, uint32_t height); 
	private:
		VulkanDevice* m_Device;
	
		std::vector<AllocatedImage> m_Framebuffers;
		std::vector<FramebufferTextureFormat> m_Attachments;

		bool m_WindowExtent;
		uint32_t m_Width, m_Height;

		VkDescriptorSet m_DescriptorSet = nullptr;
	};

}