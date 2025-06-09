#pragma once

#include "Graphics/CommandList.h"
#include "Graphics/EditorCamera.h"

#include <entt.hpp>

namespace Echo 
{

	class Physics2D;
	class Entity;

	class Scene 
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> srcScene);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntity(const std::string& name, uint64_t uuid);
		Entity GetEntityByUUID(UUID uuid);
		Entity GetPrimaryCameraEntity();

		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateEditor(CommandList& cmd, const EditorCamera& camera, Timestep ts);
		void OnUpdateRuntime(CommandList& cmd, Timestep ts);

		void OnViewportResize(uint32_t width, uint32_t height);

		uint32_t GetViewportWidth() { return m_ViewportWidth; }
		uint32_t GetViewportHeight() { return m_ViewportHeight; }

		template<typename... Components> 
		auto GetAllEntitiesWith() 
		{
			return m_Registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdd(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		Scope<Physics2D> m_Physics2D;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
		friend class EntityComponentPanel;
	};

}