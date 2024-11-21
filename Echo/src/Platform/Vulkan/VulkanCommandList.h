#pragma once

#include "Echo/Graphics/CommandList.h"

#include "VulkanDevice.h"

#include <cstdint>

#include <vulkan/vulkan.h>

namespace Echo 
{
	class VulkanCommandPool : public CommandPool 
	{
	public:
		VulkanCommandPool(VulkanDevice* device, uint32_t queueFamily, unsigned int frameOverlay);
		virtual ~VulkanCommandPool();

		virtual void* GetPool() override { return m_CommandPool; }
	private:
		void CreateCommandPool(uint32_t queueFamily, unsigned int frameOverlay);
	private:
		VulkanDevice* m_Device;

		VkCommandPool m_CommandPool;
	};

	class VulkanCommandBuffer : public CommandBuffer 
	{
	public:
		VulkanCommandBuffer(VulkanDevice* device, VkCommandPool commandPool);
		virtual ~VulkanCommandBuffer();

		virtual void* GetBuffer() override { return m_CommandBuffer; }

		virtual void Begin() override;
		virtual void End() override;
	public:
		void CreateCommandBuffer(VkCommandPool commandPool);
	private:
		VulkanDevice* m_Device;

		VkCommandBuffer m_CommandBuffer;
	};
}