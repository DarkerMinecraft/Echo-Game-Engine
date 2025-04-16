#include "pch.h"
#include "VulkanBuffer.h"

#include "VulkanCommandBuffer.h"

#include <vk_mem_alloc.h>

namespace Echo
{

	VulkanVertexBuffer::VulkanVertexBuffer(Device* device, float* data, uint32_t size, bool isDynamic)
		: m_Device((VulkanDevice*)device)
	{
		CreateBuffer(data, size, isDynamic);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(Device* device, uint32_t size, bool isDynamic)
		: m_Device((VulkanDevice*)device)
	{
		CreateBuffer(size, isDynamic);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		m_Device->DestroyBuffer(m_Buffer);
	}

	void VulkanVertexBuffer::Bind(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();

		VkBuffer vertexBuffers[] = { m_Buffer.Buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	}

	void VulkanVertexBuffer::SetData(void* data, uint32_t size)
	{
		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(m_Device->GetDevice(), m_Buffer.Buffer, &memReqs);

		if (size > memReqs.size)
		{
			AllocatedBuffer oldBuffer = m_Buffer;

			VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
				VK_BUFFER_USAGE_TRANSFER_DST_BIT |
				VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
			m_Buffer = m_Device->CreateBuffer(size, usageFlags, VMA_MEMORY_USAGE_GPU_ONLY);

			m_Device->DestroyBuffer(oldBuffer);
		}

		AllocatedBuffer staging = m_Device->CreateBuffer(
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_CPU_ONLY
		);

		void* mappedData = m_Device->GetMappedData(staging);
		memcpy(mappedData, data, size);

		m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			VkBufferCopy copy{};
			copy.srcOffset = 0;
			copy.dstOffset = 0;
			copy.size = size;

			vkCmdCopyBuffer(cmd, staging.Buffer, m_Buffer.Buffer, 1, &copy);
		});

		m_Device->DestroyBuffer(staging);
	}

	void VulkanVertexBuffer::CreateBuffer(float* data, uint32_t size, bool isDynamic)
	{
		m_Buffer = m_Device->CreateBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
										  isDynamic ? VMA_MEMORY_USAGE_GPU_ONLY : VMA_MEMORY_USAGE_CPU_TO_GPU);

		AllocatedBuffer staging = m_Device->CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		void* mappedData = m_Device->GetMappedData(staging);

		memcpy(mappedData, data, size);

		m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			VkBufferCopy vertexCopy{ 0 };
			vertexCopy.dstOffset = 0;
			vertexCopy.srcOffset = 0;
			vertexCopy.size = size;

