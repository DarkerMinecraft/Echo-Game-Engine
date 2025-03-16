#include "EditorLayer.h"

#include <array>
#include <imgui.h>
#include <ImGuizmo.h>

#include <Echo/Core/Input.h>
#include <Echo/Core/KeyCodes.h>
#include <glm/gtc/type_ptr.hpp>

#include <Echo/Utils/PlatformUtils.h>
#include <Echo/Scene/SceneSerializer.h>

#include <Echo/Math/Math.h>

namespace Echo
{

	EditorLayer::EditorLayer()
		: m_EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f)
	{

	}

	void EditorLayer::OnAttach()
	{
		FramebufferSpecification framebufferSpec;
		framebufferSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth32F };
		framebufferSpec.Width = 1280;
		framebufferSpec.Height = 720;

		m_Framebuffer = Framebuffer::Create(framebufferSpec);

		m_ActiveScene = CreateRef<Scene>();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		RendererQuad::Init(m_Framebuffer, 0);
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		RendererQuad::ResetStats();

		if (m_ViewportFocused)
			m_EditorCamera.OnUpdate(ts);

		CommandList cmd;
		cmd.SetSrcImage(m_Framebuffer);

		cmd.Begin();
		cmd.ClearColor(m_Framebuffer, 0, { 0.3f, 0.3f, 0.3f, 0.3f });
		cmd.BeginRendering(m_Framebuffer);
		m_ActiveScene->OnUpdateEditor(cmd, m_EditorCamera, ts);
		cmd.EndRendering();
		cmd.Execute();

		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f
			&& (m_Framebuffer->GetWidth() != m_ViewportSize.x || m_Framebuffer->GetHeight() != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
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
						m_ActiveScene = CreateRef<Scene>();
						m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
						m_SceneHierarchyPanel.SetContext(m_ActiveScene);

						SceneSerializer serializer(m_ActiveScene);
						serializer.Deserialize(filePath);
					}
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
		auto stats = RendererQuad::GetStats();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quad Count: %d", stats.QuadCount);
		ImGui::Text("Vertex Count: %d", stats.GetTotalVertexCount());
		ImGui::Text("Index Count: %d", stats.GetTotalIndexCount());
		ImGui::End();

		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		ImGui::Begin("Viewport");
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		auto viewportSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *(glm::vec2*)&viewportSize)
		{
			m_ViewportSize = { viewportSize.x, viewportSize.y };
		}
		ImGui::Image((ImTextureID)m_Framebuffer->GetImGuiTexture(0), ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		//Gizmos 
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GuizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, m_ViewportSize.x, m_ViewportSize.y);

			//Runtime
			/*
			auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			const glm::mat4& projection = camera.GetProjection();
			glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
			*/
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

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::Destroy()
	{
		RendererQuad::Destroy();
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		bool controlPressed = Input::IsKeyPressed(EC_KEY_LEFT_CONTROL) || Input::IsKeyPressed(EC_KEY_RIGHT_CONTROL);
		bool shiftPressed = Input::IsKeyPressed(EC_KEY_LEFT_SHIFT) || Input::IsKeyPressed(EC_KEY_RIGHT_SHIFT);

		switch (e.GetKeyCode())
		{
			case EC_KEY_S:
			{
				if (controlPressed && shiftPressed)
				{
					std::string filePath = FileDialogs::SaveFile("Echo Scene (*.echo)\0*.echo\0");
					if (!filePath.empty())
					{
						SceneSerializer serializer(m_ActiveScene);
						serializer.Serialize(filePath);
					}
					return true;
				}
				break;
			}
			case EC_KEY_N:
			{
				if (controlPressed)
				{
					m_ActiveScene = CreateRef<Scene>();
					m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
					m_SceneHierarchyPanel.SetContext(m_ActiveScene);
					return true;
				}
				break;
			}
			case EC_KEY_O:
			{
				if (controlPressed)
				{
					std::string filePath = FileDialogs::OpenFile("Echo Scene (*.echo)\0*.echo\0");
					if (!filePath.empty())
					{
						m_ActiveScene = CreateRef<Scene>();
						m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
						m_SceneHierarchyPanel.SetContext(m_ActiveScene);

						SceneSerializer serializer(m_ActiveScene);
						serializer.Deserialize(filePath);
					}
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

}