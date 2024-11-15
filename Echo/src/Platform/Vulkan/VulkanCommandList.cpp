#include "pch.h"
#include "VulkanCommandList.h"

namespace Echo 
{

	VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, VkCommandPool commandPool)
		: m_Device(device), m_CommandPool(commandPool)
	{
		CreateCommandBuffer();
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{

	}

	void VulkanCommandBuffer::Begin(uint32_t imageCount)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		m_Device->GetSwapchain()->StartRenderPass(imageCount);
		m_Device->GetSwapchain()->SetState();
	}

	void VulkanCommandBuffer::Draw(Vertex vertex)
	{

	}

	void VulkanCommandBuffer::End()
	{
		m_Device->GetSwapchain()->EndRenderPass();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void VulkanCommandBuffer::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(m_Device->GetDevice(), &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers!");
		}
	}

}