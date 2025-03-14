#include "pch.h"
#include "VulkanImage.h"
#include "Platform/Vulkan/Utils/VulkanInitializers.h"

#include "backends/imgui_impl_vulkan.h"
#include "Utils/VulkanDescriptors.h"
#include "Utils/VulkanImages.h"

namespace Echo
{

	VulkanImage::VulkanImage(Device* device, const ImageDescription& desc)
		: m_Device((VulkanDevice*)device)
	{
		CreateAllocatedImage(desc);
		m_Device->AddImage(this);
	}

	VulkanImage::~VulkanImage()
	{
		Destroy();
	}

	uint32_t VulkanImage::GetWidth()
	{
		if (m_UseDrawImage || m_DrawImageExtent)
		{
			return m_Device->GetDrawExtent().width;
		}

		return m_Width;
	}

	uint32_t VulkanImage::GetHeight()
	{
		if (m_UseDrawImage || m_DrawImageExtent)
		{
			return m_Device->GetDrawExtent().height;
		}

		return m_Height;
	}

	void* VulkanImage::GetColorAttachmentID()
	{
		if (m_DescriptorSet) 
		{
			ImGui_ImplVulkan_RemoveTexture(m_DescriptorSet);
		}

		m_DescriptorSet = ImGui_ImplVulkan_AddTexture(m_Sampler, m_Image.ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		return m_DescriptorSet;
	}

	void VulkanImage::Resize(const glm::vec2& size)
	{
		if (m_Width == size.x && m_Height == size.y)
			return;
		m_Width = size.x;
		m_Height = size.y;
		AllocatedImage oldImage = m_Image;
		m_Image = m_Device->CreateImage({ m_Width, m_Height, 1 }, m_Format,
										VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
		m_Device->DestroyImage(oldImage);

		if(m_DrawToImGui) 
		{
			m_Device->ImmediateSubmit([&](VkCommandBuffer cmd) 
			{
				VulkanImages::TransitionImage(cmd, m_Image.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			});
		}
		else 
		{
			m_Device->ImmediateSubmit([&](VkCommandBuffer cmd) 
			{
				VulkanImages::TransitionImage(cmd, m_Image.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
			});
		}
	}

	void VulkanImage::Destroy()
	{
		if (m_HasBeenDestroyed)
			return; 

		if (!m_UseDrawImage)
		{
			m_Device->DestroyImage(m_Image);
		}
		if (m_DescriptorSet)
		{
			ImGui_ImplVulkan_RemoveTexture(m_DescriptorSet);
		}

		vkDestroySampler(m_Device->GetDevice(), m_Sampler, nullptr);
		m_HasBeenDestroyed = true;
	}

	void VulkanImage::UpdateSize()
	{
		if (m_DrawImageExtent)
		{
			CreateImage(m_Device->GetDrawExtent().width, m_Device->GetDrawExtent().height);
		}
	}

	AllocatedImage VulkanImage::GetImage()
	{
		if (m_UseDrawImage)
			return m_Device->GetDrawImage();
		else return m_Image;
	}

	void VulkanImage::CreateAllocatedImage(const ImageDescription& desc)
	{
		m_UseDrawImage = desc.DrawImage;
		m_DrawImageExtent = desc.DrawImageExtent;
		m_Width = desc.Width;
		m_Height = desc.Height;
		m_DrawToImGui = desc.DrawToImGui;

		if (desc.DrawImageExtent)
		{
			m_Width = m_Device->GetDrawExtent().width;
			m_Height = m_Device->GetDrawExtent().height;
		}

		if (!m_UseDrawImage)
		{
			VkExtent3D drawImageExtent = {
				m_Width,
				m_Height,
				1
			};

			auto MapImageFormat = [](ImageFormat format) -> VkFormat
			{
				switch (format)
				{
					case RGBA8:
						return VK_FORMAT_R8G8B8A8_UNORM;
					case RGBA16F:
						return VK_FORMAT_R16G16B16A16_SFLOAT;
					case RGBA32F:
						return VK_FORMAT_R32G32B32A32_SFLOAT;
					case R32F:
						return VK_FORMAT_R32_SFLOAT;
					case RG32F:
						return VK_FORMAT_R32G32_SFLOAT;
					case RGB32F:
						return VK_FORMAT_R32G32B32_SFLOAT;
					case RGBA16:
						return VK_FORMAT_R16G16B16A16_UNORM;
					case RGBA8Srgb:
						return VK_FORMAT_R8G8B8A8_SRGB;
					case BGRA8:
						return VK_FORMAT_B8G8R8A8_UNORM;
					case Depth32F:
						return VK_FORMAT_D32_SFLOAT;
					case Depth24Stencil8:
						return VK_FORMAT_D24_UNORM_S8_UINT;
					default:
						return VK_FORMAT_UNDEFINED;
				}
			};

			m_Image = m_Device->CreateImage(drawImageExtent, MapImageFormat(desc.Format),
									  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
			m_Format = MapImageFormat(desc.Format);
		}

		VkSamplerCreateInfo sampl = { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };

		sampl.magFilter = VK_FILTER_NEAREST;
		sampl.minFilter = VK_FILTER_NEAREST;

		vkCreateSampler(m_Device->GetDevice(), &sampl, nullptr, &m_Sampler);
	}

	void VulkanImage::CreateImage(uint32_t width, uint32_t height)
	{
		AllocatedImage oldImage = m_Image;
		m_Image = m_Device->CreateImage({ width, height, 1 }, m_Format,
											  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
		m_Device->DestroyImage(oldImage);
	}

}