#pragma once

#include "VulkanDevice.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Echo 
{

	class VulkanSwapchain 
	{
	public:
		VulkanSwapchain(VulkanDevice* device, uint32_t width, uint32_t height, VulkanSwapchain* oldSwapchain = nullptr);
		~VulkanSwapchain();

		uint32_t AcquireNextImage();

		VkImage GetSwapchainImage(uint32_t imageIndex) { return m_SwapchainImages[imageIndex]; }
		VkImageView GetSwapchainImageView(uint32_t imageIndex) { return m_SwapchainImageViews[imageIndex]; }

		VkSwapchainKHR GetSwapchain() { return m_Swapchain; }
		VkFormat GetFormat() { return m_SwapchainImageFormat; }

		VkExtent2D GetExtent() { return m_SwapchainExtent; }
		
		void DestroySwapchain();
	private:
		void CreateSwapchain(uint32_t width, uint32_t height);
		void CreateSwapchain(uint32_t width, uint32_t height, VkSwapchainKHR& oldSwapchain);
	private:
		VulkanDevice* m_Device;

		VkSwapchainKHR m_Swapchain;
		VkFormat m_SwapchainImageFormat;

		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;
		VkExtent2D m_SwapchainExtent;
	};

}