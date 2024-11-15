#include "pch.h"
#include "VulkanSwapchain.h"
#include <GLFW/glfw3.h>
#include "VulkanCommandList.h"

namespace Echo 
{

	VulkanSwapchain::VulkanSwapchain(VulkanDevice* device, const SwapchainWin32CreateInfo& createInfo)
		: m_Device(device), m_CreateInfo(createInfo)
	{
		CreateSwapchain(createInfo);
		CreateImageViews();
		CreateRenderPass();
		CreateFramebuffers();
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		vkDestroyRenderPass(m_Device->GetDevice(), m_RenderPass, nullptr);
		
		for (auto framebuffer : m_SwapchainFramebuffers)
		{
			vkDestroyFramebuffer(m_Device->GetDevice(), framebuffer, nullptr);
		}

		for (auto imageView : m_SwapchainImageViews)
		{
			vkDestroyImageView(m_Device->GetDevice(), imageView, nullptr);
		}

		vkDestroySwapchainKHR(m_Device->GetDevice(), m_Swapchain, nullptr);
	}

	void VulkanSwapchain::StartRenderPass(uint32_t imageIndex)
	{
		VkExtent2D vExtent{};
		vExtent.width = m_Extent.Width;
		vExtent.height = m_Extent.Height;

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_SwapchainFramebuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = vExtent;

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(((VulkanCommandBuffer*) m_Device->GetCommandBuffer())->GetBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanSwapchain::SetState()
	{
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_Extent.Width);
		viewport.height = static_cast<float>(m_Extent.Height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(((VulkanCommandBuffer*)m_Device->GetCommandBuffer())->GetBuffer(), 0, 1, &viewport);

		VkExtent2D vExtent{};
		vExtent.width = m_Extent.Width;
		vExtent.height = m_Extent.Height;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = vExtent;
		vkCmdSetScissor(((VulkanCommandBuffer*)m_Device->GetCommandBuffer())->GetBuffer(), 0, 1, &scissor);
	}

	void VulkanSwapchain::EndRenderPass()
	{
		vkCmdEndRenderPass(((VulkanCommandBuffer*)m_Device->GetCommandBuffer())->GetBuffer());
	}

	void VulkanSwapchain::CreateSwapchain(const SwapchainWin32CreateInfo& swapchainCreateInfo)
	{
		SwapChainSupportDetails swapChainSupport = m_Device->QuerySwapChainSupport();

		ChooseSwapExtent(swapChainSupport.Capabilities);
		VkExtent2D vExtent{};
		vExtent.width = m_Extent.Width;
		vExtent.height = m_Extent.Height;

		uint32_t imageCount = swapchainCreateInfo.ImageCount;
		m_SwapchainImageFormat = m_Device->ConvertToVulkanFormat(swapchainCreateInfo.PreferredFormat);
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Device->GetSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = m_SwapchainImageFormat;
		createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		createInfo.imageExtent = vExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_CreateInfo.PresentMode == PresentMode::Mailbox ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(m_Device->GetDevice(), &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(m_Device->GetDevice(), m_Swapchain, &imageCount, nullptr);
		m_SwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device->GetDevice(), m_Swapchain, &imageCount, m_SwapchainImages.data());
	}

	void VulkanSwapchain::CreateImageViews()
	{
		m_SwapchainImageViews.resize(m_SwapchainImages.size());

		for (size_t i = 0; i < m_SwapchainImages.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapchainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_SwapchainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			if (vkCreateImageView(m_Device->GetDevice(), &createInfo, nullptr, &m_SwapchainImageViews[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create image views!");
			}
		}
	}

	void VulkanSwapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			m_Extent = {
				capabilities.currentExtent.width,
				capabilities.currentExtent.height
			};
		}
		else
		{
			int width, height;
			glfwGetFramebufferSize((GLFWwindow*)m_CreateInfo.Hwnd, &width, &height);

			Extent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.Width = std::clamp(actualExtent.Width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.Height = std::clamp(actualExtent.Height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			m_Extent = actualExtent;
		}
	}

	void VulkanSwapchain::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = m_Device->ConvertToVulkanFormat(m_CreateInfo.PreferredFormat);;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		if (vkCreateRenderPass(m_Device->GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create render pass!");
		}
	}

	void VulkanSwapchain::CreateFramebuffers()
	{
		m_SwapchainFramebuffers.resize(m_SwapchainImageViews.size());
		for (size_t i = 0; i < m_SwapchainImageViews.size(); i++)
		{
			VkImageView attachments[] = {
				m_SwapchainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_Extent.Width;
			framebufferInfo.height = m_Extent.Height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_Device->GetDevice(), &framebufferInfo, nullptr, &m_SwapchainFramebuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create framebuffer!");
			}
		}
	}

}