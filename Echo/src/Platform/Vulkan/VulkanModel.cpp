#include "pch.h"
#include "VulkanModel.h"

#include "Echo/Core/Application.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace Echo
{

	VulkanModel::VulkanModel(std::vector<uint32_t> indices, std::vector<Vertex> vertices, Ref<Texture> texture)
		: m_Device((VulkanDevice*)Application::Get().GetWindow().GetDevice()), m_IndicesCount(indices.size()), m_Texture(texture)
	{
		CreateModel(indices, vertices);
	}

	VulkanModel::VulkanModel(const std::string& objFilePath, Ref<Texture> texture)
		: m_Device((VulkanDevice*)Application::Get().GetWindow().GetDevice()), m_Texture(texture)
	{
		CreateModel(objFilePath);
	}

	VulkanModel::~VulkanModel()
	{
		m_Device->DestroyBuffer(m_MeshBuffers.IndexBuffer);
		m_Device->DestroyBuffer(m_MeshBuffers.VertexBuffer);
	}

	void VulkanModel::Bind(Pipeline* pipeline)
	{
		VkCommandBuffer cmd = m_Device->GetCurrentCommandBuffer();

		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmd, 0, 1, &m_MeshBuffers.VertexBuffer.Buffer, offsets);
		vkCmdBindIndexBuffer(cmd, m_MeshBuffers.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);

		pipeline->UpdatePushConstants(m_PushConstants);
		if (m_Texture)
		{
			m_Texture->Bind(pipeline);
		}
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
	}

	void VulkanModel::CreateModel(const std::string& objFilePath)
	{
		std::vector<uint32_t> indices;
		std::vector<Vertex> vertices;
		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, error;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, objFilePath.c_str()))
		{
			throw std::runtime_error(warn + error);
		}

		for (const auto& shape : shapes) 
		{
			for (const auto& index : shape.mesh.indices) 
			{
				Vertex vertex{};

				vertex.Vertices =
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.TexCoords =
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.Normal =
				{
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};

				vertex.Color = { 1.0f, 1.0f, 1.0f, 1.0f };

				vertices.push_back(vertex);
				
				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}

		m_IndicesCount = indices.size();
		m_MeshBuffers = m_Device->UploadMesh(indices, vertices);
	}

}