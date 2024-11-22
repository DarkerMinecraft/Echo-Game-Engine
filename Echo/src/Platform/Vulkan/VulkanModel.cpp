#include "pch.h"
#include "VulkanModel.h"

#include "Echo/Core/Application.h"

namespace Echo
{

	VulkanModel::VulkanModel(std::vector<uint32_t> indices, std::vector<Vertex> vertices)
		: m_Device((VulkanDevice*)Application::Get().GetWindow().GetDevice()), m_IndicesCount(indices.size())
	{
		CreateModel(indices, vertices);
	}

	VulkanModel::~VulkanModel()
	{

	}

	void VulkanModel::Bind()
	{
		VkCommandBuffer cmd = m_Device->GetCurrentCommandBuffer();

		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmd, 0, 1, &m_MeshBuffers.VertexBuffer.Buffer, offsets);
		vkCmdBindIndexBuffer(cmd, m_MeshBuffers.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	VkVertexInputBindingDescription VulkanModel::GetBindingDescription()
	{
		return { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX };
	}

	std::array<VkVertexInputAttributeDescription, 4> VulkanModel::GetAttributeDescriptions()
	{
		return {
			VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertices)},
			VkVertexInputAttributeDescription{1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, TexCoords)},
			VkVertexInputAttributeDescription{2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Normal)},
			VkVertexInputAttributeDescription{3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, Color)}
		};
	}

	void VulkanModel::CreateModel(std::vector<uint32_t> indices, std::vector<Vertex> vertices)
	{
		m_MeshBuffers = m_Device->UploadMesh(indices, vertices);

		m_Device->GetDeletionQueue().PushFunction([&]()
		{
			m_Device->DestroyBuffer(m_MeshBuffers.IndexBuffer);
			m_Device->DestroyBuffer(m_MeshBuffers.VertexBuffer);
		});
	}

}