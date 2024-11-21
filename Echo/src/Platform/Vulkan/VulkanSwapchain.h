#pragma once

#include "Echo/Graphics/Swapchain.h"
#include <cstdint>
#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanSwapchain : public Swapchain 
	{
	public:
		VulkanSwapchain(VulkanDevice* device, uint32_t width, uint32_t height);
		virtual ~VulkanSwapchain();

		virtual uint32_t AcquireNextImage() override;

		virtual void* GetSwapchainImage(uint32_t imageIndex) override { return m_SwapchainImages[imageIndex]; }
		virtual void* GetSwapchainImageView(uint32_t imageIndex) override { return m_SwapchainImageViews[imageIndex]; }

		virtual Extent2D GetExtent() override { 
			Extent2D extent{};
			extent.Width = m_SwapchainExtent.width;
			extent.Height = m_SwapchainExtent.height;

			return extent;
		}
	public:
		VkSwapchainKHR GetSwapchain() { return m_Swapchain; }
		VkFormat GetFormat() { return m_SwapchainImageFormat; }
	private:
		void CreateSwapchain(uint32_t width, uint32_t height);
		void DestroySwapchain(); 
	private:
		VulkanDevice* m_Device;

		VkSwapchainKHR m_Swapchain;
		VkFormat m_SwapchainImageFormat;

		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;
		VkExtent2D m_SwapchainExtent;
	};

}