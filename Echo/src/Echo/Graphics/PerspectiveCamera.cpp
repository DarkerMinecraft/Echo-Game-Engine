#include "pch.h"
#include "PerspectiveCamera.h"

#include "Echo/Core/Application.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace Echo 
{

	PerspectiveCamera::PerspectiveCamera(const glm::vec3& position, const glm::vec3& rotation, float fov, float nearPlane, float farPlane)
		: m_FarPlane(farPlane), m_NearPlane(nearPlane), m_Fov(glm::radians(fov)), m_Position(position), m_Rotation(rotation)
	{
		UpdateProjectionMatrix();
		RecalculateViewMatrix();
	}

	void PerspectiveCamera::UpdateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(m_Fov, Application::Get().GetWindow().GetAspectRatio(), m_NearPlane, m_FarPlane);
		m_ProjectionMatrix[1][1] *= -1;
	}

	void PerspectiveCamera::RecalculateViewMatrix()
	{
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), -glm::radians(m_Rotation.z), glm::vec3(0, 0, 1))
			* glm::rotate(glm::mat4(1.0f), -glm::radians(m_Rotation.y), glm::vec3(0, 1, 0))
			* glm::rotate(glm::mat4(1.0f), -glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));

		glm::mat4 translation = glm::translate(glm::mat4(1.0f), -m_Position);

		m_ViewMatrix = rotation * translation;
		m_ProjViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}