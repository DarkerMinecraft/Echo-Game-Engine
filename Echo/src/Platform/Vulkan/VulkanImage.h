#pragma once

#include "Echo/Graphics/Image.h"

#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanImage : public Image 
	{
	public:
		VulkanImage(Device* device, const ImageDescription& desc);
		~VulkanImage();

		virtual uint32_t GetWidth() override;
		virtual uint32_t GetHeight() override;
		virtual void* GetImGuiTexture() override;

		virtual void Resize(uint32_t width, uint32_t height) override { Resize({ width, height }); };
		virtual void Resize(const glm::vec2& size) override;

		virtual void Destroy() override;

		void UpdateSize();
		AllocatedImage GetImage() { return m_Image; }

		void TransitionImageLayout(VkCommandBuffer cmd, VkImageLayout newLayout);
		VkImageLayout GetCurrentLayout() { return m_CurrentLayout; }

		VkSampler GetSampler() { return m_Sampler; }
	private:
		void CreateAllocatedImage(const ImageDescription& desc);
		void CreateImage(uint32_t width, uint32_t height); 
	private:
		VulkanDevice* m_Device;
		uint32_t m_Width, m_Height;

		AllocatedImage m_Image;
		VkImageLayout m_CurrentLayout;
		VkFormat m_Format;

		bool m_WindowExtent;

		VkDescriptorSet m_DescriptorSet = nullptr;
		VkSampler m_Sampler;

		VkImageUsageFlags m_UsageFlags;

		bool m_HasBeenDestroyed = false;
	};

}