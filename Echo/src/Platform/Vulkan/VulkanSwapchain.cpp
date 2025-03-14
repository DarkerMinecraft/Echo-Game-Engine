#include "pch.h"
#include "VulkanSwapchain.h"

#include "VkBootstrap.h"

namespace Echo
{

	VulkanSwapchain::VulkanSwapchain(VulkanDevice* device, uint32_t width, uint32_t height)
		: m_Device(device), m_Width(width), m_Height(height)
	{
		CreateSwapchain();
	}

	VulkanSwapchain::VulkanSwapchain(VulkanDevice* device, VulkanSwapchain* oldSwapchain, uint32_t width, uint32_t height)
		: m_Device(device), m_Width(width), m_Height(height)
	{
		CreateSwapchain(oldSwapchain);
		oldSwapchain->DestroySwapchain();
	}

	void VulkanSwapchain::CreateSwapchain()
	{
		vkb::SwapchainBuilder swapchainBuilder{ m_Device->GetPhysicalDevice(), m_Device->GetDevice(), m_Device->GetSurface()};

		m_Format = VK_FORMAT_B8G8R8A8_UNORM;

		vkb::Swapchain vkbSwapchain = swapchainBuilder
			.set_desired_format(VkSurfaceFormatKHR{ .format = m_Format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
			.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
			.set_desired_extent(m_Width, m_Height)
			.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.build()
			.value();

		m_Extent = vkbSwapchain.extent;

		m_Swapchain = vkbSwapchain.swapchain;
		m_Images = vkbSwapchain.get_images().value();
		m_ImageViews = vkbSwapchain.get_image_views().value();
	}

	uint32_t VulkanSwapchain::AcquireNextImage(VkSemaphore semaphore)
	{
		uint32_t imageIndex;
		vkAcquireNextImageKHR(m_Device->GetDevice(), m_Swapchain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &imageIndex);

		return imageIndex;
	}

	void VulkanSwapchain::DestroySwapchain()
	{
		vkDestroySwapchainKHR(m_Device->GetDevice(), m_Swapchain, nullptr);

		for (int i = 0; i < m_ImageViews.size(); i++)
		{
			vkDestroyImageView(m_Device->GetDevice(), m_ImageViews[i], nullptr);
		}
	}

	void VulkanSwapchain::CreateSwapchain(VulkanSwapchain* oldSwapchain)
	{
		vkb::SwapchainBuilder swapchainBuilder{ m_Device->GetPhysicalDevice(), m_Device->GetDevice(), m_Device->GetSurface() };

		m_Format = VK_FORMAT_B8G8R8A8_UNORM;

		vkb::Swapchain vkbSwapchain = swapchainBuilder
			.set_desired_format(VkSurfaceFormatKHR{ .format = m_Format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
			.set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
			.set_desired_extent(m_Width, m_Height)
			.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.set_old_swapchain(oldSwapchain->GetSwapchain())
			.build()
			.value();

		m_Extent = vkbSwapchain.extent;

		m_Swapchain = vkbSwapchain.swapchain;
		m_Images = vkbSwapchain.get_images().value();
		m_ImageViews = vkbSwapchain.get_image_views().value();
	}

}

