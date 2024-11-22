#include "pch.h"
#include "VulkanCommandList.h"

namespace Echo
{

	VulkanCommandPool::VulkanCommandPool(VulkanDevice* device, uint32_t queueFamily, unsigned int frameOverlay)
		: m_Device(device)
	{
		CreateCommandPool(queueFamily, frameOverlay);
	}

	VulkanCommandPool::~VulkanCommandPool()
	{
		vkDestroyCommandPool(m_Device->GetDevice(), m_CommandPool, nullptr);
	}

	void VulkanCommandPool::CreateCommandPool(uint32_t queueFamily, unsigned int frameOverlay)
	{
		VkCommandPoolCreateInfo commandPoolInfo = {};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.pNext = nullptr;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolInfo.queueFamilyIndex = queueFamily;

		if (vkCreateCommandPool(m_Device->GetDevice(), &commandPoolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create command pool");
		}
	}

	VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, VkCommandPool commandPool)
		: m_Device(device), m_CommandPool(commandPool)
	{
		CreateCommandBuffer(commandPool);
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		vkFreeCommandBuffers(m_Device->GetDevice(), m_CommandPool, 1, &m_CommandBuffer);
	}

	void VulkanCommandBuffer::Begin()
	{
		vkResetCommandBuffer(m_CommandBuffer, 0);

		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.pNext = nullptr;

		info.pInheritanceInfo = nullptr;
		info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(m_CommandBuffer, &info) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}
	}

	void VulkanCommandBuffer::End()
	{
		if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to end recording command buffer");
		}
	}

	void VulkanCommandBuffer::CreateCommandBuffer(VkCommandPool commandPool)
	{
		VkCommandBufferAllocateInfo cmdAllocInfo = {};
		cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdAllocInfo.pNext = nullptr;
		cmdAllocInfo.commandPool = commandPool;
		cmdAllocInfo.commandBufferCount = 1;
		cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		if (vkAllocateCommandBuffers(m_Device->GetDevice(), &cmdAllocInfo, &m_CommandBuffer) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}
}
