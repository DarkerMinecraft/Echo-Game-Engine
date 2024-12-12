#pragma once 

#include "Echo/Core/Base.h"

#include "Pipeline.h"
#include "Texture.h"

#include <vector>
#include <string>

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace Echo 
{

	struct Vertex
	{
		alignas(16) glm::vec3 Vertices;
		alignas(8) glm::vec2 TexCoords;
		alignas(16) glm::vec3 Normal;
		alignas(16) glm::vec4 Color;

		bool operator==(const Vertex& other) const
		{
			return Vertices == other.Vertices && Color == other.Color && TexCoords == other.TexCoords && Normal == other.Normal;
		}
	};

	class Model 
	{
	public:
		virtual ~Model() = default;

		virtual void Bind(Pipeline* pipeline) = 0;
		virtual void UpdatePushConstants(void* pushConstants) = 0;

		virtual uint32_t GetIndicesCount() = 0;

		static Ref<Model> Create(std::vector<uint32_t> indices, std::vector<Vertex> vertices, Ref<Texture> texture = nullptr); 
		static Ref<Model> Create(const std::string& objFilePath, Ref<Texture> texture = nullptr);
	};

}

namespace std
{
	template<> struct hash<Echo::Vertex>
	{
		size_t operator()(Echo::Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.Vertices) ^
					(hash<glm::vec4>()(vertex.Color) << 1)) >> 1) ^
					(hash<glm::vec2>()(vertex.TexCoords) << 1);
		}
	};
}