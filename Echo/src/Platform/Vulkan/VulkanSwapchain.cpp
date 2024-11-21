#include "pch.h"
#include "VulkanSwapchain.h"

#include "VkBootstrap.h"

namespace Echo 
{



	VulkanSwapchain::VulkanSwapchain(VulkanDevice* device, uint32_t width, uint32_t height)
		: m_Device(device)
	{
		CreateSwapchain(width, height);
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		DestroySwapchain();
	}

	uint32_t VulkanSwapchain::AcquireNextImage()
	{
		uint32_t imageIndex;
		VkSemaphore swapchainSemaphore = ((VkSemaphore)m_Device->GetCurrentFrame().SwapchainSemaphore->GetSemaphore());
		vkAcquireNextImageKHR(m_Device->GetDevice(),
							  m_Swapchain,
							  1000000000,
							  swapchainSemaphore,
							  nullptr,
							  &imageIndex);

		return imageIndex;
	}

	void VulkanSwapchain::CreateSwapchain(uint32_t width, uint32_t height)
	{
		vkb::SwapchainBuilder swapchainBuilder{ m_Device->GetPhysicalDevice(), m_Device->GetDevice(), m_Device->GetSurface()};

		m_SwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

		vkb::Swapchain vkbSwapchain = swapchainBuilder
			.set_desired_format(VkSurfaceFormatKHR{ .format = m_SwapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
			.set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
			.set_desired_extent(width, height)
			.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.build()
			.value();

		m_SwapchainExtent = vkbSwapchain.extent;

		//store swapchain and its related images
		m_Swapchain = vkbSwapchain.swapchain;
		m_SwapchainImages = vkbSwapchain.get_images().value();
		m_SwapchainImageViews = vkbSwapchain.get_image_views().value();
	}

	void VulkanSwapchain::DestroySwapchain()
	{
		vkDestroySwapchainKHR(m_Device->GetDevice(), m_Swapchain, nullptr);

		for (int i = 0; i < m_SwapchainImageViews.size(); i++)
		{

			vkDestroyImageView(m_Device->GetDevice(), m_SwapchainImageViews[i], nullptr);
		}
	}

}