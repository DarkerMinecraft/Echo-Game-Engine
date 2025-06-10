#include "EditorLayer.h"

#include <array>
#include <imgui.h>
#include <ImGuizmo.h>

#include <Core/Application.h>

#include <Core/Input.h>
#include <Core/MouseButtonCodes.h>
#include <Core/KeyCodes.h>
#include <glm/gtc/type_ptr.hpp>

#include <Utils/PlatformUtils.h>
#include <Serializer/SceneSerializer.h>

#include <Math/Math.h>

namespace Echo
{

	EditorLayer::EditorLayer()
		: m_EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f)
	{

	}

	void EditorLayer::OnAttach()
	{
		EC_PROFILE_FUNCTION();

		m_ContentBrowserPanel.SetGlobalDirectory(AssetRegistry::GetGlobalPath());
		m_SceneHierarchyPanel.GetEntityComponentPanel().SetGlobalDirectory(AssetRegistry::GetGlobalPath());

		FramebufferSpecification msaaFramebufferSpec;
		msaaFramebufferSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RedInt };
		msaaFramebufferSpec.Width = 1280;
		msaaFramebufferSpec.Height = 720;
		msaaFramebufferSpec.UseSamples = true;

		m_MsaaFramebuffer = Framebuffer::Create(msaaFramebufferSpec);

		FramebufferSpecification mainFramebufferSpec;
		mainFramebufferSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RedInt };
		mainFramebufferSpec.Width = 1280;
		mainFramebufferSpec.Height = 720;

		m_MainFramebuffer = Framebuffer::Create(mainFramebufferSpec);

		FramebufferSpecification finalFramebufferSpec;
		finalFramebufferSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
		finalFramebufferSpec.Width = 1280;
		finalFramebufferSpec.Height = 720;

		m_FinalFramebuffer = Framebuffer::Create(finalFramebufferSpec);

		m_OutlineShader = AssetRegistry::LoadAsset<ShaderAsset>("Resources/shaders/outlineShader.slang");

		PipelineSpecification outlineSpec{};
		outlineSpec.CullMode = Cull::None;

		outlineSpec.RenderTarget = m_FinalFramebuffer; 
		m_OutlinePipeline = Pipeline::Create(m_OutlineShader->GetShader(), outlineSpec);

		m_OutlineParams.selectedEntityID = -2;
		m_OutlineParams.outlineColor = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f); 
		m_OutlineParams.outlineThickness = 2.0f;
		m_OutlineBuffer = UniformBuffer::Create(&m_OutlineParams, sizeof(OutlineParams));

		m_OutlineShader->SetPipeline(m_OutlinePipeline);

		m_Window = &Application::Get().GetWindow();

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_PlayButton = AssetRegistry::LoadAsset<TextureAsset>("Resources/textures/PlayButton.png");
		m_StopButton = AssetRegistry::LoadAsset<TextureAsset>("Resources/textures/StopButton.png");

		Renderer2D::Init(m_MsaaFramebuffer, 0);
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		EC_PROFILE_FUNCTION();
		Renderer2D::ResetStats();

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_OutlineParams.selectedEntityID != (int)(uint32_t)selectedEntity)
		{
			m_OutlineParams.selectedEntityID = (int)(uint32_t)selectedEntity;
		}

		{
			EC_PROFILE_SCOPE("MultiSample Render");
			CommandList cmd;
			cmd.SetSourceFramebuffer(m_MsaaFramebuffer);

			cmd.Begin();
			cmd.ClearColor(m_MsaaFramebuffer, 0, { 0.3f, 0.3f, 0.3f, 0.0f });
			cmd.ClearColor(m_MsaaFramebuffer, 1, { -1.0f, 0.0f, 0.0f, 0.0f });
			cmd.BeginRendering(m_MsaaFramebuffer);
			if (m_SceneState == Edit)
			{
				if (m_ViewportFocused)
					m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnUpdateEditor(cmd, m_EditorCamera, ts);
				Renderer2D::BeginScene(cmd, m_EditorCamera);
				OnOverlayRender();
				Renderer2D::EndScene();
			}
			else if (m_SceneState == Play)
			{
				m_GuizmoType = -1;
				m_OutlineParams.selectedEntityID = -2;

				m_ActiveScene->OnUpdateRuntime(cmd, ts);
			}
			cmd.EndRendering();
			cmd.Execute();
			m_MsaaFramebuffer->ResolveToFramebuffer(m_MainFramebuffer.get());
		}
		/*
		{
			EC_PROFILE_SCOPE("No Samples Render");
			m_OutlineBuffer->SetData(&m_OutlineParams, sizeof(OutlineParams));

			CommandList cmd;
			cmd.SetSourceFramebuffer(m_FinalFramebuffer);
			cmd.Begin();
			cmd.BeginRendering(m_FinalFramebuffer);
			cmd.BindPipeline(m_OutlinePipeline);
			m_OutlinePipeline->BindResource(0, 0, m_MainFramebuffer, 0);
			m_OutlinePipeline->BindResource(1, 0, m_MsaaFramebuffer, 1);
			m_OutlinePipeline->BindResource(2, 0, m_OutlineBuffer);
			cmd.Draw(3, 1, 0, 0);
			cmd.EndRendering();
			cmd.Execute();
		}
		*/

		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f
			&& (m_MsaaFramebuffer->GetWidth() != m_ViewportSize.x || m_MsaaFramebuffer->GetHeight() != m_ViewportSize.y))
		{
			m_MsaaFramebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_MainFramebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_FinalFramebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

			m_EditorCamera.SetViewportSize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		if (m_SceneState == Edit)
		{
			auto [mx, my] = ImGui::GetMousePos();
			mx -= m_ViewportBounds[0].x;
			my -= m_ViewportBounds[0].y;
			glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
			my = viewportSize.y - my;

			int mouseX = (int)mx;
			int mouseY = (int)my;
			if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
			{
				int texX = (int)((mouseX / viewportSize.x) * m_MainFramebuffer->GetWidth());
				int texY = (int)((mouseY / viewportSize.y) * m_MainFramebuffer->GetHeight());
				if (!ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
				{
					int entityID = m_MainFramebuffer->ReadPixel(1, texX, texY);
					if (entityID != -1)
					{
						m_Window->SetCursor(Cursor::HAND);
						if (Input::IsMouseButtonPressed(EC_MOUSE_BUTTON_LEFT))
						{
							m_SceneHierarchyPanel.SetSelectedEntity(entityID);
							m_OutlineParams.selectedEntityID = entityID;
						}
					}
					else
					{
						m_Window->SetCursor(Cursor::ARROW);
						if (Input::IsMouseButtonPressed(EC_MOUSE_BUTTON_LEFT))
						{
							m_SceneHierarchyPanel.SetSelectedEntity(-1);
							m_OutlineParams.selectedEntityID = -2;
						}
					}
				}
			}
		}

	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		m_EditorCamera.OnEvent(e);

		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	void EditorLayer::OnImGuiRender()
	{
		EC_PROFILE_FUNCTION();
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static bool dockspace_open = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize.x = 250;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = 32;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
					m_ActiveScene = CreateRef<Scene>();
					m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
					m_SceneHierarchyPanel.SetContext(m_ActiveScene);
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
				{
					std::string filePath = FileDialogs::OpenFile("Echo Scene (*.echo)\0*.echo\0");

					if (!filePath.empty())
					{
						OpenScene(filePath);
					}
				}

				if (ImGui::MenuItem("Save...", "Ctrl+S"))
				{
					SaveScene();
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				{
					std::string filePath = FileDialogs::SaveFile("Echo Scene (*.echo)\0*.echo\0");
					if (!filePath.empty())
					{
						SceneSerializer serializer(m_ActiveScene);
						serializer.Serialize(filePath);
					}
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::End();

		ImGui::Begin("RendererQuad::Stats");
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quad Count: %d", stats.QuadCount);
		ImGui::Text("Vertex Count: %d", stats.GetTotalQuadVertexCount());
		ImGui::Text("Index Count: %d", stats.GetTotalQuadIndexCount());
		ImGui::End();

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel.OnImGuiRender();

		ViewportUI();
		ToolbarUI();
	}

	void EditorLayer::ToolbarUI()
	{
		EC_PROFILE_FUNCTION();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

		auto& colors = ImGui::GetStyle().Colors;
		auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		auto& buttonActive = colors[ImGuiCol_ButtonActive];

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImTextureID icon = m_SceneState == Edit ? (ImTextureID)m_PlayButton->GetImGuiResourceID() : (ImTextureID)m_StopButton->GetImGuiResourceID();

		float size = ImGui::GetWindowHeight() - 4.0f;
		ImGui::SameLine((ImGui::GetWindowWidth() - size) * 0.5f);
		if (ImGui::ImageButton("##type", icon, ImVec2(size, size)))
		{
			if (m_SceneState == SceneState::Edit)
			{
				OnScenePlay();
			}
			else
			{
				OnSceneEdit();
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::ViewportUI()
	{
		EC_PROFILE_FUNCTION();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportOffset = ImGui::GetCursorPos();

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().SetImGuiBlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		auto viewportSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *(glm::vec2*)&viewportSize)
		{
			m_ViewportSize = { viewportSize.x, viewportSize.y };
		}
		void* imGuiID = m_MainFramebuffer->GetImGuiTexture(0);
		ImGui::Image((ImTextureID)imGuiID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(AssetRegistry::GetGlobalPath() / path);
			}
			ImGui::EndDragDropTarget();
		}

		auto windowSize = ImGui::GetWindowSize();
		auto minBound = ImGui::GetWindowPos();
		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;

		ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
		m_ViewportBounds[0] = { minBound.x, minBound.y };
		m_ViewportBounds[1] = { maxBound.x, maxBound.y };

		//Gizmos 
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
		{
			if (m_GuizmoType != -1)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportSize.x, m_ViewportSize.y);

				const glm::mat4& projection = m_EditorCamera.GetProjection();
				glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

				auto& tc = selectedEntity.GetComponent<TransformComponent>();
				glm::mat4 transform = tc.GetTransform();

				bool snap = Input::IsKeyPressed(EC_KEY_LEFT_CONTROL);
				float snapValue = 0.5f;
				if (m_GuizmoType == ImGuizmo::OPERATION::ROTATE)
					snapValue = 45.0f;

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(projection),
									 (ImGuizmo::OPERATION)m_GuizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 translation, rotation, scale;
					Math::DecomposeTransform(transform, translation, rotation, scale);

					glm::vec3 deltaRot = rotation - tc.Rotation;
					tc.Translation = translation;
					tc.Rotation += deltaRot;
					tc.Scale = scale;
				}
			}
		}
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void EditorLayer::Destroy()
	{
		Renderer2D::Destroy();
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		bool controlPressed = Input::IsKeyPressed(EC_KEY_LEFT_CONTROL) || Input::IsKeyPressed(EC_KEY_RIGHT_CONTROL);
		bool shiftPressed = Input::IsKeyPressed(EC_KEY_LEFT_SHIFT) || Input::IsKeyPressed(EC_KEY_RIGHT_SHIFT);

		switch (e.GetKeyCode())
		{
			case EC_KEY_S:
			{
				if (controlPressed)
				{
					if (shiftPressed)
						SaveSceneAs();
					else
						SaveScene();

					return true;
				}
				break;
			}
			case EC_KEY_N:
			{
				if (controlPressed)
				{
					NewScene();

					return true;
				}
				break;
			}
			case EC_KEY_O:
			{
				if (controlPressed)
				{
					OpenScene();

					return true;
				}
				break;
			}
			case EC_KEY_Q:
				m_GuizmoType = -1;
				break;
			case EC_KEY_W:
				m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case EC_KEY_E:
				m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case EC_KEY_R:
				m_GuizmoType = ImGuizmo::OPERATION::SCALE;
				break;
			default:
				break;
		}
		return false;
	}

	void EditorLayer::NewScene()
	{
		EC_PROFILE_FUNCTION();
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		EC_PROFILE_FUNCTION();
		std::string filePath = FileDialogs::OpenFile("Echo Scene (*.echo)\0*.echo\0");
		if (!filePath.empty())
		{
			OpenScene(filePath);
			m_CurrentScenePath = filePath;
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		EC_PROFILE_FUNCTION();
		m_CurrentScenePath = path;

		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_EditorScene);

		SceneSerializer serializer(m_EditorScene);
		serializer.Deserialize(path.string());

		m_ActiveScene = m_EditorScene;
	}

	void EditorLayer::SaveSceneAs()
	{
		EC_PROFILE_FUNCTION();
		std::string filePath = FileDialogs::SaveFile("Echo Scene (*.echo)\0*.echo\0");
		if (!filePath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filePath);
		}
	}

	void EditorLayer::SaveScene()
	{
		EC_PROFILE_FUNCTION();
		if (m_CurrentScenePath.empty())
			SaveSceneAs();
		else
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(m_CurrentScenePath.string());
		}
	}

	void EditorLayer::OnScenePlay()
	{
		EC_PROFILE_FUNCTION();
		m_ActiveScene = Scene::Copy(m_EditorScene);

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_ActiveScene->OnRuntimeStart();
		m_SceneState = SceneState::Play;
	}

	void EditorLayer::OnSceneEdit()
	{
		EC_PROFILE_FUNCTION();
		m_ActiveScene->OnRuntimeStop();
		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}


	void EditorLayer::OnOverlayRender()
	{
		auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>(); 
		for (auto entity : view) 
		{
			auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);
			glm::mat4 transform = glm::translate(tc.GetTransform(), glm::vec3(0, 0, 0.1f));

			Renderer2D::DrawCircle({ .InstanceID = -1, .Color = glm::vec4(0, 1, 0, 1), .OutlineThickness = 0.05f }, transform);
		}
	}

}