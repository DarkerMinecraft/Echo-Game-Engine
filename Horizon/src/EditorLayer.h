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

		void OnScenePlay();
		void OnSceneEdit();

		void OnOverlayRender(CommandList& cmd);

		//UI 
		void ToolbarUI();
		void ViewportUI();

		void SettingsUI();
		void DebugToolsUI();
	private:
		Ref<Framebuffer> m_MsaaFramebuffer;
		Ref<Framebuffer> m_MainFramebuffer;
		Ref<Framebuffer> m_FinalFramebuffer;

		Ref<ShaderAsset> m_OutlineShader;
		Ref<Pipeline> m_OutlinePipeline;

		Ref<UniformBuffer> m_OutlineBuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;

		bool m_PrimaryCamera = false;
		bool m_ShowPhysicsColliders = false; 

		Window* m_Window;

		glm::vec2 m_ViewportSize{1600, 900};
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportBounds[2];

		EditorCamera m_EditorCamera;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		enum SceneState 
		{
			Edit = 0, Play = 1
		};

		struct OutlineParams
		{
			alignas(4) int selectedEntityID;
			alignas(16) glm::vec4 outlineColor;
			alignas(4) float outlineThickness;
		} m_OutlineParams;

		SceneState m_SceneState = SceneState::Edit;

		Ref<TextureAsset> m_PlayButton;
		Ref<TextureAsset> m_StopButton; 

		std::filesystem::path m_CurrentScenePath;

		int m_GuizmoType = -1;

		float m_FrameTime = 0.0f;
    	float m_FPS = 0.0f;
    	std::vector<float> m_FrameTimeHistory;
    	static constexpr size_t MAX_FRAME_HISTORY = 100;

		float m_DisplayUpdateTimer = 0.0f;
		float m_DisplayUpdateInterval = 1.0f; // Update every 1 second
		float m_DisplayFPS = 0.0f;
		float m_DisplayFrameTime = 0.0f;

		// Averaging accumulators
		float m_FPSAccumulator = 0.0f;
		float m_FrameTimeAccumulator = 0.0f;
		int m_SampleCount = 0;
	};
}

