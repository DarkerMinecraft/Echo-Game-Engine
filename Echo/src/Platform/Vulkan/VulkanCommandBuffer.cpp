#include "pch.h"
#include "VulkanCommandBuffer.h"

#include "VulkanSwapchain.h"
#include "Utils/VulkanInitializers.h"
#include "Utils/VulkanImages.h"
#include "VulkanImage.h"

#include "Echo/Core/Application.h"

namespace Echo 
{

	VulkanCommandBuffer::VulkanCommandBuffer(Device* device)
		: m_Device((VulkanDevice*) device)
	{
		InitCommands();

		m_SrcImage = m_Device->GetDrawImage();
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		vkDeviceWaitIdle(m_Device->GetDevice());
		vkQueueWaitIdle(m_Device->GetGraphicsQueue());

		vkFreeCommandBuffers(m_Device->GetDevice(), m_CommandPool, 1, &m_CommandBuffer);
		vkDestroyCommandPool(m_Device->GetDevice(), m_CommandPool, nullptr);

		vkDestroyFence(m_Device->GetDevice(), m_RenderFence, nullptr);
		vkDestroySemaphore(m_Device->GetDevice(), m_WaitSemaphore, nullptr);
		vkDestroySemaphore(m_Device->GetDevice(), m_SignalSemaphore, nullptr);
	}

	void VulkanCommandBuffer::Start()
	{
		vkDeviceWaitIdle(m_Device->GetDevice());

		vkWaitForFences(m_Device->GetDevice(), 1, &m_RenderFence, VK_TRUE, UINT64_MAX);
		vkResetFences(m_Device->GetDevice(), 1, &m_RenderFence);

		m_ImageIndex = m_Device->GetSwapchain().AcquireNextImage(m_WaitSemaphore);
		if (m_ImageIndex == -1) 
		{
			m_Device->RecreateSwapchain(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(), &m_Device->GetSwapchain());
			m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
			{
				VulkanImages::TransitionImage(cmd, m_Device->GetDrawImage().Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
			});
			return;
		}

		Reset();
		VkCommandBufferBeginInfo beginInfo = VulkanInitializers::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
	}

	void VulkanCommandBuffer::End()
	{
		VulkanImages::TransitionImage(m_CommandBuffer, m_Device->GetSwapchainImage(m_ImageIndex), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VulkanImages::TransitionImage(m_CommandBuffer, m_SrcImage.Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

		VulkanImages::CopyImageToImage(m_CommandBuffer, m_SrcImage.Image, m_Device->GetSwapchainImage(m_ImageIndex), { m_SrcImage.ImageExtent.width, m_SrcImage.ImageExtent.height }, m_Device->GetSwapchain().GetExtent());
		VulkanImages::TransitionImage(m_CommandBuffer, m_Device->GetSwapchainImage(m_ImageIndex), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		vkEndCommandBuffer(m_CommandBuffer);
	}

	void VulkanCommandBuffer::Submit()
	{
		VkCommandBufferSubmitInfo cmdInfo = VulkanInitializers::CommandBufferSubmitInfo(m_CommandBuffer);
		VkSemaphoreSubmitInfo signalSemaphoreInfo = VulkanInitializers::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, m_SignalSemaphore);
		VkSemaphoreSubmitInfo waitSemaphoreInfo = VulkanInitializers::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, m_WaitSemaphore);

		VkSubmitInfo2 submitInfo = VulkanInitializers::SubmitInfo(&cmdInfo, &signalSemaphoreInfo, &waitSemaphoreInfo);
		vkQueueSubmit2(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_RenderFence);

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		VkSwapchainKHR swapchain = m_Device->GetSwapchain().GetSwapchain();
		presentInfo.pSwapchains = &swapchain;
		presentInfo.swapchainCount = 1;

		presentInfo.pWaitSemaphores = &m_SignalSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &m_ImageIndex;

		VkResult presentResult = vkQueuePresentKHR(m_Device->GetGraphicsQueue(), &presentInfo);

		if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || Application::Get().GetWindow().WasWindowResized())
		{
			Application::Get().GetWindow().ResetWindowResizedFlag();
			m_Device->RecreateSwapchain(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(), &m_Device->GetSwapchain());

			m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
			{
				VulkanImages::TransitionImage(cmd, m_Device->GetDrawImage().Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
			});
		}
		else if (presentResult != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}
		if (presentResult == VK_SUCCESS) 
		{
			m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
			{
				VulkanImages::TransitionImage(cmd, m_Device->GetDrawImage().Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
			});
		}
	}

	void VulkanCommandBuffer::Reset()
	{
		vkResetCommandBuffer(m_CommandBuffer, 0);
	}

	void VulkanCommandBuffer::SetSourceImage(Ref<Image> srcImage)
	{
		m_SrcImage = ((VulkanImage*)srcImage.get())->GetImage();
	}

	void VulkanCommandBuffer::InitCommands()
	{
		VkCommandPoolCreateInfo poolCreateInfo = VulkanInitializers::CommandPoolCreateInfo(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_Device->GetGraphicsQueueFamily());
		vkCreateCommandPool(m_Device->GetDevice(), &poolCreateInfo, nullptr, &m_CommandPool);
		VkCommandBufferAllocateInfo cmdCreateInfo = VulkanInitializers::CommandBufferAllocateInfo(m_CommandPool, 1);
		vkAllocateCommandBuffers(m_Device->GetDevice(), &cmdCreateInfo, &m_CommandBuffer);

		VkFenceCreateInfo fenceCreateInfo = VulkanInitializers::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		vkCreateFence(m_Device->GetDevice(), &fenceCreateInfo, nullptr, &m_RenderFence);

		VkSemaphoreCreateInfo semaphoreCreateInfo = VulkanInitializers::SemaphoreCreateInfo();
		vkCreateSemaphore(m_Device->GetDevice(), &semaphoreCreateInfo, nullptr, &m_WaitSemaphore);
		vkCreateSemaphore(m_Device->GetDevice(), &semaphoreCreateInfo, nullptr, &m_SignalSemaphore);
	}

}