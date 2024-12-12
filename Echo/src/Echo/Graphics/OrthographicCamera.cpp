#include "pch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Echo 
{

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, 0.0001f, (float) 1000))
	{
		m_ProjViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.f), m_Position) *
			glm::rotate(glm::mat4(1.f), glm::radians(m_Rotation.x), glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.f), glm::radians(m_Rotation.y), glm::vec3(0, 1, 0)) *
			glm::rotate(glm::mat4(1.f), glm::radians(m_Rotation.z), glm::vec3(0, 0, 1)); 

		m_ViewMatrix = glm::inverse(transform);
		m_ProjViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}