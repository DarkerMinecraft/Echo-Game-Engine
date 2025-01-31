#pragma once

#include "Echo/Graphics/Buffer.h"

#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanBuffer : public Buffer 
	{
	public:
		VulkanBuffer(VulkanDevice* device, const BufferDesc& bufferDescription);
		virtual ~VulkanBuffer();

		virtual void BindBuffer() override;
		virtual void WriteToBuffer(void* data) override;

		virtual void* GetNativeBuffer() override { return m_Buffer; }
	private:
		void CreateBuffer(const BufferDesc& desc);

		void CreateStagingBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	private:
		VulkanDevice* m_Device;

		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;

		uint32_t m_BufferUsage;
	};

}