#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Echo/Core/Timestep.h"
#include "Echo/Events/WindowEvents.h"
#include "Echo/Events/MouseEvents.h"

namespace Echo 
{

	class Camera 
	{
	public:
		Camera() = default;
		Camera(const glm::mat4 projection)
			: m_Projection(projection) {}
		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
	protected:
		glm::mat4 m_Projection{1.0f};
	};

}