			vkCmdCopyBuffer(cmd, staging.Buffer, m_Buffer.Buffer, 1, &vertexCopy);
		});

		m_Device->DestroyBuffer(staging);
	}

	void VulkanVertexBuffer::CreateBuffer(uint32_t size, bool isDynamic)
	{
		m_Buffer = m_Device->CreateBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
										  isDynamic ? VMA_MEMORY_USAGE_GPU_ONLY : VMA_MEMORY_USAGE_CPU_TO_GPU);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(Device* device, std::vector<uint32_t> indices)
		: m_Device((VulkanDevice*)device)
	{
		CreateBuffer(indices);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(Device* device, uint32_t* indices, uint32_t count)
		: m_Device((VulkanDevice*)device)
	{
		CreateBuffer(indices, count);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		m_Device->DestroyBuffer(m_Buffer);
	}

	void VulkanIndexBuffer::Bind(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();

		vkCmdBindIndexBuffer(commandBuffer, m_Buffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanIndexBuffer::SetIndices(std::vector<uint32_t> indices)
	{

	}

	void VulkanIndexBuffer::SetIndices(uint32_t* indices, uint32_t count)
	{

	}

	void VulkanIndexBuffer::CreateBuffer(std::vector<uint32_t> indices)
	{
		const size_t bufferSize = indices.size() * sizeof(uint32_t);

		m_Buffer = m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
										  VMA_MEMORY_USAGE_GPU_ONLY);

		AllocatedBuffer staging = m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		void* data = m_Device->GetMappedData(staging);

		memcpy(data, indices.data(), bufferSize);

		m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			VkBufferCopy indexCopy{ 0 };
			indexCopy.dstOffset = 0;
			indexCopy.srcOffset = 0;
			indexCopy.size = bufferSize;

			vkCmdCopyBuffer(cmd, staging.Buffer, m_Buffer.Buffer, 1, &indexCopy);
		});

		m_Device->DestroyBuffer(staging);
		m_IndicesCount = indices.size();
	}

	void VulkanIndexBuffer::CreateBuffer(uint32_t* indices, uint32_t count)
	{
		const size_t bufferSize = count * sizeof(uint32_t);

		m_Buffer = m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
										  VMA_MEMORY_USAGE_GPU_ONLY);

		AllocatedBuffer staging = m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		void* data = m_Device->GetMappedData(staging);

		memcpy(data, indices, bufferSize);

		m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			VkBufferCopy indexCopy{ 0 };
			indexCopy.dstOffset = 0;
			indexCopy.srcOffset = 0;
			indexCopy.size = bufferSize;

			vkCmdCopyBuffer(cmd, staging.Buffer, m_Buffer.Buffer, 1, &indexCopy);
		});

		m_Device->DestroyBuffer(staging);
		m_IndicesCount = count;
	}

	VulkanIndirectBuffer::VulkanIndirectBuffer(Device* device)
		: m_Device((VulkanDevice*)device)
	{
		CreateBuffer();
	}

	VulkanIndirectBuffer::~VulkanIndirectBuffer()
	{
		m_Device->DestroyBuffer(m_Buffer);
	}

	void VulkanIndirectBuffer::AddToIndirectBuffer(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
	{
		VkDrawIndexedIndirectCommand cmd{};
		cmd.indexCount = indexCount;
		cmd.instanceCount = instanceCount;
		cmd.firstIndex = firstIndex;
		cmd.vertexOffset = vertexOffset;
		cmd.firstInstance = firstInstance;
		m_IndirectCommands.push_back(cmd);

		UpdateIndirectBufferGPU();
	}

	void VulkanIndirectBuffer::ClearIndirectBuffer()
	{
		m_IndirectCommands.clear();

		AllocatedBuffer oldBuffer = m_Buffer;
		CreateBuffer();

		m_Device->DestroyBuffer(oldBuffer);
	}

	void VulkanIndirectBuffer::CreateBuffer()
	{
		uint32_t bufferSize = sizeof(VkDrawIndexedIndirectCommand) * 100;

		m_Buffer = m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
										  VMA_MEMORY_USAGE_GPU_ONLY);

		if (m_IndirectCommands.empty())
			return; 

		AllocatedBuffer stagingBuffer = m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		void* data = m_Device->GetMappedData(stagingBuffer);

		memcpy(data, m_IndirectCommands.data(), bufferSize);

		m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			VkBufferCopy copy{ 0 };
			copy.srcOffset = 0;
			copy.dstOffset = 0;
			copy.size = bufferSize;
			vkCmdCopyBuffer(cmd, stagingBuffer.Buffer, m_Buffer.Buffer, 1, &copy);
		});

		m_Device->DestroyBuffer(stagingBuffer);
	}

	void VulkanIndirectBuffer::UpdateIndirectBufferGPU()
	{
		uint32_t bufferSize = sizeof(VkDrawIndexedIndirectCommand) * m_IndirectCommands.size();

		AllocatedBuffer stagingBuffer = m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		void* data = m_Device->GetMappedData(stagingBuffer);

		m_Device->ImmediateSubmit([&](VkCommandBuffer cmd) 
		{
			memcpy(data, m_IndirectCommands.data(), bufferSize);
			VkBufferCopy copy{ 0 };
			copy.srcOffset = 0;
			copy.dstOffset = 0;
			copy.size = bufferSize;
			vkCmdCopyBuffer(cmd, stagingBuffer.Buffer, m_Buffer.Buffer, 1, &copy);
		});

		m_Device->DestroyBuffer(stagingBuffer);
	}

	VulkanUniformBuffer::VulkanUniformBuffer(Device* device, void* data, uint32_t size)
		: m_Device((VulkanDevice*)device), m_Size(size)
	{
		CreateBuffer(data, size);
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		m_Device->DestroyBuffer(m_Buffer);
	}

	void VulkanUniformBuffer::SetData(void* data, uint32_t size)
	{
		m_Size = size;

		AllocatedBuffer oldBuffer = m_Buffer;

		m_Buffer = m_Device->CreateBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
										  VMA_MEMORY_USAGE_CPU_TO_GPU);

		AllocatedBuffer staging = m_Device->CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		void* mappedData = m_Device->GetMappedData(staging);

		memcpy(mappedData, data, size);

		m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			VkBufferCopy copy{};
			copy.srcOffset = 0;
			copy.dstOffset = 0;
			copy.size = size;
			vkCmdCopyBuffer(cmd, staging.Buffer, m_Buffer.Buffer, 1, &copy);
		});

		m_Device->DestroyBuffer(staging);
		m_Device->DestroyBuffer(oldBuffer);
	}

	void VulkanUniformBuffer::CreateBuffer(void* data, uint32_t size)
	{
		m_Buffer = m_Device->CreateBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
										  VMA_MEMORY_USAGE_CPU_TO_GPU);

		AllocatedBuffer staging = m_Device->CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		void* mappedData = m_Device->GetMappedData(staging);

		memcpy(mappedData, data, size);

		m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			VkBufferCopy copy{};
			copy.srcOffset = 0;
			copy.dstOffset = 0;
			copy.size = size;
			vkCmdCopyBuffer(cmd, staging.Buffer, m_Buffer.Buffer, 1, &copy);
		});

		m_Device->DestroyBuffer(staging);
	}

}