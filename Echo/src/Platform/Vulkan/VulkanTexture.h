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

		VkImage GetImage() { return m_Image.Image; }
		VkImageView GetImageView() { return m_Image.ImageView; }
		VkSampler GetSampler() { return m_Sampler; }
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
	private:
		void CreateTexture(const TextureDesc& textureDesciption);
		void LoadTexture(const std::string& textureFilePath);
	private:
		VulkanDevice* m_Device;

		AllocatedImage m_Image;
		VkSampler m_Sampler;

		uint32_t m_Width, m_Height;
		stbi_uc* m_Pixels;
	};

}