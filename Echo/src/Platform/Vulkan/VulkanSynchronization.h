#pragma once

#include "Echo/Graphics/Synchronization.h"

#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanSemaphore : public Semaphore 
	{
	public:
		VulkanSemaphore(VulkanDevice* device);
		virtual ~VulkanSemaphore();

		virtual void* GetSemaphore() override { return m_Semaphore; }
	private:
		void CreateSemaphore();
	private:
		VulkanDevice* m_Device;

		VkSemaphore m_Semaphore;
	};

	class VulkanFence : public Fence 
	{
	public:
		VulkanFence(VulkanDevice* device);
		virtual ~VulkanFence(); 

		virtual void* GetFence() override { return m_Fence; }

		virtual void Wait() override;
		virtual void Reset() override;
	private:
		void CreateFence();
	private:
		VulkanDevice* m_Device;

		VkFence m_Fence;
	};
}