#include "pch.h"
#include "VulkanMesh.h"

namespace Echo 
{

	VulkanMesh::VulkanMesh(VulkanDevice* device, std::vector<Vertex3D> meshData, std::vector<uint32_t> indices)
		: m_Device(device)
	{

	}

	VulkanMesh::VulkanMesh(VulkanDevice* device, std::vector<Vertex2D> meshData, std::vector<uint32_t> indices) 
		: m_Device(device)
	{

	}

	VulkanMesh::~VulkanMesh()
	{

	}

	void VulkanMesh::Bind()
	{

	}

	void VulkanMesh::CreateMesh(std::vector<Vertex3D> meshData, std::vector<uint32_t> indices)
	{
		m_IndicesSize = static_cast<uint32_t>(indices.size());

		const size_t vertexBufferSize = meshData.size() * sizeof(Vertex3D);
		const size_t indicesBufferSize = indices.size() * sizeof(uint32_t);

		BufferDesc vertexBufferData{};
		vertexBufferData.BufferSize = vertexBufferSize;
		vertexBufferData.Usage = BufferUsage::BUFFER_USAGE_VERTEX;
		vertexBufferData.Flags = MemoryFlags::MEMORY_DEVICE_LOCAL;
		vertexBufferData.InitialData = &meshData;
		vertexBufferData.UseStagingBuffer = true;

		m_VertexBuffer = m_Device->CreateBuffer(vertexBufferData);

		BufferDesc indicesBufferData{};
		indicesBufferData.BufferSize = indicesBufferSize;
		indicesBufferData.Usage = BufferUsage::BUFFER_USAGE_INDEX;
		indicesBufferData.Flags = MemoryFlags::MEMORY_DEVICE_LOCAL;
		indicesBufferData.InitialData = &indices;
		indicesBufferData.UseStagingBuffer = true;

		m_IndicesBuffer = m_Device->CreateBuffer(indicesBufferData);
	}

	void VulkanMesh::CreateMesh(std::vector<Vertex2D> meshData, std::vector<uint32_t> indices)
	{
		m_IndicesSize = static_cast<uint32_t>(indices.size());

		const size_t vertexBufferSize = meshData.size() * sizeof(Vertex2D);
		const size_t indicesBufferSize = indices.size() * sizeof(uint32_t);

		BufferDesc vertexBufferData{};
		vertexBufferData.BufferSize = vertexBufferSize;
		vertexBufferData.Usage = BufferUsage::BUFFER_USAGE_VERTEX;
		vertexBufferData.Flags = MemoryFlags::MEMORY_DEVICE_LOCAL;
		vertexBufferData.InitialData = &meshData;
		vertexBufferData.UseStagingBuffer = true;

		m_VertexBuffer = m_Device->CreateBuffer(vertexBufferData);

		BufferDesc indicesBufferData{};
		indicesBufferData.BufferSize = indicesBufferSize;
		indicesBufferData.Usage = BufferUsage::BUFFER_USAGE_INDEX;
		indicesBufferData.Flags = MemoryFlags::MEMORY_DEVICE_LOCAL;
		indicesBufferData.InitialData = &indices;
		indicesBufferData.UseStagingBuffer = true;

		m_IndicesBuffer = m_Device->CreateBuffer(indicesBufferData);
	}

}