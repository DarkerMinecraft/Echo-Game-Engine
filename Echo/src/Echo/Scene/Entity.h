#pragma once

#include "Scene.h"
#include "Core/UUID.h"
#include "Components.h"
#include "ComponentRegistry.h"

namespace Echo 
{

	class Entity
	{
	public:
		Entity(entt::entity handle, Scene* scene);
		Entity() = default;
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) 
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...); 
			ComponentMetadata* meta = ComponentRegistry::GetMetadata<T>();
			if (meta && meta->InitializeComponent)
			{
				meta->InitializeComponent(*this, &component);
			}

			if (HasComponent<ComponentOrderComponent>()) 
			{
				GetComponent<ComponentOrderComponent>().ComponentOrder.push_back(meta->Name);
			}
			else 
			{
				GetComponentOrder();
				GetComponent<ComponentOrderComponent>().ComponentOrder.push_back(meta->Name);
			}

			return component;
		}

		template<typename T>
		T& GetComponent() const { return m_Scene->m_Registry.get<T>(m_EntityHandle); }

		template<typename T>
		bool HasComponent() const { return m_Scene->m_Registry.any_of<T>(m_EntityHandle); }

		template<typename T>
		void RemoveComponent() { m_Scene->m_Registry.remove<T>(m_EntityHandle); }

		operator bool() const { return m_EntityHandle != entt::null; }
		operator uint32_t() const { return (uint32_t) m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }

		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

		entt::entity GetHandle() const { return m_EntityHandle; }
		const UUID GetUUID() const { return GetComponent<IDComponent>().ID; }

		glm::mat4 GetTransform() { return GetComponent<TransformComponent>().GetTransform(); }
		const glm::mat4 GetTransform() const { return GetComponent<TransformComponent>().GetTransform(); }

		Scene* GetScene() { return m_Scene; }

		void SetComponentOrder(const std::vector<std::string>& order);
		std::vector<std::string> GetComponentOrder();
		void ReorderComponent(const std::string& componentName, size_t newIndex);

		int GetDuplicatedNumber() { return m_DuplicateNumber; }
		void AddDuplicatedNumber() { m_DuplicateNumber++; }
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;

		int m_DuplicateNumber = 1;
	};

}