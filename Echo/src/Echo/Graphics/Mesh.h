#pragma once

#include "Buffer.h"

namespace Echo 
{

	struct Vertex 
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec3 Normal;
	};

	class Mesh 
	{
	public:
		virtual ~Mesh() = default;

		virtual void Destroy() = 0;

		virtual Ref<VertexBuffer> GetVertexBuffer() = 0;
		virtual Ref<IndexBuffer> GetIndexBuffer() = 0;

		virtual void UpdateVertexBuffer(const std::vector<Vertex>& vertices) = 0;
		virtual void UpdateIndexBuffer(const std::vector<uint32_t>& indices) = 0;

		static Ref<Mesh> Create(const std::string& path); 
		static Ref<Mesh> Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	};

}