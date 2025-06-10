#pragma once

#include "Graphics/Primitives/Mesh.h"

#include "VulkanDevice.h"

namespace Echo 
{
	class VulkanMesh : public Mesh
	{
	public:
		VulkanMesh(Device* device, const std::string& path);
		VulkanMesh(Device* device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~VulkanMesh();

		virtual Ref<VertexBuffer> GetVertexBuffer() override { return m_VertexBuffer; };
		virtual Ref<IndexBuffer> GetIndexBuffer() override { return m_IndexBuffer; };

		virtual void UpdateVertexBuffer(const std::vector<Vertex>& vertices) override;
		virtual void UpdateIndexBuffer(const std::vector<uint32_t>& indices) override;

		virtual void Destroy() override;
	private:
		VulkanDevice* m_Device;

		void LoadModel(const std::string& path);

		void CreateVertexBuffer(const std::vector<Vertex>& vertices);

		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		bool m_IsDestroyed = false;
	};
}