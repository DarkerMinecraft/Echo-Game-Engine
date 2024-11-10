#include "pch.h"
#include "VulkanModel.h"

#include "Echo/Core/Application.h"

namespace Echo
{

	VulkanModel::VulkanModel(const std::vector<Vertex>& vertices)
		: m_Device((VulkanDevice*)Application::Get().GetWindow().GetDevice())
	{
		CreateVertexBuffers(vertices);
	}

	void VulkanModel::Destroy()
	{
		vkDestroyBuffer(m_Device->GetDevice(), m_VertexBuffer, nullptr);
		vkFreeMemory(m_Device->GetDevice(), m_VertexBufferMemory, nullptr);
	}

	void VulkanModel::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { m_VertexBuffer };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	void VulkanModel::Draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
	}

	void VulkanModel::CreateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_VertexCount = static_cast<uint32_t>(vertices.size());
		EC_CORE_ASSERT(m_VertexCount >= 3, "Vertex count must be at least 3")

		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
		m_Device->CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_VertexBuffer,
			m_VertexBufferMemory);

		void* data;
		vkMapMemory(m_Device->GetDevice(), m_VertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_Device->GetDevice(), m_VertexBufferMemory);
	}

	std::vector<VkVertexInputBindingDescription> VulkanModel::VulkanVertex::GetBindingDescription()
	{
		return { {0, sizeof(VulkanVertex), VK_VERTEX_INPUT_RATE_VERTEX} };
	}

	std::vector<VkVertexInputAttributeDescription> VulkanModel::VulkanVertex::GetAttributeDescription()
	{
		return { {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VulkanVertex, Position)}, {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Color)}};
	}

}