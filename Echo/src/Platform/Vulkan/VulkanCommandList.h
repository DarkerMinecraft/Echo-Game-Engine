#pragma once

#include "Echo/Graphics/CommandList.h"

#include "VulkanDevice.h"

namespace Echo 
{
	class VulkanCommandBuffer : public CommandBuffer 
	{
	public:
		VulkanCommandBuffer(VulkanDevice* device, VkCommandPool commandPool);
		virtual ~VulkanCommandBuffer();

		virtual void Begin(uint32_t imageCount) override;
		virtual void Draw(uint32_t imageCount, Vertex vertex) override;
		virtual void End(uint32_t imageCount) override;
	public:
		VkCommandBuffer GetBuffer() { return m_CommandBuffer; }
	private:
		void CreateCommandBuffer();
	private:
		VulkanDevice* m_Device;
		VkCommandPool m_CommandPool;

		VkCommandBuffer m_CommandBuffer;
	};
}