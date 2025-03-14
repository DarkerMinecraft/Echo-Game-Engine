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
		virtual void* GetColorAttachmentID() override;

		virtual void Resize(uint32_t width, uint32_t height) override { Resize({ width, height }); };
		virtual void Resize(const glm::vec2& size) override;

		virtual void Destroy() override;

		void UpdateSize();
		AllocatedImage GetImage();
	private:
		void CreateAllocatedImage(const ImageDescription& desc);
		void CreateImage(uint32_t width, uint32_t height); 
	private:
		VulkanDevice* m_Device;
		uint32_t m_Width, m_Height;

		AllocatedImage m_Image;

		bool m_UseDrawImage;
		bool m_DrawImageExtent;
		bool m_DrawToImGui;
		VkFormat m_Format;

		VkDescriptorSet m_DescriptorSet = nullptr;
		VkSampler m_Sampler;

		bool m_HasBeenDestroyed = false;
	};

}