#pragma once

#include <Echo.h>

#include <Echo/Events/WindowEvents.h>
#include <renderdoc_app.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Echo/Graphics/EditorCamera.h"

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
		Ref<Framebuffer> m_Framebuffer;
		Ref<Scene> m_ActiveScene;

		bool m_PrimaryCamera = false;

		Window* m_Window;

		glm::vec2 m_ViewportSize{1600, 900};
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportBounds[2];

		EditorCamera m_EditorCamera;

		SceneHierarchyPanel m_SceneHierarchyPanel;

		int m_GuizmoType = -1;
	};
}

