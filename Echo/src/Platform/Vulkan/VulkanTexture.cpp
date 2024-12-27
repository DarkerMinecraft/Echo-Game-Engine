#include "pch.h"
#include "VulkanTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Echo 
{

	

	VulkanTexture::VulkanTexture(VulkanDevice* device, const TextureDesc& textureDescription)
		: m_Device(device)
	{
		CreateTexture(textureDescription);
	}

	VulkanTexture::~VulkanTexture()
	{
		m_Device->DestroyImage(m_Image);
	}

	void VulkanTexture::CreateTexture(const TextureDesc& textureDesciption)
	{
		if (textureDesciption.TexturePath != "") 
		{
			LoadTexture(textureDesciption.TexturePath);
		}
		else 
		{
			m_Width = textureDesciption.Width;
			m_Height = textureDesciption.Height;

			VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
			if (textureDesciption.Format == TextureFormat::Depth32F)
			{
				format = VK_FORMAT_D32_SFLOAT;
			}

			VkImageUsageFlags usage = 0;
			if (HasUsage(textureDesciption.Usage, TextureUsage::Sampled))
			{
				usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
			}
			if (HasUsage(textureDesciption.Usage, TextureUsage::ColorAttachment))
			{
				usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}
			if (HasUsage(textureDesciption.Usage, TextureUsage::DepthAttachment))
			{
				usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			}

			m_Image = m_Device->CreateImage(VkExtent3D{ static_cast<unsigned int>(m_Width), static_cast<unsigned int>(m_Height), 1 }, format, usage);
		}
	}

	void VulkanTexture::LoadTexture(const std::string& textureFilePath)
	{
		int texWidth, texHeight, texChannels;
		m_Pixels = stbi_load(textureFilePath.c_str(),
									&texWidth, &texHeight, &texChannels,
									STBI_rgb_alpha);

		if (!m_Pixels) 
		{
			EC_CORE_ERROR("Failed to load texture: {0}", textureFilePath);
		}

		m_Width = static_cast<uint32_t>(texWidth);
		m_Height = static_cast<uint32_t>(texHeight);
			
		m_Image = m_Device->CreateImage(m_Pixels, VkExtent3D{ static_cast<unsigned int>(texWidth), static_cast<unsigned int>(texHeight), 1 }, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

		stbi_image_free(m_Pixels);
	}

}