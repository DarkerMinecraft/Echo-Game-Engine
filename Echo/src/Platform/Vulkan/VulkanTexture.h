#pragma once

#include "VulkanDevice.h"

#include "Echo/Graphics/Texture.h"

#include "stb_image.h"

namespace Echo
{

	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(VulkanDevice* device, const TextureDesc& textureDescription);
		virtual ~VulkanTexture();

		AllocatedImage GetAllocatedImage() { return m_Image; }

		VkImage GetImage() { return m_Image.Image; }
		VkImageView GetImageView() { return m_Image.ImageView; }
		VkSampler GetSampler() { return m_Sampler; }
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
	public:
		void UpdateSwapchainExtent(uint32_t width, uint32_t height);
	private:
		void CreateTexture(const TextureDesc& textureDesciption);
		void LoadTexture(const std::string& textureFilePath);
	private:
		VulkanDevice* m_Device;

		VkImageUsageFlags m_Usage;
		VkFormat m_Format;

		AllocatedImage m_Image;
		VkSampler m_Sampler;

		uint32_t m_Width, m_Height;
		stbi_uc* m_Pixels;

		TextureUsage m_TextureUsage;
		bool m_UseSwapchainExtent;
	};

}