#pragma once

#include "glm/glm.hpp"

namespace Echo 
{
	class PerspectiveCamera 
	{
	public:
		PerspectiveCamera(const glm::vec3& position, const glm::vec3& rotation, float fov, float nearPlane, float farPlane);

		void SetPosition(const glm::vec3 position) { m_Position = position; RecalculateViewMatrix(); }
		void SetRotation(const glm::vec3 rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetProjViewMatrix() const { return m_ProjViewMatrix; }

		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::vec3& GetRotation() const { return m_Rotation; }

		void UpdateProjectionMatrix();
	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjViewMatrix;

		glm::vec3 m_Position;
		glm::vec3 m_Rotation;

		float m_Fov;
		float m_NearPlane;
		float m_FarPlane;
	};
}