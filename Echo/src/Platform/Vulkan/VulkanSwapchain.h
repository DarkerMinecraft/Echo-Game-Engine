#pragma once 

#include "Primitives/VulkanDevice.h"

namespace Echo 
{

	class VulkanSwapchain 
	{
	public:
		VulkanSwapchain(VulkanDevice* device, uint32_t width, uint32_t height);
		VulkanSwapchain(VulkanDevice* device, VulkanSwapchain* oldSwapchain, uint32_t width, uint32_t height);

		VkExtent2D GetExtent() { return m_Extent; }
		VkSwapchainKHR GetSwapchain() { return m_Swapchain; }

		uint32_t AcquireNextImage(VkSemaphore semaphore);

		VkImage GetImage(uint32_t index) { return m_Images[index]; }
		VkImageView GetImageView(uint32_t index) { return m_ImageViews[index]; }

		void DestroySwapchain();
	private:
		void CreateSwapchain();
		void CreateSwapchain(VulkanSwapchain* oldSwapchain);
	private:
		VulkanDevice* m_Device;
		uint32_t m_Width, m_Height;
		
		VkSwapchainKHR m_Swapchain;
		VkFormat m_Format;
		VkExtent2D m_Extent;

		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;
	};

}