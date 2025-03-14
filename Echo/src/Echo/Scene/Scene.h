#pragma once

#include "Echo/Graphics/CommandList.h"

#include <entt.hpp>

namespace Echo 
{

	class Entity;

	class Scene 
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		Entity GetPrimaryCameraEntity();

		void DestroyEntity(Entity entity);

		void OnRender(CommandList& cmd);
		void OnUpdate(Timestep ts);

		void OnViewportResize(uint32_t width, uint32_t height);
	private:
		template<typename T>
		void OnComponentAdd(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
		friend class EntityComponentPanel;
	};

}