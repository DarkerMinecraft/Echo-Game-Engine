#include "pch.h"
#include "Maths.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

namespace Echo
{
	glm::mat4 Maths::CreateTransformationMatrix(const glm::vec3 position, 
												 const glm::vec3 rotation,
												 float scale)
	{
		return glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1)) *
			glm::scale(glm::mat4(1.0f), { scale, scale, scale });
	}
}

