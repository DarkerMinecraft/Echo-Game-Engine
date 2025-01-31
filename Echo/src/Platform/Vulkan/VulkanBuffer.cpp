#include "pch.h"
#include "VulkanBuffer.h"

namespace Echo 
{



	VulkanBuffer::VulkanBuffer(VulkanDevice* device, const BufferDesc& bufferDescription)
		: m_Device(device)
	{
		CreateBuffer(bufferDescription);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		vkDestroyBuffer(m_Device->GetDevice(), m_Buffer, nullptr);
		vkFreeMemory(m_Device->GetDevice(), m_BufferMemory, nullptr);
	}

	void VulkanBuffer::BindBuffer()
	{
		VkCommandBuffer cmd = m_Device->GetActiveCommandBuffer();

		if (m_BufferUsage & BUFFER_USAGE_VERTEX) 
		{
			VkBuffer vertexBuffers[] = { m_Buffer };
			vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, { 0 });
		} else if (m_BufferUsage & BUFFER_USAGE_INDEX) 
		{
			vkCmdBindIndexBuffer(cmd, m_Buffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void VulkanBuffer::WriteToBuffer(void* data)
	{
		void* mappedData;
		vkMapMemory(m_Device->GetDevice(), m_BufferMemory, 0, sizeof(data), 0, &mappedData);
		memcpy(mappedData, data, sizeof(data));
		vkUnmapMemory(m_Device->GetDevice(), m_BufferMemory);
	}

	void VulkanBuffer::CreateBuffer(const BufferDesc& desc)
	{
		VkBufferUsageFlags usageFlags = 0;
		if (desc.Usage & BUFFER_USAGE_VERTEX) usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		if (desc.Usage & BUFFER_USAGE_INDEX) usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		if (desc.Usage & BUFFER_USAGE_UNIFORM) usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		if (desc.Usage & BUFFER_USAGE_STORAGE) usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if (desc.Usage & BUFFER_USAGE_TRANSFER_SRC) usageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if (desc.Usage & BUFFER_USAGE_TRANSFER_DST) usageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		if (desc.UseStagingBuffer) usageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		m_BufferUsage = desc.Usage;

		VkMemoryPropertyFlags memoryProperties = 0;
		if (desc.Flags & MEMORY_HOST_VISIBLE) memoryProperties |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		if (desc.Flags & MEMORY_DEVICE_LOCAL) memoryProperties |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		if (desc.Flags & MEMORY_HOST_COHERENT) memoryProperties |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = desc.BufferSize;
		bufferInfo.usage = usageFlags;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_Device->GetDevice(), &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_Device->GetDevice(), m_Buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = m_Device->FindMemoryType(memRequirements.memoryTypeBits, memoryProperties);

		if (vkAllocateMemory(m_Device->GetDevice(), &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate buffer memory!");
		}

		vkBindBufferMemory(m_Device->GetDevice(), m_Buffer, m_BufferMemory, 0);

		if (desc.InitialData && !desc.UseStagingBuffer)
		{
			void* mappedData;
			vkMapMemory(m_Device->GetDevice(), m_BufferMemory, 0, desc.BufferSize, 0, &mappedData);
			memcpy(mappedData, desc.InitialData, desc.BufferSize);
			vkUnmapMemory(m_Device->GetDevice(), m_BufferMemory);
		}

		if (desc.InitialData && desc.UseStagingBuffer) 
		{
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;

			CreateStagingBuffer(desc.BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(m_Device->GetDevice(), stagingBufferMemory, 0, desc.BufferSize, 0, &data);
			memcpy(data, desc.InitialData, desc.BufferSize);
			vkUnmapMemory(m_Device->GetDevice(), stagingBufferMemory);

			VkCommandBuffer cmd = m_Device->BeginSingleTimeCommands();
			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0; 
			copyRegion.size = desc.BufferSize;
			vkCmdCopyBuffer(cmd, stagingBuffer, m_Buffer, 1, &copyRegion);
			m_Device->EndSingleTimeCommands(cmd);

			vkDestroyBuffer(m_Device->GetDevice(), stagingBuffer, nullptr);
			vkFreeMemory(m_Device->GetDevice(), stagingBufferMemory, nullptr);
		}
	}

	void VulkanBuffer::CreateStagingBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_Device->GetDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_Device->GetDevice(), buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = m_Device->FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_Device->GetDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate buffer memory!");
		}

		vkBindBufferMemory(m_Device->GetDevice(), buffer, bufferMemory, 0);
	}

}