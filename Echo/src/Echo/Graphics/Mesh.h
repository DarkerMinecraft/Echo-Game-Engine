#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Echo 
{

	struct Vertex3D 
	{
		glm::vec3 Vertices;
		glm::vec2 TexCoords;
		glm::vec3 Normals;
	};

	struct Vertex2D
	{
		glm::vec2 Vertices;
		glm::vec2 TexCoords;
	};

	class Mesh 
	{
	public:
		virtual ~Mesh() = default;

		virtual void Bind() = 0;

		virtual uint32_t GetIndicesSize() = 0;
	};

}