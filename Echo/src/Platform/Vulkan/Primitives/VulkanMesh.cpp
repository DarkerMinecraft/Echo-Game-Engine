#include "pch.h"
#include "VulkanMesh.h"

#define TINYOBJLOADER_IMPLEMENTATION 
#define TINYOBJLOADER_USE_MAPBOX_EARCUT

#include "tiny_obj_loader.h"

namespace Echo
{

	VulkanMesh::VulkanMesh(Device* device, const std::string& path)
		: m_Device(static_cast<VulkanDevice*>(device))
	{
		LoadModel(path);
	}

	VulkanMesh::VulkanMesh(Device* device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		: m_Device(static_cast<VulkanDevice*>(device))
		, m_IndexBuffer(IndexBuffer::Create(indices))
	{
		CreateVertexBuffer(vertices);
	}

	VulkanMesh::~VulkanMesh()
	{

	}

	void VulkanMesh::UpdateVertexBuffer(const std::vector<Vertex>& vertices)
	{

	}

	void VulkanMesh::UpdateIndexBuffer(const std::vector<uint32_t>& indices)
	{
		m_IndexBuffer->SetIndices(indices);
	}

	void VulkanMesh::Destroy()
	{

	}

	void VulkanMesh::LoadModel(const std::string& path)
	{
		EC_PROFILE_FUNCTION();
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		for (const auto& shape : shapes)
		{
			size_t indexOffset = 0;
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
			{
				size_t fv = size_t(shape.mesh.num_face_vertices[f]);
				for (size_t v = 0; v < fv; v++)
				{
					tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];
					Vertex vertex;
					vertex.Position.x = attrib.vertices[3 * idx.vertex_index + 0];
					vertex.Position.y = attrib.vertices[3 * idx.vertex_index + 1];
					vertex.Position.z = attrib.vertices[3 * idx.vertex_index + 2];
					vertex.TexCoord.x = attrib.texcoords[2 * idx.texcoord_index + 0];
					vertex.TexCoord.y = attrib.texcoords[2 * idx.texcoord_index + 1];
					vertex.Normal.x = attrib.normals[3 * idx.normal_index + 0];
					vertex.Normal.y = attrib.normals[3 * idx.normal_index + 1];
					vertex.Normal.z = attrib.normals[3 * idx.normal_index + 2];

					vertices.push_back(vertex);
					indices.push_back(static_cast<uint32_t>(indices.size()));
				}
				indexOffset += fv;
			}
		}

		CreateVertexBuffer(vertices);
		m_IndexBuffer = IndexBuffer::Create(indices);
	}

	void VulkanMesh::CreateVertexBuffer(const std::vector<Vertex>& vertices)
	{
		EC_PROFILE_FUNCTION();
		const size_t floatsPerVertex = 8; // 3 + 2 + 3
		std::vector<float> bufferData(vertices.size() * floatsPerVertex);

		for (size_t i = 0; i < vertices.size(); i++)
		{
			size_t offset = i * floatsPerVertex;

			// Position (vec3)
			bufferData[offset + 0] = vertices[i].Position.x;
			bufferData[offset + 1] = vertices[i].Position.y;
			bufferData[offset + 2] = vertices[i].Position.z;

			// TexCoord (vec2)
			bufferData[offset + 3] = vertices[i].TexCoord.x;
			bufferData[offset + 4] = vertices[i].TexCoord.y;

			// Normal (vec3)
			bufferData[offset + 5] = vertices[i].Normal.x;
			bufferData[offset + 6] = vertices[i].Normal.y;
			bufferData[offset + 7] = vertices[i].Normal.z;
		}

		m_VertexBuffer = VertexBuffer::Create(bufferData.data(), bufferData.size() * sizeof(float), false);
	}

}
