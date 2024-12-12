#pragma once

#include <glm/glm.hpp>

namespace Echo 
{
	class Maths 
	{
	public:
		static glm::mat4 CreateTransformationMatrix(const glm::vec3 position,
													 const glm::vec3 rotation,
													 float scale);
	};
}