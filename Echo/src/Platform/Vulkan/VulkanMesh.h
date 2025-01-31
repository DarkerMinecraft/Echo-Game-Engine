#pragma once

#include "Echo/Graphics/Mesh.h"
#include "Utils/VulkanTypes.h"

#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanMesh : public Mesh 
	{
	public:
		VulkanMesh(VulkanDevice* device, std::vector<Vertex3D> meshData, std::vector<uint32_t> indices);
		VulkanMesh(VulkanDevice* device, std::vector<Vertex2D> meshData, std::vector<uint32_t> indices);
		virtual ~VulkanMesh(); 

		virtual void Bind() override;

		virtual uint32_t GetIndicesSize() override { return m_IndicesSize; };
	private:
		void CreateMesh(std::vector<Vertex3D> meshData, std::vector<uint32_t> indices);
		void CreateMesh(std::vector<Vertex2D> meshData, std::vector<uint32_t> indices);
	private:
		VulkanDevice* m_Device;

		uint32_t m_IndicesSize;

		Ref<Buffer> m_VertexBuffer;
		Ref<Buffer> m_IndicesBuffer;
	};

}