#pragma once

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace Echo 
{

	class Camera 
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projectionMatrix)
			: m_ProjectionMatrix(projectionMatrix)
		{}
		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; };
	protected:
		glm::mat4 m_ProjectionMatrix;
	};

}
