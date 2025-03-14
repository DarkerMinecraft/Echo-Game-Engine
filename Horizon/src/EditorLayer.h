#pragma once

#include <Echo.h>

#include <Echo/Events/WindowEvents.h>
#include <renderdoc_app.h>

#include "Panels/SceneHierarchyPanel.h"

namespace Echo
{

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts) override;

		virtual void OnEvent(Event& e) override;

		virtual void OnImGuiRender() override;

		virtual void Destroy() override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
	private:
		Ref<Image> m_Image;
		Ref<Scene> m_ActiveScene;

		bool m_PrimaryCamera = false;

		glm::vec2 m_ViewportSize{1600, 900};
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		SceneHierarchyPanel m_SceneHierarchyPanel;

		int m_GuizmoType = -1;

		RENDERDOC_API_1_1_2* m_RdocAPI = NULL;
	};
}

