#pragma once 

#include "Echo/Core/Base.h"

#include <vector>
#include <glm/glm.hpp>

namespace Echo 
{

	struct Vertex 
	{
		alignas(16) glm::vec3 Vertices;
		alignas(8) glm::vec2 TexCoords;
		alignas(16) glm::vec3 Normal;
		alignas(16) glm::vec4 Color;
	};

	class Model 
	{
	public:
		virtual ~Model() = default;

		virtual void Bind() = 0;
		virtual uint32_t GetIndicesCount() = 0;

		static Ref<Model> Create(std::vector<uint32_t> indices, std::vector<Vertex> vertices); 
	};

}