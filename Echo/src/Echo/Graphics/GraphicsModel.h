#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Echo
{
	class GraphicsModel
	{
	public:
		struct Vertex
		{
			glm::vec2 Position;
			glm::vec3 Color;
		};

		virtual ~GraphicsModel() = default;

		virtual void Destroy() = 0;

		static Ref<GraphicsModel> Create(const std::vector<Vertex>& vertices);
	};
}