#pragma once

#include "Echo/Scene/Scene.h"
#include "Echo/Scene/Entity.h"
#include "Echo/Core/Base.h"

#include "EntityComponentPanel.h"

namespace Echo 
{

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);
		void SetContext(const Ref<Scene>& scene);

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(int entityID);

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;

		EntityComponentPanel m_EntityComponentPanel;
	};

}