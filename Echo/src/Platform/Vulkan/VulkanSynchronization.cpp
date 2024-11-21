#include "pch.h"
#include "VulkanSynchronization.h"

namespace Echo 
{



	VulkanSemaphore::VulkanSemaphore(VulkanDevice* device)
		: m_Device(device)
	{
		CreateSemaphore();
	}

	VulkanSemaphore::~VulkanSemaphore()
	{
		vkDestroySemaphore(m_Device->GetDevice(), m_Semaphore, nullptr);
	}

	void VulkanSemaphore::CreateSemaphore()
	{
		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		info.pNext = nullptr;
		
		if (vkCreateSemaphore(m_Device->GetDevice(), &info, nullptr, &m_Semaphore) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create semaphore");
		}
	}

	VulkanFence::VulkanFence(VulkanDevice* device)
		: m_Device(device)
	{
		CreateFence();
	}

	VulkanFence::~VulkanFence()
	{
		vkDestroyFence(m_Device->GetDevice(), m_Fence, nullptr);
	}

	void VulkanFence::Wait()
	{
		vkWaitForFences(m_Device->GetDevice(), 1, &m_Fence, true, 1000000000);
	}

	void VulkanFence::Reset()
	{
		vkResetFences(m_Device->GetDevice(), 1, &m_Fence);
	}

	void VulkanFence::CreateFence()
	{
		VkFenceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateFence(m_Device->GetDevice(), &info, nullptr, &m_Fence) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create fence");
		}
	}

}