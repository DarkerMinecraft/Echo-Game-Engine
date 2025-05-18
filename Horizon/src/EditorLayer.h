#pragma once

#include <Echo.h>

#include <Events/WindowEvents.h>
#include <Graphics/EditorCamera.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

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

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		//UI 
		void ToolbarUI();
		void ViewportUI();
	private:
		Ref<Framebuffer> m_MainFramebuffer;
		Ref<Framebuffer> m_FinalFramebuffer;

		Ref<Shader> m_OutlineShader;
		Ref<Pipeline> m_OutlinePipeline;

		Ref<UniformBuffer> m_OutlineBuffer;

		Ref<Scene> m_ActiveScene;

		bool m_PrimaryCamera = false;

		Window* m_Window;

		glm::vec2 m_ViewportSize{1600, 900};
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportBounds[2];

		EditorCamera m_EditorCamera;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		AssetRegistry* m_AssetRegistry;

		enum SceneState 
		{
			Edit = 0, Play = 1
		};

		struct OutlineParams
		{
			alignas(4) int selectedEntityID;
			alignas(16) glm::vec4 outlineColor;
			alignas(4) float outlineThickness;
		};
		OutlineParams m_OutlineParams;

		SceneState m_SceneState = SceneState::Edit;

		Ref<Texture2D> m_PlayButton;
		Ref<Texture2D> m_StopButton; 

		std::filesystem::path m_CurrentScenePath;

		int m_GuizmoType = -1;
	};
}

