#pragma once

#include "entt.hpp"
#include "Echo/Core/Timestep.h"

namespace Echo 
{

	class Scene 
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(Timestep ts);
	private:
		entt::registry m_Registry;
	};

}