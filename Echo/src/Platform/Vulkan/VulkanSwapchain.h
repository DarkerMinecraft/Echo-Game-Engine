#pragma once

#include "Echo/Graphics/Swapchain.h"
#include "VulkanDevice.h"

namespace Echo 
{
	class VulkanSwapchain : public Swapchain 
	{
	public:
		VulkanSwapchain(VulkanDevice* device, const SwapchainWin32CreateInfo& createInfo);
		~VulkanSwapchain();

		virtual Extent2D GetExtent() override { return m_Extent; }

		virtual void StartRenderPass(uint32_t imageIndex) override;
		virtual void SetState();
		virtual void EndRenderPass() override;
	public:
		VkSwapchainKHR GetSwapchain() { return m_Swapchain; }
		VkRenderPass GetRenderPass() { return m_RenderPass; }
	private:
		void CreateSwapchain(const SwapchainWin32CreateInfo& createInfo);
		void CreateImageViews(); 

		void ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		void CreateRenderPass();

		void CreateFramebuffers();
	private:
		const SwapchainWin32CreateInfo m_CreateInfo;
		VulkanDevice* m_Device;
		Extent2D m_Extent;

		VkSwapchainKHR m_Swapchain;
		VkFormat m_SwapchainImageFormat;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;
		std::vector<VkFramebuffer> m_SwapchainFramebuffers;

		VkRenderPass m_RenderPass;

	};
}