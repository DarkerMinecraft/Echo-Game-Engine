#include "pch.h"
#include "VulkanImage.h"
#include "Platform/Vulkan/Utils/VulkanInitializers.h"

namespace Echo
{

	VulkanImage::VulkanImage(Device* device, const ImageDescription& desc)
		: m_Device((VulkanDevice*)device)
	{
		CreateAllocatedImage(desc);
	}

	VulkanImage::~VulkanImage()
	{
		if (!m_UseDrawImage)
		{
			m_Device->DestroyImage(m_Image);
		}
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

	void* VulkanImage::GetImageHandle()
	{
		if(m_UseDrawImage)
			return m_Device->GetDrawImage().Image;
		else return m_Image.Image;
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

			m_Image.ImageFormat = MapImageFormat(desc.Format);
			m_Image.ImageExtent = drawImageExtent;

			VkImageUsageFlags drawImageUsages{};
			drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
			drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			VkImageCreateInfo rimg_info = VulkanInitializers::ImageCreateInfo(m_Image.ImageFormat, drawImageUsages, drawImageExtent);

			VmaAllocationCreateInfo rimg_allocinfo = {};
			rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			vmaCreateImage(m_Device->GetAllocator(), &rimg_info, &rimg_allocinfo, &m_Image.Image, &m_Image.Allocation, nullptr);

			VkImageViewCreateInfo rview_info = VulkanInitializers::ImageViewCreateInfo(m_Image.ImageFormat, m_Image.Image, VK_IMAGE_ASPECT_COLOR_BIT);

			vkCreateImageView(m_Device->GetDevice(), &rview_info, nullptr, &m_Image.ImageView);
		}
	}

}