#include "pch.h"
#include "VulkanTexture.h"

#include "VulkanSwapchain.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Utils/VulkanImages.h"

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

	void VulkanTexture::UpdateSwapchainExtent(uint32_t width, uint32_t height)
	{
		if (!m_UseSwapchainExtent) return;

		m_Width = width;
		m_Height = height;

		m_Device->DestroyImage(m_Image);
		m_Image = m_Device->CreateImage(VkExtent3D{ static_cast<unsigned int>(m_Width), static_cast<unsigned int>(m_Height), 1 }, m_Format, m_Usage);

		VkCommandBuffer cmd = m_Device->BeginSingleTimeCommands();
		if (HasUsage(m_TextureUsage, TextureUsage::ColorAttachment))
		{
			VulkanImages::TransitionImage(cmd, m_Image.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}
		else if (HasUsage(m_TextureUsage, TextureUsage::DepthAttachment))
		{
			VulkanImages::TransitionImage(cmd, m_Image.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
		}
		else
		{
			VulkanImages::TransitionImage(cmd, m_Image.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
		}
		m_Device->EndSingleTimeCommands(cmd);
	}

	void VulkanTexture::CreateTexture(const TextureDesc& textureDesciption)
	{
		m_UseSwapchainExtent = textureDesciption.UseSwapchainExtent;
		m_TextureUsage = textureDesciption.Usage;

		if (textureDesciption.TexturePath != "") 
		{
			LoadTexture(textureDesciption.TexturePath);
		}
		else 
		{
			if (!textureDesciption.UseSwapchainExtent)
			{
				m_Width = textureDesciption.Width;
				m_Height = textureDesciption.Height;
			}
			else 
			{
				VulkanSwapchain* swapchain = m_Device->GetSwapchain();

				m_Width = swapchain->GetExtent().width;
				m_Height = swapchain->GetExtent().height;
			}

			auto MapTextureFormat = [](TextureFormat format) -> VkFormat
			{
				switch (format)
				{
					case TextureFormat::RGBA8:
						return VK_FORMAT_R8G8B8A8_UNORM;
					case TextureFormat::Depth32F:
						return VK_FORMAT_D32_SFLOAT;
					case TextureFormat::BGRA8:
						return VK_FORMAT_B8G8R8A8_UNORM;
					case TextureFormat::RGBA16:
						return VK_FORMAT_R16G16B16A16_SFLOAT;
					default:
						throw std::runtime_error("Unknown Texture Format");
				}
			};

			m_Format = MapTextureFormat(textureDesciption.Format);

			m_Usage = 0;
			if (HasUsage(textureDesciption.Usage, TextureUsage::Sampled))
			{
				m_Usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
			}
			if (HasUsage(textureDesciption.Usage, TextureUsage::ColorAttachment))
			{
				m_Usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}
			if (HasUsage(textureDesciption.Usage, TextureUsage::DepthAttachment))
			{
				m_Usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			}
			if (HasUsage(textureDesciption.Usage, TextureUsage::TransferSrc)) 
			{
				m_Usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			}
			if (HasUsage(textureDesciption.Usage, TextureUsage::TransferDst)) 
			{
				m_Usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			}
			if (HasUsage(textureDesciption.Usage, TextureUsage::Storage))
			{
				m_Usage |= VK_IMAGE_USAGE_STORAGE_BIT;
			}

			m_Image = m_Device->CreateImage(VkExtent3D{ static_cast<unsigned int>(m_Width), static_cast<unsigned int>(m_Height), 1 }, m_Format, m_Usage);

			VkCommandBuffer cmd = m_Device->BeginSingleTimeCommands();
			if (HasUsage(textureDesciption.Usage, TextureUsage::ColorAttachment))
			{
				VulkanImages::TransitionImage(cmd, m_Image.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			}
			else if (HasUsage(textureDesciption.Usage, TextureUsage::DepthAttachment))
			{
				VulkanImages::TransitionImage(cmd, m_Image.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
			}
			else 
			{
				VulkanImages::TransitionImage(cmd, m_Image.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
			}
			m_Device->EndSingleTimeCommands(cmd);
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