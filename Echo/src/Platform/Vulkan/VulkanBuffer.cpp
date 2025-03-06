#include "pch.h"
#include "VulkanBuffer.h"

#include "VulkanCommandBuffer.h"

#include <vk_mem_alloc.h>

namespace Echo
{

	VulkanVertexBuffer::VulkanVertexBuffer(Device* device, std::vector<float> vertices)
		: m_Device((VulkanDevice*)device)
	{
		CreateBuffer(vertices);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(Device* device, std::vector<float> vertices, std::vector<float> colors)
		: m_Device((VulkanDevice*)device)
	{
		CreateBuffer(vertices, colors);
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

	void VulkanVertexBuffer::CreateBuffer(std::vector<float> vertices)
	{
		const size_t bufferSize = vertices.size() * sizeof(float);

		m_Buffer = m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
										  VMA_MEMORY_USAGE_GPU_ONLY);

		AllocatedBuffer staging = m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		void* data = m_Device->GetMappedData(staging);

		memcpy(data, vertices.data(), bufferSize);

		m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			VkBufferCopy vertexCopy{ 0 };
			vertexCopy.dstOffset = 0;
			vertexCopy.srcOffset = 0;
			vertexCopy.size = bufferSize;

			vkCmdCopyBuffer(cmd, staging.Buffer, m_Buffer.Buffer, 1, &vertexCopy);
		});

		m_Device->DestroyBuffer(staging);
	}

	void VulkanVertexBuffer::CreateBuffer(std::vector<float> vertices, std::vector<float> colors)
	{
		const size_t positionComponentCount = 2;
		const size_t colorComponentCount = 3;

		size_t vertexCount = vertices.size() / positionComponentCount;

		const size_t interleavedComponentCount = positionComponentCount + colorComponentCount;

		std::vector<float> interleaved;
		interleaved.resize(vertexCount * interleavedComponentCount);

		for (size_t i = 0; i < vertexCount; i++)
		{
			for (size_t j = 0; j < positionComponentCount; j++)
			{
				interleaved[i * interleavedComponentCount + j] = vertices[i * positionComponentCount + j];
			}

			for (size_t j = 0; j < colorComponentCount; j++)
			{
				interleaved[i * interleavedComponentCount + positionComponentCount + j] = colors[i * colorComponentCount + j];
			}
		}

		const size_t bufferSize = interleaved.size() * sizeof(float);

		m_Buffer = m_Device->CreateBuffer(bufferSize,
										  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
										  VMA_MEMORY_USAGE_GPU_ONLY);

		AllocatedBuffer staging = m_Device->CreateBuffer(bufferSize,
														 VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		void* data = m_Device->GetMappedData(staging);
		memcpy(data, interleaved.data(), bufferSize);

		m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = bufferSize;
			vkCmdCopyBuffer(cmd, staging.Buffer, m_Buffer.Buffer, 1, &copyRegion);
		});

		m_Device->DestroyBuffer(staging);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(Device* device, std::vector<uint32_t> indices)
		: m_Device((VulkanDevice*)device)
	{
		CreateBuffer(indices);
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

	void VulkanIndexBuffer::CreateBuffer(std::vector<uint32_t> indices)
	{
		const size_t bufferSize = indices.size() * sizeof(uint32_t);

		m_Buffer = m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
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
	}

}