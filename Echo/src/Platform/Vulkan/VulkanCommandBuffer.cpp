#include "pch.h"
#include "VulkanCommandBuffer.h"

#include "VulkanSwapchain.h"
#include "Utils/VulkanInitializers.h"
#include "Utils/VulkanImages.h"
#include "VulkanFramebuffer.h"

#include "Core/Application.h"

#include <backends/imgui_impl_vulkan.h>

namespace Echo
{

	VulkanCommandBuffer::VulkanCommandBuffer(Device* device)
		: m_Device(static_cast<VulkanDevice*>(device)), m_FrameData(m_Device->GetFrameData())
	{

	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{

	}

	void VulkanCommandBuffer::Start()
	{
		m_FrameData = m_Device->GetFrameData();

		vkWaitForFences(m_Device->GetDevice(), 1, &m_FrameData.RenderFence, VK_TRUE, UINT64_MAX);
		vkResetFences(m_Device->GetDevice(), 1, &m_FrameData.RenderFence);

		if (m_FrameData.IsFirstPass)
		{
			m_FrameData.ImageIndex = m_Device->GetSwapchain().AcquireNextImage(
				m_FrameData.SwapchainSemaphore
			);


			if (m_FrameData.ImageIndex == -1)
			{
				m_Device->RecreateSwapchain(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(), &m_Device->GetSwapchain());
				return;
			}
		}

		m_ImageIndex = m_FrameData.ImageIndex;

		vkResetCommandBuffer(m_FrameData.CommandBuffer, 0);
		VkCommandBufferBeginInfo beginInfo = VulkanInitializers::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		vkBeginCommandBuffer(m_FrameData.CommandBuffer, &beginInfo);

		VulkanImages::TransitionImage(m_FrameData.CommandBuffer, m_Device->GetSwapchainImage(m_ImageIndex),
									  m_FrameData.IsFirstPass ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
									  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);


	}

	void VulkanCommandBuffer::End()
	{
		if (m_ShouldPresent)
		{
			if (m_DrawToSwapchain)
			{
				VulkanImages::TransitionImage(m_FrameData.CommandBuffer, m_Device->GetSwapchainImage(m_ImageIndex), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			}
			else
			{
				m_Framebuffer->TransitionImageLayout(m_FrameData.CommandBuffer, 0, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
				VulkanImages::TransitionImage(m_FrameData.CommandBuffer, m_Device->GetSwapchainImage(m_ImageIndex), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
				VulkanImages::CopyImageToImage(m_FrameData.CommandBuffer, m_Framebuffer->GetImage(0).Image, m_Device->GetSwapchainImage(m_ImageIndex), { m_Framebuffer->GetImage(0).ImageExtent.width, m_Framebuffer->GetImage(0).ImageExtent.height }, { m_Device->GetSwapchain().GetExtent().width, m_Device->GetSwapchain().GetExtent().height });
				m_Framebuffer->TransitionImageLayout(m_FrameData.CommandBuffer, 0, VK_IMAGE_LAYOUT_GENERAL);
				VulkanImages::TransitionImage(m_FrameData.CommandBuffer, m_Device->GetSwapchainImage(m_ImageIndex),
											  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
											  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			}
		}

		vkEndCommandBuffer(m_FrameData.CommandBuffer);
	}

	void VulkanCommandBuffer::Submit(bool isLastPass)
	{
		VkCommandBufferSubmitInfo cmdInfo = VulkanInitializers::CommandBufferSubmitInfo(m_FrameData.CommandBuffer);

		if (!m_ShouldPresent || !isLastPass)
		{
			VkSubmitInfo2 submitInfo = VulkanInitializers::SubmitInfo(&cmdInfo, nullptr, nullptr);
			vkQueueSubmit2(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_FrameData.RenderFence);
			vkWaitForFences(m_Device->GetDevice(), 1, &m_FrameData.RenderFence, VK_TRUE, UINT64_MAX);

			m_FrameData.IsFirstPass = false;
			return;
		}

		VkSemaphoreSubmitInfo signalSemaphoreInfo = VulkanInitializers::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, m_FrameData.RenderSemaphore);
		VkSemaphoreSubmitInfo waitSemaphoreInfo = VulkanInitializers::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, m_FrameData.SwapchainSemaphore);

		VkSubmitInfo2 submitInfo = VulkanInitializers::SubmitInfo(&cmdInfo, &signalSemaphoreInfo, &waitSemaphoreInfo);
		vkQueueSubmit2(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_FrameData.RenderFence);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		VkSwapchainKHR swapchain = m_Device->GetSwapchain().GetSwapchain();
		presentInfo.pSwapchains = &swapchain;
		presentInfo.swapchainCount = 1;

		VkSemaphore renderSemaphore = m_FrameData.RenderSemaphore;
		presentInfo.pWaitSemaphores = &renderSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &m_ImageIndex;
		VkResult presentResult = vkQueuePresentKHR(m_Device->GetGraphicsQueue(), &presentInfo);

		if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || Application::Get().GetWindow().WasWindowResized())
		{
			Application::Get().GetWindow().ResetWindowResizedFlag();
			m_Device->RecreateSwapchain(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(), &m_Device->GetSwapchain());
		}
		else if (presentResult != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}

		m_Device->AddFrame();
		m_FrameData.IsFirstPass = true;
	}


	void VulkanCommandBuffer::SetSourceFramebuffer(Ref<Framebuffer> framebuffer)
	{
		m_Framebuffer = static_cast<VulkanFramebuffer*>(framebuffer.get());
	}

}